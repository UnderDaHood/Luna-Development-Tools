// Lic:
// Luna's Father
// Scenario Aliaser
// 
// 
// 
// (c) Jeroen P. Broks, 2024
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
// Version: 24.01.02
// EndLic
// Languages/Scenario/Dutch/Maps/CH2_Dungeon_SkyTower_1
using System.Data;
using System.Text;
using TrickyUnits;

class ScenAlias {

    static readonly string LDir = @"E:\Projects\Applications\Scyndi\Luna's Father\src\Assets\Tricky Private\Languages\Scenario".Replace("\\", "/");
    static readonly string OutF = @"E:\Projects\Applications\Scyndi\Luna's Father\src\Assets\JQL\Languages".Replace("\\", "/");
    static readonly string[] Languages = new string[] { "Dutch", "English" };

    readonly int leadingzeros;
    readonly string map;
    readonly int num;

    string snum(int n) {        
            var sn = $"{n}";
            while (sn.Length < leadingzeros + 1) sn = $"0{sn}";
            return sn;
        
    }

    ScenAlias(string _map,int _num,int _leadingzeros=0) {
        map = _map;
        leadingzeros= _leadingzeros;
        num = _num;
    }

    static List<ScenAlias> Lijst = new List<ScenAlias>();


    static void Main(string[] args) {
        var JQL = new StringBuilder("JQL\n\n");
        Lijst.Add(new ScenAlias("Maps/CH2_Dungeon_SkyTower", 4));
        

        foreach(var d in Lijst) {
            Console.WriteLine($"Processing: {d.map} ({d.num})");
            foreach (var l in Languages) {
                var Dung = $"{d.map}_{d.snum(1)}";                
                for(int i=2;i<=d.num;i++) {
                    JQL.Append($"Raw:{LDir}/{l}/{Dung}>Scenario/{l}/{d.map}_{d.snum(i)}\n");
                }
            }
        }
        //Console.WriteLine(JQL.ToString()); // Debug
        QuickStream.SaveString(OutF, JQL);
    
    }
}