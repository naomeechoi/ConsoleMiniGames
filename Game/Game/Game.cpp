#include "Game.h"
#include "Core/LevelManager.h"
#include "PreGameLevel/StartLevel.h"
#include "PreGameLevel/MenuLevel.h"
#include "PreGameLevel/LoadingLevel.h"
#include "SpotTheDifference/SpotTheDifferenceLevel.h"
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

	GetLevelManager()->SetLevel((int)LevelType::Start);

	onMenu.push_back(std::make_pair((int)LevelType::Start, "Start Screen"));
	onMenu.push_back(std::make_pair((int)LevelType::SpotTheDifference, "Spot The Difference"));
	onMenu.push_back(std::make_pair((int)LevelType::Quit, "Quit"));
}

Game::~Game()
{
}
