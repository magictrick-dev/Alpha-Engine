#pragma once
#include "assimp/material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <platform/registry.hpp>
#include <platform/filesystem.hpp>
#include <platform/renderer/mesh.hpp>
#include <platform/renderer/texture.hpp>

#include <filesystem>
#include <print>

struct Submodel
{
    std::vector<texture_t> diffuse_textures;
    std::vector<texture_t> specular_textures;
    std::vector<texture_t> normal_textures;
    std::vector<texture_t> height_maps;
    std::string mesh_name;
    mesh_t mesh;
};

struct Model
{
    std::string location;
    std::vector<Submodel> submodels;
    std::vector<Model> children;
};

inline std::vector<texture_t>
load_material_textures(Model *model, aiMaterial *material, aiTextureType type)
{

    std::vector<texture_t> textures;
    
    uint32_t texture_count = material->GetTextureCount(type);
    for (uint32_t index = 0; index < texture_count; ++index)
    {

        aiString asset_string;
        material->GetTexture(type, index, &asset_string);

        std::string location = model->location + asset_string.C_Str();
        texture_t loaded_texture = pde_renderer_create_texture(location);
        if (loaded_texture != PDE_INVALID_TEXTURE)
        {
            textures.push_back(loaded_texture);
        }

        else
        {
            std::println("Failed to loaded texture at {} from asset path {}",
                    location, asset_string.C_Str());
            textures.push_back(pde_renderer_get_default_texture());
        }

    }

    // Backup textures.
    if (textures.empty()) 
    {

        texture_t fill_texture = pde_renderer_get_default_texture();

        if (type != aiTextureType_DIFFUSE) 
        {
            fill_texture = pde_renderer_get_default_void_texture();
        }

        textures.push_back(fill_texture);

    }

    return textures;

}

inline void
load_submodels(Model *model, const aiScene *scene, aiNode *node)
{

    uint32_t mesh_count = node->mNumMeshes;

    for (uint32_t mesh_index = 0; mesh_index < mesh_count; ++mesh_index)
    {

        aiMesh *mesh = scene->mMeshes[node->mMeshes[mesh_index]];

        MeshSpecification mesh_specification = {};

        uint32_t vertex_count = mesh->mNumVertices;
        for (uint32_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
        {

            MeshVertex current = {};

            if (mesh->HasPositions())
            {
                current.position.x = mesh->mVertices[vertex_index].x;
                current.position.y = mesh->mVertices[vertex_index].y;
                current.position.z = mesh->mVertices[vertex_index].z;
            }

            if (mesh->HasNormals())
            {
                current.normal.x = mesh->mNormals[vertex_index].x;
                current.normal.y = mesh->mNormals[vertex_index].y;
                current.normal.z = mesh->mNormals[vertex_index].z;
            }

            if (mesh->mTextureCoords[0])
            {
                current.texture_coordinates.x = mesh->mTextureCoords[0][vertex_index].x;
                current.texture_coordinates.y = mesh->mTextureCoords[0][vertex_index].y;
            }
            else
            {
                current.texture_coordinates = {};
            }

            mesh_specification.vertices.push_back(current);

        }

        uint32_t face_count = mesh->mNumFaces;
        for (uint32_t index = 0; index < face_count; ++index)
        {

            aiFace &face = mesh->mFaces[index];
            for (uint32_t i = 0; i < face.mNumIndices; ++i) 
                mesh_specification.indices.push_back(face.mIndices[i]);

        }

        mesh_t model_mesh = pde_renderer_create_mesh(&mesh_specification);

        Submodel submodel = {};
        submodel.mesh = model_mesh;
        submodel.mesh_name = mesh->mName.C_Str();

        uint32_t material_index = mesh->mMaterialIndex;
        if (material_index >= 0)
        {

            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            submodel.diffuse_textures = load_material_textures(model, material, aiTextureType_DIFFUSE);
            submodel.specular_textures = load_material_textures(model, material, aiTextureType_SPECULAR);
            submodel.normal_textures = load_material_textures(model, material, aiTextureType_HEIGHT);
            submodel.height_maps = load_material_textures(model, material, aiTextureType_AMBIENT);

        }

        model->submodels.push_back(submodel);

    }

    uint32_t children = node->mNumChildren;
    for (uint32_t i = 0; i < children; ++i) 
    {

        Model child_model = {}; // Relax, it's just a computer science term.
        child_model.location = model->location;
        load_submodels(&child_model, scene, node->mChildren[i]);
        model->children.push_back(child_model);

    }

}

inline bool
load_model(Model *root_model, std::string path)
{

    std::string location = pde_filesystem_resolve(path);
    if (location == "") return false;
    if (root_model == NULL) return false;

    root_model->location = std::filesystem::path(location).parent_path().string() + "\\";

    Assimp::Importer importer;
    stbi_set_flip_vertically_on_load(true);
    const aiScene *scene = importer.ReadFile(location, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) return false;
    stbi_set_flip_vertically_on_load(false);

    load_submodels(root_model, scene, scene->mRootNode);

    return true;

}

