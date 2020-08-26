#pragma once

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model {
private:

   static std::vector<Texture> loadedTextures;
   std::vector<Mesh> Meshes;
   std::string Directory;

public:

   /**
      Loads a model form the indicated file path.
    */
   inline Model(const std::string& path) {
      loadModel(path);
   }

   /**
      Draws the model using the shader pased as a parameter.
    */
   void draw(Shader& shader);

private:

   /**
      Loads the model from the indicated file path.
    */
   void loadModel(const std::string& path);

   /**
      Recursively processes the indicated node.
    */
   void processNode(aiNode* node, const aiScene* scene);

   /**
      Processes the Assimp mesh.
    */
   Mesh processMesh(aiMesh* mesh, const aiScene* scene);

   /**
      Loads the textures from the Assimp data structures.
    */
   std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};