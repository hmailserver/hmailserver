// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Controls;
using hMailServer.Administrator.Utilities.Localization;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   class Strings
   {
      static string _languageName = "English";
      static hMailServer.Language _language = null;

      public Strings()
      {
         
      }

      static public string LoadLanguage(string language)
      {
         _languageName = language;

          try
          {
              _language = APICreator.Application.GlobalObjects.Languages.get_ItemByName(_languageName);

              return _languageName;
          }
          catch (Exception)
          {
              string message = string.Format("The language which hMailServer is configured to use {0} is not available.\r\nEnglish will be used instead.", language);
              MessageBox.Show(message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);

              try
              {
                  _language = APICreator.Application.GlobalObjects.Languages.get_ItemByName("English");

                  return "English";
              }
              catch (Exception)
              {
                // 
              }
          }

          return "";
         
      }

      static public void Localize(Control container)
      {
         if (_language == null)
            return;

         Type controlType = container.GetType();
         if (controlType == typeof(TextBox) ||
             controlType == typeof(ucText) ||   
             controlType == typeof(ucDateTimePicker) ||
             controlType == typeof(ucPassword))
         {
            return;
         }

         if (controlType == typeof(TreeView))
         {
            LocalizeTreeView(container as TreeView);
            return;
         }

         if (container.Text.Length > 0)
         {
            container.Text = Translate(container.Text);
         }

         foreach (Control control in container.Controls)
         {
            Localize(control);
         }

         if (container.GetType() == typeof(MenuStrip))
         {
            MenuStrip menu = container as MenuStrip;
            foreach(ToolStripMenuItem item in menu.Items)
            {
               Localize(item);


               Localize(item.DropDownItems);
            }
         }

         if (container.GetType() == typeof(ListView) ||
             container.GetType() == typeof(ucListView))
         {
            ListView view = container as ListView;
            foreach (ColumnHeader header in view.Columns)
            {
               Localize(header);
            }
         }

         if (container.GetType() == typeof(ContextMenuStrip))
         {
            ContextMenuStrip strip = container as ContextMenuStrip;

            foreach(ToolStripItem item in strip.Items)
            {
               Localize(item);
            }

         }


         
      }

      static public void Localize(ToolStripItem tsItem)
      {
         tsItem.Text = Translate(tsItem.Text);
      }

      static public void Localize(ToolStripItemCollection tsColl)
      {
         foreach (ToolStripItem item in tsColl)
         {
            Localize(item);
         }
         
      }

      static public void Localize(ColumnHeader header)
      {
         header.Text = Translate(header.Text);
      }

      static public string Localize(string text)
      {
         return Translate(text);
      }

      static public void LocalizeTreeView(TreeView treeView)
      {
         foreach (TreeNode node in treeView.Nodes)
         {
            Localize(node);
         }
      }

      static public void Localize(TreeNode node)
      {
         node.Text = Translate(node.Text);

      }

      static string Translate(string text)
      {
         if (_language == null)
            return text;

         if (text == "@")
            return text;

         return _language.get_String(text);
      }
   }
}
