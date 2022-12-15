// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;
using Microsoft.Win32;

namespace hMailServer.Administrator
{
   public partial class ucListView : ListView
   {
      private const string ListViewSaveLocation = @"Software\hMailServer\Administrator\ListViews";

      private ListViewColumnSorter _columnSorter;

      public delegate void BeforeSelectedIndexChangedHandler();
      public event BeforeSelectedIndexChangedHandler BeforeSelectedIndexChanged;
      
      List<int> _numericSortOrders = new List<int>();
      private bool _numericalColumnsLoaded = false;

      List<int> _datetimeSortOrders = new List<int>();
      private bool _datetimeColumnsLoaded = false;

      List<int> _ipaddressSortOrders = new List<int>();
      private bool _ipaddressColumnsLoaded = false;

      public ucListView()
      {
         _columnSorter = new ListViewColumnSorter();

         this.ListViewItemSorter = _columnSorter;

         this.FullRowSelect = true;
      }

      private void LoadNumericalColumns()
      {
         foreach (ColumnHeader column in Columns)
         {
            var tag = column.Tag as string;
            if (string.IsNullOrEmpty(tag))
               continue;

            if (string.Compare(tag, "Numeric", true, CultureInfo.InvariantCulture) == 0)
               _numericSortOrders.Add(column.Index);
         }
      }
      
      private void LoadDateTimeColumns()
      {
         foreach (ColumnHeader column in Columns)
         {
            var tag = column.Tag as string;
            if (string.IsNullOrEmpty(tag))
               continue;

            if (string.Compare(tag, "DateTime", true, CultureInfo.InvariantCulture) == 0)
               _datetimeSortOrders.Add(column.Index);
         }
      }

      private void LoadIPAddressColumns()
      {
         foreach (ColumnHeader column in Columns)
         {
            var tag = column.Tag as string;
            if (string.IsNullOrEmpty(tag))
               continue;

            if (string.Compare(tag, "IPAddress", true, CultureInfo.InvariantCulture) == 0)
               _ipaddressSortOrders.Add(column.Index);
         }
      }

      protected override void OnSelectedIndexChanged(EventArgs e)
      {
         base.OnSelectedIndexChanged(e);
      }

      protected override void OnMouseDown(MouseEventArgs e)
      {
         ListViewHitTestInfo info = base.HitTest(new System.Drawing.Point(e.X, e.Y));
         ListViewItem item = info.Item;

         if (item != null)
         {
            int index = item.Index;
            if (this.SelectedIndices.Count == 0)
            {
               if (index != 0)
                  OnBeforeSelectedIndexChanged();
            }
            if (this.SelectedIndices.Count != 0 && index != this.SelectedIndices[0])
            {
               OnBeforeSelectedIndexChanged();
            }
         }

         base.OnMouseDown(e);
      }

      protected override void OnKeyDown(KeyEventArgs e)
      {
         OnBeforeSelectedIndexChanged();

         base.OnKeyDown(e);
      }

      protected virtual void OnBeforeSelectedIndexChanged()
      {
         if (BeforeSelectedIndexChanged != null)
         {
            BeforeSelectedIndexChanged();
         }
      }

      protected override void  OnColumnClick(ColumnClickEventArgs e)
      {
         if (!_numericalColumnsLoaded)
         {
            LoadNumericalColumns();
            _numericalColumnsLoaded = true;
         }

         if (!_datetimeColumnsLoaded)
         {
            LoadDateTimeColumns();
            _datetimeColumnsLoaded = true;
         }

         if (!_ipaddressColumnsLoaded)
         {
            LoadIPAddressColumns();
            _ipaddressColumnsLoaded = true;
         }

         // Determine if clicked column is already the column that is being sorted.
         if (e.Column == _columnSorter.SortColumn)
         {
            // Reverse the current sort direction for this column.
            if (_columnSorter.Order == SortOrder.Ascending)
            {
               _columnSorter.Order = SortOrder.Descending;
            }
            else
            {
               _columnSorter.Order = SortOrder.Ascending;
            }
         }
         else
         {
            // Set the column number that is to be sorted; default to ascending.
            _columnSorter.SortColumn = e.Column;
            _columnSorter.Order = SortOrder.Ascending;
         }

         if (_numericSortOrders.Contains(e.Column))
            _columnSorter.NumericSort = true;

         if (_datetimeSortOrders.Contains(e.Column))
            _columnSorter.DateTimeSort = true;

         if (_ipaddressSortOrders.Contains(e.Column))
            _columnSorter.IPAddressSort = true;

         // Perform the sort with these new sort options.
         this.Sort();

 	      base.OnColumnClick(e);
      }

      /// <summary>
      /// Saves the width of the columns in the registry.
      /// </summary>
      /// <param name="viewName"></param>
      public void SaveWidths(string viewName)
      {
         var currentUserKey = Registry.CurrentUser;
         var administratorKey = currentUserKey.CreateSubKey(ListViewSaveLocation);

         for (int i = 0; i < Columns.Count; i++)
         {
            var columnHeader = Columns[i];
            string columnName = viewName + "_" + columnHeader.Text;
            administratorKey.SetValue(columnName, columnHeader.Width);
         }

         administratorKey.Close();
         currentUserKey.Close();

      }

      /// <summary>
      /// Loads the width of the columns from the registry.
      /// </summary>
      /// <param name="viewName"></param>
      public void LoadWidths(string viewName)
      {
         var currentUserKey = Registry.CurrentUser;
         var administratorKey = currentUserKey.CreateSubKey(ListViewSaveLocation);

         for (int i = 0; i < Columns.Count; i++)
         {
            var columnHeader = Columns[i];
            string columnName = viewName + "_" + columnHeader.Text;
            object value = administratorKey.GetValue(columnName, null);

            if (value is int)
               columnHeader.Width = Convert.ToInt32(value);
         }

         administratorKey.Close();
         currentUserKey.Close();
      }
   }
}
