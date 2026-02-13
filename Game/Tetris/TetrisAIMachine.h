#pragma once
#include "TetrisPlayer.h"
#include "TetrisBoard.h"
#include "System/Timer.h"
#include "Math/Vector2.h"

class TetrisAIMachine
{
    typedef void (TetrisAIMachine::* AIStateFunc)(float deltaTime);
public:
    TetrisAIMachine(Vector2 worldPos, float lockTime);
    ~TetrisAIMachine();

    void BeginPlay(std::deque<PieceType> pieceQueue);
	void Tick(float deltaTime);
    void Draw();
    bool ValidCheck();
    
    void AddTrashLines(int count);
    int GetCleanCount();
	void SynclonizeSpawnPieceWithPlayer(PieceType piece);

    void InsertPieceToQueue(PieceType piece);
    void Clear();

private:
    TetrisPlayer* aiPlayer = nullptr;
    TetrisBoard* aiBoard = nullptr;

    AIStateFunc aiCurState = nullptr;

private:
    bool AIFindBestPos(PieceType type, int x, int y, int rot);
    bool AIGetBehaveSequence(int bestX, int bestY, int bestRot, int x, int y, int rot);
    void SpawnAINewPiece();
    void AIChangeState(AIStateFunc nextState);

    // 각 상태 로직
    void AIStateFalling(float deltaTime);
    void AIStateLocking(float deltaTime);
    void AIStateLineClearing(float deltaTime); // 라인 삭제 상태 추가
    void AIStateGameOver(float deltaTime);

    void AIRotate();
    void AIMoveHorizontal(bool isLeft);
    bool AIMoveDown();

    MinigameEngine::Timer aiOneMoveTimer;
    MinigameEngine::Timer aiLockDelayTimer;
    float aiOneMoveTime = 0.0f;

    enum Behavior
    {
        rotate,
        right,
        left,
        down,
    };
    // TODO: 우선 하드코딩
    // 0, x축움직임
    std::deque<Behavior> aiBehaviorSequence;
    int aiDestY = 0;

    Vector2 worldPos;
	float lockTime = 0.5f;

    std::optional<bool> requestedEnd;
    bool isPlaying = false;

public:
    std::optional<bool> ConsumeRequestedEnd();
};

