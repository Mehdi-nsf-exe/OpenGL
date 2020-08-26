#pragma once

#include <string>
#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"

struct Vertex {
   glm::vec3 Position;
   glm::vec3 Normal;
   glm::vec2 TexCoords;
};

struct Texture {
   unsigned int Id;
   std::string type;
   std::string path;


   /**
      Exception that indicates an error occured in texture loading.
    */
   class TextureLoadingFailure : public std::exception {
   public:
      explicit TextureLoadingFailure() {}
   };

   /**
      Loads a texture form the file with the indicated path.
   */
   Texture(const std::string& path, const std::string& directory, const std::string& typeName);
};

class Mesh {
private:

   std::vector<Vertex> Vertices;
   std::vector<unsigned int> Indices;
   std::vector<Texture> Textures;
   unsigned int VAO;
   unsigned int VBO;
   unsigned int EBO;

public:

   /**
      Creates and initializes a mesh with all the OpenGL Configurations.
    */
   Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

   /**
      Draws a mesh using the indicated shader.
    */
   void draw(Shader& shader);
   
private:

   /**
      Sets up all the OpenGL configuration of the mesh.
    */
   void setupMesh();
};