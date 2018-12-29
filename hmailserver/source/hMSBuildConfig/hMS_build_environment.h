#pragma once
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <atlstr.h>

using namespace std;

class hMS_build_environment
{
public:
	hMS_build_environment();
	~hMS_build_environment();	
	
	bool hMS_build_environment::get_build_info_summary();
	bool hMS_build_environment::get_is_devenv_running();
	std::string hMS_build_environment::get_os_buildinfo();
	bool hMS_build_environment::get_have_admin_permissions();
	std::string hMS_build_environment::ws2s(const std::wstring& wstr);
	std::string hMS_build_environment::get_os_description();
	std::string hMS_build_environment::get_os_arch();
	CString hMS_build_environment::GetTimestamp();
	std::string hMS_build_environment::get_hms_service_status();
	std::string hMS_build_environment::get_hms_service_start_mode();
	std::string hMS_build_environment::get_hms_service_account_name();


private:
	BSTR  hMS_build_environment::char2bstr(char* string);
	char* hMS_build_environment::substr(char* arr, size_t begin, size_t len);
	bool  hMS_build_environment::IsProcessRunning(const wchar_t* processName);

};

