// Lic:
// Luna's Father
// Configure Treasure Editor
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

#include "Treasure_Config.hpp"
#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>
#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>
#include <JCR6_Core.hpp>

using namespace Slyvina::TQSG;
using namespace Slyvina::Units;

namespace Luna_Treasure {

	static GINIE _Config{nullptr};

	void RegConfigFile(std::string RCF) {
		//*
		if (!FileExists(RCF)) {
			QCol->Doing("Creating", RCF);
			SaveString(RCF, "[sys]\nCreated=" + CurrentDate());
		}
		QCol->Doing("Loading", RCF);
		_Config = LoadGINIE(RCF, RCF, "Config File");
		//*/
	}

	SDL_Rect WindowSizes() {
		return {
			0,0,
			AskInt(_Config,"Window","Width","Desired Window Width:",1200),
			AskInt(_Config,"Window","Height","Desired Window Height:",950)
		};
	}

	TImageFont DefaultFont() {
		//*
		static TImageFont __ret{nullptr};
		if (!__ret) {
			auto ffile{ Ask(_Config, "Fonts", "Default", "\7Font for [DEFAULT]:") };
			QCol->Doing("Loading", "Default Font >> "+ffile);
			if (!FileExists(ffile)) {
				QCol->Error("Font file does not exist: " + ffile);
				return nullptr;
			}
			__ret = LoadImageFont(ffile);
			if (!__ret) {
				QCol->Error("Loading font file failed: " + ffile);
			}
		}
		return __ret;
		//*/
	}

	TImageFont  FontForKthuraList() {
		//*
		static TImageFont __ret{nullptr};
		if (!__ret) {
			auto ffile{ Ask(_Config, "Fonts", "KthuraList", "\7Font for [Kthura ListT]:") };
			QCol->Doing("Loading", "Kthura List Font >> " + ffile);
			if (!FileExists(ffile)) {
				QCol->Error("Font file does not exist: " + ffile);
				return nullptr;
			}
			__ret = LoadImageFont(ffile);
			if (!__ret) {
				QCol->Error("Loading font file failed: " + ffile);
			}
		}
		return __ret;
		//*/
	}

	Slyvina::TQSG::TImageFont ButtonFont() {
		static TImageFont __ret{nullptr};
		if (!__ret) {
			auto ffile{ Ask(_Config, "Fonts", "Button", "\7Font for [Buttons]:") };
			QCol->Doing("Loading", "Kthura List Font >> " + ffile);
			if (!FileExists(ffile)) {
				QCol->Error("Font file does not exist: " + ffile);
				return nullptr;
			}
			__ret = LoadImageFont(ffile);
			if (!__ret) {
				QCol->Error("Loading font file failed: " + ffile);
				QCol->Doing("JCR6", Slyvina::JCR6::Last()->ErrorMessage);
			}
		}
		return __ret;
	}

	TImageFont SysFont() {
		static TImageFont __ret{nullptr};
		if (!__ret) {
			auto ffile{ Ask(_Config, "Fonts", "Sys", "\7Font for [Sys]:") };
			QCol->Doing("Loading", "Sys Font >> " + ffile);
			if (!FileExists(ffile)) {
				QCol->Error("Font file does not exist: " + ffile);
				return nullptr;
			}
			QCol->Doing("Loading font:", ffile);
			__ret = LoadImageFont(ffile);
			if (!__ret) {
				QCol->Error("Loading font file failed: " + ffile);
				QCol->Doing("JCR6", Slyvina::JCR6::Last()->ErrorMessage);
			}
		}
		return __ret;
	}

	std::string DirKthura() {
		return Ask(_Config, "Directories", "Kthura", "Where to find the Kthura Map Files:");
	}
	std::string DirOutput() {
		return Ask(_Config, "Directories", "Output", "Where to put the output files:");
	}
	std::string DirItems() {
		return Ask(_Config, "Directories", "Items", "Where to put the item files:");
	}
}