// Lic:
// Luna's Father
// Foe Editor - Main Window
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
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using TrickyUnits;

namespace Foe {
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		static internal MainWindow Me;

		public MainWindow() {
			InitializeComponent();
			Me= this;
			Dirry.InitAltDrives();
			DirectoryConfig.RefreshFoeList(ListBox_FoeList);
			TFoe.RegGadget(NameFoe, "METADATA", "NAME");
			TFoe.RegGadget(DescFoe, "METADATA", "DESC");
			TFoe.RegGadget(ImageFoe, "METADATA", "IMAGE");
			TFoe.RegGadget(ColorR, "Color", "Red");
			TFoe.RegGadget(ColorG, "Color", "Green");
			TFoe.RegGadget(ColorB, "Color", "Blue");
			TFoe.RegGadget(FoeIsBoss, "METADATA", "BOSS");
			TFoe.RegGadget(AI_Script, "FOE_AI", "Script");

			TFoe.RegGadget(sk1Strength, "Stat1", "Strength");
			TFoe.RegGadget(sk2Strength, "Stat2", "Strength");
			TFoe.RegGadget(sk3Strength, "Stat3", "Strength");
			TFoe.RegGadget(LnkStrength, "Link", "Strength");
			TFoe.RegGadget(sk1Defense, "Stat1", "Defense");
			TFoe.RegGadget(sk2Defense, "Stat2", "Defense");
			TFoe.RegGadget(sk3Defense, "Stat3", "Defense");
			TFoe.RegGadget(LnkDefense, "Link", "Defense");
			TFoe.RegGadget(sk1Intelligence, "Stat1", "Intelligence");
			TFoe.RegGadget(sk2Intelligence, "Stat2", "Intelligence");
			TFoe.RegGadget(sk3Intelligence, "Stat3", "Intelligence");
			TFoe.RegGadget(LnkIntelligence, "Link", "Intelligence");
			TFoe.RegGadget(sk1Resistance, "Stat1", "Resistance");
			TFoe.RegGadget(sk2Resistance, "Stat2", "Resistance");
			TFoe.RegGadget(sk3Resistance, "Stat3", "Resistance");
			TFoe.RegGadget(LnkResistance, "Link", "Resistance");
            TFoe.RegGadget(sk1Agility, "Stat1", "Agility");
            TFoe.RegGadget(sk2Agility, "Stat2", "Agility");
            TFoe.RegGadget(sk3Agility, "Stat3", "Agility");
            TFoe.RegGadget(LnkAgility, "Link", "Agility");
            TFoe.RegGadget(sk1Accuracy, "Stat1", "Accuracy");
            TFoe.RegGadget(sk2Accuracy, "Stat2", "Accuracy");
            TFoe.RegGadget(sk3Accuracy, "Stat3", "Accuracy");
            TFoe.RegGadget(LnkAccuracy, "Link", "Accuracy");
            TFoe.RegGadget(sk1Evasion, "Stat1", "Evasion");
            TFoe.RegGadget(sk2Evasion, "Stat2", "Evasion");
            TFoe.RegGadget(sk3Evasion, "Stat3", "Evasion");
            TFoe.RegGadget(LnkEvasion, "Link", "Evasion");
            TFoe.RegGadget(sk1HP, "Stat1", "HP");
            TFoe.RegGadget(sk2HP, "Stat2", "HP");
            TFoe.RegGadget(sk3HP, "Stat3", "HP");
            TFoe.RegGadget(LnkHP, "Link", "HP");

            TFoe.AIActTarLnk[AI_ACT_Target1] = 1;
			TFoe.AIActTarLnk[AI_ACT_Target2] = 2;
			TFoe.AIActTarLnk[AI_ACT_Target3] = 3;
			TFoe.AIActRateLnk[AI_ACT_Rate1] = 1;
			TFoe.AIActRateLnk[AI_ACT_Rate2] = 2;
			TFoe.AIActRateLnk[AI_ACT_Rate3] = 3;
			EditTabber.IsEnabled = false;
		}

		private void Action_NewFoe_Click(object sender, RoutedEventArgs e) {
			var NF = Textbox_NewFoe.Text;
			if (Textbox_NewFoe.Text == "") return;
			if (File.Exists($"{DirectoryConfig.Foes}/{NF}.ini")) {
				Confirm.Error($"File {NF} already exists!"); return;
			}
			TFoe.Get(NF);
			DirectoryConfig.RefreshFoeList(ListBox_FoeList);
			Textbox_NewFoe.Text = "";
		}

		private void TextBaseEdit(object sender, TextChangedEventArgs e) {
			var s = (TextBox)sender;
			var d = TFoe.TBLink[s];
			if (TFoe.MayEdit && TFoe.Current!=null) {
				TFoe.Current.Data[d.Cat, d.Val] = s.Text.Replace("\n","<nl>").Replace("\r","");
			}
		}

		private void PickFoe(object sender, SelectionChangedEventArgs e) {
			TFoe.UpdateGadgets();
		}

		private void CheckBaseEdit(object sender, RoutedEventArgs e) {
			var s=(CheckBox)sender;
			var d = TFoe.CBLink[s];
			s.Content = $"{s.IsChecked}";
			if (TFoe.MayEdit && TFoe.Current != null) {
				TFoe.Current.Data[d.Cat, d.Val] = $"{s.IsChecked}";
			}
		}

		private void ActualAddIAA() {
			var gitem = AI_ActionNo.SelectedItem; if (gitem == null) return;
			var iact = AI_ActionNo.SelectedItem.ToString();
			TFoe.Current.Data.ListAdd("IAA", "HaveIAA", iact);
			DirectoryConfig.RefreshIAA();
		}

		private void ActualRemoveIAA() {
			var gitem = AI_ActionYes.SelectedItem; if (gitem == null) return;
			var iact = AI_ActionYes.SelectedItem.ToString();
			TFoe.Current.Data.ListRemove("IAA", "HaveIAA", iact);
			DirectoryConfig.RefreshIAA();
		}

		public void AIGridUpdateShow() {
			if (AI_ActionYes.SelectedItem == null)
				AI_ActParameterGrid.Visibility = Visibility.Hidden;
			else
				AI_ActParameterGrid.Visibility= Visibility.Visible;
		}

		private void LBIAA_Add(object sender, MouseButtonEventArgs e) => ActualAddIAA();
		private void LBIAA_Remove(object sender, MouseButtonEventArgs e) { }

		private void AI_ActionAddButton_Click(object sender, RoutedEventArgs e) => ActualAddIAA();

		private void AI_ActionRemoveButton_Click(object sender, RoutedEventArgs e) => ActualRemoveIAA();

		private void AI_WorkOutAction(object sender, SelectionChangedEventArgs e) {
			TFoe.UpdateGadgetsAI();
			AIGridUpdateShow();
		}

		private void ChangeAIRate(object sender, TextChangedEventArgs e) {
			var Gadget = (TextBox)sender;
			var AI_Item = AI_ActionYes.SelectedItem; if (AI_Item == null) return;
			var AI_Text = AI_Item.ToString();
			var Tag =  $"ACTION::{AI_Text}";
			if (TFoe.MayEdit) TFoe.Current.Data[Tag, $"Rate_Skill{TFoe.AIActRateLnk[Gadget]}"] = Gadget.Text;
		}

		private void ChangeAITarget(object sender, SelectionChangedEventArgs e) {
			var Gadget = (ComboBox)sender;
			var AI_Item = AI_ActionYes.SelectedItem; if (AI_Item == null) return;
			var AI_Text = AI_Item.ToString();
			var Tag = $"ACTION::{AI_Text}";
			if (TFoe.MayEdit) {
				var TRGT_Item = Gadget.SelectedItem; if (TRGT_Item == null) return;
				var TRGT_Text = TRGT_Item.ToString().Replace("System.Windows.Controls.ComboBoxItem:", "").Trim(); 
				TFoe.Current.Data[Tag, $"TRGT_Skill{TFoe.AIActTarLnk[Gadget]}"] = TRGT_Text;
			}
		}

		bool LinkStatOperation = false;
		private void StatChanged(object sender, TextChangedEventArgs e) {
			if (!TFoe.MayEdit) return;
			TextBaseEdit(sender, e); // A redirect that had to happen anyway
			if (LinkStatOperation) return;
			var me = (TextBox)sender;			
			var GID = TFoe.TBLink[me];
			bool Go = false;
			foreach (var lnk in TFoe.CBLink) { Go = Go || (lnk.Value.Cat == "Link" && lnk.Value.Val == GID.Val && lnk.Key.IsChecked==true); }
			if (!Go) return;
			LinkStatOperation = true;
			foreach(var lg in TFoe.TBLink) {
				Debug.WriteLine($"{GID.Cat}::{GID.Val}> Check {lg.Value.Cat}::{lg.Value.Val}");
				if (qstr.Prefixed(lg.Value.Cat,"Stat") && lg.Value.Val==GID.Val && GID.Cat != lg.Value.Cat) {
					lg.Key.Text = me.Text;
				}
			}
			LinkStatOperation = false;

		}
	}
}