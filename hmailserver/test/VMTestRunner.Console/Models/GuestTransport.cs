namespace VMTestRunner.Console
{
   public enum GuestTransport
   {
      /// <summary>
      /// PowerShell Direct and Copy-VMFile. Windows 10 / Server 2016 and later.
      /// </summary>
      PowerShellDirect,

      /// <summary>
      /// Administrative shares and WMI, over the network. Used for older guests.
      /// </summary>
      Network,
   }
}
