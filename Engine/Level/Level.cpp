#include "Level.h"
#include "Actor/Actor.h"

namespace Wanted
{
	Level::Level()
	{
	}

	Level::~Level()
	{
		for (Actor*& actor : actors)
		{
			if (!actor)
				continue;

			delete actor;
			actor = nullptr;
		}

		actors.clear();
	}

	void Level::BeginPlay()
	{
		for (Actor* actor : actors)
		{
			if (actor->HasBeganPlay())
				continue;

			actor->BeginPlay();
		}
	}

	void Level::Tick(float deletaTime)
	{
		for (Actor* actor : actors)
		{
			actor->Tick(deletaTime);
		}
	}

	void Level::Draw()
	{
		for (Actor* actor : actors)
		{
			Actor* zUpActor = nullptr;
			for (Actor* otherActor : actors)
			{
				if (actor == otherActor)
					continue;

				if (actor->GetPosition() == otherActor->GetPosition())
				{
					if (actor->GetSortingOrder() < otherActor->GetSortingOrder())
						zUpActor = otherActor;
				}
			}

			if (zUpActor)
				continue;

			actor->Draw();
		}
	}

	void Level::PostProcess()
	{
		ProcessAddAndDestroyActors();
	}
	
	void Level::AddNewActor(Actor* const newActor)
	{
		// 한 프레임 끝나기 전에 추가가되는 것을 방지 하기 위해 임시 배열에 저장
		addRequestedActors.emplace_back(newActor);
		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		// 제거 처리
		for (auto iter = actors.begin(); iter != actors.end();)
		{
			if ((*iter)->DestroyRequested())
			{
				delete *iter;
				iter = actors.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		// 추가 처리
		if (addRequestedActors.empty())
			return;

		for (Actor* const requestedActor : addRequestedActors)
		{
			actors.emplace_back(requestedActor);
		}
		addRequestedActors.clear();
	}
}