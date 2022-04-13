#pragma once

#include "Common.h"

namespace library
{
    /*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
      Struct:    SimpleVertex
      Summary:  Simple vertex structure containing a single field of the
                type XMFLOAT3
    C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
    struct SimpleVertex
    {
        XMFLOAT3 Position;
        XMFLOAT2 TexCoord;
    };

    /*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
      Struct:   CBChangeOnCameraMovement
      Summary:  Constant buffer containing view matrix
    S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
    struct CBChangeOnCameraMovement
    {
        XMMATRIX View;
    };

    /*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
      Struct:   CBChangeOnResize
      Summary:  Constant buffer containing projection matrix
    S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
    struct CBChangeOnResize
    {
        XMMATRIX Projection;
    };

    /*S+S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S+++S
      Struct:   CBChangesEveryFrame
      Summary:  Constant buffer containing world matrix
    S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S---S-S*/
    struct CBChangesEveryFrame
    {
        XMMATRIX World;
    };
}