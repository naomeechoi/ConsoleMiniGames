#pragma once
#include "Common/Common.h"
#include <vector>
#include <optional>
#include <Math/Vector2.h>
#include <Util/Color.h>

namespace MinigameEngine
{
	class Input;
	
	class NAOMI_API Level
	{
	public:
		Level();
		virtual ~Level();

		virtual void OnExit();
		virtual void BeginPlay();
		virtual void Tick(float deltaTime, Input* input);
		virtual void Draw();

		void RequestChangeLevel(int levelID);
		void RequestShowResult(int result);

		std::optional<int> ConsumeRequestedLevel();
		std::optional<int> ConsumeRequestedShowResult();

		void SetLevelDisplaySize(Vector2 displaySize);
		Vector2 GetLevelDisplaySize();
		Color GetEdgeColor();
		void SetResult(int result);

	protected:
		std::optional<int> requestedLevel;
		std::optional<int> requestedShowResult;
		Vector2 displaySize;
		Color edgeColor = Color::Green;
		int result = -1;
	};
}

