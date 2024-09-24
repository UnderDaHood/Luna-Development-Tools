// Lic:
// Dev/Luna Development Tools/MapAction/MapAction.cpp
// Version: 24.08.09
// Copyright (C) 2023, 2024 Jeroen Petrus Broks
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
#include <SlyvRandom.hpp>
#include <SlyvRoman.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_Save.hpp>
#include <Kthura_Compiler.hpp>
#include <Lunatic.hpp>

#include <JCR6_RealDir.hpp>
#include <JCR6_zlib.hpp>

using namespace Slyvina;
using namespace Units;
using namespace Kthura;
using namespace Lunatic;

#define tome_realsize 337.0
#define tome_mapsize 64.0

std::string 
kthuradir{},
kthuracompiled{},
configfile{},
changelogfile{};

GINIE config{ nullptr };
GINIE changelog{ nullptr };
VecString Maps{ nullptr };
uint32 CountBlackOrbs{ 0 };
uint32 CountSeals{ 0 };
GINIE SkyTower{ nullptr };

// The compiler doesn't need this, but the Visual Studio IDE kept complaining about this, and this Macro shuts it up.
#define QCol Slyvina::Units::QCol

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
		OutPutFile{ OutDir + "/" + mapb + "/" + what + ".lua" },
		OutPutDir{ ExtractDir(OutPutFile) };
	if (!DirectoryExists(OutPutDir)) {
		QCol->Doing("Creating", OutPutDir);
		MakeDir(OutPutDir);
	}
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
	changelog = LoadOptGINIE(changelogfile, changelogfile, "(c) Jeroen P. Broks");
	Slyvina::Kthura::CompileStorage = Ask("Compile", "Storage", "Compilation Storage: ", "zlib");
}

void ScanForMaps() {
	kthuradir = Ask("Directories", "Raw", "Where to find the raw Kthura maps: ");
	kthuracompiled = Ask("Directories", "Compiled", "Where to find the compiled Kthura Maps:");
	QCol->Doing("Scanning for maps", kthuradir);
	Maps = FileList(kthuradir);
	QCol->Doing("=> Found", Maps->size(),"\n\n");
}

void ScanMap(std::string mapfile,bool forced=false) {
	auto fmapfile{ kthuradir + "/" + mapfile };
	auto IsSkyTower{ Prefixed(Upper(mapfile), "CH2_DUNGEON_SKYTOWER") };
	if (!forced) {
		bool uptodate{ FileSize(fmapfile) == changelog->IntValue(mapfile,"Size") };
		uptodate = uptodate && FileDate(fmapfile) == changelog->Value(mapfile, "Date"); 
		uptodate = uptodate && (int)FileTimeStamp(fmapfile) == changelog->IntValue(mapfile, "TimeStamp");
		uptodate = uptodate && FileExists(kthuracompiled + "/" + mapfile);
		/*
		std::cout << "Up to date:" << uptodate << "\n";
		std::cout << "Date:" << boolstring(FileDate(fmapfile) == changelog->Value(mapfile, "Date")) << "\t" << FileDate(fmapfile) << "\t" << changelog->Value(mapfile, "Date") << "\n";
		std::cout << "Time:" << boolstring((int)FileTimeStamp(fmapfile) == changelog->IntValue(mapfile, "TimeStamp")) << "\t" << FileTimeStamp(fmapfile) << "\t" << changelog->IntValue(mapfile, "TimeStamp")<<"\n";
		std::cout << "Trgt:" << boolstring(FileExists(kthuracompiled + "/" + mapfile)) << "\t" << kthuracompiled + "/" + mapfile);
		//*/
		if (uptodate) {
			QCol->Doing("Skipping", mapfile,"\t");
			QCol->Green("Up to date\n");
			return;
		}
	}
	if (IsSkyTower && (!SkyTower)) {
		auto SkyTowerFile{ Ask(config,"SkyTower","DataFile","I need the skytower color file: ") };
		SkyTower = LoadOptGINIE(SkyTowerFile, SkyTowerFile, "Color settings for the skytower. The map itself doesn't need them, but the arena does\nGenerated by MapAction\nCopyright Jeroen P. Broks!");
	}
	printf("\n\n");
	changelog->Value(mapfile, "Size", FileSize(fmapfile));
	changelog->Value(mapfile, "Date", FileDate(fmapfile));
	changelog->Value(mapfile, "TimeStamp", FileTimeStamp(fmapfile));
	QCol->Doing("Processing", mapfile);
	QCol->Doing("=> Loading", "Map");
	auto kthmap = LoadKthura(fmapfile);
	std::string ZoneAction{"-- ZA: " + mapfile + " --\n\nlocal _ZA = {}\n\n_ZA.ENTER={}\n_ZA.CYCLE={}\n_ZA.LEAVE={}\n\n\n"};
	std::string Clickables{"-- CLK: " + mapfile + "\n\nlocal _CLK = {}\n"};
	std::string Pass{"-- PASS: " + mapfile + "\n\nlocal _PASS = {}\n"};
	std::string Targets{"-- TARGETS: " + mapfile + "\n\nlocal _TARGETS = {}\n"};
	std::string Crystal{"-- CRYSTAL: " + mapfile + "\n\nlocal _CRYSTAL = {}\n"};
	std::string Secret{"-- SECRET: " + mapfile + "\n\nlocal _SECRET = {}\n"};
	std::string Crack{"-- CRACK: " + mapfile + "\n\nlocal _CRACK = {}\n"};
	std::string Poles{"-- POLES: " + mapfile + "\n\nlocal _POLES = {}\n"};
	std::string Cogs{"-- COGS: " + mapfile + "\n\nlocal _COGS = {}\n"};
	bool HasCogs{ false };
	auto layers{ kthmap->Layers() };
	if (!layers->size())QCol->Error("\7No layers!");
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
		Pass += "\n\n\n-- Layer: " + layname + " --\n";
		Pass += "do\n";
		Pass += "\t__pass = {}; _PASS[\"" + layname + "\"] = __pass;\n";
		Targets += "\n\n\n-- Layer: " + layname + " --\n";
		Targets += "do\n";
		Targets += "\tlocal __targets = {}; _TARGETS[\"" + layname + "\"] = __targets;\n";
		Crystal += "do\n";
		Crystal += "\tlocal __crystal = {}; _CRYSTAL[\"" + layname + "\"] = __crystal;\n";
		Secret += "\do\n\tlocal __secret = {}; _SECRET[\"" + layname + "\"] =  __secret; \n";
		Crack += "\do\n\tlocal __crack = { _cracks_ = {} }; _CRACK[\"" + layname + "\"] =  __crack; \n";
		Poles += "\n\n\n-- Layer: " + layname + "-- \n";
		Poles += "do\n\t local __poles = { }; _POLES[\"" + layname + "\"] = __poles\n";
		Cogs += "\n\n\n-- Layer: " + layname + "-- \n";
		Cogs += "do\n\t local __cogs = { }; _COGS[\"" + layname + "\"] = __cogs\n";

		for (auto o = lay->FirstObject(); o; o = o->Next()) {
			QCol->Doing("=> Object", o->ID(), "\r");
			if (o->texture() == "") {
				switch (o->Kind()) {
				case KthuraKind::TiledArea:
				case KthuraKind::Picture:
				case KthuraKind::StretchedArea:
				case KthuraKind::Obstacle:
					QCol->Red(
						"EEEEE RRRR  RRRR   OOOO  RRRR\n"
						"E     R   R R   R O    O R   R\n"
						"EEE   RRRR  RRRR  O    O RRRR\n"
						"E     R   R R   R O    O R   R\n"
						"EEEEE R   R R   R  OOOO  R   R\n"
					);
					QCol->Error(o->SKind() + TrSPrintF(" requires a texture, but object #%d has none! Removal is in order!\7",o->ID()));
					QCol->LMagenta("\n");
					ReadLine("Hit enter to continue!");
					lay->Kill(o);
					Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					goto endofobjectloop;
				default:
					break;
				}
			}
			if (IsSkyTower) {
				auto STTex{ Upper(o->texture()) };
				if (STTex == "GFX/TEXTURES/DUNGEON/GREY/WALL_NORTH.PNG") {
					auto val{ TrSPrintF("%d,%d,%d",o->r(),o->g(),o->b()) };
					QCol->Doing("=> Skytower wall", val);
					SkyTower->Value(layname, "Wall", val);
				}
				if (STTex == "GFX/TEXTURES/DUNGEON/GREY/FLOOR.PNG") {
					auto val{ TrSPrintF("%d,%d,%d",o->r(),o->g(),o->b()) };
					QCol->Doing("=> Skytower floor", val);
					SkyTower->Value(layname, "Floor", val);
				}
			}
			if (o->Tag().size()) {
				if (Prefixed(Upper(o->Tag()), "PASS_")) {
					// Pass data is to tell the engine where Scyndi's whip or Myrah's arrows can just pass through, even when the zones are otherwise impassible.
					int
						sx{ (int)floor(o->x() / 32) },
						sy{ (int)floor(o->y() / 32) },
						zw{ (int)ceil(o->w() / 32) },
						zh{ (int)ceil(o->h()) / 32 },
						ex{ sx + zw },
						ey{ sy + zh };
					QCol->Doing("Pass", o->Tag());
					Pass += "\t-- Object: " + o->Tag() + " --\n";
					Pass += TrSPrintF("\tfor x = %d,%d,1 do\n\t\t__pass[x] = __pass[x] or {}\n", sx, ex);
					Pass += TrSPrintF("\t\tfor y = %d,%d,1 do\n\t\t\t__pass[x][y] = true\n", sy, ey);
					Pass += "\t\tend\n\t\end\n";
				}
				if (Prefixed(Upper(o->Tag()), "GO_") && (o->Kind()==KthuraKind::TiledArea || o->Kind() == KthuraKind::Zone || o->Kind() == KthuraKind::StretchedArea || o->Kind() == KthuraKind::Rect)) {
					QCol->Warn("Invalid GO tag!");
					ReadLine("\7Hit enter to continue!");
					QCol->Doing("Fixing", o->ID());
					o->Tag("GO:" + ChReplace(o->Tag(),'_',':'));
					QCol->Doing("Saving", kthuradir + "/" + mapfile);
					Kthura_Save(kthmap, kthuradir + "/" + mapfile);
				}
				if (Prefixed(Upper(o->Tag()), "GO:")) {
					QCol->Doing("=> Go", o->Tag());
					ZoneAction += " \t_enter[\"" + o->Tag() + "\"] = Scyndi.Globals.MapGoTo\n";
				}
				if (Upper(o->Tag()) == "KOKONORA" || Upper(o->Tag()) == "NEONOR" || Upper(o->Tag()) == "TINKERISLAND" || Upper(o->Tag()) == "TINKER-ISLAND" || Upper(o->Tag()) == "DELISTO") {
					QCol->Doing(o->Tag(), o->ID());
					ZoneAction += " \t_enter[\"" + o->Tag() + "\"] = Scyndi.Globals." + StReplace(o->Tag(),"-","") + "\n";
					QCol->Doing("=> To Worldmap", o->Tag());
				}
				if (Prefixed(Upper(o->Tag()), "ZA_") || Prefixed(Upper(o->Tag()), "ZAE_") || Prefixed(Upper(o->Tag()), "ZALE_") || Prefixed(Upper(o->Tag()), "ZAEL_") || Upper(Upper(o->Tag())) == "BYE") {
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
				if (Upper(o->Tag()) == "NEXT") {
					QCol->Doing("=> Next", layname);
					ZoneAction += "\t_enter[\"Next\"] = Scyndi.Globals.Next_Layer\n";
				}
				if (Upper(o->Tag()) == "PREV") {
					QCol->Doing("=> Prev", layname);
					ZoneAction += "\t_enter[\"Prev\"] = Scyndi.Globals.Prev_Layer\n";
				}
				if (Prefixed(Upper(o->Tag()), "SECRET")) {
					QCol->Doing("=> Secret", o->Tag());
					Secret += "\t__secret[#__secret] = \"" + o->Tag() + "\"\n"; // Don't forget, Lua may define it, but Scyndi will read it!
					ZoneAction += "\t_enter[\"" + o->Tag() + "\"] = function() Scyndi.Class.Kthura.ShowByLabel(\"" + o->Tag() + "\") end\n";
					ZoneAction += "\t_leave[\"" + o->Tag() + "\"] = function() Scyndi.Class.Kthura.HideByLabel(\"" + o->Tag() + "\") end\n";
				}
				if (Prefixed(o->Tag(), "NPC_MT_")) {
					QCol->Doing("=> NPC Maptext", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.MapText\n";
				} else if (Prefixed(o->Tag(), "NPC_")) {
					QCol->Doing("=> NPC", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.MapScript[\"" + o->Tag() + "\"]\n";
				}
				if (Prefixed(Upper(o->Tag()), "BREAK_") || Prefixed(Upper(o->Tag()), "CRACK_")) {
					QCol->Doing("=> Crack", o->Tag());
					//auto tel{ config->IntValue("Crack","Count") };
					auto cracktag{ o->Tag().substr(FindFirst(o->Tag(),'_') + 1) };
					auto CrackTex{ "CRTEX_" + cracktag };				
					lay->RemapTags();
					if (!lay->HasTag(CrackTex)) {
						auto co{ lay->NewObject(KthuraKind::TiledArea) };
						co->x(o->x());
						co->y(o->y());
						co->w(o->w());
						co->h(o->h());
						co->dominance(std::max(2u,o->dominance())); o->dominance(std::max(o->dominance() - 1,1u));
						co->labels(o->labels());
						co->Tag(CrackTex);
						co->visible(true);
						co->a(255);
						co->texture("GFX/Objects/Crack.png");
						QCol->Doing("=> Added", co->Tag());
						lay->RemapTags();
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					if (lay->Obj(CrackTex)->texture() == "") {
						QCol->Doing("Fixing", "TextureLess Crack");
						lay->Obj(CrackTex)->texture("GFX/Objects/Crack.png");
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Crack += "\t__crack._cracks_[#__crack._cracks_] = \"" + o->Tag() + "\"\n";
					if (Yes(config, "Crack", mapfile + "::" + layname + "::" + o->Tag() + "::Has_Event", "Does cracked wall '" + o->Tag() + "' have an event tied to it")) {
						Crack += "\t__crack[\"" + o->Tag() + "\"] = \"" + Ask(config, "Crack", mapfile + "::" + layname + "::" + o->Tag() + "::Event", "Event:", o->Tag()) + "\"\n";
					}

				}
			}
			if (o->Kind() == KthuraKind::StretchedArea) {
				if (Upper(o->texture()) == "GFX/TREASURE/CRYSTALMANAMENT.PNG") {
					auto tel{ config->IntValue("Crystal","Count") };
					std::string Tag{ o->Tag() };
					if (!o->Tag().size()) {
						do { Tag = "MANAMENT_CRYSTAL_" + ToRoman(tel++); } while (lay->HasTag(Tag));
						QCol->Doing("=> Tagged Crystal", Tag);
						o->Tag(Tag);
						config->Value("Crystal", "Count", tel);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					QCol->Doing("=> Crystal", Tag);
					auto elem{ Ask(config,"Crystal::Element",Tag,"What element should this crystal have? ","Random") };
					auto num{ std::max((o->w() / 32) * (o->h() / 32),1) };
					num = AskInt(config, "Crystal::Num", Tag, "How many manaments should be extracted?", num);
					Crystal += TrSPrintF("\t__crystal[\"%s\"] = { Element = \"%s\", Num=%d }\n", Tag.c_str(), elem.c_str(), num);
				}
			}			
			if (o->Kind() == KthuraKind::Obstacle) {
				if (Upper(ExtractDir(o->texture())) == "GFX/TEXTURES/SWITCH") {
					auto mustsave{ false };
					if (Prefixed(Upper(o->Tag()), "NPC_")) {
						QCol->Error("\7NPC_ not allowed as prefix for switches!");
						o->Tag(""); mustsave = true;
					}
					if (o->Tag() == "") {
						std::string a;
						do {
							QCol->Yellow(TrSPrintF("Please tag the switch at (%d,%d): ", o->x(), o->y()));
							a = Trim(ReadLine());
						} while (a == "" || lay->HasTag(a));
						o->Tag(a);
						mustsave = true;
					}
					if (o->data("ONCE") == "") {
						std::string a{""};
						do{
							QCol->Yellow(TrSPrintF("Is the switch at (%d,%d) only to be used once? ", o->x(), o->y()));
							auto b{ Upper(Trim(ReadLine())) };
							if (b.size()) {								
								switch (b[0]) {
								case 'Y':
								case 'J':
									a = "YES"; break;
								case 'N':
									a = "NO"; break;

								}
							}
						} while (a == "");
						o->data("ONCE", a);
						mustsave = true;
					}
					if (mustsave) Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					QCol->Doing("=> Switch", o->Tag(),"\t");
					if (o->data("ONCE") == "YES") QCol->Magenta("(ONCE)"); QCol->Grey("\n");
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.FlipSwitch\n";
				}
				if (Prefixed(Upper(o->texture()), "GFX/TEXTURES/COGS/")) {
					HasCogs = true;
					QCol->Magenta("NOTE!!!\t");
					QCol->Yellow("Cog found! Make sure to use the mapscript lib for cogs in order to to make them work!\n");
					if (!o->Tag().size()) {
						QCol->Warn("Cog untagged! Autotagging!");
						std::string Tag;
						do { config->IncValue("COGS", "Count"); Tag = "Cog: " + ToRoman(config->IntValue("Cogs", "Count")); } while (lay->HasTag(Tag));
						QCol->Doing("= Tagging", Tag);
						o->Tag(Tag);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Cogs += "\t__cogs[\"" + o->Tag() + TrSPrintF("\"] = { Deg = %d, Spd = %5.2f, Dir=%d }\n", Rand.Get(0, 360), (double)Rand.Get(1, 1000) / 100.0, Rand.Get(0, 1));
				}
				if (Upper(o->texture()) == "GFX/OBJECTS/SCYNDI HOOK SPOT.PNG") {
					static auto polenum{config->IntValue("Poles", "Count")};
					if (o->Tag() == "") {
						std::string NewTag;
						do {
							NewTag = "ScyndiPole_" + ToRoman(polenum++);
						} while (lay->HasTag(NewTag));
						QCol->Doing("=> Tagged", NewTag);
						o->Tag(NewTag);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
						config->Value("Poles", "Count", polenum);
					}
					QCol->Doing("=> Pole", o->Tag());
					Poles += "\t__poles[\"" + o->Tag() + "\"]" + TrSPrintF(" = {x = % 4d, y = % 4d}\n", o->x(), o->y() - 8);
				}
				if (Upper(o->texture()) == "GFX/SEAL/SEAL.JPBF") {
					if (o->Tag() != "PURPLESEAL") {
						o->Tag("PURPLESEAL");
						o->animspeed(0);
						QCol->Doing("=> Tagging","Purple Seal");
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);						
					}
					QCol->Doing("=> Purple seals", ++CountSeals);
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.PurpleSeal\n";
				}
				if (Upper(o->texture()) == "GFX/TEXTURES/OBJECTS/BRANDEND.JPBF" && o->animspeed() < 0) {
					QCol->Doing("=> Animating", TrSPrintF("Torch #%d", o->ID()));
					o->animspeed(Rand.Get(1, 8));
					Kthura_Save(kthmap, kthuradir + "/" + mapfile);
				}
				if (Upper(o->texture()) == "GFX/TREASURE/RECIPE.PNG") {
					auto UnTagged{ !o->Tag().size() };
					if (UnTagged) {
						QCol->Error("Untagged Recipe Found");
						while (UnTagged) {
							lay->RemapTags();
							auto NewTag{ ReadLine("Tag it now: ") };
							UnTagged = lay->HasTag(NewTag);
							if (UnTagged) QCol->Error("Tag already exists!");  
							auto IAADir{ Ask(config,"Directories","IAA","Where can I find the IAA files for checkup?") };
							auto AlchemyItem{ IAADir + "/ITM_ALC_" + NewTag + ".ini" };
							if (!FileExists(AlchemyItem)) {
								QCol->Error("No alchemy item with that name appears to exist");
								UnTagged = true;
							}
							if (VecHasString(config->List("Recipes", "Have"), NewTag)) {
								UnTagged = true;
								QCol->Error("Another recipe has been named that! Pick another!");
							}
							if (!UnTagged) {
								o->Tag(NewTag);
								config->Add("Recipes", "Have", NewTag, true, true);
							}
						}						
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					QCol->Doing("=> Recipe", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.GetRecipe\n";
				}
				if (Upper(o->texture()) == "GFX/TARGET/UNHIT.PNG") {
					auto UnTagged{ !o->Tag().size() };
					if (UnTagged) {
						QCol->Error("Untagged Target found!");
						while (UnTagged) {
							lay->RemapTags();
							auto NewTag{ ReadLine("Tag it now: ") };
							UnTagged = lay->HasTag(NewTag);
							if (UnTagged) QCol->Error("Tag already exists!");  else	o->Tag(NewTag);
						}
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					Targets += "\t__targets[\"" + o->Tag() + "\"] = Scyndi.Globals.MapScript[\"" + o->Tag() + "\"]\n";
				}
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
				if (Upper(o->texture()) == "GFX/TREASURE/BLACK ORB.PNG") {
					//                     \GFX\Treasure\Black Orb.png
					auto TagNr{ ++CountBlackOrbs };
					if (o->Tag() == "") {
						std::string NewTag{};
						do { NewTag = "BlackOrb__" + ToRoman(TagNr++); } while (lay->HasTag(NewTag));
						QCol->Doing("Tagging", NewTag);
						o->Tag(NewTag);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					QCol->Doing("=> Black orb", TrSPrintF("%03d (%s)", CountBlackOrbs, o->Tag().c_str()));
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.GetBlackOrb\n";
				}
				if (Lower(o->texture()) == "gfx/mapspots/travel.png") {
					static auto n{ 0 };
					if (Upper(mapfile) == "CH3_TOWN_EQUUS" || Upper(mapfile) == "CH4_DUNGEON_HIGHWAY") {
						if (o->Tag() != "Travel_King") {
							QCol->Doing("Tagging Traveler's Emblem", "Travel_King");
							o->Tag("Travel_King");
							Kthura_Save(kthmap, kthuradir + "/" + mapfile);
						}
					} else if (!Prefixed(o->Tag(), "TRAVELERSEMBLEM_")) {
						std::string t;
						do { t = "TRAVELERSEMBLEM_" + ToRoman(++n); } while (lay->HasTag(t));
						QCol->Doing("Tagging Traveler's Emblem", t);
						o->Tag(t);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					QCol->Doing("=> Travel", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.GetTravelersEmblem\n";					
				}
				if (Upper(ExtractDir(o->texture())) == "GFX/TOME") {
					static double scale{ (tome_mapsize / tome_realsize) * 1000.0 };
					static int tomecount{ 0 };
					o->scalex((int)scale);
					o->scaley((int)scale);
					std::string prefix{"Tome_" + StripAll(o->texture())};
					std::string nt;
					if (!Prefixed(o->Tag(), prefix)) {
						do { nt = prefix + "_" + ToRoman(++tomecount); } while (lay->HasTag(nt));
						QCol->Doing("=> Retagging", nt);
						o->Tag(nt);
						Kthura_Save(kthmap, kthuradir + "/" + mapfile);
					}
					QCol->Doing("=> Tome", o->Tag());
					Clickables += "\t__click[\"" + o->Tag() + "\"] = Scyndi.Globals.GetTome\n";
				}
			}
		endofobjectloop:;
		}
		ZoneAction += "end\n\n";
		Clickables += "end\n\n";
		Pass += "end\n\n";
		Targets += "end\n\n";
		Crystal += "end\n\n";
		Secret += "end\n\n";
		Crack += "end\n\n";
		Poles += "end\n\n";
		Cogs += "end\n\n";
	}
	ZoneAction += "\n\nreturn _ZA\n";
	Clickables += "\n\nreturn _CLK\n";
	Pass += "\n\nreturn _PASS\n";
	Targets += "\n\nreturn _TARGETS\n";
	Crystal += "\n\nreturn _CRYSTAL\n";
	Secret += "\n\nreturn _SECRET\n";
	Crack += "\n\nreturn _CRACK\n";
	Poles += "\n\nreturn _POLES\n";
	Cogs += "\n\nreturn _COGS\n";
	PlainSave("ZoneAction", mapfile, ZoneAction);
	PlainSave("Clickables", mapfile, Clickables);
	PlainSave("Pass", mapfile, Pass);
	PlainSave("Targets", mapfile, Targets);
	PlainSave("Crystal", mapfile, Crystal);
	PlainSave("Secret", mapfile, Secret);
	PlainSave("Crack", mapfile, Crack);
	PlainSave("Poles", mapfile, Poles);
	if (HasCogs) PlainSave("Cogs", mapfile, Cogs); // Only a few have these, so why bother having this file in all freaking map dirs, eh?
	QCol->Doing("=> Black Orbs", CountBlackOrbs);
	QCol->Doing("Compiling", mapfile);
	auto BTC = JCR6::CreateJCR6(kthuracompiled + "/" + mapfile);
	Slyvina::Kthura::Compile(kthmap, BTC);
	BTC->AddString("Last Updated by MapAction:" + CurrentDate() + "; " + CurrentTime() + "\nWith MapAction Build: " + __DATE__ + "; " + __TIME__, "MapActionLog", "Store", "Jeroen P. Broks", "Just a few notes");
	BTC->Close();
}

int main(int c,char **a) {
	bool forced{ false };
	JCR6::JCR6_InitRealDir();
	JCR6::init_zlib();
	QCol->LMagenta("Map Action!\n");
	QCol->Doing("Coded by", "Jeroen P. Broks","\n\n");
	configfile = ChReplace(StripExt(a[0]) + ".ini",'\\','/');
	changelogfile = ChReplace(StripExt(a[0]) + ".changelog.ini", '\\', '/');	
	for (int i = 1; i < c; ++i) forced = forced || Upper(a[i]) == "FORCE";
	LoadConfig();
	ScanForMaps();
	for (auto mf : *Maps) ScanMap(mf,forced);
	QCol->Reset();
	QCol->Doing("Purple seals counted", CountSeals);
	QCol->Reset();
	return 0;
}