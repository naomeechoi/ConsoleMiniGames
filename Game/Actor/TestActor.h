#pragma once
#include "Actor/Actor.h"

class TestActor : public Wanted::Actor
{
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
};

