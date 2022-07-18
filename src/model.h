#ifndef MODEL_H_
#define MODEL_H_
#include "mesh.h"
#include "material.h"

#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sge
{

class Model
{
public:
	Model(const std::string &path, bool flip_uv);
    const std::unordered_map<std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>>& get_material_to_mesh();
    const std::vector<std::shared_ptr<Mesh>>& get_meshes();
private:
    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<std::string> getMaterialTextures(aiMaterial *mat, aiTextureType type);
private:
    std::string m_path;
    std::string m_directory;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;
    std::unordered_map<std::shared_ptr<Material>, std::vector<std::shared_ptr<Mesh>>> m_material_to_meshes;
};

};
#endif
