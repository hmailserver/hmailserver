////////////////////////////////////////////////////////////////////////////////////
// TabOrderManager.cs
//
// By Scott McMaster (smcmaste@hotmail.com)
// 10/10/2004
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
// Martin Knafve: According to ReadMe, this may be used for any purposes.
//
//
////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Windows.Forms;
using System.Collections;
using System.Collections.Specialized;
using System.Diagnostics;

namespace hMailServer.Shared
{
	/// <summary>
	/// Dynamically determine and set a tab order for a container and children according to a given strategy.
	/// </summary>
	public class TabOrderManager
	{
		/// <summary>
		/// Compare two controls in the selected tab scheme.
		/// </summary>
		private class TabSchemeComparer : IComparer
		{
			private TabScheme comparisonScheme;

			#region IComparer Members

			public int Compare(object x, object y)
			{
				Control control1 = x as Control;
				Control control2 = y as Control;
				
				if( control1 == null || control2 == null )
				{
					Debug.Assert( false, "Attempting to compare a non-control" );
					return 0;
				}

				if( comparisonScheme == TabScheme.None )
				{
					// Nothing to do.
					return 0;
				}

				if( comparisonScheme == TabScheme.AcrossFirst )
				{
					// The primary direction to sort is the y direction (using the Top property).
					// If two controls have the same y coordination, then we sort them by their x's.
					if( control1.Top < control2.Top )
					{
						return -1;
					}
					else if( control1.Top > control2.Top )
					{
						return 1;
					}
					else
					{
						return ( control1.Left.CompareTo( control2.Left) );
					}
				}
				else    // comparisonScheme = TabScheme.DownFirst
				{
					// The primary direction to sort is the x direction (using the Left property).
					// If two controls have the same x coordination, then we sort them by their y's.
					if( control1.Left < control2.Left )
					{
						return -1;
					}
					else if( control1.Left > control2.Left )
					{
						return 1;
					}
					else
					{
						return (control1.Top.CompareTo( control2.Top ));
					}
				}
			}

			#endregion

			/// <summary>
			/// Create a tab scheme comparer that compares using the given scheme.
			/// </summary>
			/// <param name="scheme"></param>
			public TabSchemeComparer( TabScheme scheme )
			{
				comparisonScheme = scheme;
			}
		}

		/// <summary>
		/// The container whose tab order we manage.
		/// </summary>
		private Control container;

		/// <summary>
		/// Hash of controls to schemes so that individual containers can have different ordering
		/// strategies than their parents.
		/// </summary>
		private Hashtable schemeOverrides;

		/// <summary>
		/// The tab index we start numbering from when the tab order is applied.
		/// </summary>
		private int curTabIndex = 0;

		/// <summary>
		/// The general tab-ordering strategy (i.e. whether we tab across rows first, or down columns).
		/// </summary>
		public enum TabScheme
		{
			None,
			AcrossFirst,
			DownFirst
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="container">The container whose tab order we manage.</param>
		public TabOrderManager(Control container)
		{
			this.container = container;
			this.curTabIndex = 0;
			this.schemeOverrides = new Hashtable();
		}

		/// <summary>
		/// Construct a tab order manager that starts numbering at the given tab index.
		/// </summary>
		/// <param name="container">The container whose tab order we manage.</param>
		/// <param name="curTabIndex">Where to start numbering.</param>
		/// <param name="schemeOverrides">List of controls with explicitly defined schemes.</param>
		private TabOrderManager(Control container, int curTabIndex, Hashtable schemeOverrides )
		{
			this.container = container;
			this.curTabIndex = curTabIndex;
			this.schemeOverrides = schemeOverrides;
		}

		/// <summary>
		/// Explicitly set a tab order scheme for a given (presumably container) control.
		/// </summary>
		/// <param name="c">The control to set the scheme for.</param>
		/// <param name="scheme">The requested scheme.</param>
		public void SetSchemeForControl( Control c, TabScheme scheme )
		{
			schemeOverrides[c] = scheme;
		}

		/// <summary>
		/// Recursively set the tab order on this container and all of its children.
		/// </summary>
		/// <param name="scheme">The tab ordering strategy to apply.</param>
		/// <returns>The next tab index to be used.</returns>
		public int SetTabOrder( TabScheme scheme )
		{
			// Tab order isn't important enough to ever cause a crash, so replace any exceptions
			// with assertions.
			try
			{
				ArrayList controlArraySorted = new ArrayList();
				controlArraySorted.AddRange( container.Controls );

            if (!(container is TabControl))
				   controlArraySorted.Sort( new TabSchemeComparer( scheme ) );

				foreach( Control c in controlArraySorted )
				{
					c.TabIndex = curTabIndex++;
					if( c.Controls.Count > 0 )
					{
						// Control has children -- recurse.
						TabScheme childScheme = scheme;
						if( schemeOverrides.Contains( c ) )
						{
							childScheme = (TabScheme) schemeOverrides[c];
						}
						curTabIndex = (new TabOrderManager( c, curTabIndex, schemeOverrides )).SetTabOrder( childScheme );
					}
				}

				return curTabIndex;
			}
			catch( Exception e )
			{
				Debug.Assert( false, "Exception in TabOrderManager.SetTabOrder:  " + e.Message );
				return 0;
			}
		}
	}
}
