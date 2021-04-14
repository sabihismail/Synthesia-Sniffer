#pragma once

#include "../../libs/json/json.hpp"
#include <string>
#include <filesystem>
#include <fstream>

#define nameof(name) #name

using json = nlohmann::json;

class Settings
{
public:
	std::string songFormat = "$? - $title - $artist";

	Settings();

private:

};

