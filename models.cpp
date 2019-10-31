#include "shader.h"
#include "models.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void Mesh::setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				 &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
}

Mesh::Mesh(
	vector<Vertex> vertices,
	vector<unsigned int> indices,
	vector<Texture> textures
) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  draw_mode = GL_TRIANGLES;
  material = Material::Default();

  setupMesh();
}

void Mesh::draw(Shader shader) {

  unsigned int diffuseNr  = 1;
  unsigned int specularNr = 1;
  unsigned int normalNr   = 1;

  shader.setMaterial("uMaterial", material);

  for(unsigned int i = 0; i < textures.size(); i++) {
      // activate proper texture unit before binding
      string number;
      string name = textures[i].type;

      if(name == "uTextureDiffuse") {
          number = std::to_string(diffuseNr++);
          glActiveTexture(GL_TEXTURE0);
          shader.setInt(("" + name + number).c_str(), 0);
      } else if(name == "uTextureSpecular") {
          number = std::to_string(specularNr++);
          glActiveTexture(GL_TEXTURE1);
          shader.setInt(("" + name + number).c_str(), 1);
      } else if(name == "uTextureNormal") {
          number = std::to_string(normalNr++);
          glActiveTexture(GL_TEXTURE2);
          shader.setInt(("" + name + number).c_str(), 2);
      }

      // TODO: Fix whatever the fuck this is
      glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

	shader.setBool("uTextured", diffuseNr > 1);
	shader.setBool("uSpecmapped", specularNr > 1);
	shader.setBool("uNormaled", normalNr > 1);

  glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);

  // TODO: Generalize this
	if (indices.size() > 0) {
      glDrawElements(draw_mode, indices.size(), GL_UNSIGNED_INT, 0);
	} else {
      glDrawArrays(draw_mode, 0, vertices.size());
	}

	shader.setBool("uTextured", false);
	glBindVertexArray(0);
}

Mesh Mesh::Cube() {
    float vertices[24] = {
        1.000000,  1.000000, -1.000000,
        1.000000, -1.000000, -1.000000,
        1.000000,  1.000000, 1.000000,
        1.000000, -1.000000, 1.000000,
        -1.000000,  1.000000, -1.000000,
        -1.000000, -1.000000, -1.000000,
        -1.000000,  1.000000, 1.000000,
        -1.000000, -1.000000, 1.000000
    };

    //float tex_coords[40] = {
    //    0.625000, 0.000000,
    //    0.375000, 0.250000,
    //    0.375000, 0.000000,
    //    0.625000, 0.250000,
    //    0.375000, 0.500000,
    //    0.375000, 0.250000,
    //    0.625000, 0.500000,
    //    0.375000, 0.750000,
    //    0.625000, 0.750000,
    //    0.375000, 1.000000,
    //    0.375000, 0.500000,
    //    0.125000, 0.750000,
    //    0.125000, 0.500000,
    //    0.875000, 0.500000,
    //    0.625000, 0.500000,
    //    0.625000, 0.250000,
    //    0.625000, 0.750000,
    //    0.625000, 1.000000,
    //    0.375000, 0.750000,
    //    0.875000, 0.750000
    //};
    float tex_coords[40] = {
                            1.000000, 1.000000,
                            0.000000, 0.000000,
                            1.000000, 0.000000,
                            0.000000, 1.000000
    };
    float normals[18] = {
        0.0000, 1.0000, 0.0000,
        0.0000, 0.0000, 1.0000,
        -1.0000, 0.0000, 0.0000,
        0.0000, -1.0000, 0.0000,
        1.0000, 0.0000, 0.0000,
        0.0000, 0.0000, -1.0000
    };

    float indices[108] = {
        5,1,1 , 3,2,1 , 1,3,1,
        3,1,2 , 8,2,2 , 4,3,2,
        7,1,3 , 6,2,3 , 8,3,3,
        2,1,4 , 8,2,4,  6,3,4,
        1,1,5, 4,2,5, 2,3,5,
        5,1,6, 2,2,6 , 6,3,6,
        5,1,1 , 7,4,1, 3,2,1,
        3,1,2 , 7,4,2 , 8,2,2,
        7,1,3 , 5,4,3, 6,2,3,
        2,1,4 , 4,4,4, 8,2,4,
        1,1,5, 3,4,5, 4,2,5,
        5,1,6, 1,4,6, 2,2,6
    };

    vector<Vertex> cube_vertices;
    for(int i = 0; i < 108; i += 3) {
      int vi = (indices[i]   - 1) * 3;
      int vt = (indices[i+1] - 1) * 2;
      int vn = (indices[i+2] - 1) * 3;

      Vertex v1, v2, v3;
      v1.Position  = glm::vec3(vertices[vi] / 2, vertices[vi+1] / 2, vertices[vi+2] / 2);
      v1.TexCoords = glm::vec2(tex_coords[vt], tex_coords[vt+1]);
      v1.Normal    = glm::vec3(normals[vn], normals[vn+1], normals[vn+2]);

      // Very bad
      i += 3;
      vi = (indices[i]   - 1) * 3;
      vt = (indices[i+1] - 1) * 2;
      vn = (indices[i+2] - 1) * 3;

      v2.Position  = glm::vec3(vertices[vi] / 2, vertices[vi+1] / 2, vertices[vi+2] / 2);
      v2.TexCoords = glm::vec2(tex_coords[vt], tex_coords[vt+1]);
      v2.Normal    = glm::vec3(normals[vn], normals[vn+1], normals[vn+2]);

      // Stop judging me
      i += 3;
      vi = (indices[i]   - 1) * 3;
      vt = (indices[i+1] - 1) * 2;
      vn = (indices[i+2] - 1) * 3;

      v3.Position  = glm::vec3(vertices[vi] / 2, vertices[vi+1] / 2, vertices[vi+2] / 2);
      v3.TexCoords = glm::vec2(tex_coords[vt], tex_coords[vt+1]);
      v3.Normal    = glm::vec3(normals[vn], normals[vn+1], normals[vn+2]);

      // learnopengl.com/Advanced-Lighting/Normal-Mapping
      glm::vec3 edge1 = v2.Position - v1.Position;
      glm::vec3 edge2 = v3.Position - v1.Position;
      glm::vec2 deltaUV1 = v2.TexCoords - v1.TexCoords;
      glm::vec2 deltaUV2 = v3.TexCoords - v1.TexCoords;
      float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

      glm::vec3 tangent;
      tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
      tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
      tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
      tangent = glm::normalize(tangent);

      glm::vec3 bitangent;
      bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
      bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
      bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
      bitangent = glm::normalize(bitangent);

      v1.Tangent = tangent;
      v2.Tangent = tangent;
      v3.Tangent = tangent;

      v1.Bitangent = bitangent;
      v2.Bitangent = bitangent;
      v3.Bitangent = bitangent;

      cube_vertices.push_back(v1);
      cube_vertices.push_back(v2);
      cube_vertices.push_back(v3);
    }

    Mesh ret(cube_vertices, vector<unsigned int>(), vector<Texture>());
    return ret;
}

// Very clever, very hard to work with.
Mesh Mesh::BadCube() {
  vector<Vertex> cube_corners;
  vector<Vertex> cube_vertices;

  for (int i = 0; i < 8; i++) {
    Vertex v;

    // Because having code easily understood is for weaklings
    // Note: I have come to rue this overconfident comment to my future self
    float x = 1 & i ? 0.5 : -0.5; // West or East
    float y = 2 & i ? 0.5 : -0.5; // Top or Bottom
    float z = 4 & i ? 0.5 : -0.5; // North or South

    v.Position = glm::vec3(x,y,z);
    cube_corners.push_back(v);
  }

  int vertices[] = {1,2,4,7};

  for(int i = 0; i < 4; i++) {
    int index = vertices[i];

    int nbors[3];

    nbors[0] = 1 ^ index; // neighbor in X
    nbors[1] = 2 ^ index; // neighbor in Y
    nbors[2] = 4 ^ index; // neighbor in Z

    for (int j = 0; j < 3; j++) {
      Vertex v1, v2, v3;

      v1 = cube_corners[index];
      v2 = cube_corners[nbors[j]];
      v3 = cube_corners[nbors[(j+1) % 3]];

      glm::vec3 toV2 = normalize(v2.Position - v1.Position);
      glm::vec3 toV3 = normalize(v3.Position - v1.Position);
      glm::vec3 normal = normalize(glm::cross(toV2, toV3));

      v1.Normal = normal;
      v2.Normal = normal;
      v3.Normal = normal;

      glm::vec2 t1 = glm::vec2();
      glm::vec2 t2 = glm::vec2();
      glm::vec2 t3 = glm::vec2();

      if (i & 2) {
        t1 = glm::vec2(1,1);
        t2 = glm::vec2(0,1);
        t3 = glm::vec2(1,0);
      } else {
        t1 = glm::vec2(0,0);
        t2 = glm::vec2(1,0);
        t3 = glm::vec2(0,1);
      }

      v1.TexCoords = t1;
      v2.TexCoords = t2;
      v3.TexCoords = t3;

      cube_vertices.push_back(v1);
      cube_vertices.push_back(v2);
      cube_vertices.push_back(v3);
    }
  }

  Mesh ret(cube_vertices, vector<unsigned int>(), vector<Texture>());

  return ret;
}

Mesh Mesh::Sphere(int divisions) {
  vector<Vertex> sphere_vertices;
  vector<unsigned int> sphere_indices;

  float arc = glm::pi<float>() * 2 / divisions;

  for (int i = 0; i < divisions ; i++) {
		for (int j = 0; j < divisions ; j++) {
			Vertex v1, v2, v3, v4;
			float theta = j * arc;
			float phi = i * arc;
			v1.Position = glm::vec3(
				glm::cos(phi) * cos(theta),
				glm::sin(theta),
				glm::sin(phi) * cos(theta)
			);
			v1.Normal = v1.Position;
			sphere_vertices.push_back(v1);

			v2.Position = glm::vec3(
				glm::cos(phi) * cos(theta + arc),
				glm::sin(theta + arc),
				glm::sin(phi) * cos(theta + arc)
			);
			v2.Normal = v2.Position;
			sphere_vertices.push_back(v2);

			v3.Position = glm::vec3(
				glm::cos(phi+arc) * cos(theta),
				glm::sin(theta),
				glm::sin(phi+arc) * cos(theta)
			);
			v3.Normal = v3.Position;
			sphere_vertices.push_back(v3);

			sphere_vertices.push_back(v2);
			sphere_vertices.push_back(v3);

			v4.Position = glm::vec3(
				glm::cos(phi+arc) * cos(theta+arc),
				glm::sin(theta+arc),
				glm::sin(phi+arc) * cos(theta+arc)
			);
			v4.Normal = v4.Position;
			sphere_vertices.push_back(v4);
		}
  }

  Mesh ret(sphere_vertices, sphere_indices, vector<Texture>());

  return ret;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process all the node's meshes (if any)
  for(unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for(unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
  }
}

void Model::loadModel(string path) {
  Assimp::Importer import;
  const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);

  float max_len = 1.0f;
  for (Mesh m : meshes) {
    for (Vertex v : m.vertices) {
      max_len = glm::max(max_len, glm::length(v.Position));
    }
  }
  transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/max_len));
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
  vector<Texture> textures;
  for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;

    for(unsigned int j = 0; j < textures_loaded.size(); j++) {
      if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }

    if(!skip) {   // if texture hasn't been loaded already, load it
      Texture texture = Texture::FromPath(directory + '/' + string(str.C_Str(), str.length));
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture); // add to loaded textures
    }
  }

  return textures;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {

  vector<unsigned int> indices;
  vector<Vertex>  vertices;
  vector<Texture> textures;

  for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    // process vertex positions, normals and texture coordinates
    glm::vec3 vector;

    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;


    if(mesh->mTangents)
    {
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.Tangent = vector;

      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.Bitangent = vector;
    }

    if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    }
    else
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);


    vertices.push_back(vertex);
  }

  // process indices
  for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // process material
  if(mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                        aiTextureType_DIFFUSE, "uTextureDiffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> specularMaps = loadMaterialTextures(material,
                                                        aiTextureType_SPECULAR, "uTextureSpecular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    vector<Texture> normalMaps = loadMaterialTextures(material,
                                                        aiTextureType_HEIGHT, "uTextureNormal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
  }

  return Mesh(vertices, indices, textures);
}


Model Model::FromPath(string path) {
	Model m;
  m.transform = glm::mat4(1.0f);
	m.loadModel(path);
	return m;
}

Model Model::FromMesh(Mesh mesh) {
	Model m;
  m.transform = glm::mat4(1.0f);
	m.meshes.push_back(mesh);
	return m;
}

Model Model::FromMeshes(vector<Mesh> meshes) {
	Model m;
  m.transform = glm::mat4(1.0f);
	m.meshes = meshes;
	return m;
}

void Model::draw(Shader shader)
{
  for(Mesh m: meshes) {
    m.draw(shader);
  }
}
