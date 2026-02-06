#include "UIColorEffect.h"
#include "Util/Color.h"
#include "System/Timer.h"

UIColorEffect::UIColorEffect(Color& controlledColor, Color effectColor, float totalTime, int blankCount)
	: controlledColor(controlledColor), originColor(controlledColor), effectColor(effectColor), totalTime(totalTime), blackCount(blankCount)
{
	totalTimer.SetTargetTime(totalTime);
	blankTimer.SetTargetTime(totalTime / (float)blackCount);
}

void UIColorEffect::Tick(float deltaTime)
{
	if (!isStarted)
		return;

	totalTimer.Tick(deltaTime);

	// 전체 시간 종료로 색 효과 끝내기
	if (totalTimer.IsTimeOut())
	{
		isStarted = false;
		Stop();
		return;
	}

	blankTimer.Tick(deltaTime);
	if (!blankTimer.IsTimeOut())
		return;

	blankTimer.Reset();
	controlledColor = (controlledColor == originColor) ? effectColor : originColor;
}

void UIColorEffect::Start()
{
	isStarted = true;
	Reset();
}

void UIColorEffect::Stop()
{
	isStarted = false;
	Reset();
}

void UIColorEffect::Reset()
{
	totalTimer.Reset();
	blankTimer.Reset();
	controlledColor = originColor;
}