hMailServer
===========

For details, please go to http://www.hmailserver.com

Compiling
===========

Required software:

   * Visual Studio 2008 Service Pack 1
   * InnoSetup (to build installation program)

To build hMailServer, follow these steps:

1. Download the source code
2. Compile the solution hmailserver\source\Server\hMailServer\hMailServer.sln.
   This will build the hMailServer server-part (hMailServer.exe)
3. Compile the solution hmailserver\source\Tools\hMailServer Tools.sln.
   This will build hMailServer related tools, such as hMailServer Administrator and hMailServer DB Setup.
4. Compile hmailserver\installation\hMailServer.iss (using InnoSetup)
   This will build the hMailServer installation program.
