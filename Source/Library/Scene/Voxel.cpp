#include "Scene/Voxel.h"

#include "Texture/Material.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Voxel
      Summary:  Constructor
      Args:     const XMFLOAT4& outputColor
                  Color of the voxel
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::Voxel definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Voxel
      Summary:  Constructor
      Args:     std::vector<InstanceData>&& aInstanceData
                  Instance data
                const XMFLOAT4& outputColor
                  Color of the voxel
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::Voxel definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Initialize
      Summary:  Initializes a voxel
      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::Update
      Summary:  Updates the voxel every frame
      Args:     FLOAT deltaTime
                  Elapsed time
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::Update definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::GetNumVertices
      Summary:  Returns the number of vertices in the voxel

      Returns:  UINT
                  Number of vertices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::GetNumVertices definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::GetNumIndices
      Summary:  Returns the number of indices in the voxel

      Returns:  UINT
                  Number of indices
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::GetNumIndices definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::getVertices
      Summary:  Returns the pointer to the vertices data

      Returns:  const library::SimpleVertex*
                  Pointer to the vertices data
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::getVertices definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Voxel::getIndices
      Summary:  Returns the pointer to the indices data

      Returns:  const WORD*
                  Pointer to the indices data
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Voxel::getIndices definition (remove the comment)
    --------------------------------------------------------------------*/
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

    HRESULT Voxel::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext) {
        HRESULT hr = initializeInstance(pDevice);
        hr = initialize(pDevice, pImmediateContext);
        return hr;
    };
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