#include <glad/glad.h>
#define	STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Mesh.h"
#include "OpenGLErrorHandling.h"

static bool printed = false;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) :
   Vertices(vertices),
   Indices(indices),
   Textures(textures) 
   {
   setupMesh();
}

void Mesh::draw(Shader& shader) {
   
   unsigned int diffuseNum = 1;
   unsigned int specularNum = 1;
   for (unsigned int i = 0; i < Textures.size(); i++) {
      GLCall(glActiveTexture(GL_TEXTURE0 + i));
      std::string number;
      std::string name = Textures[i].type;
      if (name == "texture_diffuse") {
         number = std::to_string(diffuseNum++);
      }
      if (name == "texture_specular") {
         number = std::to_string(specularNum++);
      }
      try {
         shader.setUniform(/*"material." +*/ name + number, i);
      }
      catch (const Shader::InvalidUniformLocation & e) {
         if (!printed) {
            std::cout << "Invalid uniform location: " << name + number << std::endl;
            printed = true;
         }
      }
      GLCall(glBindTexture(GL_TEXTURE_2D, Textures[i].Id));
   }
   GLCall(glActiveTexture(GL_TEXTURE0));

   // draw mesh
   GLCall(glBindVertexArray(VAO));
   GLCall(glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0));
   GLCall(glBindVertexArray(VAO));
}

void Mesh::setupMesh() {
   GLCall(glGenVertexArrays(1, &VAO));
   GLCall(glGenBuffers(1, &VBO));
   GLCall(glGenBuffers(1, &EBO));

   GLCall(glBindVertexArray(VAO));

   GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
   GLCall(glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW));
   
   GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
   GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW));

   // Vertex positions
   GLCall(glEnableVertexAttribArray(0));
   GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position)));
   //Vertex normals
   GLCall(glEnableVertexAttribArray(1));
   GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal)));
   // Vertex texture coords
   GLCall(glEnableVertexAttribArray(2));
   GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords)));

   GLCall(glBindVertexArray(0));
}

Texture::Texture(const std::string& path, const std::string& directory, const std::string& typeName) 
   :
   path(path),
   type(typeName)
   {

   std::string fileName = directory + '/' + path;

   stbi_set_flip_vertically_on_load(true);

   int width, height, nrComponents;
   unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
   if (data == nullptr) {
      throw TextureLoadingFailure();
   }

   GLenum format;
   switch (nrComponents) {
      case 1: {
         format = GL_RED;
         break;
      }
      case 3: {
         format = GL_RGB;
         break;
      }
      case 4: {
         format = GL_RGBA;
         break;
      }
      default: {
         ASSERT(false);
      }
   }

   GLCall(glGenTextures(1, &Id));
   GLCall(glBindTexture(GL_TEXTURE_2D, Id));
   GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
   GLCall(glGenerateMipmap(GL_TEXTURE_2D));

   GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
   GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
   GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

   stbi_image_free(data);
   data = nullptr;
}