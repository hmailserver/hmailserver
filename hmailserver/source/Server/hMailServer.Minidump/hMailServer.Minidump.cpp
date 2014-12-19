// hMailServer.Minidump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>
#include <DbgHelp.h>

#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "../Common/Util/MiniDumpInput.h"

using namespace boost::interprocess;


int CreateMinidump(int process_id, int thread_id, EXCEPTION_POINTERS* pExp, const wchar_t *file_name)
{   
   HMODULE debug_help_dll = LoadLibrary(_T("dbghelp.dll"));
   if(debug_help_dll==NULL)
   {
      return -1;
   }

      HANDLE dump_file = CreateFileW(
      file_name,
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

   if(dump_file==INVALID_HANDLE_VALUE)
   {
      return -2;
   }

   MINIDUMP_EXCEPTION_INFORMATION minidump_exception_info;
   minidump_exception_info.ThreadId = thread_id;
   minidump_exception_info.ExceptionPointers = pExp;
   minidump_exception_info.ClientPointers = FALSE;

   MINIDUMP_CALLBACK_INFORMATION minidump_callback_info;
   minidump_callback_info.CallbackRoutine = NULL;
   minidump_callback_info.CallbackParam = NULL;

   typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
      HANDLE hProcess, 
      DWORD ProcessId, 
      HANDLE hFile, 
      MINIDUMP_TYPE DumpType, 
      CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
      CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam, 
      CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

   LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
      (LPMINIDUMPWRITEDUMP)GetProcAddress(debug_help_dll, "MiniDumpWriteDump");
   if(!pfnMiniDumpWriteDump)
   {    
      // Bad MiniDumpWriteDump function
      return -3;
   }

   HANDLE process = OpenProcess(
         PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
         FALSE,
         process_id);

   if (process == INVALID_HANDLE_VALUE)
      return -4;

   BOOL bWriteDump = pfnMiniDumpWriteDump(
      process,
      process_id,
      dump_file,
      MiniDumpNormal,
      &minidump_exception_info,
      NULL,
      &minidump_callback_info);


   if(!bWriteDump)
   {    
      // Error writing dump.
      return -5;
   }

   // Close file
   CloseHandle(dump_file);

   // Unload dbghelp.dll
   FreeLibrary(debug_help_dll);

   CloseHandle(process);

   return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
//   Sleep(10000);

   try
   {
      windows_shared_memory shm (open_only, HM::MiniDumpInput::SharedMemoryName.c_str(), read_write);

      mapped_region region(shm, read_write);

      HM::MiniDumpInput* view = (HM::MiniDumpInput*) region.get_address();
      
      EXCEPTION_POINTERS pointers;
      pointers.ContextRecord = &view->ContextRecord;
      pointers.ExceptionRecord = &view->ExceptionRecord;

      return CreateMinidump(view->ProcessId, view->ThreadId, &pointers, view->DumpFile);
   }
   catch (boost::interprocess::interprocess_exception)
   {
      return -100;
      
   }
}

