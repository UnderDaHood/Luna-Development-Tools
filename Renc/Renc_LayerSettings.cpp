// Lic:
// Luna's Father
// Renc - Layer Settings
// 
// 
// 
// (c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.07.18
// EndLic

#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>

#include <june19.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_LoadCompiled.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>

#include "Renc_Config.hpp"
#include "Renc_UserInterface.hpp"
#include "Renc_LayerSettings.hpp"
#include "Renc_MapData.hpp"


#define MData LayerData[_CurrentMap]
#define ChosenLayer LayerSelector->ItemText()

using namespace Slyvina;
using namespace June19;
using namespace Kthura;
using namespace Units;
using namespace TQSE;
using namespace TQSG;

namespace LunaRenc {
	static j19gadget
		* LayerSelector{ nullptr },
		* LayerPanel{ nullptr };


	static std::string _CurrentMap{""};
	static std::map<std::string, std::vector<std::string>> Layers{};
	static std::map<std::string, GINIE> LayerData{};
	static std::map<std::string, j19gadget*> Fields;
	
	static void RefreshLayerSelector(bool force = false) {
		auto umap{ _CurrentMap };
		if (force || (!Layers.count(umap))) {
			Cls(); Poll(); Flip();
			QCol->Doing("Loading", umap);
			auto _map{ XLoadKthura(DirKthura() + "/" + umap) };
			auto _layers{ _map->Layers() };
			Layers[umap].clear();
			for (auto _lay : *_layers) Layers[umap].push_back(_lay);
		}
		LayerSelector->ClearItems();
		for (auto _lay : Layers[umap]) LayerSelector->AddItem(_lay);
	}

	static void GenUpdate(j19gadget* genpanel, j19action) {
		auto umap{ ChosenMap() };
		if (umap != _CurrentMap) {
			_CurrentMap = umap;
			RefreshLayerSelector();
		}
		if (!LayerData.count(umap)) {
			auto f{ MapStuffDir() + "/" + umap + "/Layers.ini" };
			MData = LoadOptGINIE(f, f, "Layer details\nLuna's Father\n(c) Jeroen P. broks");
			for (auto _lay : Layers[umap]) {
				MData->NewValue(_lay, "MaxEncounters", "5");
			}
		}
		
	}

	static void LayerSelectorDraw(j19gadget* g, j19action) {
		static double hue{ 0 }; hue = hue + .1; if (hue > 360) hue = 0;
		g->SetForegroundHSV(hue, 1, 1);
		g->SetBackgroundHSV(hue, 1, .2);
		static double hue2{ 180 }; hue2 = hue2 - .002; if (hue2 < 0) hue2 = 360;
		LayerPanel->SetBackgroundHSV(hue2, 1, .2);
		LayerPanel->Visible = g->SelectedItem() >= 0;
	}

	static void LayerEditAction(j19gadget* , j19action) {
		for (auto g : Fields) {
			switch (g.second->GetKind()) {
			case j19kind::Textfield:
				MData->Value(ChosenLayer, g.first, g.second->Text);
				break;
			case j19kind::CheckBox:
				MData->Value(ChosenLayer, g.first, boolstring(g.second->checked));
				break;
			default:
				QCol->Error("\7Can't edit unknown gadget kind: " + g.first);
				break;
			}
		}
	}

	static void cbox(j19gadget* g, j19action) {
		if (g->checked) {
			g->Caption = "True";
			g->SetForeground(0, 255, 0);
		} else {
			g->Caption = "False";
			g->SetForeground(255, 0, 0);
		}
	}

	static void PickLayer(j19gadget* g, j19action) {
		if (g->SelectedItem() < 0) return;
		for (auto g : Fields) {
			switch (g.second->GetKind()) {
			case j19kind::Textfield:
				g.second->Text = MData->Value(ChosenLayer, g.first);
				break;
			case j19kind::CheckBox:
				g.second->checked = Upper(MData->Value(ChosenLayer, g.first)) == "TRUE";
				g.second->CBDraw = cbox;
				break;
			default:
				QCol->Error("\7Can't edit unknown gadget kind: " + g.first);
				break;

			}
		}
	}

	void LayerInit(j19gadget* g) {
		QCol->Doing("Registering","Compiled Loader");
		RegCompiledXLoader();
		QCol->Doing("Setting up", "Layer based GUI page");
		g->CBDraw = GenUpdate;
		LayerSelector = CreateListBox(0, 0, 250, g->H(), g);
		LayerSelector->CBDraw = LayerSelectorDraw;
		LayerSelector->CBAction = PickLayer;
		LayerPanel = CreatePanel(LayerSelector->W() + 16, 0, g->W() - (LayerSelector->W() + 16), g->H(), g);
		CreateLabel("Layer title:", 0, 0, 200, 20, LayerPanel);
		CreateLabel("Max Encounters:", 0, 20, 200, 20, LayerPanel);
		CreateLabel("Block Encounters", 0, 40, 200, 20, LayerPanel);
		Fields["Title"] = CreateTextfield(200, 0, LayerPanel->W() - 200, 20, LayerPanel);
		Fields["MaxEncounters"] = CreateTextfield(200, 20, 100, 20, LayerPanel);
		Fields["BlockEncounter"] = CreateCheckBox("", 200, 40, 100, 20, LayerPanel);
		for (auto f : Fields) f.second->CBAction = LayerEditAction;
	}
	//void LunaRenc::LayerMapSelected(std::string s) {	}
}