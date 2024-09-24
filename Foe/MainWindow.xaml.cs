// Lic:
// Luna's Father
// Foe Editor - Main Window
// 
// 
// 
// (c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.04.19
// EndLic
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
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
			TFoe.RegGadget(DrawType, "MetaData", "DrawType");
			TFoe.RegGadget(Scale, "MetaData", "Scale");
			TFoe.RegGadget(AltX, "AltCoords", "X");
			TFoe.RegGadget(AltY, "AltCoords", "Y");

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
			TFoe.RegGadget(Joker, "Script", "Joker");

			TFoe.RegGadget(ResistEarth, "Resist_Element", "Earth");
			TFoe.RegGadget(FatalEarth, "Fatal", "Earth");
			TFoe.RegGadget(ResistWood, "Resist_Element", "Wood");
			TFoe.RegGadget(FatalWood, "Fatal", "Wood");
			TFoe.RegGadget(ResistWater, "Resist_Element", "Water");
			TFoe.RegGadget(FatalWater, "Fatal", "Water");
			TFoe.RegGadget(ResistFrost, "Resist_Element", "Frost");
			TFoe.RegGadget(FatalFrost, "Fatal", "Frost");
			TFoe.RegGadget(ResistWind, "Resist_Element", "Wind");
			TFoe.RegGadget(FatalWind, "Fatal", "Wind");
			TFoe.RegGadget(ResistLightning, "Resist_Element", "Lightning");
			TFoe.RegGadget(FatalLightning, "Fatal", "Lightning");
			TFoe.RegGadget(ResistFire, "Resist_Element", "Fire");
			TFoe.RegGadget(FatalFire, "Fatal", "Fire");
			TFoe.RegGadget(ResistLava, "Resist_Element", "Lava");
			TFoe.RegGadget(FatalLava, "Fatal", "Lava");
			TFoe.RegGadget(ResistLight, "Resist_Element", "Light");
			TFoe.RegGadget(FatalLight, "Fatal", "Light");
			TFoe.RegGadget(ResistDarkness, "Resist_Element", "Darkness");
			TFoe.RegGadget(FatalDarkness, "Fatal", "Darkness");

			TFoe.RegGadget(Experience, "Kill", "Experience");
			TFoe.RegGadget(Cash, "Kill", "Cash");

			TFoe.RegGadget(ItemDrop1, "Drop1", "Item");
			TFoe.RegGadget(ItemDrop2, "Drop2", "Item");
			TFoe.RegGadget(ItemSteal, "Steal", "Item");
			TFoe.RegGadget(ItemDrop1Rate, "Drop1", "Rate");
			TFoe.RegGadget(ItemDrop2Rate, "Drop2", "Rate");
			TFoe.RegGadget(ItemStealRate, "Steal", "Rate");

			TFoe.RegGadget(ExtractFire, "Extract", "Fire");
			TFoe.RegGadget(ExtractLava, "Extract", "Lava");
			TFoe.RegGadget(ExtractEarth, "Extract", "Earth");
			TFoe.RegGadget(ExtractWood, "Extract", "Wood");
			TFoe.RegGadget(ExtractWater, "Extract", "Water");
			TFoe.RegGadget(ExtractFrost, "Extract", "Frost");
			TFoe.RegGadget(ExtractWind, "Extract", "Wind");
			TFoe.RegGadget(ExtractLightning, "Extract", "Lightning");
			TFoe.RegGadget(ExtractLight, "Extract", "Light");
			TFoe.RegGadget(ExtractDarkness, "Extract", "Darkness");

			TFoe.RegGadget(ResistPoison, "Resist_Status", "Poison");
			TFoe.RegGadget(ResistDisease, "Resist_Status", "Disease");
			TFoe.RegGadget(ResistParalysis,"Resist_Status", "Paralysis");
			TFoe.RegGadget(ResistSleep, "Resist_Status", "Sleep");
			TFoe.RegGadget(ResistCurse, "Resist_Status", "Curse");
			TFoe.RegGadget(ResistExhausted, "Resist_Status", "Exhaustion");
			TFoe.RegGadget(ResistZombie, "Resist_Status", "Zombie");
			TFoe.RegGadget(ResistDeath, "Resist_Status", "Death");

			TFoe.RegGadget(StartStatusPoison, "Start_Status", "Poison");
			TFoe.RegGadget(StartStatusDisease, "Start_Status", "Disease");
			TFoe.RegGadget(StartStatusParalysis, "Start_Status", "Paralysis");
			TFoe.RegGadget(StartStatusExhausted, "Start_Exhausted", "Exhaustion");
			TFoe.RegGadget(StartStatusZombie, "Start_Zombie", "Zombie");
			TFoe.RegGadget(StartStatusSleep, "Start_Status", "Sleep");
			TFoe.RegGadget(StartStatusCurse, "Start_Status", "Curse");
			TFoe.RegGadget(StartStatusUndead, "Start_Status", "Undead");

			TFoe.RegGadget(Joker, "Joker", "Joker");
			
			TFoe.AIActTarLnk[AI_ACT_Target1] = 1;
			TFoe.AIActTarLnk[AI_ACT_Target2] = 2;
			TFoe.AIActTarLnk[AI_ACT_Target3] = 3;
			TFoe.AIActRateLnk[AI_ACT_Rate1] = 1;
			TFoe.AIActRateLnk[AI_ACT_Rate2] = 2;
			TFoe.AIActRateLnk[AI_ACT_Rate3] = 3;
			EditTabber.IsEnabled = false;

			RefreshSaveGames();
		}

		void RefreshSaveGames() {
			var path = DirectoryConfig.Get("SaveGameDir");
			var sgfiles = FileList.GetTree(path);
			SaveGame.Items.Clear();
			foreach (var sgfile in sgfiles) { SaveGame.Items.Add(sgfile); }
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

		private void BaseComboChange(object sender, SelectionChangedEventArgs e) {
			if (!TFoe.MayEdit) return;
			var MCV = (ComboBox)sender;
			var GID = TFoe.BXLink[MCV];
			if (MCV.SelectedItem == null)
				TFoe.Current.Data[GID.Cat, GID.Val] = "";
			else
				TFoe.Current.Data[GID.Cat, GID.Val] = MCV.SelectedItem.ToString();
		}

		private void ViewSaveGame(object sender, SelectionChangedEventArgs e) {
			var VSG = (ComboBox)sender; if (VSG.SelectedItem == null) return;
			var parameters = $"{DirectoryConfig.Get("SaveGameDir")}/{VSG.SelectedItem}";
			var cmd = DirectoryConfig.GetFile("SaveGameCharViewer");			
			var output = new StringBuilder($"$ {cmd} \"{parameters}\"\n\n");
			//QuickStream.PushDir();
			//Debug.WriteLine($"Going to dir: {Prj.OutputGit}");
			//Directory.SetCurrentDirectory(Prj.OutputGit);
			var pgit = new Process();
			pgit.StartInfo.FileName = cmd;
			pgit.StartInfo.Arguments = $"\"{parameters}\"";
			pgit.StartInfo.CreateNoWindow = true;
			pgit.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
			pgit.StartInfo.RedirectStandardOutput = true;
			pgit.StartInfo.UseShellExecute = false;
			pgit.Start();
			while (!pgit.StandardOutput.EndOfStream) {
				var p = pgit.StandardOutput.ReadLine();
				output.Append($"{p}\n");
				Debug.WriteLine($"CALL>{p}");
			}
			pgit.WaitForExit();
			output.Append($"\n\n\nDone! Exit code {pgit.ExitCode}");
			SaveGameResults.Text = output.ToString();
			//if (pgit.ExitCode != 0) {
			//	Fout.Error($"Call returned exit code {pgit.ExitCode}");
			//	return false;
			//}
			//return true;
		}

		private void TextBaseEdit_DrawType(object sender, TextChangedEventArgs e) {
			var i = qstr.ToInt(((TextBox)sender).Text);
			switch (i) {
				case 0: DrawType_Show.Content = "None"; break;
				case 1: DrawType_Show.Content = "Negative"; break;
				case 2: DrawType_Show.Content = "Flip"; break;
				case 3: DrawType_Show.Content = "Animation"; break;
				default:DrawType_Show.Content = "?? UNKNOWN ??"; break;
			}
			TextBaseEdit(sender, e);
		}
	}
	
}