// Lic:
// Luna's Father
// Treasure Chest
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

#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvDir.hpp>
#include <SlyvTime.hpp>

#include <TQSG.hpp>

#include <june19.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_Save.hpp>

#include <JCR6_Core.hpp>

#include "Treasure_Config.hpp"
#include "Treasure_Chest.hpp"


using namespace Slyvina;
using namespace June19;
using namespace Kthura;
using namespace Units;
using namespace TQSG;
using namespace std;

namespace Luna_Treasure {
	// GFX\Textures\Treasure\Chest.jpbf
	const char* ChestTex{ "GFX/TEXTURES/TREASURE/CHEST.JPBF" };

	static j19gadget
		* MainPanel{ nullptr },
		* ScanButton{ nullptr },
		* ChestSelector{ nullptr },
		* ErrorMap{ nullptr },
		* EditPanel{ nullptr },
		* RadioCash{ nullptr },
		* ValueCash{ nullptr },
		* RadioItem{ nullptr },
		* ValueItem{ nullptr },
		* NewGamePlusOnly{ nullptr },
		* ItemSelector{ nullptr },
		* ItemGroup{ nullptr }, // I need a separate group for the radio buttons
		* ItemAll{ nullptr },
		* ItemIngredient{ nullptr },
		* ItemRegular{ nullptr };
	static VecString
		ItemList{nullptr};



	static Slyvina::Kthura::Kthura KMap{nullptr};
	static map<string, GINIE> ChestData{};
	static string ChosenMap;

	inline void GetItemList() {
		if (!ItemList) {
			auto _ItemList = FileList(DirItems());
			ItemList = NewVecString();
			for (auto il : *_ItemList) ItemList->push_back(StripExt(il));
		}
	}

	static void ScanButtonDraw(j19gadget*, j19action) {
		ScanButton->Y(std::min(ScanButton->Y() + 1, MainPanel->H() - ScanButton->H()));
		ErrorMap->Visible = !KMap;
		MainPanel->Enabled = !ErrorMap->Visible;
		ChestSelector->W(std::max(51, ScanButton->W() - 16));
		ChestSelector->H(std::max(1, ScanButton->Y() - 2));
		EditPanel->X(ChestSelector->W() + 20);
		EditPanel->W(MainPanel->W() - EditPanel->X());
		EditPanel->H(EditPanel->GetParent()->H());
		EditPanel->Enabled = ChestSelector->SelectedItem() >= 0;
		ValueCash->Visible = RadioCash->checked;
		ValueItem->Visible = RadioItem->checked;
		if (NewGamePlusOnly->FB > 0) NewGamePlusOnly->FB--; else if (NewGamePlusOnly->FG > 180) NewGamePlusOnly->FG--;
		ItemSelector->X(0);
		ItemSelector->W((int)floor((EditPanel->W()-ItemSelector->X()) * .75));
		ItemSelector->H(EditPanel->H() - 120);
		ItemSelector->Enabled = EditPanel->Enabled && RadioItem->checked;
		ItemGroup->H(ItemSelector->H());
		ItemGroup->X(ItemSelector->W() + 20);
		ItemGroup->Y(ItemSelector->Y());
		ItemGroup->W(EditPanel->W() - ItemGroup->X());
		ItemGroup->BR = std::min((byte)127u, ++ItemGroup->BR);
		ItemGroup->BG = std::min((byte)90u, ++ItemGroup->BG);
	}

	static string CTag() {
		if (ChestSelector->SelectedItem() < 0) return "***ERROR***";
		return "CHEST::" + ChestSelector->ItemText();
	}

	static void SelectorHit(j19gadget* g, j19action) {
		if (g->SelectedItem() < 0) return;
		auto CCD{ ChestData[ChosenMap] };
		RadioCash->checked = Upper(CCD->Value(CTag(), "Type")) == "CASH";
		RadioItem->checked = !RadioCash->checked;
		ValueCash->Text = CCD->Value(CTag(), "Cash");
		ValueItem->Text = CCD->Value(CTag(), "Item");
		NewGamePlusOnly->checked = Upper(CCD->Value(CTag(), "NewGame+Only")) == "TRUE";		
	}

	static void RadioHit(j19gadget* g, j19action) {
		auto CCD{ ChestData[ChosenMap] };
		CCD->Value(CTag(), "Type", g->Caption);
	}

	static void EditItem(j19gadget*, j19action) {
		auto CCD{ ChestData[ChosenMap] };
		CCD->Value(CTag(), "Item", ValueItem->Text);
	}

	static void EditCash(j19gadget*, j19action) {
		auto CCD{ ChestData[ChosenMap] };
		ValueCash->Text = to_string(std::max(0, ToInt(Trim(ValueCash->Text)))); // Make sure this is a valid number
		CCD->Value(CTag(), "Cash", ValueCash->Text);
	}

	static void ToggleNGP(j19gadget*, j19action) {
		auto CCD{ ChestData[ChosenMap] };
		if (NewGamePlusOnly->checked)
			CCD->Value(CTag(), "NewGame+Only", "TRUE");
		else
			CCD->Value(CTag(), "NewGame+Only", "FALSE");
	}

	static void ItemAllAct(j19gadget*, j19action) {
		ItemSelector->ClearItems();
		GetItemList();
		for (auto& itm : *ItemList) {
			if (
				Prefixed(itm, "INGREDIENT_") ||
				Prefixed(itm, "ITM_")
				)
				ItemSelector->AddItem(itm);
		}
	}

	static void ItemIngredientAct(j19gadget*, j19action) {
		ItemSelector->ClearItems();
		GetItemList();
		for (auto& itm : *ItemList) {
			if (Prefixed(itm, "INGREDIENT_"))
				ItemSelector->AddItem(itm);
		}
	}

	static void ItemRegAct(j19gadget*, j19action) {
		ItemSelector->ClearItems();
		GetItemList();
		for (auto& itm : *ItemList) {
			if (Prefixed(itm, "ITM_"))
				ItemSelector->AddItem(itm);
		}
	}

	static void ItemSelectAct(j19gadget*, j19action) {
		if (ItemSelector->SelectedItem() < 0) return;
		ValueItem->Text = ItemSelector->ItemText();
		EditItem(ValueItem, j19action::Type);
	}
	
	void GetTreasureChests(std::string scanmap) {
		Cls();
		SetColor(255, 180, 0);
		ButtonFont()->Text("Loading!", ScreenWidth() / 2, ScreenHeight() / 2, Align::Center, Align::Center);
		Flip();
		KMap = LoadKthura(DirKthura() + "/" + scanmap);
		if (!KMap) {
			QCol->Error("Kthura map \"" + scanmap + "\" not properly loaded");
			if (JCR6::Last()->Error) {
				QCol->LMagenta("- = JCR6 reported = -\n");
				QCol->Doing("Error", JCR6::Last()->ErrorMessage);
				QCol->Doing("Main", JCR6::Last()->MainFile);
				QCol->Doing("Entry", JCR6::Last()->Entry);
				QCol->LMagenta("- =  End  report  = -\n\n\7");
			}
		}
		Cls();
		SetColor(255, 180, 0);
		ButtonFont()->Text("Scanning!", ScreenWidth() / 2, ScreenHeight() / 2, Align::Center, Align::Center);
		Flip();
		auto umap{ Upper(scanmap) };
		auto layers{ KMap->Layers() };
		auto modified{ false };
		ChestSelector->ClearItems();
		if (!ChestData.count(umap)) {
			auto ChestFile{ DirOutput() + "/" + scanmap + "/Chests.ini" };
			const char* JPB{ "(c) Jeroen P. Broks" };
			if (!FileExists(ChestFile)) 
				ChestData[umap] = ParseGINIE("[*sys*]\nCreation=" + CurrentDate() + "; " + CurrentTime(), ChestFile, JPB);
			else
				ChestData[umap] = LoadGINIE(ChestFile, ChestFile, JPB);			
		}
		auto CCD{ ChestData[umap] };		
		for (auto lay : *layers) {
			auto layer{ KMap->Layer(lay) };
			//auto CCDL{ "Layer::" + lay };
			QCol->Doing("Scanning layer", lay);
			CCD->List("Lists",lay)->clear();
			for (auto o = layer->FirstObject(); o; o = o->Next()) {
				if (Upper(o->texture()) == ChestTex && o->animframe() == 0) {
					auto chesttag = "CHEST_" + lay + TrSPrintF("__%04d_%04d", o->x(), o->y());
					if (chesttag != o->Tag()) {
						QCol->Doing("Retagging object #", o->ID(), "\t"); QCol->Pink(o->Tag()); QCol->Yellow(" => "); QCol->LGreen(chesttag + "\n\7");
						o->Tag(chesttag);
						modified = true;
					}
					ChestSelector->AddItem(chesttag);
					CCD->Add("Lists", lay, chesttag, true, true);
					auto CCDOT{ "CHEST::" + chesttag };
					if (o->r() != 255) CCD->NewValue(CCDOT, "NewGame+Only", "TRUE");
					CCD->NewValue(CCDOT, "Type", "Cash");
					CCD->NewValue(CCDOT, "Cash", "10");
				}
			}
		}
		if (modified) {
			Cls();
			SetColor(255, 180, 0);
			ButtonFont()->Text("Saving!", ScreenWidth() / 2, ScreenHeight() / 2, Align::Center, Align::Center);
			Flip();
			QCol->Doing("Saving", scanmap);
			Kthura_Save(KMap, DirKthura() + "/" + scanmap);
		}
		ChosenMap = umap;
	}

	void InitTreasureChest(j19gadget* Panel) {
		MainPanel = Panel;
		ScanButton = CreateButton("Scan for chests",0,0,Panel);
		ScanButton->SetFont(ButtonFont());
		ScanButton->SetForeground(255, 180, 0);
		ScanButton->SetBackground(50, 36, 0);
		ScanButton->CBDraw = ScanButtonDraw;

		ChestSelector = CreateListBox(0, 0, 50, 0, Panel);
		ChestSelector->SetForeground(255, 180, 0);
		ChestSelector->SetBackground(12, 6, 0);
		ChestSelector->SetFont(SysFont());
		ChestSelector->CBAction = SelectorHit;

		EditPanel = CreatePanel(0, 0, 0, 0, Panel);

		RadioCash = CreateRadioButton("Cash ", 0, 0, 250, 30,EditPanel);
		RadioItem = CreateRadioButton("Item ", 0, 35, 250, 30, EditPanel);
		RadioCash->SetForeground(255, 180, 0);
		RadioItem->SetForeground(255, 180, 0);
		RadioCash->CBAction = RadioHit;
		RadioItem->CBAction = RadioHit;

		ValueCash = CreateTextfield(200, 5, 200, EditPanel);
		ValueCash->SetForeground(255, 180, 0);
		ValueCash->SetFont(SysFont());
		ValueCash->CBAction = EditCash;

		ValueItem = CreateTextfield(200, 40, 300, EditPanel);
		ValueItem->SetForeground(255, 180, 0);
		ValueItem->SetFont(SysFont());
		ValueItem->CBAction = EditItem;

		NewGamePlusOnly = CreateCheckBox("New Game+ Only", 0, 70, 200, 30, EditPanel);
		NewGamePlusOnly->CBAction = ToggleNGP;

		ItemSelector = CreateListBox(0, 120, 100, EditPanel->H() - 120, EditPanel);
		ItemSelector->SetForeground(255, 180, 0);
		ItemSelector->SetBackground(30, 15, 0);
		ItemSelector->SetFont(SysFont());
		ItemSelector->CBAction = ItemSelectAct;

		ItemGroup = CreatePanel(0, 0, 0, 0, EditPanel);
		ItemAll = CreateRadioButton("All", 0, 0, 100, 20, ItemGroup,true);
		ItemAll->SetForeground(0, 0, 0, 255);
		ItemAll->SetFont(FontForKthuraList());
		ItemAll->CBAction = ItemAllAct;
		
		ItemAllAct(ItemAll, j19action::Check);


		ItemIngredient = CreateRadioButton("Ingredient", 2, 20, 100, 20, ItemGroup, false);
		ItemIngredient->SetForeground(0, 0, 0, 255);
		ItemIngredient->SetFont(FontForKthuraList());
		ItemIngredient->CBAction = ItemIngredientAct;

		ItemRegular = CreateRadioButton("Regular", 4, 40, 100, 20, ItemGroup, false);
		ItemRegular->SetForeground(0, 0, 0, 255);
		ItemRegular->SetFont(FontForKthuraList());
		ItemRegular->CBAction = ItemRegAct;


		// Must be last!
		ErrorMap = CreateLabel("Error in Kthura Map or none loaded!", 0, 0, Panel->W(), Panel->H(), MainPanel, 2);
		ErrorMap->SetForeground(255, 255, 0);
		ErrorMap->SetBackground(127, 0, 0);
		ErrorMap->SetFont(ButtonFont());
	}

}