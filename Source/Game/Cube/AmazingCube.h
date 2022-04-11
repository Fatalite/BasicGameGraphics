#pragma once
#include "BaseCube.h"

class AmazingCube : public BaseCube {
public:
	AmazingCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};