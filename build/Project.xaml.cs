using System;
using System.Collections.Generic;
using System.Diagnostics;
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
using System.Windows.Shapes;

namespace build
{
    /// <summary>
    /// Interaction logic for Project.xaml
    /// </summary>
    public partial class Project : Window
    {
        public Project(string name)
        {
            InitializeComponent();

            NameStr = name;

            Title = name;

            BuildConfig.SelectedIndex = 0;
        }


        string NameStr { get; set; }

        private void BuildProject_Click(object sender, RoutedEventArgs e)
        {
            ProjectsUtil.Build(NameStr, System.IO.Path.Combine("projects",NameStr), BuildConfig.SelectedItem.ToString().Replace("System.Windows.Controls.ComboBoxItem: ",""));
        }

        private void CopyDependencies_Click(object sender, RoutedEventArgs e)
        {
            ProjectsUtil.CopyDependencies(NameStr, BuildConfig.SelectedItem.ToString().Replace("System.Windows.Controls.ComboBoxItem: ", ""));
        }

        private void CopyData_Click(object sender, RoutedEventArgs e)
        {
            ProjectsUtil.CopyData(NameStr, BuildConfig.SelectedItem.ToString().Replace("System.Windows.Controls.ComboBoxItem: ", ""));
        }

        private void OpenDir_Click(object sender, RoutedEventArgs e)
        {
            Process.Start(System.IO.Path.Combine("projects", NameStr));
        }

        private void ExecProject_Click(object sender, RoutedEventArgs e)
        {
            ProcessStartInfo pci = new ProcessStartInfo();
            pci.WorkingDirectory = System.IO.Path.Combine("projects", NameStr, BuildConfig.SelectedItem.ToString().Replace("System.Windows.Controls.ComboBoxItem: ", ""));
            pci.FileName =  NameStr + ".exe";

            Process.Start(pci);
        }
        
        private void ProgramEditor2_Click(object sender, RoutedEventArgs e)
        {
            ProgramEditor dlg = new ProgramEditor(System.IO.Path.Combine("projects", NameStr, "src", "Programs"));
            dlg.ShowDialog();
        }
    }
}
