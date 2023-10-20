// Lic:
// Luna's Father
// Renc - Config
// 
// 
// 
// (c) Jeroen P. Broks, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.10.20
// EndLic
#include <JCR6_Core.hpp>
#include <JCR6_zlib.hpp>

#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>
#include <SlyvAsk.hpp>

#include "Renc_Config.hpp"

using namespace Slyvina;
using namespace Units;

namespace LunaRenc {

	static GINIE _Config{nullptr};

	void AssignConfigFile(std::string _file) {
		printf("Renc - Copyright Jeroen P. Broks\n\n");
		if (!FileExists(_file)) SaveString(_file, "[Sys]\nCreated=" + CurrentDate() + "; " + CurrentTime());
		_Config = LoadGINIE(_file, _file, "Random encounter general configuration\n(c) Jeroen P. Broks");
	}


	std::string DefaultFontFile() {
		return Ask(_Config, "Fonts", "Default", "Please help me locate DOSFont.jfbf: ");
	}
	std::string DirKthura() {
		return Ask(_Config, "Directories", "Kthura", "\7Directory where Kthura maps are located: ");
	}
	std::string MapStuffDir() {
		return Ask(_Config, "Directories", "MapStuff", "\7Directory for 'MapStuff': ");
	}
	std::string FoesDir() {
		return Ask(_Config, "Directories", "Foes", "\7Directory for 'Foes': ");
	}
}