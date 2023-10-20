// Lic:
// Luna's Father
// Renc - Layer Settings
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

#include <june19.hpp>

using namespace Slyvina;
using namespace June19;

namespace LunaRenc {
	static j19gadget
		* LayerSelector{nullptr};

	void LayerSelectorDraw(j19gadget* g, j19action) {
		static double hue{ 0 }; hue = hue + .1; if (hue > 360) hue = 0;
		g->SetForegroundHSV(hue, 1, 1);
		g->SetBackgroundHSV(hue, 1, .2);
	}

	void LayerInit(j19gadget* g) {
		LayerSelector = CreateListBox(0, 0, 250, g->H(), g);
		LayerSelector->CBDraw = LayerSelectorDraw;
	}
}