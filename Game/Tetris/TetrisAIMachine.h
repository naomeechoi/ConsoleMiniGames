#pragma once
#include "System/Timer.h"
#include "Math/Vector2.h"
#include "TetrisBlocks.h"
#include <deque>
#include <optional>

class TetrisPlayer;
class TetrisBoard;

class TetrisAIMachine
{
    enum class EBehavior
    {
        ROTATE,
        RIGHT,
        LEFT,
        DOWN
    };
    typedef void (TetrisAIMachine::* AIStateFunc)(float deltaTime);

public:
    TetrisAIMachine(MinigameEngine::Vector2 worldPos, float lockTime);
    ~TetrisAIMachine();
    TetrisAIMachine(const TetrisAIMachine&) = delete;
    TetrisAIMachine& operator=(const TetrisAIMachine&) = delete;

    void BeginPlay(std::deque<EBlockType> blockQueue);
	void Tick(float deltaTime);
    void Draw();
    
    void AddTrashLines(int count);
    int GetCleanCount();
    void InsertBlockToQueue(EBlockType block);
    void Clear();

private:
    bool ValidCheck();

    //AI Logics
private:
    bool FindBestPosition(EBlockType type, int x, int y, int rot);
    bool GetBehaveSequence(int bestX, int bestY, int bestRot, int x, int y, int rot);
    void SpawnAINewBlock();
    void ChangeAIState(AIStateFunc nextState);

    // 각 상태 로직
    void AIStateFalling(float deltaTime);
    void AIStateLocking(float deltaTime);
    void AIStateLineClearing(float deltaTime); // 라인 삭제 상태 추가
    void AIStateGameOver(float deltaTime);

    void AIRotate();
    void AIMoveHorizontal(bool isLeft);
    bool AIMoveDown();

public:
    std::optional<bool> ConsumeRequestedGameEnd();

private:
    TetrisPlayer* aiPlayer = nullptr;
    TetrisBoard* aiBoard = nullptr;
    AIStateFunc aiCurState = nullptr;

    MinigameEngine::Timer aiOneMoveTimer;
    MinigameEngine::Timer aiLockDelayTimer;
    MinigameEngine::Timer aiDyingTermTimer;
    float aiOneMoveTime = 0.0f;

    std::deque<EBehavior> aiBehaviorSequence;
    size_t behaviorIndex = 0;

    MinigameEngine::Vector2 worldPos;
	float lockTime = 0.5f;

    std::optional<bool> requestedGameEnd;
    bool isPlaying = false;
};

