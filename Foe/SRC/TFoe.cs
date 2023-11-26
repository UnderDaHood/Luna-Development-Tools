// Lic:
// Luna's Father
// Foe Editor - Foe Data Class
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
// Version: 23.11.24
// EndLic
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using TrickyUnits;

namespace Foe {

	internal struct GINID {
		internal string Cat;
		internal string Val;
		internal GINID(string _C, string _V) { Cat = _C; Val = _V; }
	}


	internal class TFoe {

		readonly internal static Dictionary<string, TFoe> _Register = new Dictionary<string, TFoe>();
		readonly internal static Dictionary<TextBox, GINID> TBLink = new Dictionary<TextBox, GINID>();
		readonly internal static Dictionary<CheckBox, GINID> CBLink = new Dictionary<CheckBox, GINID>();
		readonly internal static Dictionary<ComboBox,GINID> BXLink = new Dictionary<ComboBox,GINID>();
		readonly internal static Dictionary<TextBox,int> AIActRateLnk = new Dictionary<TextBox,int>();
		readonly internal static Dictionary<ComboBox,int> AIActTarLnk = new Dictionary<ComboBox,int>();
		public GINIE Data;
		readonly string IDTag;
		readonly string MyFile;
		static public bool MayEdit = false;

		private TFoe(string n) {
			MyFile = $"{DirectoryConfig.Foes}/{n}.ini";
			IDTag = n.ToUpper();
			Data = GINIE.FromFile(MyFile);
			_Register[IDTag] = this;
			Data.AutoSaveSource = MyFile;
			Data.NewValue("ALG_META", "001_FOE", n);
			Data.NewValue("ALG_META", "002_Creation", DateTime.Now.ToString());
			Data.NewValue("ALG_META", "003_Copyright", $"(c) {DateTime.Now.Year} Jeroen P. Broks");
			Data.NewValue("MetaData", "Name", qstr.StripDir(n));
			Data.NewValue("MetaData", "Image", $"GFX/Combat/Foe/{n}.png");
			Data.NewValue("MetaData", "Scale", "1000");
			Data.NewValue("Color", "Red", "255");
			Data.NewValue("Color", "Green", "255");
			Data.NewValue("Color", "Blue", "255");
			Data.NewValue("MetaData", "Boss", $"{qstr.Suffixed(qstr.ExtractDir(n).ToUpper(), "BOSS")}");
			Data.NewValue("FOE_AI", "Script", "Default");
			foreach (var LGID in CBLink.Values) if (LGID.Cat == "Link") Data.NewValue("Link", LGID.Val, "True");
			foreach (var LGID in TBLink) if (LGID.Value.Cat == "Resist_Element") Data.NewValue(LGID.Value.Cat, LGID.Value.Val, "0");
		}

		static public TFoe Get(string n) {
			var un=n.ToUpper();
			if (_Register.ContainsKey(un)) return _Register[un];
			return new TFoe(n);
		}

		static public string Chosen {
			get {
				var LB = MainWindow.Me.ListBox_FoeList;
				if (LB.SelectedItem == null) return "";
				return LB.SelectedItem.ToString();
			}
		}

		static public TFoe Current { get {
				if (Chosen == "") return null;
				return Get(Chosen);
			} }

		static internal void RegGadget(TextBox lb,string cat,string val) {
			TBLink[lb] = new GINID(cat, val);
		}

		static internal void RegGadget(CheckBox lb, string cat, string val) {
			CBLink[lb] = new GINID(cat, val);
		}
		static internal void RegGadget(ComboBox lb, string cat, string val) {
			BXLink[lb] = new GINID(cat, val);
		}


		static public void UpdateGadgets() {
			MainWindow.Me.EditTabber.IsEnabled = Current != null;
			if (Current == null) return;
			MayEdit = false;
			foreach (var itb in TBLink) {
#if DEBUG
				Debug.WriteLine($"<itb:{itb}>.Key = {itb.Key}; <itb:{itb}>.Value = GINID{'{'}\"{itb.Value.Cat}\",\"{itb.Value.Val}\"{'}'}");
#endif
				itb.Key.Text = Current.Data[itb.Value.Cat, itb.Value.Val].Replace("<nl>", "\r"); 
			}

			foreach (var itb in CBLink) {
#if DEBUG
				Debug.WriteLine($"<itb:{itb}>.Key = {itb.Key}; <itb:{itb}>.Value = GINID{'{'}\"{itb.Value.Cat}\",\"{itb.Value.Val}\"{'}'}");
#endif
				itb.Key.IsChecked = Current.Data[itb.Value.Cat, itb.Value.Val].ToUpper() == "TRUE";
			}
			DirectoryConfig.RefreshDrops();
			foreach (var itb in BXLink) {
				switch(itb.Value.Cat.ToUpper()) {
					case "DROP1":
					case "DROP2":
					case "STEAL":
						for(int i = 0; i < DirectoryConfig.DroppableItems.Count; ++i) {
							Debug.WriteLine($"Check {itb.Value.Cat} drop: {i} {DirectoryConfig.DroppableItems[i]} == {Current.Data[itb.Value.Cat, itb.Value.Val]} -> {DirectoryConfig.DroppableItems[i] == Current.Data[itb.Value.Cat, itb.Value.Val]}");
							if (DirectoryConfig.DroppableItems[i] == Current.Data[itb.Value.Cat, itb.Value.Val]) itb.Key.SelectedIndex = i;
						}

                        break;
					default: break;
				}
			}
			DirectoryConfig.RefreshIAA();
			MayEdit =true;
		}

		static public void UpdateGadgetsAI() {
			var AI_Item = MainWindow.Me.AI_ActionYes.SelectedItem; if (AI_Item == null) return;
			var AI_Act = AI_Item.ToString();
			var Tag = $"ACTION::{AI_Act}";
			Current.Data.NewValue(Tag, "Rate_Skill1", "1");
			Current.Data.NewValue(Tag, "Rate_Skill2", "1");
			Current.Data.NewValue(Tag, "Rate_Skill3", "1");
			Current.Data.NewValue(Tag, "TRGT_Skill1", "Random");
			Current.Data.NewValue(Tag, "TRGT_Skill2", "Random");
			Current.Data.NewValue(Tag, "TRGT_Skill3", "Random");
			MayEdit = false;
			foreach (var Rate in AIActRateLnk) { Rate.Key.Text = Current.Data[Tag, $"Rate_Skill{Rate.Value}"]; }
			foreach (var Trgt in AIActTarLnk) {
				switch (Current.Data[Tag, $"TRGT_Skill{Trgt.Value}"]) {
					case "Random": Trgt.Key.SelectedIndex=0; break;
					case "Weakest": Trgt.Key.SelectedIndex = 1; break;
					case "Strongest": Trgt.Key.SelectedIndex = 2; break;
					default:
						Trgt.Key.SelectedIndex = 0;
						Confirm.Error($"No idea what target type {Current.Data[Tag, $"TRGT_SKill{Trgt.Value}"]} is\nfor skill{Trgt.Value}, but I'm gonna set itback to 'Random'");
						Current.Data[Tag, $"TRGT_SKill{Trgt.Value}"] = "Random";
						break;
				}
			}
			MayEdit = true;
		}

	}
}