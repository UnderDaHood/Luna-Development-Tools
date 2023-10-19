// Lic:
// Luna's Father
// Renc - Map Data
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
// Version: 23.10.19
// EndLic

#include <SlyvDir.hpp>

#include "Renc_MapData.hpp"
#include "Renc_Config.hpp"

using namespace Slyvina;
using namespace Units;
using namespace June19;

namespace LunaRenc {

	std::string _ChosenMap{""};


	static void PickMap(j19gadget* g, j19action) {
		if (g->SelectedItem() < 0)
			_ChosenMap = "";
		else
			_ChosenMap = g->ItemText();
	}

	std::string ChosenMap() {
		return _ChosenMap;
	}

	void LunaRenc::ScanForMaps(Slyvina::June19::j19gadget* g) {
		auto mapsdir{ DirKthura() };
		auto maps{ FileList(mapsdir) };
		g->ClearItems();
		g->CBAction = PickMap;
		for (auto m : *maps) g->AddItem(m);
		
	}

}