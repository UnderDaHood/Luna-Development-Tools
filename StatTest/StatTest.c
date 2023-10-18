// Lic:
// Dev/Luna Development Tools/StatTest/StatTest.c
// Version: 23.10.03
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
// Stat Test

#include <stdio.h>
#include <math.h>

int st[101];

int jround(double i) {
	return floor(i+.5);
}

void HP(const char* ch, int lv1, int lv100) {
	int verschil = lv100 - lv1;
	double gain = (verschil / 99.0);
	printf("%s would gain %f HP per level!\n", ch, gain);
	for (int lv = 1; lv <= 125; ++lv) {
		int _HP = jround(lv1 + (gain * (lv - 1)));
		printf("- On level %d, %s would have %d HP\n",lv,ch,_HP);
	}
}

int main(void) {
	printf("\x1b[31m");
	for(int _lv=1;_lv<=100;_lv++) {
		if (_lv<10 || _lv%5==0) printf("%3d ",_lv);
	}
	printf("\n\x1b[32m");
	for(int i=1;i<=20;i++) {
		double st=i;
		for(int _lv=1;_lv<=100;_lv++) {
			double lv=_lv;
			if (_lv<10 || _lv%5==0) {
				printf("%3d ",i+jround( (st/(double)30) * (lv-1)));
			}			
		}
		printf("\n");
	}
	printf("\x1b[0m");
	HP("Myrah", 20, 222);
	HP("Francis", 15, 200);
	HP("Luna", 40, 500);
	HP("Scyndi", 25, 256);
	HP("Aziella", 30, 400);
	HP("Rachel", 5,300);

	return 0;
}