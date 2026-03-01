#pragma once
#include <application/editor/editor_interface.hpp>
#include <platform/registry.hpp>
#include <platform/logging.hpp>
#include <queue>

struct SceneGraphAction
{
    inline virtual void action() = 0;
};

struct RemoveEntityAction : public SceneGraphAction
{

    Entity entity = {};
    inline virtual void action() override
    {
        if (entity.tag == 0) return;
        EntityRegistry::DestroyEntity(entity);
    };

};

struct CreateEntityAction : public SceneGraphAction
{
    inline virtual void action() override
    {
        EntityRegistry::CreateEntity();
    }
};

struct CreateDirectionLightAction : public SceneGraphAction
{

    inline virtual void action() override
    {
        Entity entity = EntityRegistry::CreateEntity();
        Metadata &meta = entity.get<Metadata>();
        Transform &transform = entity.get<Transform>();
        transform.scale = { 0.10f, 0.10f, 0.10f };
        meta.name = "Directional Light";
        Material &material = entity.attach<Material>();
        material.ambient_coefficient = 1.0f;
        Mesh &mesh = entity.attach<Mesh>();
        mesh.mesh = pde_renderer_get_sphere_mesh();
        entity.attach<DirectionLight>();
        
    }

};

struct CreatePointLightAction : public SceneGraphAction
{

    inline virtual void action() override
    {

        Entity entity = EntityRegistry::CreateEntity();
        Metadata &meta = entity.get<Metadata>();
        Transform &transform = entity.get<Transform>();
        transform.scale = { 0.10f, 0.10f, 0.10f };
        meta.name = "Point Light";
        Material &material = entity.attach<Material>();
        material.ambient_coefficient = 1.0f;
        Mesh &mesh = entity.attach<Mesh>();
        mesh.mesh = pde_renderer_get_sphere_mesh();
        entity.attach<PointLight>();
        
    }

};

struct CreateCubeEntityAction : public SceneGraphAction
{
    inline virtual void action() override
    {
        Entity entity = EntityRegistry::CreateEntity();
        Metadata &meta = entity.get<Metadata>();
        meta.name = "Cube";
        Material &material = entity.attach<Material>();
        Mesh &mesh = entity.attach<Mesh>();
        mesh.mesh = pde_renderer_get_cube_mesh();
    }
};

struct CreateSphereEntityAction : public SceneGraphAction
{
    inline virtual void action() override
    {
        Entity entity = EntityRegistry::CreateEntity();
        Metadata &meta = entity.get<Metadata>();
        meta.name = "Sphere";
        Material &material = entity.attach<Material>();
        Mesh &mesh = entity.attach<Mesh>();
        mesh.mesh = pde_renderer_get_sphere_mesh();
    }
};

struct CreateQuadEntityAction : public SceneGraphAction
{

    inline virtual void action() override
    {
        Entity entity = EntityRegistry::CreateEntity();
        Metadata &meta = entity.get<Metadata>();
        meta.name = "Quad";
        Material &material = entity.attach<Material>();
        Mesh &mesh = entity.attach<Mesh>();
        mesh.mesh = pde_renderer_get_quad_mesh();
    }

};

struct CreateChildAction : public SceneGraphAction
{

    Entity parent;
    inline virtual void action() override
    {
        Entity child = EntityRegistry::CreateEntity();
        parent.add_child(child);
    };

};

class SceneGraphEditor : public EditorInterface
{

    public:
        inline SceneGraphEditor() : EditorInterface("Scene Graph") {};
        inline virtual void display_contents() override;
        inline virtual void display_menubar() override;
        inline virtual void update() override;

        inline Entity get_viewed_entity() const { return viewing; }

    private:
        inline void display_entity_edit(Entity entity);
        inline void display_entity(Entity entity);

    private:
        std::queue<std::shared_ptr<SceneGraphAction>> queued_actions;
        Entity viewing = { };
};

void SceneGraphEditor::
update()
{

    while (!queued_actions.empty())
    {
        auto current = queued_actions.front();
        queued_actions.pop();
        current->action();
    }

    if (!EntityRegistry::CheckEntity(viewing)) viewing = {};

}

void SceneGraphEditor::
display_entity(Entity entity)
{

    uint32_t flags = ImGuiTreeNodeFlags_Framed | 
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_DrawLinesToNodes;

    Metadata &metadata = entity.get<Metadata>();
    if (!metadata.children.size()) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    
    ImGui::PushID(entity.index);
    if (ImGui::TreeNodeEx(metadata.name.c_str(), flags))
    {

        if (ImGui::IsItemClicked()) viewing = entity;
        display_entity_edit(entity);

        for (auto child : metadata.children) display_entity(child);

        ImGui::TreePop();

    }
    ImGui::PopID();

}

void SceneGraphEditor::
display_contents()
{

    auto scene_entities = EntityRegistry::GetEntities(true);
    for (auto entity : scene_entities) display_entity(entity);

}

void SceneGraphEditor::
display_menubar()
{

    if (ImGui::MenuItem("Create")) ImGui::OpenPopup("entity_add");

    if (ImGui::BeginPopup("entity_add"))
    {

        ImGui::SeparatorText("Common");
        if (ImGui::MenuItem("Base Entity"))
        {
            auto action = std::make_shared<CreateEntityAction>();
            queued_actions.push(action);   
        }

        ImGui::SeparatorText("Primitive Shapes");
        if (ImGui::MenuItem("Cube Primitive"))
        {
            auto action = std::make_shared<CreateCubeEntityAction>();
            queued_actions.push(action);
        }

        if (ImGui::MenuItem("Sphere Primitive"))
        {
            auto action = std::make_shared<CreateSphereEntityAction>();
            queued_actions.push(action);
        }

        if (ImGui::MenuItem("Quad Primitive"))
        {
            auto action = std::make_shared<CreateQuadEntityAction>();
            queued_actions.push(action);
        }

        ImGui::SeparatorText("Lights");
        if (ImGui::MenuItem("Directional Light"))
        {
            auto action = std::make_shared<CreateDirectionLightAction>();
            queued_actions.push(action);
            pde_logging_engine_info("Note to developer: only one direction light may be active at a time.");
        }
        if (ImGui::MenuItem("Point Light"))
        {
            auto action = std::make_shared<CreatePointLightAction>();
            queued_actions.push(action);
        }

        ImGui::EndPopup();
    }

}

void SceneGraphEditor::
display_entity_edit(Entity entity)
{

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup("entity_edit");
    if (ImGui::BeginPopup("entity_edit"))
    {

        ImGui::SeparatorText("Edit");

        if (ImGui::Button("Create Child"))
        {

            auto action = std::make_shared<CreateChildAction>();
            action->parent = entity;
            queued_actions.push(action);

        }

        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            auto action = std::make_shared<RemoveEntityAction>();
            action->entity = entity;
            queued_actions.push(action);
        }

        ImGui::SeparatorText("Add/Remove Components");

        const auto names = EntityRegistry::GetComponentNamesRef();

        {
            std::string selected = "";
            bool was_selected = false;

            if (ImGui::BeginCombo("Add Component", selected.c_str()))
            {
                for (auto name : names)
                {
                    if (ImGui::Selectable(name.c_str()))
                    {
                        selected = name;
                        was_selected = true;
                    }
                }
                ImGui::EndCombo();
            }

            if (was_selected == true) EntityRegistry::AttachComponentByName(entity, selected);
        }

        {
            std::string selected = "";
            bool was_selected = false;

            if (ImGui::BeginCombo("Remove Component", selected.c_str()))
            {
                for (auto name : names)
                {
                    if (!EntityRegistry::HasComponentByName(entity, name)) continue;
                    if (ImGui::Selectable(name.c_str()))
                    {
                        selected = name;
                        was_selected = true;
                    }
                }
                ImGui::EndCombo();
            }

            if (was_selected == true) EntityRegistry::RemoveComponentByName(entity, selected);
        }

        ImGui::EndPopup();

    }

}

