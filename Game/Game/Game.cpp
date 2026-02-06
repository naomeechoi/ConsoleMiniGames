#include "Game.h"
#include "Core/LevelManager.h"
#include "SystemLevels/StartLevel.h"
#include "SystemLevels/MenuLevel.h"
#include "SystemLevels/LoadingLevel.h"
#include "SystemLevels/GameResultLevel.h"
#include "SpotTheDifference/SpotTheDifferenceLevel.h"
#include "CardMonte/CardMonteLevel.h"
#include "Common/LevelType.h"

Game* Game::instance = nullptr;
Game* Game::Get() { return Game::instance; };

Game::Game()
{
	instance = this;
	GetLevelManager()->AddLevel((int)LevelType::Start, std::make_unique<StartLevel>());
	GetLevelManager()->AddLevel((int)LevelType::Menu, std::make_unique<MenuLevel>());
	GetLevelManager()->AddLevel((int)LevelType::Loading, std::make_unique<LoadingLevel>());
	GetLevelManager()->AddLevel((int)LevelType::SpotTheDifference, std::make_unique<SpotTheDifferenceLevel>());
	GetLevelManager()->AddLevel((int)LevelType::CardMonte, std::make_unique<CardMonteLevel>());
	GetLevelManager()->AddLevel((int)LevelType::GameResult, std::make_unique<GameResultLevel>());

	GetLevelManager()->SetLevel((int)LevelType::Start);

	onMenu.push_back(std::make_pair((int)LevelType::Start, "Start Screen"));
	onMenu.push_back(std::make_pair((int)LevelType::SpotTheDifference, "Spot The Difference"));
	onMenu.push_back(std::make_pair((int)LevelType::CardMonte, "CardMonte"));
	onMenu.push_back(std::make_pair((int)LevelType::Quit, "Quit"));
}

Game::~Game()
{
}
