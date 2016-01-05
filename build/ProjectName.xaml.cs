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
    /// Interaction logic for ProjectName.xaml
    /// </summary>
    public partial class ProjectName : Window
    {
        public ProjectName()
        {
            InitializeComponent();
        }

        public string NameStr { get; set; }

        private void Done_Click(object sender, RoutedEventArgs e)
        {
            if (ProjectName2.Text != "" && ProjectName2.Text.All(char.IsLetterOrDigit))
            {
                NameStr = ProjectName2.Text;
                Close();
            }
        }
    }
}
