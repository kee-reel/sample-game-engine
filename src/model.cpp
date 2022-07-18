#include "model.h"

#include "resource_loader.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace {
};

namespace sge
{

Model::Model(const std::string &path, bool flip_uv) :
    m_path{path}
{
    Assimp::Importer importer;
    auto flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;
    if(flip_uv)
    {
        flags = flags | aiProcess_FlipUVs;
    }
    const aiScene *scene = importer.ReadFile(m_path, flags);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        throw std::runtime_error(importer.GetErrorString());

    auto start_i = m_path.find_first_of('/')+1;
    m_directory = m_path.substr(start_i, m_path.find_last_of('/')+1-start_i);
    m_meshes.reserve(scene->mNumMeshes);
    m_materials.resize(scene->mNumMaterials);
    for(size_t i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial *material = scene->mMaterials[i];
        auto td = getMaterialTextures(material, aiTextureType_DIFFUSE);
        auto ts = getMaterialTextures(material, aiTextureType_SPECULAR);
        auto te = getMaterialTextures(material, aiTextureType_EMISSIVE);
        if(td.size())
        {
            json config;

            auto &textures = config["texture"];
            textures["material.diffuse"] = td[0];
            if(ts.size())
                textures["material.specular"] = ts[0];
            if(te.size())
                textures["material.emission"] = te[0];

            auto &shaders = config["shader"];
            shaders["vert"] = "shaders/Basic.vert";
            shaders["frag"] = "shaders/Basic.frag";

            m_materials[i] = std::make_shared<sge::Material>(std::move(config));
        }
    }
    processNode(scene->mRootNode, scene);
}

const std::unordered_map<std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>>& Model::get_material_to_mesh()
{
    return m_material_to_meshes;
}

const std::vector<std::shared_ptr<Mesh>>& Model::get_meshes()
{
    return m_meshes;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        if(mesh->mNumVertices)
            processMesh(mesh, scene);
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::vector<std::string> Model::getMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    std::vector<std::string> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString path;
        mat->GetTexture(type, i, &path);
        std::cout << m_directory << ' ' << path.C_Str() << std::endl;
        textures.push_back(m_directory + path.C_Str());
    }
    return textures;
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<sge::Mesh::Vertex> vertices(mesh->mNumVertices);
    std::vector<sge::Mesh::Index> indices(mesh->mNumFaces * 3);
    for(size_t i = 0; i < mesh->mNumVertices; i++)
    {
        vertices[i] = { 
            glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
            glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
            (mesh->mTextureCoords[0] ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} : glm::vec2{0, 0})
        };
    }
    for(size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices[i] = {face.mIndices[0], face.mIndices[1], face.mIndices[2]};
    }
    auto new_mesh = std::make_shared<sge::Mesh>(std::move(vertices), std::move(indices));
    m_meshes.push_back(new_mesh);

    if(mesh->mMaterialIndex >= 0)
    {
        auto material = m_materials[mesh->mMaterialIndex];
        if(material)
            m_material_to_meshes[material].push_back(new_mesh);
    }
}

};
