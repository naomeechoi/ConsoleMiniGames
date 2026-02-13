#pragma once
#include "World/Level.h"
#include "Math/Vector2.h"
#include "System/Timer.h"
#include "TetrisPieces.h"
#include <deque>

class TetrisPlayer;
class TetrisAIMachine;
class TetrisBoard;
class Input;
class UITop;
class UIMessage;

class TetrisLevel : public MinigameEngine::Level
{
    // 함수 포인터 정의 (CardMonte 방식)
    typedef void (TetrisLevel::* StateFunc)(float deltaTime, MinigameEngine::Input* input);
    //typedef void (TetrisLevel::* AIStateFunc)(float deltaTime);

public:
    TetrisLevel();
    virtual ~TetrisLevel();

    virtual void BeginPlay() override;
    virtual void OnExit() override;
    void AIModeClear();
    virtual void Tick(float deltaTime, MinigameEngine::Input* input) override;
    void CheckAILose();
    void GenerateFuturePiece();
    virtual void Draw() override;

private:
    // 상태 변경 함수
    void ChangeState(StateFunc nextState);

    // 각 상태 로직
    void StateFalling(float deltaTime, MinigameEngine::Input* input);
    void StateLocking(float deltaTime, MinigameEngine::Input* input);
    void StateLineClearing(float deltaTime, MinigameEngine::Input* input); // 라인 삭제 상태 추가
    void StateGameOver(float deltaTime, MinigameEngine::Input* input);

private:
    // 공통 동작
    bool MoveDown();
    bool MoveHorizontal(bool isLeft);
    bool Rotate(MinigameEngine::Input* input);
    void HardDrop();
    void SpawnNewPiece();
    bool HandleHorizontalInput(float deltaTime, MinigameEngine::Input* input);
    int GetGhostY() const;

private:
    bool hasBeganPlay = false;
    TetrisPlayer* player = nullptr;
    TetrisBoard* board = nullptr;

    UITop* topUI = nullptr;
    UIMessage* messageUI = nullptr;

    // 상태 관리
    StateFunc curState = nullptr;

    // 타이머들
    MinigameEngine::Timer gravityTimer;      // 자동 낙하
    MinigameEngine::Timer softDropTimer;    // 소프트 드롭 속도
    MinigameEngine::Timer horizontalTimer;  // 좌우 이동 속도
    MinigameEngine::Timer lockDelayTimer;   // 고정 대기 시간

    int lockMoveCount = 0;

    bool canHold = true;

    // AI 관련 함수
private:
    bool isAIPlayMode = false;
    TetrisAIMachine* aiMachine = nullptr;

private:
    bool ValidCheck();
};