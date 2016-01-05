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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;

namespace build
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        void UpdateList()
        {
            ProjectList.Items.Clear();
            foreach (var x in ProjectsUtil.GetDirs("projects"))
            {
                ProjectList.Items.Add(x.Name);
            }

            if (ProjectList.Items.Count == 0)
            {
                LoadProject.IsEnabled = false;
                RemoveProject.IsEnabled = false;
                RenameProject.IsEnabled = false;
            }
            else
            {
                ProjectList.SelectedIndex = 0;
                LoadProject.IsEnabled = true;
                RemoveProject.IsEnabled = true;
                RenameProject.IsEnabled = true;
            }
        }

        private void Grid_Initialized(object sender, EventArgs e)
        {
            UpdateList();

            
        }

        private void CreateProject_Click(object sender, RoutedEventArgs e)
        {
            ProjectName dlg = new ProjectName();
            dlg.ShowDialog();

            ProjectsUtil.CreateProject(dlg.NameStr);

            UpdateList();
        }

        private void RemoveProject_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Are you sure you want to remove the project?", "Remove", MessageBoxButton.YesNo);

            if (result == MessageBoxResult.No) return;

            ProjectsUtil.RemoveProject(ProjectList.SelectedValue.ToString());

            UpdateList();
        }

        private void RenameProject_Click(object sender, RoutedEventArgs e)
        {
            ProjectName dlg = new ProjectName();
            dlg.ShowDialog();

            ProjectsUtil.RenameProject(System.IO.Path.Combine("projects", ProjectList.SelectedValue.ToString()), dlg.NameStr, ProjectList.SelectedValue.ToString());

            UpdateList();
        }

        private void Build_Click(object sender, RoutedEventArgs e)
        {
            ProjectsUtil.Build("Fusion3D", "engine", "Debug");
            ProjectsUtil.Build("Fusion3D", "engine", "Release");
        }

        private void LoadProject_Click(object sender, RoutedEventArgs e)
        {
            Project dlg = new Project(ProjectList.SelectedValue.ToString());
            dlg.Show();
        }
    }
}
