#pragma once
#include <application/editor/editor_interface.hpp>
#include <platform/registry.hpp>

class InspectorEditor : public EditorInterface
{

    public:
        inline InspectorEditor() : EditorInterface("Inspector") { }

        inline void set_inspection(Entity entity) 
        { 
            inspecting = entity; 
        };
        inline void set_preview(Entity entity) { previewing = entity; };
        inline Entity get_inspection() const { return inspecting; };
        inline Entity get_preview() const { return previewing; };

    private:
        inline virtual void display_contents() override;
        inline virtual void display_menubar() override;

        Entity inspecting = { };
        Entity previewing = { };

};

void InspectorEditor::
display_contents()
{

    if (inspecting.index != 0 || previewing.tag != 0)
    {

        Entity displaying = inspecting;
        if (previewing.tag != 0) displaying = previewing;

        displaying.get<Metadata>().display();
        displaying.get<Transform>().display();

        auto components = EntityRegistry::GetComponentsExcept<Transform, Metadata>(displaying);
        for (auto component : components) 
        {
            component->display();
        }

    }

}

void InspectorEditor::
display_menubar()
{

}

