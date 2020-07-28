#include "stdio.h"
#include "string.h"
#include "windows.h"

int exec(char* cmd) {
	STARTUPINFO si;  
    PROCESS_INFORMATION pi;  
    ZeroMemory(&si, sizeof(si));  
    ZeroMemory(&pi, sizeof(pi));  
	return CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

int queryJavaHome(HKEY key) {
	char* javaHome = (char*) malloc(2048);
	DWORD bufferSize = 2048;
	if(RegQueryValueEx(key, "JavaHome", NULL, NULL, javaHome, &bufferSize) == 0) {
		FILE * fh = fopen("QL.cfg", "r");
		if(fh != NULL) {
			int offset = bufferSize - 1;
			while(1) {
				char c = fgetc(fh);
				if(feof(fh)) {
					break;
				}
				javaHome[offset] = c;
				offset ++;
			}
			javaHome[offset] = 0;
			fclose(fh);
			if(exec(javaHome) != 0) {
				return 1;
			}
		}
	}
	return 0;
}

int findJRE() {
	HKEY key;
	LPCTSTR JREKeyName = "SOFTWARE\\JavaSoft\\Java Runtime Environment";
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, JREKeyName, 0, KEY_ENUMERATE_SUB_KEYS | KEY_READ, &key) == ERROR_SUCCESS) {
		DWORD subKeys, maxKeyLen;
		RegQueryInfoKey(key, NULL, NULL, NULL, &subKeys, &maxKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
		if(subKeys > 0) {
			// found in regsitry
			maxKeyLen ++;
			char* keyName = (char*) malloc(maxKeyLen);
			RegEnumKeyEx(key, 0, keyName, &maxKeyLen, NULL, NULL, NULL, NULL);
			HKEY JREKey;
			RegOpenKeyEx(key, keyName, 0, KEY_READ, &JREKey);
			int result = queryJavaHome(JREKey);
			RegCloseKey(JREKey);
			free(keyName);
			if(result) {
				return 1;
			}
		}
		RegCloseKey(key);
	}
	return 0;
}

int main() {
	if(!findJRE()) {
		MessageBox(0, "JRE not found!", "QuickLauncher", MB_OK | MB_ICONERROR);
	}
	return 0;
}
