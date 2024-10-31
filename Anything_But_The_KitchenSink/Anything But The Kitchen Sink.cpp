// License:
// 	  Dev/Luna Development Tools/Anything_But_The_KitchenSink/Anything But The Kitchen Sink.cpp
// 
// 	  Version: 24.10.23
// 	
// 	  Copyright (C) 2024 Jeroen Petrus Broks
// 	
// 	  ===========================
// 	  This file is part of a project related to the Phantasar Chronicles or another
// 	  series or saga which is property of Jeroen P. Broks.
// 	  This means that it may contain references to a story-line plus characters
// 	  which are property of Jeroen Broks. These references may only be distributed
// 	  along with an unmodified version of the game.
// 
// 	  As soon as you remove or replace ALL references to the storyline or character
// 	  references, or any termology specifically set up for the Phantasar universe,
// 	  or any other univers a story of Jeroen P. Broks is set up for,
// 	  the restrictions of this file are removed and will automatically become
// 	  zLib licensed (see below).
// 
// 	  Please note that doing so counts as a modification and must be marked as such
// 	  in accordance to the zLib license.
// 	  ===========================
// 
// 
// 	  zLib license terms:
// 	
// 	  This software is provided 'as-is', without any express or implied
// 	  warranty.  In no event will the authors be held liable for any damages
// 	  arising from the use of this software.
// 
// 	  Permission is granted to anyone to use this software for any purpose,
// 	  including commercial applications, and to alter it and redistribute it
// 	  freely, subject to the following restrictions:
// 	
// 	  1. The origin of this software must not be misrepresented; you must not
// 	     claim that you wrote the original software. If you use this software
// 	     in a product, an acknowledgment in the product documentation would be
// 	     appreciated but is not required.
// 	  2. Altered source versions must be plainly marked as such, and must not be
// 	     misrepresented as being the original software.
// 	  3. This notice may not be removed or altered from any source distribution.
// End License
#include <SlyvGINIE.hpp>
#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>

#include <JCR6_Core.hpp>
#include <JCR6_JQL.hpp>
#include <JCR6_zlib.hpp>

using namespace Slyvina;
using namespace Slyvina::JCR6;
using namespace Slyvina::Units;

GINIE Config{ nullptr };

bool UsePic(String Pic) {
	Trans2Upper(Pic);
	if (ExtractExt(Pic) != "PNG") return false;
	if (Config->HasValue("Files", Pic)) return Config->BoolValue("Files", Pic);
	if (Config->BoolValue("SkipDir",ExtractDir(Pic))) return false;
	QCol->Red("Hey! ");
	QCol->Yellow("I do not know about file ");
	QCol->Cyan(Pic + "\n");
opnieuw:
	QCol->Grey("S = Show; Add? (Y/N) ");
	auto a{ Upper(Trim(ReadLine())) };
	if (a.size()) {
		switch (a[0]) {
		case 'S': {
			auto cmd{ Ask("JCR6_Show", Platform(), "I need the JCR6 show executable. Tell me where it is: ") + " \"" + Config->Value("MainJCR", Platform()) + "\" \"" + Pic + "\"" };
			QCol->Doing("Executing", cmd);
			system(cmd.c_str());
			break;
		}
		case 'Y': Config->BoolValue("Files", Pic, true); return true;
		case 'N': Config->BoolValue("Files", Pic, false); return false;
		case 'D': Config->BoolValue("SkipDir", ExtractDir(Pic), true); return false;
		}
	}
	goto opnieuw;
}

int main(int countargs,char** args) {
	auto ConfigName{ StripExt(args[0]) + ".ini" };
	QCol->Magenta("Anything but the kitchen sink\n");
	QCol->Doing("Coded by", "Jeroen P. Broks");
	Config = LoadOptGINIE(ConfigName, ConfigName, "Anything but the kitchen sink\n(c) Jeroen P. Broks");
	AskGINIE = Config;
	ChangeDir(Ask("PWD", Platform(), "Run dir: "));
	auto JCR6File{ Ask("MainJCR", Platform(), "Main JCR6 file: ") };
	QCol->Doing("Analysing", JCR6File);
	init_zlib();
	InitJQL();
	auto J{ JCR6_Dir(JCR6File) };
	if (Last()->Error) {
		QCol->Error(Last()->ErrorMessage);
		return 500;
	}
	auto Entries{ J->Entries() };
	std::vector<String> UsePicList{};
	for (auto Ent : *Entries) {
		if (UsePic(Ent->Name())) {
			UsePicList.push_back(Ent->Name());
			QCol->Doing("Added", Ent->Name());
		}
	}
	QCol->Doing("Generating", "Script");
	String Src{ "Module\n\nTable ABTKSLIJST = {}\n\nInit\n" };
	for (auto p : UsePicList) {
		Src += "\tABTKSLIJST[Len(ABTKSLIJST)] = { [\"Tag\"] = \"ANYTHINGBUTKITCHENSINK_" + Upper(ChReplace(p, '/', '_')) + "\", [\"Entry\"]=\"" + p + "\" }\n";
	}
	Src += "End\n\n";
	auto SaveScript{ Ask("SaveScript",Platform(),"Save generated script to: ") };
	QCol->Doing("Saving", SaveScript);
	SaveString(SaveScript, Src);
	QCol->Cyan("Ok\n\n");
	QCol->Reset();
	return 0;
}
