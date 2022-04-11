#include "AmazingCube.h"


void AmazingCube::Update(_In_ FLOAT deltaTime) {


    //RotatY(deltaTime);

    Scale(1.5f, 0.3f, 0.3f);
    RotateZ(-deltaTime);
    XMVECTOR tmp = XMVectorSet(-7.0f, 0.0f, 0.0f, 0.0f);

    Translate(tmp);

    RotateZ(-deltaTime * 2.0f);
    //;


}
