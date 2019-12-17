#include "Loader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Application/Application.h"

#include "Assets/MeshBuffer.h"
#include "NeiVu/Texture.h"

#include <fstream>

using namespace Nei;
using namespace Vu;

glm::vec3 toGlm(aiVector3D v) {
  return { v.x, v.y, v.z };
}

glm::quat toGlm(aiQuaternion q) {
  return { q.w, q.x, q.y, q.z };
}

glm::mat4 toGlm(aiMatrix4x4 m) {
  mat4 ret;
  auto* data = value_ptr(ret);
  for (uint i = 0; i < 16; i++) data[i] = m[i % 4][i / 4];
  return ret;
}

ModelData Loader::load(Nei::DeviceContext* dc, fs::path const& path) {
  ModelData ret;

  Assimp::Importer importer;
  auto* scene = importer.ReadFile(path.string().c_str(),
    //aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate |
    //aiProcess_GenSmoothNormals |
    aiProcess_RemoveRedundantMaterials |
    aiProcess_PreTransformVertices |
    aiProcess_FlipWindingOrder 
  );
  if (!scene) {
    nei_error("Unable to load model {}", path.string().c_str());
    nei_error("Error: {}", importer.GetErrorString());
    return ret;
  }

  auto vertexLayout = VertexLayout::defaultLayout();

  /* MATERIALS */
  for (uint i = 0; i < scene->mNumMaterials; i++) {
    auto aMat = scene->mMaterials[i];

    auto difCount = aMat->GetTextureCount(aiTextureType_DIFFUSE);

    if (difCount > 0) {
      aiString apath;
      aMat->GetTexture(aiTextureType_DIFFUSE, 0, &apath);

      fs::path texPath = apath.C_Str();
      if (texPath.is_relative()) {
        fs::path resolved = NeiFS->resolve(texPath, false);
        if (resolved != "") {
          // tex path relative to filesystem
          texPath = resolved;
        }
        else {
          resolved = NeiFS->resolve(path.parent_path() / texPath);
          if (resolved != "") {
            // tex path relative to model
            texPath = resolved;
          }
        }
      }

      ret.textures.push_back(NeiApp->getAssetManager()->loadTexture2D(texPath));
    }
    else {
      ret.textures.push_back(NeiApp->getAssetManager()->createDummy(uvec2(512)));
    }
  }
  
  /* MESHES */

  Ptr mesh = new Mesh(dc);
  mesh->setVertexLayout(vertexLayout);

  ret.mesh = mesh;
  // count vertices
  uint meshVertexCount = 0;
  uint meshIndexCount = 0;
  for (uint i = 0; i < scene->mNumMeshes; i++) {
    auto aMesh = scene->mMeshes[i];
    meshVertexCount += aMesh->mNumVertices;
    if (aMesh->HasFaces())
      meshIndexCount += aMesh->mNumFaces * 3;
  }

  mesh->resizeVertices(meshVertexCount);
  mesh->resizeIndices(meshIndexCount);
  auto vptr = static_cast<Vertex_Default*>(mesh->vertexPtr());
  auto vptrStart = vptr;
  auto iptr = mesh->indexPtr();

  uint indexOffset = 0;

  for (uint i = 0; i < scene->mNumMeshes; i++) {
    auto aMesh = scene->mMeshes[i];
    uint vertexCount = aMesh->mNumVertices;

    bool normals = aMesh->HasNormals();
    bool texCoords = aMesh->HasTextureCoords(0);
    int materialId = aMesh->mMaterialIndex;

    for (uint j = 0; j < vertexCount; j++) {
      vptr->position = toGlm(aMesh->mVertices[j]);
      if (normals)vptr->normal = toGlm(aMesh->mNormals[j]);
      if (texCoords)vptr->texCoord = toGlm(aMesh->mTextureCoords[0][j]);
      vptr->material = materialId;
      vptr++;
    }

    if (aMesh->HasFaces()) {
      for (uint j = 0; j < aMesh->mNumFaces; j++) {
        auto& face = aMesh->mFaces[j];
        nei_assert(face.mNumIndices == 3);
        for (uint k = 0; k < 3; k++) {
          *iptr++ = face.mIndices[k] + indexOffset;
          nei_assert(face.mIndices[k] + indexOffset < meshVertexCount);
        }
      }
      indexOffset += aMesh->mNumVertices;
    }
  }

  Ptr meshBuffer = new MeshBuffer(dc);
  meshBuffer->createFromMesh(mesh);
  mesh->setMeshBuffer(meshBuffer);

  dc->wait();

  return ret;
}
