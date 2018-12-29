#include "hMS_build_environment.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <iphlpapi.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <tlhelp32.h>
#include <sddl.h>
#include <aclapi.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <codecvt>
#include <atlbase.h>
#include <atlstr.h>

#pragma comment(lib, "wbemuuid.lib")

using namespace std;

#define MAX_NAME 256

hMS_build_environment::hMS_build_environment()
{
}


hMS_build_environment::~hMS_build_environment()
{
}

/* Converts a char* array to BSTR String */
BSTR hMS_build_environment::char2bstr(char* string)
{
	return  SysAllocString((_bstr_t)string);
}

/* Returning a substring by index from a char* array */
char* hMS_build_environment::substr(char* arr, size_t begin, size_t len)
{
	char* res = new char[len];
	for (size_t i = 0; i < len; i++)
		res[i] = *(arr + begin + i);
	res[len] = 0;
	return res;
}



bool hMS_build_environment::IsProcessRunning(const wchar_t* processName) {
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry)) {			
			if (!_wcsicmp(entry.szExeFile, processName))
				exists = true;
		}

	CloseHandle(snapshot);
	return exists;
}


// Check if the Visual Studio IDE Process is running
bool hMS_build_environment::get_is_devenv_running()
{
	return IsProcessRunning(L"devenv.exe");
}

// Check if we run with Windows Administrators Group permissionss
bool hMS_build_environment::get_have_admin_permissions()
{
	PSID administrators_group = NULL;
	SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
	BOOL result = AllocateAndInitializeSid(&nt_authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administrators_group);
	BOOL is_user_admin = FALSE;
	if (result)	{
		CheckTokenMembership(NULL, administrators_group, &is_user_admin);
		FreeSid(administrators_group);
	}

	bool state = false;
	if (is_user_admin)
		state = true;
	else
		state = false;

	return (state);
	
}

std::string hMS_build_environment::ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

/* Get the Windows Operating System build description */
std::string hMS_build_environment::get_os_buildinfo()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
		);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator *pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices *pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT BuildNumber FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"BuildNumber", 0, &vtProp, nullptr, nullptr);
	std::wstring result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	
	return ws2s(result);
}


/* Get the Windows Operating System build description */
std::string hMS_build_environment::get_os_description()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
		);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator *pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices *pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT Caption FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"Caption", 0, &vtProp, nullptr, nullptr);
	std::wstring result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return ws2s(result);
}


/* Get the Windows Operating System build description */
std::string hMS_build_environment::get_os_arch()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
		);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator *pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices *pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT OSArchitecture FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"OSArchitecture", 0, &vtProp, nullptr, nullptr);
	std::wstring result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return ws2s(result);
}

CString hMS_build_environment::GetTimestamp()
{
	time_t     now = time(nullptr);
	char       buf[80];
	struct tm tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return (CString)buf;
}


// Check Windows Service Status
std::string hMS_build_environment::get_hms_service_status()
{
	TCHAR szSvcName[80] = L"hMailServer";
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	char* iReturn = nullptr;

	SC_HANDLE schSCManager = OpenSCManager(
		nullptr,						// local computer
		nullptr,						// ServicesActive database 
		SC_MANAGER_QUERY_LOCK_STATUS);  // full access rights 

	if (nullptr == schSCManager) {
	}

	// Get a handle to the service.
	SC_HANDLE schService = OpenService(
		schSCManager,         // SCM database 
		szSvcName,            // name of service 			
		SERVICE_QUERY_STATUS);

	if (schService == nullptr)
	{
		CloseServiceHandle(schSCManager);
	}

	// Make sure the service is not already stopped.
	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		iReturn = "Error! Cannot access Servicedata";
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED) {
		iReturn = "stopped";
	}

	if (ssp.dwCurrentState == SERVICE_RUNNING) {
		iReturn = "running";
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return iReturn;
}

// Check Windows hMailServer Service Startmode
std::string hMS_build_environment::get_hms_service_start_mode()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
		);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator *pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices *pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT startmode FROM Win32_Service"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"startmode", 0, &vtProp, nullptr, nullptr);
	std::wstring result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return ws2s(result);
}


// Check Windows the Username for the Service Status
std::string hMS_build_environment::get_hms_service_account_name()
{
	HRESULT hres;

	hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hres)) {}

	hres = CoInitializeSecurity(
		nullptr,
		-1,
		nullptr,
		nullptr,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		nullptr,
		EOAC_NONE,
		nullptr
		);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemLocator *pLoc = nullptr;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		CoUninitialize();
	}

	IWbemServices *pSvc = nullptr;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		nullptr,                 // User name. NULL = current user
		nullptr,                 // User password. NULL = current
		nullptr,                 // Locale. NULL indicates current
		0,                       // Security flags.
		nullptr,                 // Authority (for example, Kerberos)
		nullptr,                 // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres)) {
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		nullptr,                     // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		nullptr,                     // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IEnumWbemClassObject* pEnumerator = nullptr;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT startname FROM Win32_Service"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		nullptr,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	IWbemClassObject *pclsObj = nullptr;
	ULONG uReturn = 0;

	pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

	VARIANT vtProp;
	pclsObj->Get(L"startname", 0, &vtProp, nullptr, nullptr);
	std::wstring result = vtProp.bstrVal;
	VariantClear(&vtProp);
	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return ws2s(result);
}

// Get summary
bool hMS_build_environment::get_build_info_summary()
{
	std::string permission_info = "hMSBuildConfig has Admin permissions";
	if (get_have_admin_permissions())
		cout << permission_info + ": Yes\n";

	std::string devenv_info = "Visual Studio IDE process active";
	if (get_is_devenv_running())
		cout << devenv_info + ": Yes\n";

	cout << "OS-Info: " + get_os_description() +" "+get_os_arch() +" Build (" + get_os_buildinfo() + ") " + "\n";

	cout << "hMailServer Status: " + get_hms_service_status() + ", Startmode: "+get_hms_service_start_mode() +"\n";
	cout << "hMailServer Service Accountname: " + get_hms_service_account_name() + "\n";
	
	

	return true;
}