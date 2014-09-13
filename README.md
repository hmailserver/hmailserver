hMailServer
===========

hMailServer is an open source email server for Microsoft Windows.

This page describes how to compile hMailServer. For more information about hMailServer, please go to http://www.hmailserver.com

Compiling hMailServer
====================

Required software:

   * Visual Studio 2008 Service Pack 1
   * InnoSetup (to build installation program)
   * Perl (ActiveState ActivePerl Communityedition 32 bit works fine)

3rd party libraries
===================

Some 3rd party libraries which hMailServer relies on are large and updated frequently. Rather than including these large libraries into the hMailServer git repository, they have to be downloaded and built, currently manually. When you build hMailServer, hMailServer will use a system environment variable, named hMailServerLibs, to locate these libraries.

Create an environment variable named hMailServerLibs pointing at a folder where you will store hMailServer libraries, such as C:\Dev\hMailLibs.

Building OpenSSL
----------------
1. Download OpenSSL 1.0.1i from http://www.openssl.org/source/ and put it into %hMailServerLibs%\<OpenSSL-Version>.  
   You should now have a folder named %hMailServerLibs%\<OpenSSL-version>, for example C:\Dev\hMailLibs\openssl-1.0.1i
2. Open up a Visual Studio 2008 32bit Command prompt
3. Change dir to %hMailServerLibs%\<OpenSSL-version>.
4. Run Perl Configure no-asm VC-WIN32
5. Run ms\do_ms
6. Run nmake -f ms\ntdll.mak

Building Boost
--------------
1. Download Boost 1.56.0 from http://www.boost.org/ and put it into %hMailServerLibs%\<Boost-Version>.  
   You should now have a folder named %hMailServerLibs%\<Boost-Version>, for example C:\Dev\hMailLibs\boost_1_56_0
2. Open a Command prompt
3. Change dir to %hMailServerLibs%\<Boost-Version>.
4. Run bootstrap.bat
5. Run bjam.exe --toolset=msvc-9.0 --build-type=complete


Building hMailServer
--------------------
1. Download the source code from this Git repository.
2. Compile the solution hmailserver\source\Server\hMailServer\hMailServer.sln.
   This will build the hMailServer server-part (hMailServer.exe)
3. Compile the solution hmailserver\source\Tools\hMailServer Tools.sln.
   This will build hMailServer related tools, such as hMailServer Administrator and hMailServer DB Setup.
4. Compile hmailserver\installation\hMailServer.iss (using InnoSetup)
   This will build the hMailServer installation program.

Releasing hMailServer
--------------------

Without finding any serious issues:

1. Run all integration tests on supported versions of Windows and the different supported databases. 
2. Run all server stress tests
3. Enable Gflags (gflags /p /enable hmailserver.exe) and run all integration tests to check for memory issues
4. Run for at least 2 weeks
5. Wait for at least 500 downloads of the beta version
