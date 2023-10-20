// Lic:
// Luna's Father
// Renc - User Interface
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

#include <TQSG.hpp>
#include <TQSE.hpp>

#include <june19.hpp>

#include "Renc_UserInterface.hpp"
#include "Renc_Config.hpp"
#include "Renc_MapData.hpp"
#include "Renc_RandomEncounters.hpp"
#include "Renc_LayerSettings.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace TQSE;
using namespace June19;

namespace LunaRenc {
	namespace UI {
		static TImageFont DefaultFont{nullptr};

		static j19gadget
			* ListKthura{ nullptr },
			* WorkPanel{ nullptr },
			* RencPanel{ nullptr },
			* BossPanel{ nullptr },
			* LayerPanel{ nullptr };

		static std::map<std::string, j19gadget*> TypePanels;

		static void ShowWork(j19gadget*, j19action) { WorkPanel->Visible = ChosenMap() != ""; }

		static void ShowType(j19gadget* g, j19action) { TypePanels[g->Caption]->Visible = g->checked; }

		static j19gadget* MakeType(std::string Capt, int x, byte r, byte g, byte b,bool checked=false) {
			auto radio = CreateRadioButton(Capt, x, 2, 100, 20, WorkPanel, checked);
			radio->SetForeground(180, 255, 0, 255);
			radio->CBDraw = ShowType;
			TypePanels[Capt] = CreatePanel(0, 30, WorkPanel->W(), WorkPanel->H() - 30,WorkPanel);
			TypePanels[Capt]->SetBackground(r, g, b);
			return TypePanels[Capt];
		}


		void Init(std::string MyDir) {
			AssignConfigFile(MyDir + "/Renc.Config.ini");
			Graphics(1200, 1000, "Luna's Father - Random Encounters");
			DefaultFont = LoadImageFont(DefaultFontFile());
			j19gadget::SetDefaultFont(DefaultFontFile());
			ListKthura = CreateListBox(0, 0, 250, WorkScreen()->H(), WorkScreen());
			ListKthura->SetForeground(180, 0, 255);
			ListKthura->SetBackground(18, 0, 25);
			ListKthura->CBDraw = ShowWork;
			WorkPanel = CreatePanel(250, 0, WorkScreen()->W() - 250, WorkScreen()->H(),WorkScreen());
			WorkPanel->SetBackground(18, 25, 0, 255);
			RencPanel = MakeType("Random Encounters", 0, 0, 0, 127, true);
			BossPanel = MakeType("Boss Fights", 255, 25, 0, 0);
			LayerPanel = MakeType("Field settings", 512, 18, 25, 0);
			Init_Renc(RencPanel);
			LayerInit(LayerPanel);
			ScanForMaps(ListKthura);
			
		}

		void Run() {
			while (true) {
				Poll();
				Cls();
				WorkScreen()->Draw();
				Flip();
				if (AppTerminate()) return;
			}
		}

		void Done() {
			FreeJune19();
		}
	}
}