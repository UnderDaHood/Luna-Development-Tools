// Lic:
// Luna's Father
// Random Encounters
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
// Version: 23.10.21
// EndLic
#include <june19.hpp>

#include <SlyvDir.hpp>

#include "Renc_Config.hpp"
#include "Renc_RandomEncounters.hpp"
#include "Renc_MapData.hpp"

#define MData _RencData[_CurrentMap]

using namespace Slyvina;
using namespace Units;
using namespace June19;

namespace LunaRenc {

	const int midmargin = 25;
	const int listheight = 250;

	static VecString Foes{nullptr};
	static std::map<std::string, GINIE> _RencData{};
	static std::string _CurrentMap;

	static j19gadget
		* FoeYes{ nullptr },
		* FoeNo{ nullptr },
		* BAdd{ nullptr },
		* BRem{ nullptr },
		* FoePanel{ nullptr },
		* NewGamePlusOnly{ nullptr };

	struct QTB {
		std::string cat{}, key{};
		j19gadget* box{ nullptr };
	};
	std::vector<QTB> QuickTextBoxes{};


	static void UpdateFoeListBoxes() {
		auto havefoe{ MData->List("GENERAL","HaveFoes") };
		FoeNo->ClearItems();
		FoeYes->ClearItems();
		for (auto f : *Foes) {
			auto foe{ Upper(StripExt(f)) };
			auto ext{ Upper(ExtractExt(f)) };
			if (VectorContains(*havefoe, foe)) {
				FoeYes->AddItem(foe);
			} else {
				FoeNo->AddItem(foe);
			}
		}
	}

	static void QTBAct(j19gadget* g, j19action) {
		for (auto q : QuickTextBoxes) {
			if (q.cat == "*FOE")
				MData->Value(FoeYes->ItemText(), q.key, g->Text);
			else
				MData->Value(q.cat, q.key, g->Text);
		}
	}

	static void AutoUpdate(j19gadget* g, j19action a) {
		auto umap{ Upper(ChosenMap()) };
		// Update Foe lists
		if (umap != _CurrentMap) {
			_CurrentMap = umap;
			if (!_RencData.count(umap)) {
				auto fl{ MapStuffDir() + "/" + umap + "/RENC.INI" };
				_RencData[umap] = LoadOptGINIE(fl, fl, "# Random ecounter data for Luna's Father\n(c) Jeroen P. Broks");
			}
			MData->NewValue("General", "Music", "Music/Combat/Renc/Default/");
			UpdateFoeListBoxes();
			
		}
		// Buttons active
		BAdd->Enabled = FoeNo->SelectedItem() >= 0;
		BRem->Enabled = FoeYes->SelectedItem() >= 0;

		// Show work gadgets
		FoePanel->Visible = BRem->Enabled;

	}

	static void RencNoAction(j19gadget* g, j19action a) {
		if (a == j19action::DoubleClick) {
			//printf("Gonna add selected item %d\n", (int)g->SelectedItem());
			if (g->SelectedItem() >= 0) {
				std::cout << "Adding foe " << g->ItemText() << " to map " << _CurrentMap << "\n";
				MData->Add("GENERAL", "HaveFoes", g->ItemText(), true, true);
				UpdateFoeListBoxes();
			}
		}
	}
	static void BAddView(j19gadget* g, j19action) { g->X((g->GetParent()->W() / 2) - (g->W() / 2)); }
	static void BRemView(j19gadget* g, j19action) { BAddView(g, j19action::Draw); g->Y(listheight - g->H()); }
	static void RencAddClick(j19gadget*, j19action a) { RencNoAction(FoeNo, j19action::DoubleClick); }

	static void RencRemClick(j19gadget*, j19action a) { MData->Remove("GENERAL", "HaveFoes", FoeYes->ItemText()); UpdateFoeListBoxes(); };

	static void DCheckbox(j19gadget* g, j19action a) {
		if (g->checked) {
			g->SetForeground(180, 255, 0, 255);
			g->Caption = "True";
		} else {
			g->SetForeground(255, 0, 0, 255);
			g->Caption = "False";
		}
	}

	static void CheckNGP(j19gadget* g, j19action) {
		if (g->checked)
			MData->Value(FoeYes->ItemText(), "NewGamePlusOnly", "True");
		else
			MData->Value(FoeYes->ItemText(), "NewGamePlusOnly", "False");
	}

	static void QuickBox(std::string caption,std::string c, std::string k,int y,j19gadget*parent=nullptr) {
		if (parent == nullptr) parent = FoePanel;
		auto Label{ CreateLabel(caption,0,y,200,20,parent) }; 
		auto w{ 200 }; if (Upper(k) == "MUSIC") w = 600;
		Label->SetForeground(0, 180, 255);
		QTB DTA{ c,k,CreateTextfield(200,y,w,parent) };
		DTA.box->CBAction = QTBAct;
		DTA.box->SetForeground(255, 180, 0);
		DTA.box->SetBackground(25, 18, 0);
		QuickTextBoxes.push_back(DTA);
	}

	static void SelectFoe(j19gadget* g, j19action) {
		for (auto q : QuickTextBoxes) {
			if (q.cat == "*FOE")
				q.box->Text = MData->Value(g->ItemText(), q.key);
			else
				q.box->Text = MData->Value(q.cat, q.key);
		}
		NewGamePlusOnly->checked = Upper(MData->Value(g->ItemText(), "NewGamePlusOnly")) == "TRUE";
	}

	void Init_Renc(Slyvina::June19::j19gadget* panel) {
		int midx{ panel->W() / 2 };
		int midy{ listheight / 2 };
		panel->CBDraw = AutoUpdate;
		FoeNo = CreateListBox(0, 0, midx - midmargin, listheight, panel);
		FoeYes = CreateListBox(midx + midmargin, 0, midx - midmargin, listheight, panel);
		FoeNo->SetForeground(255, 0, 0, 255);
		FoeNo->SetBackground(25, 0, 0, 255);
		FoeNo->CBAction = RencNoAction;
		FoeYes->SetForeground(0, 255, 0, 255);
		FoeYes->SetBackground(0, 25, 0, 255);
		FoeYes->CBAction = SelectFoe;
		BAdd = CreateButton(">>", midx - midmargin, 0, panel);
		BAdd->CBDraw = BAddView;
		BAdd->CBAction = RencAddClick;
		BAdd->SetForeground(0, 255, 0, 255);
		BAdd->SetBackground(0, 25, 0, 255);
		BRem = CreateButton("<<", midx - midmargin, 100, panel);
		BRem->CBDraw = BRemView;
		BRem->CBAction = RencRemClick;
		BRem->SetForeground(255, 0, 0, 255);
		BRem->SetBackground(25, 0, 0, 255);
		Foes = GetTree(FoesDir());
		FoePanel = CreatePanel(0, listheight, panel->W(), panel->H() - listheight, panel);
		FoePanel->BA = 0;
		QuickBox("Easy mode", "*FOE", "RATE1", 0);
		QuickBox("Regular mode", "*FOE", "RATE2", 21);
		QuickBox("Hard mode", "*FOE", "RATE3", 42);
		CreateLabel("New Game+ only", 0, 63, 200, 20, FoePanel)->SetForeground(0, 180, 255, 255);
		NewGamePlusOnly = CreateCheckBox("", 200, 63, 200, 20, FoePanel);
		NewGamePlusOnly->CBDraw = DCheckbox;
		NewGamePlusOnly->CBAction = CheckNGP;
		QuickBox("Music", "GENERAL", "MUSIC", 120);
		QuickBox("Arena", "GENERAL", "Arena", 151);
	}
}