#include "FriendCube.h"


void FriendCube::Update(_In_ FLOAT deltaTime) {
    
    
    //RotatY(deltaTime);
    
    Scale(0.3f, 0.3f, 0.3f);
    RotateZ(-deltaTime);
    XMVECTOR tmp = XMVectorSet(4.0f, 0.0f, 0.0f, 0.0f);

    Translate(tmp);

    RotateY(-deltaTime*2.0f);
    //;

    
}

