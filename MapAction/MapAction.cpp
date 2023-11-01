// Lic:
// Dev/Luna Development Tools/MapAction/MapAction.cpp
// Version: 23.11.01
// Copyright (C) 2023 Jeroen Petrus Broks
// 
// ===========================
// This file is part of a project related to the Phantasar Chronicles or another
// series or saga which is property of Jeroen P. Broks.
// This means that it may contain references to a story-line plus characters
// which are property of Jeroen Broks. These references may only be distributed
// along with an unmodified version of the game.
// 
// As soon as you remove or replace ALL references to the storyline or character
// references, or any termology specifically set up for the Phantasar universe,
// or any other univers a story of Jeroen P. Broks is set up for,
// the restrictions of this file are removed and will automatically become
// zLib licensed (see below).
// 
// Please note that doing so counts as a modification and must be marked as such
// in accordance to the zLib license.
// ===========================
// zLib license terms:
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic

#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>
#include <SlyvAsk.hpp>
#include <SlyvDir.hpp>
#include <SlyvStream.hpp>
#include <SlyvRoman.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_Save.hpp>
#include <Lunatic.hpp>

using namespace Slyvina;
using namespace Units;
using namespace Kthura;
using namespace Lunatic;

std::string kthuradir{};
std::string configfile{};
GINIE config{ nullptr };
VecString Maps{ nullptr };


#pragma region Compile

static bool err{ false };

std::shared_ptr<std::vector<char>> OutBuf{ nullptr };

static int LuaPaniek(lua_State* L) {
	QCol->Error("Compiling Lua translation failed");
	//std::string Trace{};
	//Error("Lua Error!");
	QCol->Doing("Lua#", lua_gettop(L));
	for (int i = 1; i <= lua_gettop(L); i++) {
		QCol->Magenta(TrSPrintF("Arg #%03d\t", i));
		switch (lua_type(L, i)) {
		case LUA_TSTRING:
			QCol->Doing("String", "\"" + std::string(luaL_checkstring(L, i)) + "\"");
			//Trace += luaL_checkstring(L, i); Trace += "\n";
			break;
		case LUA_TNUMBER:
			QCol->Doing("Number ", std::to_string(luaL_checknumber(L, i)));
		case LUA_TFUNCTION:
			QCol->Doing("Function", "<>");
		default:
			QCol->Doing("Unknown: ", lua_type(L, i));
			break;
		}
		///cout << "\n";
	}
	//Error("", false, true);
	//exit(11);
	err = true;
	return 0;
}

static int DumpLua(lua_State* L, const void* p, size_t sz, void* ud) {

	auto pcp{ (char*)p };
	for (size_t i = 0; i < sz; ++i) OutBuf->push_back(pcp[i]);
	return 0;
}

bool SaveTranslation(std::string what, std::string source, std::string OutFile) {
	err = false;
	auto L{ luaL_newstate() };
	//auto source{ Trans->LuaSource };
	QCol->Doing("Compiling", "what");
	lua_atpanic(L, LuaPaniek);
	luaL_openlibs(L);
	luaL_loadstring(L, source.c_str());
	//lua_call(L, 0, 0);
	if (!err) {
		OutBuf = std::make_shared < std::vector < char > >();
		lua_dump(L, DumpLua, NULL, 0);
	}
	if (!OutBuf->size()) {
		QCol->Error("Lua translation failed!");
		LuaPaniek(L); // Test
		QCol->LGreen("<source>\n");
		auto Lines = Split(source, '\n');
		for (size_t ln = 0; ln < Lines->size(); ln++) {
			QCol->LMagenta(TrSPrintF("%9d\t", ln + 1));
			QCol->LCyan((*Lines)[ln]);
			QCol->White("\n");
		}
		QCol->LGreen("</source>\n");
		lua_close(L);
		return false;
	}

	std::string
		OutDir{Ask("Directories", "Output", "Where to store the actions data: ")},
		OutPutFile{ OutDir + "/" + OutFile + ".lbc" };
	QCol->Doing("Saving", OutPutFile);
	auto BT{ WriteFile(OutPutFile) };
	for (auto ch : *OutBuf) BT->Write(ch);
	BT->Close();

	
	lua_close(L);
	return !err;
}
#pragma endregion

#pragma region PlainSave
bool PlainSave(std::string what, std::string mapb, std::string src) {
	std::string
		OutDir{Ask("Directories", "Output", "Where to store the actions data: ")},
		OutPutFile{ OutDir + "/" + mapb + "/" + what + ".lua" };
	OutPutFile = ChReplace(OutPutFile, '\\', '/');
	QCol->Doing("Saving", OutPutFile);
	SaveString(OutPutFile, src);
}
#pragma endregion


void LoadConfig() {
	if (!FileExists(configfile)) {
		QCol->Doing("Creating", configfile);
		SaveString(configfile, "[sys]\nCreated=" + CurrentTime());
	}
	QCol->Doing("Loading", configfile);
	config = LoadGINIE(configfile, configfile, "(c) Jeroen P. Broks");
	AskGINIE = config;
}

void ScanForMaps() {
	kthuradir = Ask("Directories", "Input", "Where to find the original Kthura maps: ");
	QCol->Doing("Scanning for maps", kthuradir);
	Maps = FileList(kthuradir);
	QCol->Doing("=> Found", Maps->size(),"\n\n");
}

void ScanMap(std::string mapfile) {
	printf("\n\n");
	QCol->Doing("Processing", mapfile);
	QCol->Doing("=> Loading", "Map");
	auto kthmap = LoadKthura(kthuradir + "/" + mapfile);
	std::string ZoneAction{"-- ZA: " + mapfile + " --\n\nlocal _ZA = {}\n\n_ZA.ENTER={}\n_ZA.CYCLE={}\n_ZA.LEAVE={}\n\n"};
	std::string Clickables{"-- CLK: " + mapfile + "\n\nlocal _CLK = {}\n"};
	auto layers{ kthmap->Layers()};
	for (auto layname : *layers) {
		QCol->Doing("=> Layer", layname);
		auto lay{ kthmap->Layer(layname) };
		ZoneAction += "\n\n\n-- Layer: " + layname + " --\n";
		ZoneAction += "_ZA.ENTER[\"" + layname + "\"] = {}\n";
		ZoneAction += "_ZA.CYCLE[\"" + layname + "\"] = {}\n";
		ZoneAction += "_ZA.LEAVE[\"" + layname + "\"] = {}\n";
		ZoneAction += "do\n";
		ZoneAction += "\tlocal _enter = _ZA.ENTER[\"" + layname + "\"]\n";
		ZoneAction += "\tlocal _cycle = _ZA.CYCLE[\"" + layname + "\"]\n";
		ZoneAction += "\tlocal _leave = _ZA.LEAVE[\"" + layname + "\"]\n";
		Clickables += "\n\n\n-- Layer: " + layname + " --\n";
		Clickables += "do\n";
		Clickables += "\t_CLK[\"" + layname + "\"] = {}\n\t local __click = _CLK[\"" + layname + "\"]\n";
		for (auto o = lay->FirstObject(); o; o = o->Next()) {
			QCol->Doing("=> Object", o->ID(), "\r");
			if (o->Tag().size()) {
				if (Prefixed(o->Tag(), "ZA_") || Prefixed(o->Tag(), "ZAE_") || Prefixed(o->Tag(), "ZALE_") || Prefixed(o->Tag(), "ZAEL_") || Upper(o->Tag()) == "BYE") {
					QCol->Doing("=> Enter", o->Tag());
					//ZoneAction += "\t_enter[#_enter+1] = \"" + o->Tag() += "\"\n"; 
					ZoneAction += " \t_enter[\"" + o->Tag() + "\"] = \"" + o->Tag() + "\"\n";
				}
				if (Prefixed(o->Tag(), "ZAL_") || Prefixed(o->Tag(), "ZALE_") || Prefixed(o->Tag(), "ZAEL_")) {
					QCol->Doing("=> Leave", o->Tag());
					ZoneAction += " \t_leave[\"" + o->Tag() + "\"] = \"" + o->Tag() + "\"\n";
					//ZoneAction += "\t_leave[#_leave] = \"" + o->Tag() += "\"\n"; 
				}
				if (Prefixed(o->Tag(), "ZAC_")) {
					QCol->Doing("=> Cycle", o->Tag());
					//ZoneAction += "_cycle[#_cycle] = \"" + o->Tag() += "\"\n"; 
					ZoneAction += " \t_cycle[\"" + o->Tag() + "\"] = \"" + o->Tag() + "\"\n";
				}
				if (Prefixed(o->Tag(), "NPC_MT_")) {
					QCol->Doing("=> NPC Maptext", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.MapText\n";
				} else if (Prefixed(o->Tag(), "NPC_")) {
					QCol->Doing("=> NPC", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.MapScript[\"" + o->Tag() + "\"]\n";
				}
			}
			if (o->Kind() == KthuraKind::Obstacle) {
				if (Upper(o->texture()) == "GFX/MAPSPOTS/SAVE.PNG") {
					QCol->Warn("FAULTY SAVESPOT!!!!\7\7\7");
					if (FindFirst(Upper(mapfile), "_TOWN_") >= 0) {
						QCol->Doing("Replacing by", "Blue Save Orb");
						o->texture("GFX/SaveSpot/Blue.png");
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					} else {
						QCol->Doing("Replacing by", "Red Save Orb");
						o->texture("GFX/SaveSpot/Red.png");
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
				}
				if (Upper(o->texture()) == "GFX/SAVESPOT/RED.PNG") {
					static auto n{0};
					if (!Prefixed(o->Tag(), "SAVE_RED")) {
						std::string t;
						do { t = "SAVE_RED_" + ToRoman(n++); } while (lay->HasTag(t));
						QCol->Doing("Tagging red", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.Red_SaveSpot\n";
				}
				if (Upper(o->texture()) == "GFX/SAVESPOT/GREEN.PNG") {
					static auto n{ 0 };
					if (!Prefixed(o->Tag(), "SAVE_GREEN")) {
						std::string t;
						do { t = "SAVE_GREEN_" + ToRoman(n++); } while (lay->HasTag(t));
						QCol->Doing("Tagging green", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.Green_SaveSpot\n";
				}
				if (Upper(o->texture()) == "GFX/SAVESPOT/BLUE.PNG") {
					static auto n{ 0 };
					if (!Prefixed(o->Tag(), "SAVE_BLUE")) {
						std::string t;
						do { t = "SAVE_BLUE_" + ToRoman(n++); } while (lay->HasTag(t));
						QCol->Doing("Tagging blue", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.Blue_SaveSpot\n";
				}
				if (Upper(o->texture()) == "GFX/TREASURE/BAG.PNG") {
					static auto n{ 0 };
					if (!Prefixed(o->Tag(), "RANDOM_TREASURE_BAG_")) {
						std::string t;
						do { t = "RANDOM_TREASURE_BAG_" + ToRoman(n++); } while (lay->HasTag(t));
						QCol->Doing("Tagging random treasure bag", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.Random_Treasure\n";
				}
				if (Upper(o->texture()) == "GFX/TEXTURES/TREASURE/CHEST.JPBF" && o->animframe() == 0) {
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.Open_Chest\n";				
				}
				if (Lower(o->texture()) == "gfx/mapspots/travel.png") {
					static auto n{ 0 };
					if (!Prefixed(o->Tag(), "TRAVELERSEMBLEM_")) {
						std::string t;
						do { t = "TRAVELERSEMBLEM_" + ToRoman(++n); } while (lay->HasTag(t));
						QCol->Doing("Tagging Traveler's Emblem", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.GetTravelersEmblem\n";					
				}
			}
		}
		ZoneAction += "end\n\n";
		Clickables += "end\n\n";
	}
	ZoneAction += "\n\nreturn _ZA\n";
	Clickables += "\n\nreturn _CLK\n";
	PlainSave("ZoneAction", mapfile, ZoneAction);
	PlainSave("Clickables", mapfile, Clickables);
}

int main(int c,char **a) {
	QCol->LMagenta("Map Action!\n");
	QCol->Doing("Coded by", "Jeroen P. Broks","\n\n");
	configfile = ChReplace(StripExt(a[0]) + ".ini",'\\','/');
	LoadConfig();
	ScanForMaps();
	for (auto mf : *Maps) ScanMap(mf);
	QCol->Reset();
	return 0;
}