#include "YourCube.h"


void YourCube::Update(_In_ FLOAT deltaTime) {
	Scale(1.0f, 1.0f, 1.0f);
	RotateY(deltaTime);
};

