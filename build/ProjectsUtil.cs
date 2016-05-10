using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics;
using System.IO.Compression;
using System.Windows;
using System.Xml;

namespace build
{
    public static class ProjectsUtil
    {
        public static DirectoryInfo[] GetDirs(string name)
        {
            DirectoryInfo dir = new DirectoryInfo(name);

            return dir.GetDirectories();
        }

        public static FileInfo[] GetFiles(string name)
        {
            DirectoryInfo dir = new DirectoryInfo(name);

            return dir.GetFiles();
        }

        public static void Empty(this System.IO.DirectoryInfo directory)
        {
            foreach (System.IO.FileInfo file in directory.GetFiles()) file.Delete();
            foreach (System.IO.DirectoryInfo subDirectory in directory.GetDirectories()) subDirectory.Delete(true);
        }

        public static void CreateProject(string name)
        {
            if (!Directory.Exists("temp"))
                Directory.CreateDirectory("temp");

            DirectoryInfo tmp = new DirectoryInfo("temp");
            tmp.Empty();

            System.IO.Compression.ZipFile.ExtractToDirectory(Path.Combine("templates", "PLACEHOLDER.zip"), "temp");

            RenameProject("temp", name);


            Directory.Move("temp", Path.Combine("projects", name));
        }

        public static void CopyDependencies(string name, string config)
        {
            try
            {
                System.IO.Compression.ZipFile.ExtractToDirectory(Path.Combine("templates", "libraries.zip"), Path.Combine("projects", name, config));
            }
            catch { }

            try
            {
                System.IO.Compression.ZipFile.ExtractToDirectory(Path.Combine("templates", "python.zip"), Path.Combine("projects", name, config));
            }
            catch { }

            try
            {
                System.IO.Compression.ZipFile.ExtractToDirectory(Path.Combine("templates", "data.zip"), Path.Combine("projects", name, config));
            }
            catch { }
            MessageBox.Show("Operation is complete!");
        }

        public static void GenerateProgram(string dir, string name)
        {
            try
            {
                System.IO.Compression.ZipFile.ExtractToDirectory(Path.Combine("templates", "PLACEHOLDERProgram.zip"), dir);
            }
            catch { }

            RenameProgram(dir, "PLACEHOLDERProgram", name);
        }

        public static void RenameProgram(string dir, string oldName, string name)
        {
            var h = File.ReadAllText(Path.Combine(dir, oldName+".h"));
            File.WriteAllText(Path.Combine(dir, oldName + ".h"), h.Replace(oldName, name));

            var cpp = File.ReadAllText(Path.Combine(dir, oldName + ".cpp"));
            File.WriteAllText(Path.Combine(dir, oldName + ".cpp"), cpp.Replace(oldName, name));

            File.Move(Path.Combine(dir, oldName + ".h"), Path.Combine(dir, (oldName + ".h").Replace(oldName, name)));
            File.Move(Path.Combine(dir, oldName + ".cpp"), Path.Combine(dir, (oldName + ".cpp").Replace(oldName, name)));
        }

        public static void RemoveProgram(string dir, string name)
        {
            if (File.Exists(Path.Combine(dir, name + ".h")))
                File.Delete(Path.Combine(dir, name + ".h"));

            if (File.Exists(Path.Combine(dir, name + ".cpp")))
                File.Delete(Path.Combine(dir, name + ".cpp"));
        }

        public static void CopyData(string name, string config)
        {
            string sourcedirectory = Path.Combine("projects", name, "data");

            foreach (string dirPath in Directory.GetDirectories(sourcedirectory, "*", SearchOption.AllDirectories))
            {
                Directory.CreateDirectory(dirPath.Replace(sourcedirectory, Path.Combine("projects",name,config,"data")));
            }
            foreach (string newPath in Directory.GetFiles(sourcedirectory, "*.*", SearchOption.AllDirectories))
            {
                File.Copy(newPath, newPath.Replace(sourcedirectory, Path.Combine("projects", name, config, "data")), true);
            }

            MessageBox.Show("Operation is complete!");
        }

        public static void RemoveProject(string name)
        {
            DirectoryInfo dir = new DirectoryInfo(Path.Combine("projects", name));

            dir.Empty();

            Directory.Delete(Path.Combine("projects", name));
        }

        public static void GenerateHeader(string name)
        {
            string header =
            "// DO NOT EDIT. USE BUILD TOOL FOR UPDATES.\n"
            + "#pragma once\n\n"
            + "#include \"3DEngine.h\"\n\n";

            string userspace = 
            "inline void RegisterUserspace (CoreEngine * engine)"
            + "{"
            + "//----\nengine->SetUserspace ([]() {\n";

            DirectoryInfo dir = new DirectoryInfo(Path.Combine("projects", name, "src", "Programs"));
            foreach (var x in dir.GetFiles())
            {
                if (x.Name.Contains(".h"))
                {
                    header += string.Format("#include \"Programs\\{0}\"\n", x.Name);
                    userspace += string.Format("REGISTER_CLASS({0});\n", x.Name.Split('.')[0]);
                }
            }

            userspace += "});}\n";

            File.WriteAllText(Path.Combine("projects", name, "src", "main.generated.h"), header + userspace);
        }

        public static void Build(string name, string init, string config)
        {
            if (init.Contains("projects"))
            {
                GenerateHeader(name);
            }

            XmlDocument doc = new XmlDocument();
            doc.LoadXml(File.ReadAllText(Path.Combine("projects", name, name + ".vcxproj")));

            XmlElement el = doc.DocumentElement;

            XmlNodeList l = el.ChildNodes;

            XmlNode heads = l[2];
            XmlNode srcs = l[1];

            heads.RemoveAll();
            srcs.RemoveAll();

            foreach(string file in Directory.EnumerateFiles(
                Path.Combine("projects", name, "src"), "*.h", SearchOption.AllDirectories))
            {
                XmlElement n = doc.CreateElement("ClInclude", doc.DocumentElement.NamespaceURI);
                XmlAttribute a = doc.CreateAttribute("Include");
                a.Value = file.Replace(Path.Combine("projects", name),"").Substring(1);
                
                n.SetAttributeNode(a);
                n.RemoveAttribute("xmlns");

                heads.AppendChild(n);
            }

            foreach (string file in Directory.EnumerateFiles(
                Path.Combine("projects", name, "src"), "*.cpp", SearchOption.AllDirectories))
            {
                XmlElement n = doc.CreateElement("ClCompile", doc.DocumentElement.NamespaceURI);
                XmlAttribute a = doc.CreateAttribute("Include");
                a.Value = file.Replace(Path.Combine("projects", name), "").Substring(1);

                n.SetAttributeNode(a);
                n.RemoveAttribute("xmlns");

                srcs.AppendChild(n);
            }

            doc.Save(Path.Combine("projects", name, name + ".vcxproj"));

            ProcessStartInfo pci = new ProcessStartInfo();
            pci.FileName = "build.bat";
            pci.Arguments = name+".sln "+config;
            pci.WorkingDirectory = init;

            Process proc = new Process();
            proc.StartInfo = pci;

            proc.Start();
            proc.WaitForExit();

        }

        public static void RenameProject(string path, string name, string oldName="PLACEHOLDER")
        {
            if (!Directory.Exists(path))
                return;

            if (oldName != "PLACEHOLDER")
                Directory.Move(path, path.Replace(oldName, name)); path = path.Replace(oldName, name);

            foreach (var x in GetFolderTree(path))
            {
                if (x.Contains(".vs")) continue;
                var z = Path.Combine(path, x);

                if (z.Contains(oldName))
                {
                    File.Move(z, z.Replace(oldName, name));
                }

                var y = File.ReadAllText(z.Replace(oldName, name));
                File.WriteAllText(z.Replace(oldName, name), y.Replace(oldName, name));
            }
        }


        public static List<string> GetFolderTree(string path, string basepath = "")
        {
            if (basepath == "")
                basepath = path;

            List<String> files = new List<String>();
            try
            {
                foreach (string f in Directory.GetFiles(path))
                {
                    files.Add(f.Replace(basepath + "\\", ""));
                }
                foreach (string d in Directory.GetDirectories(path))
                {
                    files.AddRange(GetFolderTree(d, basepath));
                }
            }
            catch (System.Exception excpt)
            {
                Console.WriteLine(excpt.Message);
            }

            return files;
        }
    }

    public static class ZipArchiveExtensions
    {
        public static void ExtractToDirectory(this ZipArchive archive, string destinationDirectoryName, bool overwrite)
        {
            if (!overwrite)
            {
                archive.ExtractToDirectory(destinationDirectoryName);
                return;
            }
            foreach (ZipArchiveEntry file in archive.Entries)
            {
                string completeFileName = Path.Combine(destinationDirectoryName, file.FullName);
                if (file.Name == "")
                {// Assuming Empty for Directory
                    Directory.CreateDirectory(Path.GetDirectoryName(completeFileName));
                    continue;
                }
                file.ExtractToFile(completeFileName, true);
            }
        }
    }
}
