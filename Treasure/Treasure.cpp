// Lic:
// Luna's Father
// Treasure Editor
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
// Version: 23.12.15
// EndLic

#include <SDL.h>

#include <SlyvString.hpp>

#include <JCR6_Core.hpp>
#include <JCR6_zlib.hpp>
#include <JCR6_JQL.hpp>
#include <JCR6_RealDir.hpp>

#include "Treasure_Config.hpp"
#include "Treasure_UI.hpp"


using namespace Luna_Treasure;
using namespace Slyvina::Units;
using namespace Slyvina::JCR6;

int main(int ac, char** c) {
	JCR6_InitRealDir();
	std::cout << "Treasure for \"Luna's Father\"\nCoded by: Jeroen P. Broks\n\n\n";
	RegConfigFile(StripExt(c[0]) + ".ini");
	init_zlib();
	InitJQL();
	CreateUI();
	RunUI();

}