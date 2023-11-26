// Lic:
// Luna's Father
// Foe Editor - Directory Configuration
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
// Version: 23.11.11
// EndLic
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;
//using System.Windows.Forms;
//using System.Windows.Forms.Design;
using TrickyUnits;

namespace Foe {


	static internal class DirectoryConfig {


		static readonly string DirFile = Dirry.AD("Scyndi:Projects/Applications/Scyndi/Luna's Father/Dev/Tools/Foes.Directories.ini");

		static private GINIE Directories;

		static DirectoryConfig() {
			Debug.WriteLine($"Reading: {DirFile}");
			Directories = GINIE.FromFile(DirFile);
			Directories.AutoSaveSource = DirFile;
		}

		static public string Get(string path) {
			if (Directories["Dirs",path]=="") {
				Confirm.Error($"No directory set for '{path}'. Please select it");
				var d = FFS.RequestDir().Replace('\\','/');
				if (d == "") Environment.Exit(255);
				Directories["Dirs", path] = d;

			}
			return Dirry.AD(Directories["Dirs", path]);
		}

		static public string GetFile(string file) {
			if (Directories["Files",file]=="") {
                Confirm.Error($"No file set for '{file}'. Please select it");
				var f = FFS.RequestFile().Replace("\\","/");
                if (f == "") Environment.Exit(255);
               Directories["Files", file] = f;
            }
            return Dirry.AD(Directories["Files", file]);
        }

		static public string Foes => Get("Foes");

		static public string IAA => Get("IAA");

		static public void RefreshFoeList(ListBox lb) {
			lb.Items.Clear();
			Debug.WriteLine($"Searching dir: {Foes}");
			var lfoes = FileList.GetTree(Foes);
			foreach(var ifoe in lfoes) {
				if (qstr.ExtractExt(ifoe).ToUpper() == "INI") {
					Debug.WriteLine($"Adding foe: {ifoe}");
					lb.Items.Add(qstr.StripExt(ifoe));
				}
			}
		}

		static bool AllowedIAAPrefix(string abl) {
			return
				qstr.Prefixed(abl, "ITM_REG_") ||
				qstr.Prefixed(abl, "ITM_ALC_") ||
				qstr.Prefixed(abl, "ACT_") ||
				qstr.Prefixed(abl, "ABL_");
			
		}


		static public void RefreshIAA(ListBox lb,bool have) {
			lb.Items.Clear();
			if (TFoe.Current == null) return;
			Debug.WriteLine($"Searching dir: {IAA}");
			var liaa = FileList.GetTree(IAA);
			foreach (var iiaa in liaa) {
				if (qstr.ExtractExt(iiaa).ToUpper() == "INI") {
					var abl = qstr.StripExt(iiaa).ToUpper();
					if (have == TFoe.Current.Data.List("IAA", "HaveIAA").Contains(abl) && AllowedIAAPrefix(abl)) {
						Debug.WriteLine($"Adding IAA: {iiaa}");
						lb.Items.Add(qstr.StripExt(iiaa));
					}
				}
			}
		}

		static bool AllowedIAADropPrefix(string abl) {
			return
				qstr.Prefixed(abl, "ITM_REG_") ||
				qstr.Prefixed(abl, "ITM_ALC_") ||
				qstr.Prefixed(abl, "INGREDIENT_");
		}

		static public void RefreshIAA() {
			RefreshIAA(MainWindow.Me.AI_ActionYes, true);
			RefreshIAA(MainWindow.Me.AI_ActionNo, false);
			MainWindow.Me.AIGridUpdateShow();
		}

		internal static List<string> DroppableItems = null;

		static internal void GetDroppableItem(bool Force=false) {
			if (!Force)	if (DroppableItems != null) return;
			DroppableItems=new List<string>();
			Debug.WriteLine($"Searching dir: {IAA}");
            var liaa = FileList.GetTree(IAA);
            foreach (var iiaa in liaa) {
                if (qstr.ExtractExt(iiaa).ToUpper() == "INI") {
                    var abl = qstr.StripExt(iiaa).ToUpper();
                    if (AllowedIAADropPrefix(abl)) {
                        Debug.WriteLine($"Adding IAA: {iiaa}");
                        DroppableItems.Add(qstr.StripExt(iiaa));
                    }
                }
            }
        }

        static public void RefreshDrops(ComboBox lb) {
			lb.Items.Clear();
			if (TFoe.Current == null) return;
			GetDroppableItem();
			foreach (var i in DroppableItems) lb.Items.Add(i);
			/*
			var liaa = FileList.GetTree(IAA);
			foreach (var iiaa in liaa) {
				if (qstr.ExtractExt(iiaa).ToUpper() == "INI") {
					var abl = qstr.StripExt(iiaa).ToUpper();
					if (AllowedIAADropPrefix(abl)) {
						Debug.WriteLine($"Adding IAA: {iiaa}");
						lb.Items.Add(qstr.StripExt(iiaa));
					}
				}
			}
			*/
		}

		static public void RefreshDrops() {
			RefreshDrops(MainWindow.Me.ItemDrop1);
			RefreshDrops(MainWindow.Me.ItemDrop2);
			RefreshDrops(MainWindow.Me.ItemSteal);
		}

	}
}