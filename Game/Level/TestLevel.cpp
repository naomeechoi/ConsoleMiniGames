#include "TestLevel.h"
#include "Actor/Player.h"
#include "Actor/Wall.h"
#include "Actor/Box.h"
#include "Actor/Ground.h"
#include "Actor/Target.h"
#include <iostream>

/*
#: 벽
.: 바닥
p: 플레이어
b: 박스
t: 타겟
*/
TestLevel::TestLevel()
{
	//AddNewActor(new Player());
	LoadMap("Map.txt");
}

void TestLevel::LoadMap(const char* fileName)
{
	// 파일 로드
	char path[2048] = {};
	sprintf_s(path, 2048, "../Assets/%s", fileName);

	// 파일 열기
	FILE* file = nullptr;
	fopen_s(&file, path, "rt");

	// 예외 처리
	if (!file)
	{
		std::cerr << "Faild to open map file.\n";
		__debugbreak();
	}

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);
	// 맵 읽기
	char* data = new char[fileSize + 1];
	size_t readSize = fread(data, sizeof(char), fileSize, file);
	
	// 읽어온 문자열을 이용해 파싱
	int idx = -1;

	Wanted::Vector2 position = Vector2::Zero;

	while (true)
	{
		idx++;
		if (idx >= readSize)
			break;

		char mapChar = data[idx];

		position.x++;
		if (mapChar == '\n')
		{
			position.y++;
			position.x = 0;
			continue;
		}

		switch (mapChar)
		{
		case '1':
		case '#':
			AddNewActor(new Wall(position));
			break;
		
		case '.':
			AddNewActor(new Ground(position));
			break;

		case 'p':
			AddNewActor(new Player(position));
			AddNewActor(new Ground(position));
			break;

		case 'b':
			AddNewActor(new Box(position));
			AddNewActor(new Ground(position));
			break;

		case 't':
			AddNewActor(new Target(position));
			break;
		}
	}

	// TEST: 읽어온 데이터 임시로 출력
	//std::cout << data;
	delete[] data;
	fclose(file);
}
