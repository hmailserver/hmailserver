#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include "hMS_build_environment.h"
#include <windows.h>
#include <tlhelp32.h>
#include <sddl.h>
#include <aclapi.h>

using namespace std;

int main(int argc, char **argv)
{
		printf("hMSBuildConfig 1.0.0.1\n\n");
			
		// Start main loop
		if (argc < 2) {
			printf("Error: No build commands provided\n");
		} 
		
		else {

			hMS_build_environment hdevenv = hMS_build_environment();

			// Check if DevEnv.exe is running
			if (strcmp(argv[1], "--get_vs_devenvinfo") == 0) {

				std::string devenv_info = "Visual Studio IDE Process detected";
					if (hdevenv.get_is_devenv_running()) 
						cout << devenv_info + ": Yes";		
			}

			// Check if hMSBuildConfig has admin permissions 
			if (strcmp(argv[1], "--get_hmsc_permissions") == 0) {				

				std::string permission_info = "hMSBuildConfig has Admin permissions";
				if (hdevenv.get_have_admin_permissions())
					cout << permission_info + ": Yes";
			}
						

			// Query build summary
			if (strcmp(argv[1], "--get_build_info_summary") == 0) {

				cout << "# Buildinfo summary\n";
				if (!hdevenv.get_build_info_summary())
					cout << "Error: Build environment data not accessible.";
			}


			// leave loop
		}

		#ifdef _DEBUG
			cin.get();
		#endif	
	
	return EXIT_SUCCESS;
}