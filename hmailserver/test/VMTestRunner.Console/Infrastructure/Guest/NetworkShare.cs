using System;
using System.Runtime.InteropServices;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Authenticates this process against a remote machine so its administrative
   /// shares (C$, ...) can be used with ordinary file operations.
   /// </summary>
   public class NetworkShare : IDisposable
   {
      private const int NoError = 0;
      private const int ErrorSessionCredentialConflict = 1219;
      private const int ConnectTemporary = 0x00000004;

      private readonly string _remoteName;
      private bool _connected;

      public NetworkShare(string server, string username, string password)
      {
         _remoteName = $@"\\{server}\IPC$";

         var resource = new NetResource
         {
            Scope = 2, // RESOURCE_GLOBALNET
            Type = 1,  // RESOURCETYPE_DISK
            DisplayType = 0,
            Usage = 0,
            RemoteName = _remoteName,
         };

         int result = WNetAddConnection2(resource, password, username, ConnectTemporary);

         // A session against the same server may already exist, for example from an
         // earlier run. Drop it and connect with the credentials we were given.
         if (result == ErrorSessionCredentialConflict)
         {
            WNetCancelConnection2(_remoteName, 0, true);
            result = WNetAddConnection2(resource, password, username, ConnectTemporary);
         }

         if (result != NoError)
            throw new Exception($"NetworkShare: Unable to connect to {_remoteName} as {username}. Error {result}.");

         _connected = true;
      }

      public void Dispose()
      {
         if (!_connected)
            return;

         WNetCancelConnection2(_remoteName, 0, true);
         _connected = false;
      }

      [DllImport("mpr.dll", CharSet = CharSet.Unicode)]
      private static extern int WNetAddConnection2(NetResource netResource, string password, string username, int flags);

      [DllImport("mpr.dll", CharSet = CharSet.Unicode)]
      private static extern int WNetCancelConnection2(string name, int flags, bool force);

      [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
      private class NetResource
      {
         public int Scope;
         public int Type;
         public int DisplayType;
         public int Usage;
         public string LocalName;
         public string RemoteName;
         public string Comment;
         public string Provider;
      }
   }
}
