#pragma once
#include <utilities/definitions.hpp>
#include <vector>
#include <unordered_map>

union Entity
{
    int64_t identifier;
    struct
    {
        int32_t index;
        int16_t generation;
        int16_t flags;
    };
};

class ComponentInterface
{
    public:
        virtual bool contains_entity(const Entity &entity) = 0;
};

template <typename T>
class Component : public ComponentInterface
{

    public:
        virtual bool 
        contains_entity(const Entity &entity) override
        {
            ALPHA_ASSERT_NO_IMPLEMENTATION();
            return false;
        }
        
    private:
        std::vector<T> data;
        std::unordered_map<uint64_t, Entity> dense_to_sparse;
        std::unordered_map<Entity, uint64_t> sparse_to_dense;

};

class Registry
{

    public:

    private:
        std::vector<Entity> entities_active;
        std::vector<Entity> entities_inactive;
        std::unordered_map<std::string_view, ComponentInterface*> component_map;

};

