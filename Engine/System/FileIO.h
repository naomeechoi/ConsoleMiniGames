#pragma once
#include "Common/Common.h"
#include <string>

namespace MinigameEngine
{
	class NAOMI_API FileIO
	{
	public:
		static std::string ReadFileFast(const std::string& path);
		static void RemoveCR(std::string& buffer);
	};
}