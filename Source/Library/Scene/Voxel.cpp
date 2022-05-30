#include "Scene/Voxel.h"

#include "Texture/Material.h"

namespace library
{
    Voxel::Voxel(_In_ const XMFLOAT4& outputColor)
        :InstancedRenderable(outputColor) {};
    /*
    && is new in C++11.
    int&& a means "a" is an r-value reference.
    && is normally only used to declare a parameter of a function.
    And it only takes a r-value expression.
    If you don't know what an r-value is,
    the simple explanation is that it doesn't have a memory address.
    E.g. the number 6, and character 'v' are both r-values.
    int a, a is an l-value, however (a+2) is an r-value.
    !!!!move function!!!!!fk
    */

    Voxel::Voxel(_In_ std::vector<InstanceData>&& aInstanceData, _In_ const XMFLOAT4& outputColor)
        :InstancedRenderable(std::move(aInstanceData), outputColor) {};

    HRESULT Voxel::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        BasicMeshEntry basicMeshEntry;
        basicMeshEntry.uNumIndices = NUM_INDICES;

        m_aMeshes.push_back(basicMeshEntry);

        HRESULT hr = initialize(pDevice, pImmediateContext);
        if (FAILED(hr))
        {
            return hr;
        }

        hr = initializeInstance(pDevice);
        if (FAILED(hr))
        {
            return hr;
        }

        if (HasTexture() > 0)
        {
            hr = SetMaterialOfMesh(0, 0);
            if (FAILED(hr))
            {
                return hr;
            }
        }

        return S_OK;
    }
    void Voxel::Update(_In_ FLOAT deltaTime) {
        //
    }

    UINT Voxel::GetNumVertices() const {
        return NUM_VERTICES;
    }
    UINT Voxel::GetNumIndices() const {
        return NUM_INDICES;
    }
    const SimpleVertex* Voxel::getVertices() const {
        return VERTICES;
    }
    const WORD* Voxel::getIndices() const {
        return INDICES;
    }
}