#pragma once
#include "Common/Common.h"
#include <string>

namespace MinigameEngine
{
	class NAOMI_API FileIO
	{
	public:
		static std::string ReadFile(const std::string& path);
		static void RemoveCR(std::string& buffer);
		static std::string ReadFixedWidthText(const std::string& path, size_t width);
	};
}