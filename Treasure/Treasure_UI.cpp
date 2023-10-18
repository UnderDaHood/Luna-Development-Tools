// Lic:
// Luna's Father
// Treasure Editor User Interface
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
// Version: 23.10.15
// EndLic

#include <SlyvDir.hpp>
#include <SlyvQCol.hpp>

#include <june19.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>

#include "Treasure_UI.hpp"
#include "Treasure_Config.hpp"
#include "Treasure_Random.hpp"
#include "Treasure_Chest.hpp"

using namespace Slyvina;
using namespace Slyvina::TQSE;
using namespace Slyvina::TQSG;
using namespace Slyvina::Units;
using namespace Slyvina::June19;

namespace Luna_Treasure {

	//*
	j19gadget
		* Back{ nullptr },
		* MapList{ nullptr },
		* TabPanel{ nullptr };
	//*/

#pragma region Tabs
	void TabClick(j19gadget* g, j19action);

	typedef void (*FChosenMap)(std::string);

	class TTab {
		static uint64 count;
	public:
		static std::map<uint64, std::shared_ptr<TTab>> Tabs;
		static uint64 Chosen;
		uint64 id{ count++ };
		std::string Title{};
		TTab(std::string _Title);
		j19gadget* Button;
		j19gadget* Panel{ nullptr };
		FChosenMap ChosenMap{ nullptr };
	};

	uint64 TTab::count{0};
	uint64 TTab::Chosen{0};
	std::map<uint64, std::shared_ptr<TTab>> TTab::Tabs{};
	TTab::TTab(std::string _Title) {
		Title = _Title;
		Tabs[id] = std::shared_ptr<TTab>(this);
		Button = CreateButton(_Title, 0, 0, Back);
		Button->SetFont(ButtonFont());
		Button->CBAction = TabClick;
	}
#pragma endregion

#pragma region CallBacks
	void UpdateTabs(j19gadget*, j19action) {
		for (auto d : TTab::Tabs) {
			//printf("%d -> %s\n", (int)d.first, d.second->Title.c_str()); // debug
			if (d.first == 0) {
				d.second->Button->X(TabPanel->X());
			} else {
				d.second->Button->X(TTab::Tabs[d.first - 1]->Button->DrawX() + TTab::Tabs[d.first - 1]->Button->W());
			}
			d.second->Button->Y((TabPanel->DrawY() - (d.second->Button->H()-1)));
			if (!d.second->Panel) {
				d.second->Panel = CreatePanel(5, 5, TabPanel->W() - 10, TabPanel->H() - 10, TabPanel);
				d.second->Panel->SetForeground(255, 180, 0);
				d.second->Panel->SetBackground(25, 18, 0);
			}
			if (d.first == TTab::Chosen) {
				d.second->Button->SetBackground(255, 180, 0, 255);
				d.second->Button->SetForeground(25, 18, 0, 255);
				d.second->Panel->Visible = true;
			} else {
				d.second->Button->SetForeground(255, 180, 0, 255);
				d.second->Button->SetBackground(25, 18, 0, 255);
				if (d.second->Panel) d.second->Panel->Visible = false;
			}
		}
		TabPanel->Enabled = MapList->SelectedItem() >= 0;
	}

	void TabClick(j19gadget* g, j19action) {
		for (auto d : TTab::Tabs) {
			if (g->Caption == d.second->Title) {
				TTab::Chosen = d.first;
			}
		}
	}

	void SelectedMap(j19gadget* lb, j19action) {
		if (lb->SelectedItem() < 0) return;
		for (auto d : TTab::Tabs) {
			if (!d.second->ChosenMap) {
				QCol->Error("\7No 'ChosenMap' function for tab " + d.second->Title);
			} else {
				d.second->ChosenMap(lb->ItemText());
			}
		}
	}
#pragma endregion

	void Luna_Treasure::CreateUI() {
		//*
		Graphics(WindowSizes().w, WindowSizes().h, "Luna's Father - Map Treasures");
		Cls(); Poll(); Flip();
		j19gadget::SetDefaultFont(DefaultFont());
		Back = CreatePanel(0, 0, WindowSizes().w, WorkScreen()->H(), WorkScreen());
		Back->SetBackground(25, 18, 0,255);
		MapList = CreateListBox(5, 5, 200, Back->H() - 10, Back);
		MapList->SetForeground(255, 180, 0);
		MapList->SetBackground(50, 36, 0);
		MapList->ClearItems();
		MapList->SetFont(FontForKthuraList());
		MapList->CBAction = SelectedMap;

		auto maps{ FileList(DirKthura()) };
		for (auto m : *maps) MapList->AddItem(m);

		new TTab("Random");
		new TTab("Chests");

		TabPanel = CreatePanel(MapList->W() + 25, 40, Back->W() - MapList->W() - 25, Back->H() - 60, Back);
		TabPanel->SetBackground(255, 180, 0, 255);
		TabPanel->CBDraw = UpdateTabs;
		UpdateTabs(nullptr, j19action::Unknown);

		InitRandomTreasure(TTab::Tabs[0]->Panel);
		TTab::Tabs[0]->ChosenMap = GetRandomTreasures;

		InitTreasureChest(TTab::Tabs[1]->Panel);
		TTab::Tabs[1]->ChosenMap = GetTreasureChests;
		//*/
	}

	void RunUI() {
		//*
		Cls();
		do {
			Cls();
			Poll();
			if (AppTerminate()) return;
			WorkScreen()->Draw();
			Flip();
		} while (true);
		//*/
	}

	void CloseUI() {
		FreeJune19();
	}
	
}