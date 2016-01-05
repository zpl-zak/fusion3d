using System;
using System.Collections.Generic;
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
    /// Interaction logic for ProgramEditor.xaml
    /// </summary>
    public partial class ProgramEditor : Window
    {
        public ProgramEditor(string name)
        {
            InitializeComponent();

            NameStr = name;

            UpdateList();
        }

        void UpdateList()
        {
            ProgramList.Items.Clear();
            foreach (var x in ProjectsUtil.GetFiles(NameStr))
            {
                if (x.Name.Contains(".h"))
                    ProgramList.Items.Add(System.IO.Path.GetFileNameWithoutExtension(x.Name));
            }

            if (ProgramList.Items.Count == 0)
            {
                RemoveProgram.IsEnabled = false;
                RenameProgram.IsEnabled = false;
            }
            else
            {
                ProgramList.SelectedIndex = 0;
                RemoveProgram.IsEnabled = true;
                RenameProgram.IsEnabled = true;
            }
        }

        string NameStr { get; set; }

        private void GenerateProgram_Click(object sender, RoutedEventArgs e)
        {
            ProjectName dlg = new ProjectName();
            dlg.ShowDialog();
            ProjectsUtil.GenerateProgram(NameStr, dlg.NameStr);

            UpdateList();
        }

        private void RenameProgram_Click(object sender, RoutedEventArgs e)
        {
            ProjectName dlg = new ProjectName();
            dlg.ShowDialog();
            ProjectsUtil.RenameProgram(NameStr, ProgramList.SelectedValue.ToString(), dlg.NameStr);

            UpdateList();
        }

        private void RemoveProgram_Click(object sender, RoutedEventArgs e)
        {
            ProjectsUtil.RemoveProgram(NameStr, ProgramList.SelectedValue.ToString());
        }
    }
}
