// Lic:
// Luna's Father
// Split tool
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
// Version: 23.11.26
// EndLic
#include <JCR6_Core.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_Save.hpp>

#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvAsk.hpp>

using namespace Slyvina;
using namespace Kthura;
using namespace Units;
using namespace std;

GINIE Config;

void Process(string kthmap) {
//E:\Projects\Applications\Scyndi\Luna's Father\src\Assets\Tricky Private\Kthura
	auto KDir = Ask(Config, "Directories", "Kthura", "Where can I find the Kthura maps?");
	//QCol->Doing("Dir", KDir);
	map<string, Slyvina::Kthura::Kthura> OutputMap;
	auto KFile{ KDir + "/" + kthmap };
	QCol->Doing("Loading", KFile);
	auto OriMap = LoadKthura(KFile);
	auto Layers = OriMap->Layers();
	for (auto Layer : *Layers) {
		if (Yes(Config, kthmap, "UseLayer::" + Layer, "Use layer " + Layer)) {
			QCol->Doing("Layer", Layer);
			auto LD{ OriMap->Layer(Layer) };
			int sx{ 0 }, sy{ 0 };
			if (!LD->HasTag("Split")) {
				sx = AskInt(Config, kthmap, "Split::" + Layer + "::X", "Split from X: ");
				sy = AskInt(Config, kthmap, "Split::" + Layer + "::Y", "Split from Y: ");
			} else {
				auto o{ LD->Obj("Split") };
				if (Yes(Config, kthmap, "Split::" + Layer + "::UseX", "Use X"))  sx = o->x();
				if (Yes(Config, kthmap, "Split::" + Layer + "::UseX", "Use Y"))  sx = o->y();
			}
			for (auto o = LD->FirstObject(); o; o = o->Next()) {
				auto
					ix{ o->x() > sx },
					iy{ o->y() > sy };
				auto
					T{ TrSPrintF("%s_Split%d%d",Layer.c_str(),ix,iy) };
				if (!OutputMap.count(T)) {
					OutputMap[T] = CreateKthura();
					auto OLay{ OutputMap[T]->NewLayer(Layer) };
					OLay->AutoRemap(false);
					if (Layer != "__BASE") OutputMap[T]->KillLayer("__BASE");
				}
				auto OutLay{ OutputMap[T]->Layer(Layer) };
				auto oo{ OutLay->NewObject(o->Kind()) };
				oo->alpha(o->alpha());
				oo->animframe(o->animframe());
				oo->animskip(o->animskip());
				oo->dominance(o->dominance());
				oo->blend(o->blend());
				oo->blue(o->blue());
				oo->forcepassible(o->forcepassible());
				oo->impassible(o->impassible());
				oo->green(o->green());
				oo->height(o->height());
				oo->width(o->width());
				oo->insertx(o->insertx());
				oo->inserty(o->inserty());
				oo->labels(o->labels());
				oo->red(o->red());
				oo->rotate(o->rotate());
				oo->scalex(o->scalex());
				oo->scaley(o->scaley());
				oo->Tag(o->Tag());
				oo->texture(o->texture());
				oo->visible(o->visible());
				oo->x(o->x());
				oo->y(o->y());
				auto data{ o->data() };
				for (auto k : *data) {
					oo->data(k.first, k.second);
				}
				QCol->Doing(to_string(o->ID()), T+"::"+to_string(oo->ID()));
			}

		} else {
			QCol->Doing("Skipping", Layer);
		}
	}
	for (auto sm : OutputMap) {
		auto OFile{ KDir + "/" + kthmap + "_" + sm.first };
		QCol->Doing("Saving", sm.first);
		Kthura_Save(sm.second, OFile);
	}
}

int main(int argscount, char** args) {
	Config = LoadGINIE(StripExt(args[0]) + ".ini", StripExt(args[0]) + ".ini", "Split Kthura Configuration\n(c) Jeroen P. Broks");
	for (int i = 1; i < argscount; i++) {
		QCol->Doing(TrSPrintF("Map %3d/%3d", i, argscount - 1), args[i]);
		Process(args[i]);
	}
	QCol->Reset();
}