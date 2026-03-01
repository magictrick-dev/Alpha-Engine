#pragma once
#include <platform/utilities.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <print>
#include <cassert>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/implot.h>

struct Entity
{
  
    union
    {
        uint64_t tag;
        struct
        {
            uint32_t index;
            uint16_t generation;
            bool active;
        };
    };

    inline bool operator==(const Entity &other) const { return (tag == other.tag); }
    inline bool operator!=(const Entity &other) const { return (tag != other.tag); }

    inline void kill();
    inline bool valid();
    inline void set_parent(Entity parent);
    inline void add_child(Entity child);
    template <typename T, typename... Args> T& attach(Args... args);
    template <typename T> T& remove();
    template <typename T> T& get();
    template <typename... Ts> bool has();

};

class ComponentInterface
{

    public:
        inline virtual void display() = 0;
        inline virtual void update(float delta_time) { }
        Entity self;

};

#include <platform/components.hpp>

class SparseSetInterface
{

    public:
        virtual bool contains(uint32_t sparse_index) const = 0;
        virtual void insert(uint32_t sparse_index) = 0;
        virtual void remove(uint32_t sparse_index) = 0;

        virtual size_t size() = 0;
        virtual bool empty() = 0;
        virtual void clear() = 0;
        virtual uint32_t at(uint32_t dense_index) = 0;
        virtual uint32_t sparse_to_dense(uint32_t sparse_index) = 0;
        virtual typename std::vector<uint32_t>::iterator begin() = 0;
        virtual typename std::vector<uint32_t>::iterator end() = 0;
        virtual typename std::vector<uint32_t>::const_iterator begin() const = 0;
        virtual typename std::vector<uint32_t>::const_iterator end() const = 0;
        virtual ComponentInterface* get_component_interface(uint32_t sparse_index) = 0;

};

template <typename T>
class SparseSet : public SparseSetInterface
{

    static_assert(std::is_base_of<ComponentInterface, T>::value, "Must be derived ComponentInterface");

    public:
        template <typename... Args> inline void 
        emplace(uint32_t sparse_index, Args... args)
        {
            if (contains(sparse_index))
            {
                const uint32_t dense_index = sparse_map[sparse_index];
                data[dense_index] = T { args... };
            }
            else
            {
                const uint32_t dense_index = data.size();
                data.emplace_back(T { args... });
                sparse_map[sparse_index] = dense_index;
                dense_map.push_back(sparse_index);
            }
        }

        virtual void 
        insert(uint32_t sparse_index) override
        {

            if (contains(sparse_index))
            {

                const uint32_t dense_index = sparse_map[sparse_index];
                data[dense_index] = T { };

            }
            else
            {

                const uint32_t dense_index = data.size();
                data.emplace_back(T { });
                sparse_map[sparse_index] = dense_index;
                dense_map.push_back(sparse_index);

            }

        }

        virtual inline bool 
        contains(uint32_t sparse_index) const override
        {

            return sparse_map.find(sparse_index) != sparse_map.end();

        }

        virtual inline void
        remove(uint32_t sparse_index) override
        {

            if (data.size() == 0) return;
            if (!contains(sparse_index)) return;

            const uint32_t dense_index = sparse_map[sparse_index];
            const uint32_t tail_sparse_index = dense_map.back();
            sparse_map.erase(sparse_index);

            std::swap(data[dense_index], data.back());
            std::swap(dense_map[dense_index], dense_map.back());
            data.pop_back();
            dense_map.pop_back();

            if (dense_index != data.size()) sparse_map[tail_sparse_index] = dense_index;

        }

        inline T* 
        get(uint32_t sparse_index)
        {

            if (contains(sparse_index))
            {
                const uint32_t dense_index = sparse_map[sparse_index];
                return &data[dense_index];
            }

            return nullptr;

        }

        virtual inline void 
        clear() override
        {

            data.clear();
            dense_map.clear();
            sparse_map.clear();

        }

        virtual inline ComponentInterface*
        get_component_interface(uint32_t sparse_index) override
        {

            static_assert(std::is_base_of<ComponentInterface, T>::value, "");

            if (sparse_map.find(sparse_index) == sparse_map.end()) return NULL;
            uint32_t dense = sparse_map[sparse_index];

            T* entry = &data[dense];
            ComponentInterface *result= dynamic_cast<ComponentInterface*>(entry);
            return result;

        }

        inline std::vector<T> get_data() { return this->data; }
        inline const std::vector<T> get_data() const { return this->data; }

        virtual uint32_t sparse_to_dense(uint32_t sparse_index) override { return this->sparse_map[sparse_index]; }
        virtual uint32_t at(uint32_t dense_index) override { return this->dense_map[dense_index]; }
        virtual inline bool empty() override { return data.size() == 0; }
        virtual inline size_t size() override { return this->dense_map.size(); }
        virtual inline typename std::vector<uint32_t>::iterator begin() override { return this->dense_map.begin(); }
        virtual inline typename std::vector<uint32_t>::iterator end() override { return this->dense_map.end(); }
        virtual inline typename std::vector<uint32_t>::const_iterator begin() const override { return this->dense_map.begin(); }
        virtual inline typename std::vector<uint32_t>::const_iterator end() const override { return this->dense_map.end(); }

    private:
        std::vector<T> data;
        std::vector<uint32_t> dense_map;
        std::unordered_map<uint32_t, uint32_t> sparse_map;

};

class EntityRegistry
{

    public:
        static inline Entity 
        CreateEntity()
        {

            if (!initialized) Initialize();

            if (inactives.size())
            {

                const uint32_t index = inactives.front();
                inactives.pop();

                Entity &entity = entities[index];
                entity.active = true;

                entity.attach<Metadata>();
                entity.attach<Transform>();
                return entity;

            }

            else
            {

                const uint32_t index = entities.size();

                Entity entity = {};
                entity.index = index;
                entity.generation = 0;
                entity.active = true;

                entities.push_back(entity);

                entity.attach<Metadata>();
                entity.attach<Transform>();
                return entity;

            }
        }

        static inline bool 
        DestroyEntity(Entity entity)
        {

            const uint32_t index = entity.index;
            if (entities[index] != entity) return false;

            // If it has a parent, remove it from the parent.
            Metadata &meta = entity.get<Metadata>();
            if (meta.parent.tag != 0)
            {
                Metadata &parent_meta = meta.parent.get<Metadata>();
                uint32_t remove_index = 0;
                for (size_t i = 0; i < parent_meta.children.size(); ++i)
                {
                    if (parent_meta.children[i].index == entity.index)
                    {
                        remove_index = i;
                        break;
                    }
                }
                parent_meta.children.erase(parent_meta.children.begin() + remove_index);
            }

            // NOTE(Chris): We need to copy the array since a call to destroy entity
            //              modifies the internal list.
            std::vector<Entity> children = meta.children;
            for (size_t i = 0; i < children.size(); ++i) DestroyEntity(children[i]);
            assert(meta.children.size() == 0);

            // Clear previous components and reset.
            entities[index].generation++;
            entities[index].active = false;
            for (auto component : components)
            {
                if (component.second->contains(entity.index))
                    component.second->remove(entity.index);
            }

            inactives.push(index);
            return true;

        }

        static inline void
        SetParentRelationship(Entity parent, Entity child)
        {

            if (parent.index == child.index)
            {
                std::println("Entity registry attempted to set a parent relationship recursively. Ignored request.");
                return;
            }

            Metadata &parent_metadata = parent.get<Metadata>();
            Metadata &child_metadata = child.get<Metadata>();

            parent_metadata.children.push_back(child);
            child_metadata.parent = parent;
            return;

        }

        static inline bool
        CheckEntity(Entity entity)
        {

            const uint32_t index = entity.index;
            const bool result = (entities[index] == entity && entities[index].active);
            return result;
            
        }

        template <typename T, typename... Args> static inline bool
        AttachComponent(Entity entity, Args... args)
        {

            static_assert(std::is_base_of<ComponentInterface, T>::value, "");

            if (!CheckEntity(entity)) return false;
            const uint32_t index = entity.index;

            GenerateComponentSet<T>();

            constexpr size_t component_hash = TypeID::Hash<T>();
            SparseSet<T> *set = dynamic_cast<SparseSet<T>*>(components[component_hash].get());
            assert(set != NULL);

            T component = T { args... };
            component.self = entity;

            set->emplace(index, std::move(component));
            return true;

        }

        template <typename T> static inline bool
        HasComponent(Entity entity)
        {

            constexpr size_t component_hash = TypeID::Hash<T>();
            SparseSet<T> *set = dynamic_cast<SparseSet<T>*>(components[component_hash].get());
            assert(set != NULL);

            const uint32_t index = entity.index;
            const bool result = (set->contains(index));
            return result;

        }

        template <typename... Ts> static inline bool
        HasComponents(Entity entity)
        {

            constexpr std::array<size_t, sizeof...(Ts)> types = TypeID::HashList<Ts...>();

            bool valid = true;
            for (auto hash : types)
            {
                if (components.find(hash) != components.end())
                {
                    if (!components[hash]->contains(entity.index))
                    {
                        valid = false;
                        break;
                    }
                }
            }

            return valid;

        }

        template <typename T> static inline T&
        GetComponent(Entity entity)
        {

            constexpr size_t component_hash = TypeID::Hash<T>();
            SparseSet<T> *set = dynamic_cast<SparseSet<T>*>(components[component_hash].get());
            assert(set != NULL);

            if (!CheckEntity(entity)|| !HasComponent<T>(entity))
            {
                std::println("Entity Registry Warning: Fetching component from an invalid entity.");
                return *set->get(blank_entity.index);   
            }

            const uint32_t index = entity.index;
            return *set->get(index);
            
        }

        template <typename T> static inline void
        RemoveComponent(Entity entity)
        {

            if (!CheckEntity(entity)) return;
            const uint32_t index = entity.index;
            
            constexpr size_t component_hash = TypeID::Hash<T>();

            SparseSet<T>* set = dynamic_cast<SparseSet<T>*>(components[component_hash].get());
            assert(set != NULL);

            set->remove(index);

        }

        template <bool top_level, typename... Ts> static inline std::vector<Entity> 
        GetEntitiesWith(bool respect_ignore = true)
        {

            constexpr std::array<size_t, sizeof...(Ts)> types = TypeID::HashList<Ts...>();
            if constexpr (types.size() == 0) return {};

            for (auto type : types)
            {
                if (components.find(type) == components.end()) return {};
            }

            if constexpr (types.size() == 1)
            {

                constexpr size_t component_hash = types[0];
                std::vector<Entity> result;

                const size_t count = components[component_hash]->size();
                for (size_t i = 0; i < count; ++i)
                {

                    std::shared_ptr<SparseSetInterface> set = components[component_hash];
                    const uint32_t entity_index = set->at(i);

                    // Top level entities ensure that we only ever filter entities that
                    // don't have parents.
                    if constexpr (top_level == true)
                    {
                        if (entities[entity_index].get<Metadata>().parent.tag != 0) continue;
                    }

                    if (respect_ignore && entities[entity_index].get<Metadata>().ignore == true) continue;

                    if (entities[entity_index].active && entity_index != blank_entity.index)
                    {
                        result.push_back(entities[entity_index]);
                    }

                }

                return result;

            }

            else
            {

                size_t minimum_hash_set = types[0];
                for (size_t i = 1; i < types.size(); ++i)
                {
                    const size_t current = components[minimum_hash_set]->size();
                    const size_t check = components[types[i]]->size();
                    if (check < current) minimum_hash_set = types[i];
                }

                std::shared_ptr<SparseSetInterface> minimum_set = components[minimum_hash_set];
                std::vector<uint32_t> minimum_indices(minimum_set->begin(), minimum_set->end());

                std::vector<Entity> result;

                for (auto index : minimum_indices)
                {

                    // Top level entities ensure that we only ever filter entities that
                    // don't have parents.
                    if constexpr (top_level == true)
                    {
                        if (entities[index].get<Metadata>().parent.tag != 0) continue;
                    }

                    if (respect_ignore && entities[index].get<Metadata>().ignore == true) continue;

                    bool valid = true;
                    for (size_t i = 0; i < types.size(); ++i)
                    {

                        size_t current_type = types[i];
                        if (!components[current_type]->contains(index))
                        {
                            valid = false;
                            break;
                        }

                    }

                    if (valid == false || blank_entity.index == index) continue;
                    result.push_back(entities[index]);

                }

                return result;

            }

        }

        static inline std::vector<Entity> 
        GetEntities(bool top_level = false, bool respect_ignore = true)
        {

            std::vector<Entity> result;
            for (auto entity : entities) 
            {
                if (entity != blank_entity && entity.active && (respect_ignore && !entity.get<Metadata>().ignore))
                {
                    if (top_level && entities[entity.index].get<Metadata>().parent.tag != 0) continue;
                    result.push_back(entity);
                }
            }
            return result;

        }

        static inline std::vector<ComponentInterface*>
        GetComponents(Entity entity)
        {
            if (!CheckEntity(entity)) return {};

            std::vector<ComponentInterface*> interfaces;
            for (auto component_set : components)
            {
                if (component_set.second->contains(entity.index)) 
                    interfaces.push_back(component_set.second->get_component_interface(entity.index));
            }

            return interfaces;

        }

        template <typename... Ts> static inline std::vector<ComponentInterface*>
        GetComponentsExcept(Entity entity)
        {
            if (!CheckEntity(entity)) return {};

            constexpr std::array<size_t, sizeof...(Ts)> types = TypeID::HashList<Ts...>();
            std::unordered_set<size_t> hashes(types.begin(), types.end());

            std::vector<ComponentInterface*> interfaces;
            for (auto component_set : components)
            {
                if (hashes.find(component_set.first) != hashes.end()) continue;
                if (component_set.second->contains(entity.index)) 
                    interfaces.push_back(component_set.second->get_component_interface(entity.index));
            }

            return interfaces;

        }

        static inline std::vector<std::string>
        GetComponentNames()
        {
            return component_names;
        }

        static inline const std::vector<std::string>&
        GetComponentNamesRef()
        {
            return component_names;
        }

        static inline bool
        ComponentExistsByName(std::string name)
        {

            const bool result = (component_types.find(name) != component_types.end());
            return result;

        }

        static inline void
        AttachComponentByName(Entity entity, std::string name)
        {

            if (!ComponentExistsByName(name))
            {
                std::println("Entity registry failed to find a componeny by name: {}", name);
                return;
            }

            if (!CheckEntity(entity)) return;
            components[component_types[name]]->insert(entity.index);

        }

        static inline void
        RemoveComponentByName(Entity entity, std::string name)
        {

            if (!ComponentExistsByName(name))
            {
                std::println("Entity registry failed to find a componeny by name: {}", name);
                return;
            }

            if (!CheckEntity(entity)) return;
            components[component_types[name]]->remove(entity.index);

        }

        template <typename T> static inline void
        RegisterComponent()
        {

            if (!initialized) Initialize();

            constexpr std::string_view component_name = TypeID::Name<T>();
            constexpr size_t component_hash = TypeID::Hash<T>();
            if (components.find(component_hash) != components.end()) return;

            std::string name(component_name.begin(), component_name.end());
            component_types[name] = component_hash;
            component_names.emplace_back(name);
            components[component_hash] = std::make_shared<SparseSet<T>>();

            blank_entity.attach<T>();

        }

        static inline bool
        HasComponentByName(Entity entity, std::string name)
        {

            if (component_types.find(name) == component_types.end()) return false;

            size_t hash = component_types[name];

            const bool result = (components[hash]->contains(entity.index));
            return result;


        }
        
        static inline void
        Initialize()
        {

            if (initialized == false)
            {
                
                const uint32_t index = entities.size();

                Entity entity = {};
                entity.index = index;
                entity.generation = 0;
                entity.active = true;

                entities.push_back(entity);
                blank_entity = entity;
                initialized = true;

            }

        }

    private:
        template <typename T> static inline void
        GenerateComponentSet()
        {

            if (!initialized) Initialize();

            constexpr std::string_view component_name = TypeID::Name<T>();
            constexpr size_t component_hash = TypeID::Hash<T>();
            if (components.find(component_hash) != components.end()) return;

            std::string name(component_name.begin(), component_name.end());
            component_types[name] = component_hash;
            component_names.emplace_back(name);
            components[component_hash] = std::make_shared<SparseSet<T>>();

            AttachComponent<T>(blank_entity);

        }

    public:
        static inline std::unordered_map<size_t, std::shared_ptr<SparseSetInterface>> components;
        static inline std::unordered_map<std::string, size_t> component_types;
        static inline std::vector<std::string> component_names;
        static inline std::vector<Entity> entities;
        static inline std::queue<uint32_t> inactives;

        static inline bool initialized = false;
        static inline Entity blank_entity = {};

};

inline void Entity::
kill()
{
    EntityRegistry::DestroyEntity(*this);
}

inline bool Entity::
valid()
{
    return EntityRegistry::CheckEntity(*this);
}

template <typename T, typename... Args> T& Entity::
attach(Args... args)
{
    EntityRegistry::AttachComponent<T>(*this, args...);
    return EntityRegistry::GetComponent<T>(*this);
}

template <typename T> T& Entity::
remove()
{
    EntityRegistry::RemoveComponent<T>(*this);
}

template <typename T> T& Entity::
get()
{
    return EntityRegistry::GetComponent<T>(*this);
}

template <typename... Ts> bool Entity::
has()
{
    return EntityRegistry::HasComponents<Ts...>(*this);
}

void Entity::
set_parent(Entity parent)
{
    EntityRegistry::SetParentRelationship(parent, *this);
}

void Entity::
add_child(Entity child)
{
    EntityRegistry::SetParentRelationship(*this, child);
}

