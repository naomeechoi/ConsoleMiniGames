#define NOMINMAX
#include "CardMonteLevel.h"
#include "CardMonteMode.h"
#include "Render/Renderer.h"
#include "System/Input.h"
#include "System/FileIO.h"
#include "Common/GameCommon.h"
#include "UI/UITop.h"
#include "UI/UILoadingBar.h"
#include "UI/UIColorEffect.h"
#include "UI/UIMessage.h"
#include "Util/Random.h"
#include "Util/Delete.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>

const int CARD_LEFT_X_OFFSET = 10;
const int UI_BOTTOM_OFFSET_Y = 3;
const float BLANK_TIME = 4.0f;
const int BLANK_COUNT = 20;
const int MESSAGE_UI_OFFSET_X = 5;

using namespace MinigameEngine;

CardMonteLevel::CardMonteLevel()
{
	LoadSetting();
}

CardMonteLevel::~CardMonteLevel()
{
    OnExit();

    SafeDelete(topUI);
    SafeDelete(loadingBarUI);
    SafeDelete(colorEffectUI);
    SafeDelete(messageUI);
    SafeDelete(mode);
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
        topUI = new UITop(displaySize.x, Vector2(UI_START_POS_X, UI_START_POS_Y), "Card Monte");

    if (!loadingBarUI)
        loadingBarUI = new UILoadingBar(Vector2(UI_START_POS_X, displaySize.y - UI_BOTTOM_OFFSET_Y), (float)(displaySize.x - MESSAGE_UI_OFFSET_X), playTime, '#');

    if (!colorEffectUI)
        colorEffectUI = new UIColorEffect(edgeColor, Color::LightRed, BLANK_TIME, BLANK_COUNT);
    
    if (!messageUI)
        messageUI = new UIMessage();

    if (!ValidCheck())
    {
        return;
    }

    SetAnswer();
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

    if (colorEffectUI)
    {
        colorEffectUI->Tick(deltaTime);
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
        /*if (curState == &CardMonteLevel::StateChoose)
        {
            if ((mode->Check(cards[i].num)))
                bgColor = Color::Red;
        }*/

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
        assert(false && "/Assets/CardMonte/setting.txt cannot open");
        return;
    }

    std::string line;
    std::string currentSprite;
    bool readingImage = false;

    while (std::getline(file, line)) {
        FileIO::RemoveCR(line);

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
            cardWidth = stoi(line.substr(line.find('=') + 1));
        }
        else if (line.find("cardHeight") != std::string::npos) {
            cardHeight = stoi(line.substr(line.find('=') + 1));
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
            currentSprite += line.substr(line.find('=') + 1) + "\n";
            readingImage = true;
        }
        else if (readingImage) {
            currentSprite += line + "\n";
        }
    }

    if (!currentSprite.empty()) {
        cardSprites.push_back(currentSprite);
    }
}

void CardMonteLevel::CardSetting()
{
    float remainingSpace = float(displaySize.x - 2 * CARD_LEFT_X_OFFSET - cardCount * cardWidth);
    float cardGap = (cardCount > 1) ? remainingSpace / (cardCount - 1) : 0.0f;

    float posY = float(displaySize.y / 2 - cardHeight / 2); // 세로 중앙

    cards.clear();
    for (int i = 0; i < cardCount; i++) {
        SCard card;
        card.num = i + 1;
        card.pos.x = CARD_LEFT_X_OFFSET + i * (cardWidth + (int)cardGap);
        card.pos.y = (int)posY;
        card.originPos = card.pos;
        cards.push_back(card);
    }

    cardMidIdx = (int)cardSprites[0].size() / 2 - 1;
}

void CardMonteLevel::SetAnswer()
{
    if (!ValidCheck())
        return;

    int answerCardIdx = Random::Random(0, (int)cards.size() - 1);
    int answer = cards[answerCardIdx].num;
    mode->SetAnswer(answer);
    std::string findMsg = message + std::to_string(answer);
    messageUI->Start(displaySize.x - 2, Vector2(3, displaySize.y - MESSAGE_UI_OFFSET_X), findMsg, " ");
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

    if (colorEffectUI)
        colorEffectUI->Stop();

    if (messageUI)
        messageUI->Clear();
}

bool CardMonteLevel::ValidCheck()
{
    const int pointSize = 5;
    void* checks[pointSize] = { mode, topUI, loadingBarUI, colorEffectUI, messageUI };
    for (int i = 0; i < pointSize; i++)
    {
        if (checks[i] == nullptr)
        {
            RequestChangeLevel((int)LevelType::Menu);
            return false;
        }
    }

    return true;
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
    if (stateTimer.IsTimeOut())
        ChangeState(&CardMonteLevel::StateFilp, showingTime);
}

void CardMonteLevel::StateFilp(float deltatime)
{
    stateTimer.Tick(deltatime);
    
    FlipCard(false);
    if (stateTimer.IsTimeOut())
    {
        spriteIdx = (int)cardSprites.size() - 1;
        ChangeState(&CardMonteLevel::StateShuffle, cardFilpTime);
    }
}

void CardMonteLevel::StateShuffle(float deltatime)
{
    stateTimer.Tick(deltatime);
    float t = stateTimer.GetRatio();

    const SShufflePair& pair = shufflePairs[currentShuffleIdx];

    SCard* cardA = nullptr;
    SCard* cardB = nullptr;
    for (auto& card : cards)
    {
        if (card.num == pair.a) cardA = &card;
        else if (card.num == pair.b) cardB = &card;
    }

    if (!cardA || !cardB)
        return;

    cardA->pos = CircularLerp(cardA->originPos, cardB->originPos, t, true);
    cardB->pos = CircularLerp(cardB->originPos, cardA->originPos, t, false);

    if (stateTimer.IsTimeOut())
    {
        // put each other's pos
        Vector2 tmpOrigin = cardA->originPos;
        cardA->pos = cardB->originPos;
        cardB->pos = tmpOrigin;

        // exchange originPos
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

    if (colorEffectUI)
    {
        colorEffectUI->Start();
    }

    if (stateTimer.IsTimeOut())
    {
        isSuccess = false;
        ChangeState(&CardMonteLevel::StateGameOver, cardFilpTime);
    }
}

void CardMonteLevel::StateGameOver(float deltatime)
{
    stateTimer.Tick(deltatime);

    FlipCard(true);
    if (stateTimer.IsTimeOut())
    {
        spriteIdx = 0;
        ChangeState(&CardMonteLevel::StateWaitToExit, showingTime);
    }
}

void CardMonteLevel::StateWaitToExit(float deltatime)
{
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

        SShufflePair pair;
        pair.a = numA;
        pair.b = numB;

        shufflePairs.push_back(pair);
    }
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

Vector2 CardMonteLevel::CircularLerp(const Vector2& start, const Vector2& end, float t, bool topArc)
{
    Vector2 startCenterPos = GetCenter(start);
    Vector2 endCenterPos = GetCenter(end);

    Vector2 center = { (startCenterPos.x + endCenterPos.x) / 2, (startCenterPos.y + endCenterPos.y) / 2 };
    
    float radiusX = std::abs(endCenterPos.x - startCenterPos.x) / 2.0f;
    float radiusY = radiusX * 0.3f;

    float angle = topArc ? (t * 3.14159f) : ((1.0f - t) * 3.14159f);

    Vector2 pos;
    pos.x = (int)(center.x + radiusX * cos(angle));
    
    if(topArc)
        pos.y = (int)(center.y - radiusY * sin(angle));
    else
        pos.y = (int)(center.y + radiusY * sin(angle));

    return CenterToTopLeft(pos);
}

void CardMonteLevel::FlipCard(bool isOpen)
{

    float t = stateTimer.GetRatio();
    int maxIdx = (int)cardSprites.size() - 1;
    int flipIdx = isOpen ? (int)((1 - t) * maxIdx) : (int)(t * maxIdx);
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
        isSuccess = mode->Check(chosenNum);

        ChangeState(&CardMonteLevel::StateGameOver, cardFilpTime);

        if (loadingBarUI)
            loadingBarUI->Stop();
    }
}