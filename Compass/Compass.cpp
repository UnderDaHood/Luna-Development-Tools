// Lic:
// Dev/Luna Development Tools/Compass/Compass.cpp
// Version: 23.11.02
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
#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>
#include <SlyvAsk.hpp>
#include <SlyvDir.hpp>

#include <JCR6_RealDir.hpp>
#include <JCR6_zlib.hpp>

#include <Kthura_Core.hpp>

using namespace std;
using namespace Slyvina;
using namespace Units;
using namespace JCR6;
using namespace Kthura;

string myexe{}, myexedir{}, datafile{}, kthuradir{}, compassoutdir{};
GINIE Data{ nullptr };

bool AllowedTag(string Tag) {
	Trans2Upper(Tag);
	vector<string> Forbidden{ "ZONE", "CHEST", "RNDITEM", "RANDOM_TREASURE","SAVE" };
	for (auto chk : Forbidden) if (Prefixed(Tag, chk)) return false;
	return true;
}


void AddTag(GINIE d, KthuraObject* o, string laytag, string name) {
	auto objtag{ o->Tag() };
	auto ctag = "COMPASSSPOT::" + laytag + "::" + objtag;

	d->Add("Compasstags", laytag, objtag, true, true);
	d->Value(ctag, "Name", name);
	switch (o->Kind()) {
	case KthuraKind::StretchedArea:
	case KthuraKind::Rect:
	case KthuraKind::TiledArea:	
	case KthuraKind::Zone:
		d->Value(ctag, "X", o->x() + (o->w() / 2));
		d->Value(ctag, "Y", o->y() + (o->h() / 2));
		break;
	default:
		d->Value(ctag, "X", o->x());
		d->Value(ctag, "Y", o->y());
	}
}

void ScanMap(string mapfile) {
	GINIE outdata{ ParseGINIE("[***]\nLastModified=" + CurrentDate() + "; " + CurrentTime()) };
	Data->NewValue("GlobalTags", "Next", "Yes");
	Data->NewValue("GlobalTagName", "Next", "Forward");
	Data->NewValue("GlobalTags", "Prev", "Yes");
	Data->NewValue("GlobalTagName", "Prev", "Backward");
	QCol->Doing("Processing", mapfile);
	auto kMap{ LoadKthura(kthuradir+"/"+mapfile)};
	auto lays{ *kMap->Layers() };
	for (auto laytag : lays) {
		auto lay = (*kMap)[laytag];
		QCol->Doing("=> Layer", laytag);
		for (auto o = lay->FirstObject(); o; o = o->Next()) {
			QCol->Doing("=> Object", o->ID(), "\r");
			auto Tag = o->Tag();
			auto LocalTag{ "LOCALTAG::" + mapfile + "::" + laytag + "::" + Tag };
			if (Tag.size() && AllowedTag(Tag)) {
				if (!Data->Value("GlobalTags", Tag).size()) {
					QCol->Red("Uknown item\n");
					QCol->Doing("Object", o->ID());
					QCol->Doing("Kind", o->SKind());
					QCol->Doing("Coords", TrSPrintF("(%d,%d)", o->x(), o->y()));
					QCol->Doing("Size", TrSPrintF("%dx%d", o->w(), o->h()));
					QCol->Doing("Layer", laytag);
					QCol->Doing("Map", mapfile);
					QCol->Magenta("What to do?\n");
				}
				if (Yes(Data, "GlobalTags", Tag, "Is tag \"" + Tag + "\" a global tag")) {
					AddTag(outdata, o, laytag, Ask(Data, "GlobalTagName", Tag, "Description of global tag \"" + Tag + "\":"));
				} else if (Yes(Data,LocalTag, "Use", "Use \"" + Tag + "\" as a local tag then")) {
					AddTag(outdata,o,laytag,Ask(Data,LocalTag,"Name","Name for local tag \""+Tag+"\":"));
				}
			}
		}
	}
	auto SaveFile{ compassoutdir + "/" + mapfile + "/Compass.ini" };
	auto SaveDir{ ExtractDir(SaveFile) };
	if (!DirectoryExists(SaveDir)) {
		QCol->Doing("=> Creating", SaveDir);
		MakeDir(SaveDir);
	}
	QCol->Doing("=> Saving", SaveFile);
	SaveString(SaveFile, outdata->UnParse("Compass data for: " + mapfile + "\n(c) Jeroen P. Broks!\nDo NOT extract this file from the project without prior written permission!"));
}


int main(int,char** args) {
	init_zlib();
	JCR6_InitRealDir();
	QCol->LGreen("Compass for Luna's Father!\n");
	QCol->Doing("Coded by", "Jeroen P. Broks");
	myexe = args[0]; myexedir = ExtractDir(args[0]); ChangeDir(myexedir);
	datafile = TrSPrintF("%s/Compass.ini", myexedir.c_str());
	if (!FileExists(datafile)) SaveString(datafile,"[System]\nCreated:" + CurrentDate());
	Data = LoadGINIE(datafile,datafile,"Compass config data!\n Last modified:"+CurrentDate()+"\n(c) Jeroen P. Broks");
	kthuradir = Ask(Data, "Directories", "Kthura", "Where can I find the Kthura files?");
	QCol->Doing("Analysing", kthuradir);
	auto kthuramaps{ *FileList(kthuradir) };
	QCol->Doing("Maps found", kthuramaps.size());
	compassoutdir = Ask(Data, "Directories", "Compass", "Where should I put the compass files?");
	cout << endl;
	for (auto& mf : kthuramaps) ScanMap(mf);
	QCol->Cyan("Ok\n");
	QCol->Reset();
	return 0;
}