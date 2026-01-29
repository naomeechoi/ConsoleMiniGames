#include "TestActor.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include <iostream>

TestActor::TestActor()
	: super('T', Wanted::Vector2(2,3))
{
}

void TestActor::BeginPlay()
{
	Actor::BeginPlay();
}

void TestActor::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
	if (Wanted::Input::Get().GetKeyDown('Q'))
	{
		Wanted::Engine::Get().QuitEngine();
	}
	/*std::cout << "TestActor::Tick(). deltaTime: "
		<< deltaTime << ", FPS: "
		<< (1.0f / deltaTime) << std::endl;*/
}

void TestActor::Draw()
{
	Actor::Draw();
}
