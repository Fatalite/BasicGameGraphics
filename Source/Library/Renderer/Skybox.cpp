#include "Renderer/Skybox.h"

#include "assimp/Importer.hpp"	// C++ importer interface
#include "assimp/scene.h"		// output data structure
#include "assimp/postprocess.h"	// post processing flags

namespace library
{

    Skybox::Skybox(_In_ const std::filesystem::path& cubeMapFilePath, _In_ FLOAT scale)
        :
        Model(L"Content/Common/sphere.obj"),
        m_cubeMapFileName(cubeMapFilePath),
        m_scale(scale)
    { };


    HRESULT Skybox::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) {
        //Call Parent's Initialize method
        Model::Initialize(pDevice, pImmediateContext);
        //Scale(Construct)
        
        this->Scale(m_scale, m_scale, m_scale);
        //Set the first mesh's mateiral index to 0
        this->SetMaterialOfMesh(0, 0);
        //Set and init first material's diffuse by m_cube
       
        //Set
         this->GetMaterial(0).get()->pDiffuse = std::make_shared<Texture>(m_cubeMapFileName);
         HRESULT hr = this->GetMaterial(0).get()->Initialize(pDevice, pImmediateContext);
         //Skybox::initSingleMesh(0, m_pScene->mMeshes[0]);
         return hr;
    }
    //virtual void Update(_In_ FLOAT deltaTime, _In_ const XMVECTOR& lightPosition);
     
    const std::shared_ptr<Texture>& Skybox::GetSkyboxTexture() const {
        //Returns the diffuse texture of the first material
        return m_aMaterials[0].get()->pDiffuse;
    }

    void Skybox::initSingleMesh(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh) {
        for (int i = 0; i < pMesh->mNumVertices; i++) {
            const aiVector3D zero3d(0.0f, 0.0f, 0.0f);
            const aiVector3D& position = pMesh->mVertices[i];
            const aiVector3D& normal = pMesh->mNormals[i];
            const aiVector3D& texCoord = pMesh->HasTextureCoords(0u) ?
                pMesh->mTextureCoords[0][i] : zero3d;

            SimpleVertex vertex =
            {
            .Position = XMFLOAT3(position.x, position.y, position.z),
            .TexCoord = XMFLOAT2(texCoord.x, texCoord.y),
            .Normal = XMFLOAT3(normal.x, normal.y, normal.z)
            };

            m_aVertices.push_back(vertex);
        }

        for (int i = 0; i < pMesh->mNumFaces; i++) {
            const aiFace& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3u);
            WORD aIndices[3] =
            {
            static_cast<WORD>(face.mIndices[0]),
            static_cast<WORD>(face.mIndices[1]),
            static_cast<WORD>(face.mIndices[2]),
            };

            m_aIndices.push_back(aIndices[2]);
            m_aIndices.push_back(aIndices[1]);
            m_aIndices.push_back(aIndices[0]);

        }
        initMeshBones(uMeshIndex, pMesh);
        //
    }

}