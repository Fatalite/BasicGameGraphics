#pragma once


#include "BaseCube.h"

class FriendCube : public BaseCube {
public:
	FriendCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};