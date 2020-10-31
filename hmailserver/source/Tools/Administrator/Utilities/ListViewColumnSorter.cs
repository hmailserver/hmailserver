/*
	Original code comes from http://support.microsoft.com/kb/319401.
		(How to sort a ListView control by a column in Visual C#)
	Have been modified to add support for numerical sorting.
*/
//
// MICROSOFT LIMITED PUBLIC LICENSE
// 
// This license governs use of code marked as “sample” available on this 
// Web Site without a License Agreement , as provided under the Section above
// titled “ NOTICE SPECIFIC TO SOFTWARE AVAILABLE ON THIS WEB SITE ”. If you
// use such code (the “software”), you accept this license. If you do not
// accept the license, do not use the software.
// 
// 1. Definitions
// 
// The terms “reproduce,” “reproduction,” “derivative works,” and “distribution”
// have the same meaning here as under U.S. copyright law.
// 
// A “contribution” is the original software, or any additions or changes to the 
// software.
// 
// A “contributor” is any person that distributes its contribution under 
// this license.
// 
// “Licensed patents” are a contributor’s patent claims that read directly
// on its contribution.
// 
// 2. Grant of Rights
// 
// (A) Copyright Grant- Subject to the terms of this license, including the 
// license conditions and limitations in section 3, each contributor grants
// you a non-exclusive, worldwide, royalty-free copyright license to reproduce
// its contribution, prepare derivative works of its contribution, and distribute 
// its contribution or any derivative works that you create.
// 
// (B) Patent Grant- Subject to the terms of this license, including the 
// license conditions and limitations in section 3, each contributor grants 
// you a non-exclusive, worldwide, royalty-free license under its licensed patents
// to make, have made, use, sell, offer for sale, import, and/or otherwise dispose
// of its contribution in the software or derivative works of the contribution 
// in the software.
// 
// 3. Conditions and Limitations
// 
// (A) No Trademark License- This license does not grant you rights to use any 
// contributors’ name, logo, or trademarks.
// 
// (B) If you bring a patent claim against any contributor over patents that you 
// claim are infringed by the software, your patent license from such contributor
// to the software ends automatically.
// 
// (C) If you distribute any portion of the software, you must retain all copyright,
// patent, trademark, and attribution notices that are present in the software.
// 
// (D) If you distribute any portion of the software in source code form, you may
// do so only under this license by including a complete copy of this license with
// your distribution. If you distribute any portion of the software in compiled or 
// object code form, you may only do so under a license that complies with this license.
// 
// (E) The software is licensed “as-is.” You bear the risk of using it. The contributors
// give no express warranties, guarantees or conditions. You may have additional
// consumer rights under your local laws which this license cannot change. To the extent 
// permitted under your local laws, the contributors exclude the implied warranties of 
// merchantability, fitness for a particular purpose and non-infringement.
// 
// (F) Platform Limitation- The licenses granted in sections 2(A) & 2(B) extend only to
// the software or derivative works that you create that run on a Microsoft Windows 
// operating system product.


using System.Collections;
using System.Windows.Forms;
using System;
using System.Net;

/// <summary>
/// This class is an implementation of the 'IComparer' interface.
/// </summary>
public class ListViewColumnSorter : IComparer
{
	/// <summary>
	/// Specifies the column to be sorted
	/// </summary>
	private int ColumnToSort;
	/// <summary>
	/// Specifies the order in which to sort (i.e. 'Ascending').
	/// </summary>
	private SortOrder OrderOfSort;
	/// <summary>
	/// Case insensitive comparer object
	/// </summary>
	private CaseInsensitiveComparer ObjectCompare;

	private bool _numericSort;

	private bool _datetimeSort;

	private bool _ipaddressSort;

	/// <summary>
	/// Class constructor.  Initializes various elements
	/// </summary>
	public ListViewColumnSorter()
	{
		// Initialize the column to '0'
		ColumnToSort = 0;

		// Initialize the sort order to 'none'
		OrderOfSort = SortOrder.None;

		// Initialize the CaseInsensitiveComparer object
		ObjectCompare = new CaseInsensitiveComparer();
	}

	/// <summary>
	/// This method is inherited from the IComparer interface.  It compares the two objects passed using a case insensitive comparison.
	/// </summary>
	/// <param name="x">First object to be compared</param>
	/// <param name="y">Second object to be compared</param>
	/// <returns>The result of the comparison. "0" if equal, negative if 'x' is less than 'y' and positive if 'x' is greater than 'y'</returns>
	public int Compare(object x, object y)
	{
		int compareResult = 0;
		ListViewItem listviewX, listviewY;

		// Cast the objects to be compared to ListViewItem objects
		listviewX = (ListViewItem)x;
		listviewY = (ListViewItem)y;

		ListViewItem.ListViewSubItem subItemX = ColumnToSort < listviewX.SubItems.Count ? listviewX.SubItems[ColumnToSort] : null;
		ListViewItem.ListViewSubItem subItemY = ColumnToSort < listviewY.SubItems.Count ? listviewY.SubItems[ColumnToSort] : null;

		if (subItemX == null || subItemY == null)
			return 0;

		if (_numericSort)
		{
			try
			{
				Int64 val1 = Convert.ToInt64(subItemX.Text);
				Int64 val2 = Convert.ToInt64(subItemY.Text);

				if (val1 < val2)
					compareResult = -1;
				else if (val1 > val2)
					compareResult = 1;

				if (OrderOfSort == SortOrder.Descending)
					compareResult = -compareResult;

				return compareResult;
			}
			catch (Exception)
			{

			}
		}

		if (_datetimeSort)
		{
			try
			{
				DateTime dateX;
				DateTime dateY;

				if (DateTime.TryParse(subItemX.Text, out dateX) && DateTime.TryParse(subItemY.Text, out dateY))
					compareResult = ObjectCompare.Compare(dateX, dateY);

				if (OrderOfSort == SortOrder.Descending)
					compareResult = -compareResult;

				return compareResult;
			}
			catch (Exception)
			{

			}
		}

		if (_ipaddressSort)
		{
			try
			{
				IPAddress ipaddressX;
				IPAddress ipaddressY;

				if (IPAddress.TryParse(subItemX.Text, out ipaddressX) && IPAddress.TryParse(subItemY.Text, out ipaddressY))
				{
					compareResult = ipaddressX.AddressFamily.CompareTo(ipaddressY.AddressFamily);
					if (compareResult != 0)
						return compareResult;

					var xBytes = ipaddressX.GetAddressBytes();
					var yBytes = ipaddressY.GetAddressBytes();

					var octets = Math.Min(xBytes.Length, yBytes.Length);
					for (var i = 0; i < octets; i++)
					{
						compareResult = xBytes[i].CompareTo(yBytes[i]);

						if (OrderOfSort == SortOrder.Descending)
							compareResult = -compareResult;

						if (compareResult != 0)
							return compareResult;
					}
					return compareResult;
				}
			}
			catch (Exception)
			{

			}
		}

		// Compare the two items
		compareResult = ObjectCompare.Compare(subItemX.Text, subItemY.Text);

		// Calculate correct return value based on object comparison
		if (OrderOfSort == SortOrder.Ascending)
		{
			// Ascending sort is selected, return normal result of compare operation
			return compareResult;
		}
		else if (OrderOfSort == SortOrder.Descending)
		{
			// Descending sort is selected, return negative result of compare operation
			return (-compareResult);
		}
		else
		{
			// Return '0' to indicate they are equal
			return 0;
		}
	}

	/// <summary>
	/// Gets or sets the number of the column to which to apply the sorting operation (Defaults to '0').
	/// </summary>
	public int SortColumn
	{
		set
		{
			NumericSort = false;
			DateTimeSort = false;
			IPAddressSort = false;
			ColumnToSort = value;
		}
		get
		{
			return ColumnToSort;
		}
	}

	public bool NumericSort
	{
		set
		{
			_numericSort = value;
		}
		get
		{
			return _numericSort;
		}
	}

	public bool DateTimeSort
	{
		set
		{
			_datetimeSort = value;
		}
		get
		{
			return _datetimeSort;
		}
	}

	public bool IPAddressSort
	{
		set
		{
			_ipaddressSort = value;
		}
		get
		{
			return _ipaddressSort;
		}
	}

	/// <summary>
	/// Gets or sets the order of sorting to apply (for example, 'Ascending' or 'Descending').
	/// </summary>
	public SortOrder Order
	{
		set
		{
			OrderOfSort = value;
		}
		get
		{
			return OrderOfSort;
		}
	}

}