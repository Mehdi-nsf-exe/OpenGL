
#include "Model.h"

std::vector<Texture> Model::loadedTextures;

void Model::draw(Shader& shader) {
   for (unsigned int i = 0; i < Meshes.size(); i++) {
      Meshes[i].draw(shader);
   }
}

void Model::loadModel(const std::string& path) {
   Assimp::Importer importer;
   const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
   }
   Directory = path.substr(0, path.find_last_of('/'));
   processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

   for (int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      Meshes.push_back(processMesh(mesh, scene));
   }

   for (int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
   }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;
   std::vector<Texture> textures;

   for (int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      vertex.Position.x = mesh->mVertices[i].x;
      vertex.Position.y = mesh->mVertices[i].y;
      vertex.Position.z = mesh->mVertices[i].z;
      vertex.Normal.x = mesh->mNormals[i].x;
      vertex.Normal.y = mesh->mNormals[i].y;
      vertex.Normal.z = mesh->mNormals[i].z;
      if (mesh->mTextureCoords[0]) {
         vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
         vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
      }
      else {
         vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      }
      vertices.push_back(vertex);
   }

   for (int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (int j = 0; j < face.mNumIndices; j++) {
         indices.push_back(face.mIndices[j]);
      }
   }

   if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

      std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
   }

   return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {

   std::vector<Texture> textures;
   
   for (int i = 0; i < mat->GetTextureCount(type); i++) {
      aiString str; 
      mat->GetTexture(type, i, &str);
      bool skip = false;
      for (int j = 0; j < loadedTextures.size(); j++) {
         if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0) {
            textures.push_back(loadedTextures[j]);
            skip = true;
            break;
         }
      }
      if (!skip) {
         Texture texture(str.C_Str(), Directory, typeName);
         textures.push_back(texture);
         loadedTextures.push_back(texture);
      }
   }
   return textures;
}