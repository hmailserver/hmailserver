hMailServer
===========

hMailServer is an open source email server for Microsoft Windows.

This page describes how to compile and run hMailServer in debug. 

For other information about hMailServer, please go to http://www.hmailserver.com

Compiling hMailServer
====================

Required software:

   * Visual Studio 2013 Update 3
   * InnoSetup (to build installation program)
   * Perl (ActiveState ActivePerl Communityedition 32 bit works fine)
   
**NOTE**

You should not be compiling hMailServer on a computer which already runs a production version of hMailServer. When compiling hMailServer, the compilation will stop any already running version of hMailServer, and will register the compiled version as the hMailServer version on the machine (configuring the Windows service). This means that if you are running a production version of hMailServer on the machine, this version will stop running if you compile hMailServer. If this happens, the easiest path is to reinstall the production version.

Configuring Visual Studio 2013 Express Edition
==============================================

These steps are only required if you are using Visual Studio 2013 Express Edition. The steps are required because Express Edition does not include Active Template Library which hMailServer relies on. ATL therefore needs to be installed separately:

1. Download the (Windows Driver Kit 7.1 ISO)(http://www.microsoft.com/download/en/details.aspx?id=11800)
2. Mount the ISO and run the installation. You only need to install "Build Environments"
4. Open the solution hmailserver\source\Server\hMailServer\hMailServer.sln
5. In the Solution Explorer, right click on hMailServer and select Properties
6. Select Configuraton Properties -> VC++ Directories
7. Add "{PATH-TO-WDK}\inc\atl71;" to the list of Include Directories. (Replace {PATH-TO-WDK} with the location whre you installed it.
8. Add "{PATH-TO-WDK\lib\ATL\i386" to the list of Library Directories.  (Replace {PATH-TO-WDK} with the location whre you installed it.

3rd party libraries
===================

Some 3rd party libraries which hMailServer relies on are large and updated frequently. Rather than including these large libraries into the hMailServer git repository, they have to be downloaded and built, currently manually. When you build hMailServer, Visual Studio will use a system environment variable, named hMailServerLibs, to locate these libraries.

Create an environment variable named hMailServerLibs pointing at a folder where you will store hMailServer libraries, such as C:\Dev\hMailLibs.

Building OpenSSL
----------------
1. Download OpenSSL 1.1.1* from http://www.openssl.org/source/ and put it into %hMailServerLibs%\<OpenSSL-Version>.
   You should now have a folder named %hMailServerLibs%\<OpenSSL-version>, for example C:\Dev\hMailLibs\openssl-1.1.1s
2. Start a VS2013 x86 Native Tools Command Prompt
3. Change dir to %hMailServerLibs%\<OpenSSL-version>.
3. Run the following commands:

   <pre>
   Perl Configure no-asm VC-WIN32 --prefix=%cd%\out32 --openssldir=%cd%\out32 -D_WIN32_WINNT=0x501 enable-weak-ssl-ciphers
   nmake install_sw
   </pre>

Building Boost
--------------
1. Download Boost 1.70.0 from http://www.boost.org/ and put it into %hMailServerLibs%\<Boost-Version>.  
   You should now have a folder named %hMailServerLibs%\<Boost-Version>, for example C:\Dev\hMailLibs\boost_1_63_0
2. Start a VS2013 x86 Native Tools Command Prompt
3. Change dir to %hMailServerLibs%\<Boost-Version>.
4. Run the following commands:
 
   NOTE: Change the -j parameter from 4 to the number of cores on your computer. The parameter specifies the number of parallel compilations will be done.

   <pre>
   bootstrap.bat vc12
   bjam.exe --toolset=msvc-12.0 --build-type=complete address-model=32 --build-dir=out32 -j 4
   </pre>



Building hMailServer
--------------------

Visual Studio 2013 must be started with Run as Administrator.

1. Download the source code from this Git repository.
2. Compile the solution hmailserver\source\Server\hMailServer\hMailServer.sln.
   This will build the hMailServer server-part (hMailServer.exe)
3. Compile the solution hmailserver\source\Tools\hMailServer Tools.sln.
   This will build hMailServer related tools, such as hMailServer Administrator and hMailServer DB Setup.
4. Compile hmailserver\installation\hMailServer.iss (using InnoSetup)
   This will build the hMailServer installation program.

Running in Debug
==============================================

If you want to run hMailServer in debug mode in Visual Studio, add the command argument /debug. You find this setting in the Project properties, under Configuration Properties -> Debugging.

Releasing hMailServer
--------------------

Without finding any serious issues:

1. Run all integration tests on supported versions of Windows and the different supported databases. 
2. Run all server stress tests
3. Enable Gflags (gflags /p /enable hmailserver.exe) and run all integration tests to check for memory issues
4. Run for at least 1 week in production for hMailServer.com
5. Wait for at least 500 downloads of the beta version
