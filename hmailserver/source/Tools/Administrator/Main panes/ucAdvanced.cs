// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;
using System.Net;
using System.Net.Sockets;

namespace hMailServer.Administrator
{
    public partial class ucAdvanced : UserControl, ISettingsControl
    {
        public ucAdvanced()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {

        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this);
            }
        }

        public void LoadData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            textDefaultDomain.Text = settings.DefaultDomain;
            checkIPv6Preferred.Checked = settings.IPv6PreferredEnabled;              

            IsSupportIpv6();
            Marshal.ReleaseComObject(settings);

        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            settings.DefaultDomain = textDefaultDomain.Text;
            settings.IPv6PreferredEnabled = checkIPv6Preferred.Checked;
            
            if (textAdministratorPassword.Dirty)
            {
                string newPassword = textAdministratorPassword.Text;

                settings.SetAdministratorPassword(newPassword);
            }

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);

            return true;
        }

        private void IsSupportIpv6()
        {
            bool interfaceSupportsIPv6 = false;
            IPAddress[] AllIPs = Dns.GetHostAddresses(Dns.GetHostName());
            foreach (IPAddress ip in AllIPs)
            {
                if (!IPAddress.IsLoopback(ip) && ip.AddressFamily == AddressFamily.InterNetworkV6 && !IsLocalIPv6(ip))
                {
                    interfaceSupportsIPv6 = true;
                    break;
                }
            }
            checkIPv6Preferred.Enabled = Socket.OSSupportsIPv6 && interfaceSupportsIPv6;
        }
        
        private static bool IsLocalIPv6(IPAddress address)
        {
            bool isLocalIPv6 = false;
            // Make sure we are dealing with an IPv6 address
            if (address.AddressFamily != AddressFamily.InterNetworkV6)
            {
                // The Given IP Address is Not a Valid IPv6 IP Address
                return false;
            }

            var addressAsString = address.ToString();
            var firstWord = addressAsString.Split(new[] { ':' }, StringSplitOptions.RemoveEmptyEntries)[0];

            // equivalent of 127.0.0.1 in IPv6
            if (addressAsString == "::1")
            {
                isLocalIPv6 = true;
            }
            // The original IPv6 Site Local addresses (fec0::/10) are deprecated. Unfortunately IsIPv6SiteLocal only checks for the original deprecated version:
            else if (address.IsIPv6SiteLocal)
            {
                isLocalIPv6 = true;
            }
            // These days Unique Local Addresses (ULA) are used in place of Site Local.
            // ULA has two variants:
            // fc00::/8 is not defined yet, but might be used in the future for internal-use addresses that are registered in a central place (ULA Central).
            // fd00::/8 is in use and does not have to registered anywhere.
            else if (firstWord.Length >= 4 && firstWord.Substring(0, 2) == "fc")
            {
                isLocalIPv6 = true;
            }
            else if (firstWord.Length >= 4 && firstWord.Substring(0, 2) == "fd")
            {
                isLocalIPv6 = true;
            }
            // Link local addresses (prefixed with fe80) are not routable
            else if (firstWord == "fe80")
            {
                isLocalIPv6 = true;
            }
            // Discard Prefix
            else if (firstWord == "100")
            {
                isLocalIPv6 = true;
            }
            return isLocalIPv6;
        }

        public void LoadResources()
        {
            // load the translated resources
        }
    }
}
