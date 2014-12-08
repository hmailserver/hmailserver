// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Shared
{
   public partial class ucWizard : UserControl
   {
      private List<UserControl> _pages;
      private Dictionary<string, string> _state;
      private int _currentPage;

      public delegate void PageChangedEventHandler (int currentPage, int lastPage);
      
      public event EventHandler Cancel;
      public event PageChangedEventHandler PageChanged;

      public ucWizard()
      {
         InitializeComponent();

         _pages = new List<UserControl>();
         _state = new Dictionary<string, string>();
         _currentPage = 0;

         // Check if we should run the wizard in silent mode...
         if (CommandLineParser.ContainsArgument("/silent"))
            _state = CommandLineParser.GetArguments();
      }

      public void AddPage(UserControl page)
      {
         IWizardPage wizPage = page as IWizardPage;

         if (wizPage == null)
            throw new Exception("Page is not a wizard page");

         _pages.Add(page);
      }

      public void Start()
      {
         if (CommandLineParser.ContainsArgument("/silent"))
         {
            for (int i = 0; i < _pages.Count; i++)
            {
               if (!ShowPage(i))
                  return;
            }

            if (Cancel != null)
               Cancel(this, null);
         }
         else
            ShowPage(0);

      }

      public bool ShowPage(int pageNo)
      {
         if (panelMiddle.Controls.Count > 0)
         {
            UserControl currentPage = panelMiddle.Controls[0] as UserControl;
            IWizardPage currentWizPage = currentPage as IWizardPage;

            bool nextEnabled = buttonNext.Enabled;
            bool previousEnabled = buttonPrevious.Enabled;
            bool cancelEnabled = buttonCancel.Enabled;

            buttonNext.Enabled = false;
            buttonPrevious.Enabled = false;
            buttonCancel.Enabled = false;

            try
            {
               if (!currentWizPage.OnLeavePage(pageNo > _currentPage))
                  return false;
            }
            catch (Exception ex)
            {
               MessageBox.Show(ex.Message);
               return false;
            }
            finally
            {
               buttonNext.Enabled = true;
               buttonPrevious.Enabled = true;
               buttonCancel.Enabled = true;
            }
         }

         _currentPage = pageNo;

         UserControl page = _pages[_currentPage];
         IWizardPage wizPage = page as IWizardPage;

         new TabOrderManager(page).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

         panelMiddle.Controls.Clear();
         panelMiddle.Controls.Add(page);

         page.Dock = DockStyle.Fill;

         labelTitle.Text = wizPage.Title;

         EnableDisable();
         SetButtonTexts();

         wizPage.OnShowPage(_state);

         if (PageChanged != null)
            PageChanged(_currentPage+1, _pages.Count);

         return true;
      }

      private void buttonPrevious_Click(object sender, EventArgs e)
      {
         Cursor current = Cursor.Current;

         this.Cursor = Cursors.WaitCursor;
         ShowPage(_currentPage - 1);
         this.Cursor = current;
      }

      private void buttonCancel_Click(object sender, EventArgs e)
      {
         if (Cancel != null)
            Cancel(sender, e);
      }

      private void buttonNext_Click(object sender, EventArgs e)
      {

         if (IsLastPage)
         {
            if (Cancel != null)
               Cancel(sender, e);

            return;
         }

         Cursor current = Cursor.Current;
         this.Cursor = Cursors.WaitCursor;
         ShowPage(_currentPage + 1);
         this.Cursor = current;
      }

      private bool IsLastPage
      {
         get
         {
            return _currentPage == _pages.Count - 1;
         }
      }

      private void EnableDisable()
      {
         // On the last page, we can only press Close.
         buttonCancel.Enabled = !IsLastPage;

         // Previous shouldn't be enabled on the first page or on the last page.

         buttonPrevious.Enabled = _currentPage != 0 && !IsLastPage;
      }

      private void SetButtonTexts()
      {
         if (_currentPage != _pages.Count - 1)
            buttonNext.Text = "Next >";
         else
            buttonNext.Text = "Close";
      }

      
   }
}
