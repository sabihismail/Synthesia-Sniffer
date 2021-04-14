#include "Settings.h"

constexpr auto SETTINGS_FILE = "settings.json";

Settings::Settings()
{
	if (std::filesystem::exists(SETTINGS_FILE))
	{
		std::ifstream stream(SETTINGS_FILE);
		json obj = json::parse(stream);

		songFormat = obj.value(nameof(songFormat), songFormat);
	}

	json obj
	{
		{ nameof(songFormat), songFormat }
	};

	std::ofstream file(SETTINGS_FILE);
	file << obj.dump(4);
}
