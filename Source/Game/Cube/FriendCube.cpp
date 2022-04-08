#include "FriendCube.h"


void FriendCube::Update(_In_ FLOAT deltaTime) {
    //ROTATION OPPOSITE!!! SO - DELTATIME
    XMMATRIX mSpin = XMMatrixRotationZ(-deltaTime);
    //ROTATION BY Y-AXIS
    XMMATRIX mOrbit = XMMatrixRotationY(-deltaTime * 2.0f);
    //ORBIT RADIUS
    XMMATRIX mTranslate = XMMatrixTranslation(4.0f, 0.0f, 0.0f);
    //30% SCALE
    XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

    m_world = mScale * mSpin * mTranslate * mOrbit;
};

