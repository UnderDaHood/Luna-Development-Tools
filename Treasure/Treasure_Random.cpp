// Lic:
// Luna's Father
// Treasure Editor
// 
// 
// 
// (c) Random Treasures On The Map, 2023
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
// Version: 23.10.14
// EndLic
#include <SlyvQCol.hpp>
#include <SlyvTime.hpp>

#include "Treasure_Random.hpp"
#include "Treasure_Config.hpp"
using namespace std;

using namespace Slyvina;
using namespace June19;
using namespace Units;

namespace Luna_Treasure {


	const uint16 numManaments{ 10 };
	const char Manaments[numManaments][255]{
		"Earth",
		"Wood",
		"Water",
		"Frost",
		"Wind",
		"Lightning",
		"Fire",
		"Lava",
		"Light",
		"Darkness" };

	static j19gadget
		* CheckManament[numManaments],
		* TierMin{ nullptr },
		* TierMax{ nullptr },
		* CashMin{ nullptr },
		* CashMax{ nullptr };

	static std::string RT_Chosen{};
	static std::map<std::string, GINIE>RT_Data{};
	inline GINIE RT() { return RT_Data[RT_Chosen]; }

	static void TierMinAct(j19gadget*, j19action) {
		int v{ ToInt(TierMin->Text) };
		v = std::max(v, 0);
		TierMin->Text = to_string(v);
		TierMax->Text = to_string(std::max(ToInt(TierMax->Text), v));
		RT()->Value("Tier", "Min", TierMin->Text);
		RT()->Value("Tier", "Max", TierMax->Text);
		
	}

	static void TierMaxAct(j19gadget*, j19action) {
		int v{ ToInt(TierMax->Text) };
		v = std::max(v, 0);
		TierMax->Text = to_string(v);
		TierMin->Text = to_string(std::min(ToInt(TierMin->Text), v));
		RT()->Value("Tier", "Min", TierMin->Text);
		RT()->Value("Tier", "Max", TierMax->Text);
	}

	static void CashMinAct(j19gadget*, j19action) {
		int v{ ToInt(CashMin->Text) };
		v = std::max(v, 0);
		CashMin->Text = to_string(v);
		CashMax->Text = to_string(std::max(ToInt(CashMax->Text), v));
		RT()->Value("Cash", "Min", CashMin->Text);
		RT()->Value("Cash", "Max", CashMax->Text);
	}

	static void CashMaxAct(j19gadget*, j19action) {
		int v{ ToInt(CashMax->Text) };
		v = std::max(v, 0);
		CashMax->Text = to_string(v);
		CashMin->Text = to_string(std::min(ToInt(CashMin->Text), v));
		RT()->Value("Cash", "Min", CashMin->Text);
		RT()->Value("Cash", "Max", CashMax->Text);
	}

	static void ActCheckManament(j19gadget* g, j19action) {
		static char TF[2][7]{ "FALSE", "TRUE" };
		RT_Data[RT_Chosen]->Value("Manaments", g->Caption, TF[g->checked]);
		QCol->Green("Manament "); QCol->Pink(g->Caption); QCol->Green(" is now "); QCol->LMagenta(TF[g->checked]); std::cout << "\n";
	}

	
	void Luna_Treasure::InitRandomTreasure(j19gadget* Panel) {
		uint32 y{ 0 };
		for (uint16 i = 0; i < numManaments; ++i) {
			uint32
				iy{ (uint32)(i % 5) * 30 };
			y = std::max(y, iy);
			CheckManament[i] = CreateCheckBox(Manaments[i], (i / 5) * 400, iy, 350, 30, Panel);
			CheckManament[i]->SetForeground(255, 180, 0);
			CheckManament[i]->CBAction = ActCheckManament;
		}
		y += 30;
		CreateLabel("Tier range:", 2, y, 450, 30,Panel)->SetForeground(255,180,0);
		TierMin = CreateTextfield(440, y, 200, Panel);
		TierMax = CreateTextfield(650, y, 200, Panel);
		TierMin->SetForeground(255, 180, 0);
		TierMax->SetForeground(255, 180, 0);
		TierMin->SetBackground(40, 20, 0);
		TierMax->SetBackground(40, 20, 0);
		TierMin->CBAction = TierMinAct;
		TierMax->CBAction = TierMaxAct;
		y += 40;
		CreateLabel("Cash range:", 2, y, 450, 30, Panel)->SetForeground(255, 180, 0);
		CashMin = CreateTextfield(440, y, 200, Panel);
		CashMax = CreateTextfield(650, y, 200, Panel);
		CashMin->SetForeground(255, 180, 0);
		CashMax->SetForeground(255, 180, 0);
		CashMin->SetBackground(40, 20, 0);
		CashMax->SetBackground(40, 20, 0);
		CashMin->CBAction = CashMinAct;
		CashMax->CBAction = CashMaxAct;
	}

	void GetRandomTreasures(std::string mapfile) {
		auto fullfile{ DirOutput() + "/" + mapfile + "/RandomTreasure.ini" };
		auto umf{ Upper(mapfile) };
		if (!RT_Data.count(umf)) {
			if (!FileExists(fullfile)) {
				QCol->Doing("Creating", fullfile);
				SaveString(fullfile, "[sys]\nCreation=" + CurrentDate() + "; " + CurrentTime());
			}
		}
		QCol->Doing("Loading", fullfile);
		RT_Data[umf] = LoadGINIE(fullfile, fullfile, "Random treasures for: " + mapfile + "\n(c) Jeroen P. Broks!");
		for (byte i = 0; i < numManaments; i++) {
			CheckManament[i]->checked = Upper(RT_Data[umf]->Value("Manaments", CheckManament[i]->Caption)) == "TRUE";
		}
		TierMin->Text = RT_Data[umf]->Value("Tier", "Min");
		TierMax->Text = RT_Data[umf]->Value("Tier", "Max");
		CashMin->Text = RT_Data[umf]->Value("Cash", "Min");
		CashMax->Text = RT_Data[umf]->Value("Cash", "Max");
		RT_Chosen = umf;
	}
}