using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace FusionReflector
{
    class Program
    {
        static List<string> used = new List<string>();
        static void Main(string[] args)
        {
            StringBuilder o = new StringBuilder();

            o.AppendLine("#pragma once");

            Search("src", o, true);

            used = new List<string>();

            o.AppendLine("inline void REFLECT_ENGINE( ) {");
            {
                Search("src", o, false);
            }
            o.AppendLine("}");

            string[] lines = o.ToString().Split('\n').Distinct().ToArray();
            StringBuilder sb = new StringBuilder();

            foreach (var line in lines)
                if (!line.Contains("REGISTER_CLASS();"))
                    sb.AppendLine(line);

            File.WriteAllText("test.h",o.ToString());
           // Console.ReadKey();
        }

        static void Search(string path, StringBuilder o, bool lua)
        {
            DirectoryInfo dir = new DirectoryInfo(path);

            foreach (var d in dir.GetDirectories())
            {
                Search(d.FullName, o, lua);
            }

            if (!dir.FullName.Contains("staticLibs"))
            foreach (var f in dir.GetFiles())
            {
                if (f.Extension == ".h" && !f.FullName.Contains("reflection.generated.h"))
                    if(lua)
                        ScanLua(f.FullName, o);
                    else
                        Scan(f.FullName, o);
            }
        }

        static void Scan(string path, StringBuilder o)
        {
            string[] lines = File.ReadAllLines(path);

            foreach (var l in lines)
            {
                if (l.Contains("class") && !l.EndsWith(";"))
                {
                    Match r = Regex.Match(l, "^class\\s+(\\w+)");

                    if (r.Groups[1].ToString() == "" ||( used.Exists(e => e.Contains(r.Groups[1].ToString())))) continue;

                    o.AppendLine("REGISTER_CLASS(" + r.Groups[1] + ");");
                    o.AppendLine("REGISTER_CLASS(" + r.Groups[1] + "_LUA);");


                    //o.AppendLine("{");
                    {

                    }
                    //o.AppendLine("} // " + r.Groups[1] + " CLASS");
                }
            }
        }

        static void ScanLua(string path, StringBuilder o)
        {
            Process process = new Process();
            process.StartInfo.FileName = "ctags.exe";
            process.StartInfo.Arguments = "-N -x "+path; // Note the /c command (*)
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;
            process.Start();
            //* Read the output (or the error)
            string output = process.StandardOutput.ReadToEnd();
            Console.WriteLine(output);
            string err = process.StandardError.ReadToEnd();
            Console.WriteLine(err);
            process.WaitForExit();

            bool c = false; // are we in class?

            string[] lines = output.Split('\n');

            string lc = "";

            foreach(var l in lines)
            {
                Match r = Regex.Match(l, @"(\w+)\s+(\w+)\s+\w+\s+\S+\s+?(.*)");

                if (r.Groups[2].ToString() == "class")
                {
                    if (c) FinishClass(lc, o);
                    lc = r.Groups[1].ToString();
                    if ((used.Exists(e => e.Contains(r.Groups[1].ToString())))) continue;
                    c = true;

                    o.AppendLine("class " + r.Groups[1] + "_LUA { public:");
                    o.AppendLine("static const char className[];");
                    o.AppendLine("static Luna<" + r.Groups[1] + "_LUA>::RegType methods[];");
                    o.AppendLine(r.Groups[1] + "_LUA(lua_State *L){");
                    o.AppendLine("obj = (" + r.Groups[1] + "*) lua_touserdata(L, 1);");
                    o.AppendLine("}");
                    used.Add(r.Groups[1].ToString());
                } else if(c)
                {
                    if (r.Groups[2].ToString() == "function")
                    {
                        string ms = r.Groups[3].ToString();
                        var sc = ms.IndexOf('(');
                        var ec = ms.IndexOf(')');
                        var ft = ms.Substring(0, sc);
                        ec = (ec == -1) ? ms.Length - 1 : ec;
                        string sli = ms.Substring(sc+1, ec-sc-1);
                        string[] ops = sli.Split(',');
                        o.AppendLine("inline void " + r.Groups[1] + " (lua_State *L){");
                        {
                            foreach (var op in ops)
                            {
                                var fm = Regex.Replace(op, @"\s+", " ");

                                var fmt = fm.Split(' ');

                                if (fmt.Length >= 3)
                                    fmt = new string[] { fmt[1], fmt[2] };

                                if (ft == "void")
                                {
                                    o.AppendLine("obj->" + r.Groups[1] + "((" + ft + "*)lua_nieco());");
                                }
                            }
                        }
                        o.AppendLine("}");
                    }
                }
            }
            FinishClass(lc, o);
        }

        static void FinishClass(string c, StringBuilder o)
        {
            o.AppendLine("private: {");
            {
                o.AppendLine(c + "    *    obj");
            }
            o.AppendLine("}");
        }
    }
}
