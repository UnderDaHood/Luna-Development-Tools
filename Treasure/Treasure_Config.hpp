// Lic:
// Luna's Father
// Configure Treasure Editor (header)
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
// Version: 23.10.16
// EndLic
#pragma once


#include <string>
#include <SDL.h>

#include <TQSG.hpp>

namespace Luna_Treasure {
	void RegConfigFile(std::string RCF);

	SDL_Rect WindowSizes();

	Slyvina::TQSG::TImageFont DefaultFont();
	Slyvina::TQSG::TImageFont FontForKthuraList();
	Slyvina::TQSG::TImageFont ButtonFont();
	Slyvina::TQSG::TImageFont SysFont();
	

	std::string DirKthura();
	std::string DirOutput();
	std::string DirItems();
}