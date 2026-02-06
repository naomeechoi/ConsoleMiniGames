#define NOMINMAX
#include "CardMonteLevel.h"
#include "CardMonteMode.h"
#include "Render/Renderer.h"
#include "System/Input.h"
#include "Common/LevelType.h"
#include "UI/UITop.h"
#include "UI/UILoadingBar.h"
#include "UI/UIMessage.h"
#include "Util/Random.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

enum EResult
{
	fail = 0,
	success,
};

const int PADDING = 10;

CardMonteLevel::CardMonteLevel()
{
	LoadSetting();
}

CardMonteLevel::~CardMonteLevel()
{
    OnExit();

    if (topUI)
    {
        delete topUI;
        topUI = nullptr;
    }

    if (loadingBarUI)
    {
        delete loadingBarUI;
        loadingBarUI = nullptr;
    }

    if (messageUI)
    {
        delete messageUI;
        messageUI = nullptr;
    }

    if (mode)
    {
        delete mode;
        mode = nullptr;
    }
}

void CardMonteLevel::BeginPlay()
{
    if (hasBeganPlay)
        return;

    hasBeganPlay = true;

    CardSetting();
    SetShufflePairs();

    if (!mode)
        mode = new CardMonteMode();

    if (!topUI)
        topUI = new UITop(displaySize.x, Vector2(3, 2), "Card Monte");

    if (!loadingBarUI)
        loadingBarUI = new UILoadingBar(Vector2(3, displaySize.y - 3), displaySize.x - 5, playTime, '#');

    if (!messageUI)
        messageUI = new UIMessage();

    if (!mode
        || !topUI
        || !loadingBarUI
        || !messageUI)
    {
        // 게임 시작 못하는 상황
        RequestChangeLevel((int)LevelType::Menu);
        return;
    }

    // CardMonteLevel::BeginPlay() 내부
    int answerCardIdx = Random::Random(0, (int)cards.size() - 1);
    int answer = cards[answerCardIdx].num;
    mode->SetAnswer(answer);

    // 멤버 변수인 message를 직접 수정하지 말고 지역 변수 생성
    std::string finalMessage = message + std::to_string(answer);
    messageUI->Start(displaySize.x, Vector2(3, displaySize.y - 5), finalMessage, " ");
    // 시작
    ChangeState(&CardMonteLevel::StateShowing, showingTime);
}

void CardMonteLevel::OnExit()
{
    Level::OnExit();

    hasBeganPlay = false;
    Clear();
}

void CardMonteLevel::Tick(float deltaTime, Input* input)
{
    if (input->IsKeyPressed(VK_ESCAPE))
    {
        RequestChangeLevel((int)LevelType::Menu);
    }

    if (curState)
    {
        (this->*curState)(deltaTime);
        if (curState == &CardMonteLevel::StateChoose)
            HandleChooseInput(input);
    }

    if (loadingBarUI)
    {
        loadingBarUI->Tick(deltaTime);
    }
}

void CardMonteLevel::Draw()
{
    Level::Draw();

    if (topUI)
        topUI->Draw();

    if (loadingBarUI)
        loadingBarUI->Draw();

    if (messageUI)
        messageUI->Draw();
        
    for (int i = 0; i < cardCount; i++)
    {
        std::string cardSprite = cardSprites[spriteIdx];

        if (spriteIdx == 0)
            cardSprite[cardMidIdx] = '0' + cards[i].num;

        Color color = (i == selectedIdx) ? Color::LightYellow : Color::Green;

        Color bgColor = Color::Black;
        // 디버깅용으로 빨간색 표시
        if (curState == &CardMonteLevel::StateChoose)
        {
            if ((mode->Check(cards[i].num)))
                bgColor = Color::Red;
        }

        Renderer::Get().SubmitMultiLine(
            cardSprite.c_str(),
            cards[i].pos,
            color,
            bgColor,
            0
        );
    }
}

void CardMonteLevel::LoadSetting()
{
    std::ifstream file("../Assets/CardMonte/setting.txt", std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Fail to open file: CardMonte/setting.txt" << std::endl;
        __debugbreak();
        return;
    }

    std::string line;
    std::string currentSprite;
    bool readingImage = false;

    while (std::getline(file, line)) {
        // CR 제거
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        // 변수 파싱
        if (line.find("cardCount") != std::string::npos) {
            cardCount = stoi(line.substr(line.find('=') + 1));
        }
        else if (line.find("playTime") != std::string::npos) {
            playTime = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("suffleTime") != std::string::npos) {
            suffleTime = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("suffleCount") != std::string::npos) {
            suffleCount = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("cardFilpTime") != std::string::npos) {
            cardFilpTime = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("cardWidth") != std::string::npos) {
            cardWidth = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("cardHeight") != std::string::npos) {
            cardHeight = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("showingTime") != std::string::npos) {
            showingTime = stof(line.substr(line.find('=') + 1));
        }
        else if (line.find("message") != std::string::npos) {
            message = line.substr(line.find('=') + 1);
        }
        // 이미지 시작
        else if (line.find("sprite") != std::string::npos) {
            if (!currentSprite.empty()) {
                cardSprites.push_back(currentSprite);
                currentSprite.clear();
            }
            // "=" 뒤부터 카드 내용 시작
            currentSprite += line.substr(line.find('=') + 1) + "\n";
            readingImage = true;
        }
        // 카드 이미지 누적
        else if (readingImage) {
            currentSprite += line + "\n";
        }
    }

    // 마지막 이미지 추가
    if (!currentSprite.empty()) {
        cardSprites.push_back(currentSprite);
    }
}

void CardMonteLevel::CardSetting()
{
    float remainingSpace = displaySize.x - 2 * PADDING - cardCount * cardWidth;
    float padding = (cardCount > 1) ? remainingSpace / (cardCount - 1) : 0;

    float posY = displaySize.y / 2 - cardHeight / 2; // 세로 중앙

    cards.clear();
    for (int i = 0; i < cardCount; i++) {
        Card card;
        card.num = i + 1;
        card.pos.x = PADDING + i * (cardWidth + padding);
        card.pos.y = posY;
        card.originPos = card.pos;
        cards.push_back(card);
    }

    cardMidIdx = (int)cardSprites[0].size() / 2 - 1;
}

void CardMonteLevel::Clear()
{
    cards.clear();
    shufflePairs.clear();
    spriteIdx = 0;
    currentShuffleIdx = 0;
    stateTimer.Reset();
    curState = nullptr;
    selectedIdx = -1;
    isSuccess = false;


    if (mode)
        mode->Clear();

    if (loadingBarUI)
    {
        loadingBarUI->Stop();
        loadingBarUI->Clear();
    }

    if (messageUI)
        messageUI->Clear();
}

Vector2 CardMonteLevel::GetCenter(const Vector2& pos)
{
    Vector2 center;
    center.x = pos.x + cardWidth / 2;
    center.y = pos.y + cardHeight / 2;
    return center;
}

Vector2 CardMonteLevel::CenterToTopLeft(const Vector2& center)
{
    Vector2 topLeft;
    topLeft.x = center.x - cardWidth / 2;
    topLeft.y = center.y - cardHeight / 2;
    return topLeft;
}

void CardMonteLevel::ChangeState(StateFunc next, float duration)
{
    stateTimer.Reset();
    stateTimer.SetTargetTime(duration);
    curState = next;
}

void CardMonteLevel::StateShowing(float deltatime)
{
    stateTimer.Tick(deltatime);
    //보여주는 로직
    if (stateTimer.IsTimeOut())
        ChangeState(&CardMonteLevel::StateFilp, showingTime);
}

void CardMonteLevel::StateFilp(float deltatime)
{
    stateTimer.Tick(deltatime);
    
    FlipCard(false);
    if (stateTimer.IsTimeOut())
    {
        spriteIdx = cardSprites.size() - 1;
        ChangeState(&CardMonteLevel::StateShuffle, cardFilpTime);
    }
}

void CardMonteLevel::FlipCard(bool isOpen)
{

    float t = stateTimer.GetRatio();
    int maxIdx = cardSprites.size() - 1;
    int flipIdx = isOpen ? (int)((1-t) * maxIdx) : (int)(t * maxIdx);
    flipIdx = std::clamp(flipIdx, 0, maxIdx);
    spriteIdx = flipIdx;
}

std::vector<int> CardMonteLevel::GetCurCardsOrder()
{
    std::vector<int> indices(cards.size());
    for (int i = 0; i < cards.size(); i++)
        indices[i] = i;

    std::sort(indices.begin(), indices.end(),
        [&](int a, int b)
        {
            return cards[a].originPos.x < cards[b].originPos.x;
        });

    return indices;
}

void CardMonteLevel::HandleChooseInput(Input* input)
{
    auto order = GetCurCardsOrder();

    auto it = std::find(order.begin(), order.end(), selectedIdx);
    if (it == order.end())
        return;

    int idx = (int)std::distance(order.begin(), it);

    if (input->IsKeyPressed(VK_LEFT) && idx > 0)
        selectedIdx = order[idx - 1];

    if (input->IsKeyPressed(VK_RIGHT) && idx < order.size() - 1)
        selectedIdx = order[idx + 1];

    if (input->IsKeyPressed(VK_SPACE))
    {
        int chosenNum = cards[selectedIdx].num;
        if (mode->Check(chosenNum))
        {
            ChangeState(&CardMonteLevel::StateGameWin, cardFilpTime);
        }
        else
        {
            ChangeState(&CardMonteLevel::StateGameOver, cardFilpTime);
        }
        if (loadingBarUI)
            loadingBarUI->Stop();
    }
}

void CardMonteLevel::StateShuffle(float deltatime)
{
    stateTimer.Tick(deltatime);
    float t = stateTimer.GetRatio();

    const ShufflePair& pair = shufflePairs[currentShuffleIdx];

    // num으로 카드 찾기
    Card* cardA = nullptr;
    Card* cardB = nullptr;
    for (auto& c : cards)
    {
        if (c.num == pair.a) cardA = &c;
        else if (c.num == pair.b) cardB = &c;
    }

    if (!cardA || !cardB)
        return;

    // 반원 경로 이동
    cardA->pos = CircularLerp(cardA->originPos, cardB->originPos, t, true);
    cardB->pos = CircularLerp(cardB->originPos, cardA->originPos, t, false);

    if (stateTimer.IsTimeOut())
    {
        // 최종 위치 정확히 상대방 자리로
        Vector2 tmpOrigin = cardA->originPos;
        cardA->pos = cardB->originPos;
        cardB->pos = tmpOrigin;

        // originPos도 교환
        cardA->originPos = cardB->originPos;
        cardB->originPos = tmpOrigin;

        currentShuffleIdx++;
        if (currentShuffleIdx < shufflePairs.size())
        {
            stateTimer.Reset();
        }
        else
        {
            ChangeState(&CardMonteLevel::StateChoose, playTime);
            if(loadingBarUI)
                loadingBarUI->Start();
            selectedIdx = Random::Random(0, (int)cards.size() - 1);
        }
    }
}

void CardMonteLevel::StateChoose(float deltatime)
{
    stateTimer.Tick(deltatime);
    //보여주는 로직
    if (stateTimer.IsTimeOut())
        ChangeState(&CardMonteLevel::StateGameOver, cardFilpTime);
}

void CardMonteLevel::StateGameOver(float deltatime)
{
    // 게임 끝내는 로직
    stateTimer.Tick(deltatime);

    FlipCard(true);
    if (stateTimer.IsTimeOut())
    {
        spriteIdx = 0;
        isSuccess = false;
        ChangeState(&CardMonteLevel::StateWaitToExit, showingTime);
    }
}

void CardMonteLevel::StateGameWin(float deltatime)
{
    // 게임 끝내는 로직
    stateTimer.Tick(deltatime);

    FlipCard(true);
    if (stateTimer.IsTimeOut())
    {
        spriteIdx = 0;
        isSuccess = true;
        ChangeState(&CardMonteLevel::StateWaitToExit, showingTime);
    }
}

void CardMonteLevel::StateWaitToExit(float deltatime)
{
    // 게임 끝내는 로직
    stateTimer.Tick(deltatime);

    if (stateTimer.IsTimeOut())
    {
        RequestShowResult(isSuccess ? EResult::success : EResult::fail);
        RequestChangeLevel((int)LevelType::GameResult);
    }
}


void CardMonteLevel::SetShufflePairs()
{
    shufflePairs.clear();
    currentShuffleIdx = 0;

    for (int i = 0; i < suffleCount; i++)
    {
        int numA = Random::Random(1, cardCount);
        int numB = numA;
        while (numB == numA)
            numB = Random::Random(1, cardCount);

        ShufflePair pair;
        pair.a = numA;
        pair.b = numB;

        shufflePairs.push_back(pair);
    }
}

Vector2 CardMonteLevel::CircularLerp(const Vector2& start, const Vector2& end, float t, bool topArc)
{
    Vector2 startCenterPos = GetCenter(start);
    Vector2 endCenterPos = GetCenter(end);

    Vector2 center = { (startCenterPos.x + endCenterPos.x) / 2, (startCenterPos.y + endCenterPos.y) / 2 };
    
    float radiusX = std::abs(endCenterPos.x - startCenterPos.x) / 2.0f;
    float radiusY = radiusX * 0.3f;

    float angle = topArc ? (t * 3.14159f) : ((1.0f - t) * 3.14159f); // 0~π

    Vector2 pos;
    pos.x = center.x + radiusX * cos(angle);
    
    if(topArc)
        pos.y = center.y - radiusY * sin(angle);
    else
        pos.y = center.y + radiusY * sin(angle);

    return CenterToTopLeft(pos);
}