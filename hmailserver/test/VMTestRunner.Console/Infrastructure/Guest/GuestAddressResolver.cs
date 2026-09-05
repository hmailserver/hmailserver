using System;
using System.Collections.Generic;
using System.Linq;
using System.Management.Automation;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Finds the IP address of a guest which is reached over the network.
   /// </summary>
   public class GuestAddressResolver
   {
      private readonly HyperV _vm;

      public GuestAddressResolver(HyperV vm)
      {
         _vm = vm;
      }

      /// <summary>
      /// Returns the address of the guest, or null if it isn't known yet. The caller
      /// is expected to retry while the guest is booting.
      /// </summary>
      public string Resolve()
      {
         // Guests with integration services tell us their address themselves.
         var reported = _vm.GetReportedIPAddresses().FirstOrDefault(IsUsable);

         if (reported != null)
            return reported;

         // Older guests don't, so the address is looked up from the MAC address the
         // VM was given. This requires the guest to have sent something the host has
         // seen, which it does as soon as it starts talking on the network.
         foreach (var mac in _vm.GetMacAddresses())
         {
            var address = LookupByMacAddress(mac);

            if (address != null)
               return address;
         }

         return null;
      }

      private string LookupByMacAddress(string mac)
      {
         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Get-NetNeighbor")
              .AddParameter("AddressFamily", "IPv4")
              .AddParameter("LinkLayerAddress", FormatMacAddress(mac))
              .AddParameter("ErrorAction", "SilentlyContinue");

            var results = ps.Invoke();

            return results
               .Where(neighbor => neighbor.Properties["State"]?.Value?.ToString() != "Unreachable")
               .Select(neighbor => neighbor.Properties["IPAddress"]?.Value?.ToString())
               .FirstOrDefault(IsUsable);
         }
      }

      /// <summary>
      /// Hyper-V reports 00155D0102FF, the neighbor cache wants 00-15-5D-01-02-FF.
      /// </summary>
      private static string FormatMacAddress(string mac)
      {
         if (mac.Contains("-"))
            return mac;

         var parts = new List<string>();

         for (int i = 0; i + 1 < mac.Length; i += 2)
            parts.Add(mac.Substring(i, 2));

         return string.Join("-", parts);
      }

      private static bool IsUsable(string address)
      {
         if (string.IsNullOrEmpty(address))
            return false;

         // Only IPv4, and not an address which means the guest has no network yet.
         return address.Count(c => c == '.') == 3 &&
                !address.StartsWith("169.254.") &&
                !address.StartsWith("127.");
      }
   }
}
