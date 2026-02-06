#include "Level.h"
#include "Actor.h"
#include "System/Input.h"
#include "Core/Engine.h"

namespace MinigameEngine
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

	void Level::OnExit()
	{
		result = -1;
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

	void Level::Tick(float deletaTime, Input* input)
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
			if (!actor->IsActive())
				continue;

			actor->Draw();
		}
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

	void Level::RequestChangeLevel(int levelID)
	{
		requestedLevel = levelID;
	}

	void Level::RequestShowResult(int result)
	{
		requestedShowResult = result;
	}

	std::optional<int> Level::ConsumeRequestedLevel()
	{
		auto temp = requestedLevel;
		requestedLevel.reset();
		return temp;
	}

	std::optional<int> Level::ConsumeRequestedShowResult()
	{
		auto temp = requestedShowResult;
		requestedShowResult.reset();
		return temp;
	}

	void Level::SetLevelDisplaySize(Vector2 displaySize)
	{
		this->displaySize = displaySize;
	}

	Color Level::GetEdgeColor()
	{
		return edgeColor;
	}

	void Level::SetResult(int result)
	{
		this->result = result;
	}
}