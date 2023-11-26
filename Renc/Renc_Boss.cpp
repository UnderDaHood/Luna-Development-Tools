// Lic:
// Luna's Father
// Renc - Boss
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
// Version: 23.11.11
// EndLic

#include <TQSE.hpp>

#include <SlyvTime.hpp>
#include <SlyvQCol.hpp>

#include <june19.hpp>

#include "Renc_MapData.hpp"
#include "Renc_Config.hpp"
#include <SlyvDir.hpp>

using namespace Slyvina;
using namespace June19;
using namespace Units;
using namespace TQSE;
using namespace std;

namespace LunaRenc {

	map<string, GINIE> BossBigReg{};


	string _CurrentMap{ "*******" };

	GINIE BossRec() { return BossBigReg[_CurrentMap]; }
	j19gadget
		* gBossFightRegister{ nullptr },
		* gBossFightNewField{ nullptr },
		* gBossFightNewButton{ nullptr },
		* gBossFightPanel{ nullptr },
		* gBossFightEnemyReg{ nullptr };

	map<string, j19gadget*> BossData{};

	string BossFile(string umap) {
		auto ret{ MapStuffDir() + "/" + umap + "/Boss.ini" };
		if (!FileExists(ret)) {
			QCol->Doing("Creating", ret);
			SaveString(ret, "[*sys*]\nCreated=" + CurrentDate() + "; " + CurrentTime());
		}
		return ret;
	}
	string BossFile() { return BossFile(_CurrentMap); };

	string BossFight() { return gBossFightRegister->ItemText(); }

	static void RefreshRegister() {
		auto reg{ BossBigReg[_CurrentMap]->List("*List*","List") };
		gBossFightRegister->ClearItems();
		for (auto& bss : *reg) gBossFightRegister->AddItem(bss);
	}

	static void BossTabDraw(j19gadget* g, j19action) {
		auto umap{ Upper(ChosenMap()) };
		if (umap != _CurrentMap) {
			if (!BossBigReg.count(umap)) {
				QCol->Doing("Loading", BossFile(umap));
				BossBigReg[umap] = LoadGINIE(BossFile(umap), BossFile(umap), "Boss fights for map: " + umap + "\n(c) Jeroen P. Broks");
			}
			_CurrentMap = umap;
			RefreshRegister();
		}
	}

	static void aNewBossFight(j19gadget* g, j19action) {
		g->Enabled = Trim(gBossFightNewField->Text) != ""; 
		g->Y(g->GetParent()->H() - g->H());
	}
	static void NewBossFight(j19gadget*,j19action){
		auto NBF{ gBossFightNewField->Text };
		if (VecHasString(BossRec()->List("*List*", "List"), NBF)) {
			Notify("A fight tagged as \"" + NBF + "\" already exists");
			return;
		}
		BossRec()->Add("*List*", "List", NBF, true, true);
		gBossFightNewField->Text = "";
		RefreshRegister();
	}

	static void PickFight(j19gadget* g, j19action) {
		if (g->SelectedItem() < 0) { gBossFightPanel->Visible = false; return; }
		gBossFightPanel->Visible = true;		
		BossData["TUNE"]->Text = BossRec()->NewValue(BossFight(), "Tune", "MUSIC/COMBAT/BOSS/GENTUNES/");
		BossData["HEAD"]->Text = BossRec()->Value(BossFight(), "Head");
		BossData["NAME"]->Text = BossRec()->Value(BossFight(), "Name");
		BossRec()->NewValue(BossFight(), "Announce_R", to_string(255));
		BossRec()->NewValue(BossFight(), "Announce_G", to_string(0));
		BossRec()->NewValue(BossFight(), "Announce_B", to_string(0));
		for (uint32 i = 0; i < 9; ++i) {
			for (byte j = 1; j <= 3; j++) {
				auto Tag{ TrSPrintF("SLOT%d_SKILL%d", i, j) };
				BossData[Tag]->checked = Upper(BossRec()->NewValue(BossFight(), Tag, "True")) == "TRUE";
			}
			auto FTag{ TrSPrintF("SLOT%d_FOE", i) };
			BossData[FTag]->Text = BossRec()->Value(BossFight(), FTag);
		}
	}

	static void EnemyFieldChange(j19gadget* g, j19action a) {
		for (auto iD : BossData) {
			switch (iD.second->GetKind()) {
			case j19kind::CheckBox:
				BossRec()->Value(BossFight(), iD.first, boolstring(iD.second->checked)); 
				break;
			case j19kind::Textfield:
				BossRec()->Value(BossFight(), iD.first, iD.second->Text);
				break;
			default:
				QCol->Error("Unknown field kind!");
				break;
			}
		}
	}

	static void CanPickEnemy(j19gadget* g, j19action a) {
		auto active{ j19gadget::ActiveGadget() };
		if (!active) { g->Enabled = false; return; }
		//cout << "CPE: " << (int)active << "CBA::" << (int)active->CBAction << "<==>" << (int)EnemyFieldChange << "\n"; // HARD DEBUG!
		if (active->CBAction!=EnemyFieldChange) { g->Enabled = false; return; }			
		switch (a) {
		case j19action::DoubleClick:
			if (g->SelectedItem() < 0) return;
			active->Text = g->ItemText();
			EnemyFieldChange(active, j19action::Type);
			break;
		case j19action::Draw:
			g->Enabled = true;
			break;
		default:
			cout << "?CPE: a = " << (int)a << "/" << (int)j19action::DoubleClick << endl;
			break;
		}
	}

	
	void BossInit(j19gadget* g) {
		g->CBDraw = BossTabDraw;
		gBossFightRegister = CreateListBox(0, 0, 250, g->H() - 60, g);
		gBossFightRegister->SetForeground(255, 255, 255, 255);
		gBossFightRegister->SetBackground(127, 0, 0, 255);
		gBossFightRegister->CBAction = PickFight;
		gBossFightNewField = CreateTextfield(0, g->H() - 60, 255, 30, g);
		gBossFightNewField->SetForeground(0, 180, 255, 255);
		gBossFightNewField->SetBackground(0, 0, 255, 255);
		gBossFightNewButton = CreateButton("Create new fight", 0, gBossFightNewField->DrawY() + gBossFightNewField->H(), g);
		gBossFightNewButton->SetForeground(180, 255, 0);
		gBossFightNewButton->SetBackground(0, 25, 0);
		gBossFightNewButton->CBDraw = aNewBossFight;
		gBossFightNewButton->CBAction = NewBossFight;
		gBossFightPanel = CreateGroup(250, 0, g->W() - 250, g->H(), g);
		gBossFightPanel->Visible = false;
		uint32 y{ 0 };
		for (uint32 i = 0; i < 9; ++i) {
			CreateLabel(to_string(i + 1) + ".", 3, i * 30, 30, 30, gBossFightPanel);
			for (byte j = 1; j <= 3; j++) {
				BossData[TrSPrintF("SLOT%d_SKILL%d", i, j)] = CreateCheckBox("", j * 30, i * 30, 30, 30, gBossFightPanel);
				//BossData[TrSPrintF("SLOT%d_SKILL%d", i, j)]->CBAction = EnemyFieldChange;
			}
			BossData[TrSPrintF("SLOT%d_FOE", i)] = CreateTextfield(120, i * 30, 400, gBossFightPanel);
			//BossData[TrSPrintF("SLOT%d_FOE", i)]->CBAction = EnemyFieldChange;
			y = (i * 30) + 30;
		}
		CreateLabel("Boss Head:", 5, y, 250,30, gBossFightPanel);
		BossData["HEAD"] = CreateTextfield(250, y, 300, gBossFightPanel); y += 30;
		CreateLabel("Boss Name:", 5, y, 250, 30, gBossFightPanel);
		BossData["NAME"] = CreateTextfield(250, y, 300, gBossFightPanel); y += 30;
		CreateLabel("Boss Tune:", 5, y, 250, 30, gBossFightPanel);
		BossData["TUNE"] = CreateTextfield(250, y, 300, gBossFightPanel); y += 30;
		for (auto it : BossData) it.second->CBAction = EnemyFieldChange;
		gBossFightEnemyReg = CreateListBox(5, y, 500, g->H() - (y + 10), gBossFightPanel);
		gBossFightEnemyReg->SetForeground(255, 255, 0, 255);
		gBossFightEnemyReg->SetBackground(25, 25, 0, 255);
		gBossFightEnemyReg->CBDraw = CanPickEnemy;
		gBossFightEnemyReg->CBAction = CanPickEnemy;		
		gBossFightEnemyReg->ClearItems();
		auto Foes{ GetTree(FoesDir()) };
		for (auto f : *Foes) gBossFightEnemyReg->AddItem(StripExt(f));
	}
}