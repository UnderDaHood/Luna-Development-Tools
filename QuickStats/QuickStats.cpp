// Lic:
// Luna's Father
// Quick Stats
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
// Version: 23.10.18
// EndLic

#include <SlyvString.hpp>

#include <Statistician.hpp>
#include <Statistician_LoadJCR6.hpp>

#include <JCR6_Core.hpp>
#include <JCR6_zlib.hpp>



using namespace std;
using namespace Slyvina;
using namespace JCR6;
using namespace Units;
using namespace Statistician;

static const char chars[6][10]{ "Myrah","Francis","Luna","Scyndi","Aziella","Rachel" };
VecString HaveStat{ NewVecString() };
map<string, map<string, int64>> QS;

void JCR6EH(string EM) {
	printf("JCR6 error!\n");
	printf("Error: %s\n", EM.c_str());
	printf("Entry: %s\n", Last()->Entry.c_str());
	printf("Main:  %s\n", Last()->MainFile.c_str());
	exit(1);
}

void Analyse(string f) {
	//static auto pre{ Upper("Statistician.Luna/Characters/") };
	auto K = JCR6_Dir(f);
	auto P = JCR6_LoadParty(K, "Statistician.Luna");
	auto ChL{ P->CharList() };
	for (auto ch : *ChL) {
		//printf("%s\n", ch.c_str());
		auto ChD{ P->Ch(ch) };
		auto StatL{ ChD->StatList() };
		for (auto Stat : *StatL) {
			if (!VectorContains(HaveStat, Stat)) HaveStat->push_back(Stat);
			QS[ch][Stat] = ChD->Statistic(Stat)->Total();
		}
	}
	for (int i = 0; i < 6; i++) printf("%9s ", chars[i]); printf("\n");
	for (auto Stat : *HaveStat) {
		for (int i = 0; i < 6; i++) {
			printf("%9d ", (int)QS[Upper(chars[i])][Stat]);
		}
		printf(" %s\n", Stat.c_str());
	}
}

int main(int c, char** args) {
	JCR6PANIC = JCR6EH;
	init_zlib();
	for (int i = 1; i < c; ++i) {
		printf("%d/%d: Analysing: %s\n", i , c-1, args[i]);
		Analyse(args[i]);
	}
}