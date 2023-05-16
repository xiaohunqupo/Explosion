//
// Created by Junkang on 2023/3/9.
//

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <Common/Memory.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct TextureData {
public:
    uint32_t width { 0 };
    uint32_t height { 0 };
    uint8_t  component { 0 };
    std::vector<unsigned char> buffer {};

    bool IsValid() const { return buffer.size() == (width * height * component); }

    uint32_t GetSize() {
        return buffer.size();
    }
};

struct MaterialData
{
    Common::UniqueRef<TextureData> baseColorTexture = nullptr;
    Common::UniqueRef<TextureData> normalTexture = nullptr;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec4 color;
    glm::vec3 normal;
};

// In assimp, a mesh represents a geometry or model with a single material.
struct Mesh {
    std::string name;

    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t firstVertex;
    uint32_t vertexCount;

    Common::SharedRef<MaterialData> materialData;

    struct Dimensions {
        glm::vec3 min = glm::vec3(FLT_MAX);
        glm::vec3 max = glm::vec3(-FLT_MAX);
        glm::vec3 size;
        glm::vec3 center;
        float radius;
    } dimensions;

    void SetDimensions(glm::vec3 min, glm::vec3 max) {
        dimensions.min = min;
        dimensions.max = max;
        dimensions.size = max - min;
        dimensions.center = (min + max) / 2.0f;
        dimensions.radius = glm::distance(min, max) / 2.0f;
    }

    Mesh(uint32_t firstIndex, uint32_t indexCount, uint32_t firstVertex, uint32_t vertexCount, Common::SharedRef<MaterialData>& material)
        : firstIndex(firstIndex),
        indexCount(indexCount),
        firstVertex(firstVertex),
        vertexCount(vertexCount),
        materialData(material)
            {};

    // mustn`t delele material here, for different meshes can point to the same material
};

// Can`t use shared_ptr because of shared_ptr cycle
struct Node {
    Node* parent;
    std::vector<Node*> children;

    glm::mat4 matrix {1.0f };
    std::vector<Common::UniqueRef<Mesh>> meshes;

    glm::mat4 LocalMatrix();
    glm::mat4 GetMatrix();
    ~Node();
};


class Model {
public:
    Model() = default;
    ~Model();

    void LoadFromFile(const std::string& path);
    void LoadNode(const aiScene* scene, aiNode* node, Node* parent);
    void LoadMaterials(const aiScene* scene);

private:
    Common::UniqueRef<TextureData> LoadMaterialTexture(const aiScene* scene, const aiMaterial* mat, aiTextureType type, bool fromEmbedded);

    void CreateEmptyTexture()
    {
        emptyTexture->width = 1;
        emptyTexture->height = 1;
        emptyTexture->component = 4;
        emptyTexture->buffer = std::vector<unsigned char> { 0, 0, 0, 0 };
    }

public:
    Node* rootNode;
    std::vector<Node*> linearNodes;

    std::vector<Common::SharedRef<MaterialData>> materialDatas;

    std::vector<uint32_t> raw_index_buffer;
    std::vector<Vertex> raw_vertex_buffer;

    std::string directory;
private:

    Common::UniqueRef<TextureData> emptyTexture = nullptr;

};