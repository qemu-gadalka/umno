#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <urlmon.h>
#include <stdint.h>

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shell32.lib")

#define ZIP_NAME         "UmnoGameLauncher.zip"
#define EXE_NAME         "UmnoGamesLauncher-WPF.exe"
#define DOWNLOAD_URL     "https://github.com/sirux-ah/umno/releases/download/umnoxd/UmnoGameLauncher.zip"

void get_temp_path(char* path, size_t size) {
    GetTempPathA((DWORD)size, path);
}

void delete_file(const char* path) {
    DeleteFileA(path);
}

int file_exists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

int download_zip(const char* zip_path) {
    HRESULT hr = URLDownloadToFileA(NULL, DOWNLOAD_URL, zip_path, 0, NULL);
    return hr == S_OK;
}

int unzip_zip(const char* zip_path, const char* extract_path) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), 
        "powershell -command \"Expand-Archive -Path '%s' -DestinationPath '%s' -Force\"",
        zip_path, extract_path);
    return system(cmd) == 0;
}

int main() {
    char temp_path[MAX_PATH];
    char zip_path[MAX_PATH];
    char exe_dir[MAX_PATH];
    char exe_path[MAX_PATH];

    get_temp_path(temp_path, sizeof(temp_path));
    snprintf(zip_path, sizeof(zip_path), "%s%s", temp_path, ZIP_NAME);

    if (file_exists(zip_path)) {
        delete_file(zip_path);
    }

    printf("");
    if (!download_zip(zip_path)) {
        printf("Failed to download zip file.\n");
        return 1;
    }

    printf("");
    if (!unzip_zip(zip_path, temp_path)) {
        printf("Failed to unzip file.\n");
        return 1;
    }

    snprintf(exe_dir, sizeof(exe_dir), "%sUmnoGameLauncher-V3", temp_path);
    snprintf(exe_path, sizeof(exe_path), "%s\\%s", exe_dir, EXE_NAME);

    if (!file_exists(exe_path)) {
        printf("Error: %s not found!\n", exe_path);
        return 1;
    }

    if (!SetCurrentDirectoryA(exe_dir)) {
        printf("Failed to set working directory to: %s\n", exe_dir);
        return 1;
    }

    printf("", exe_path);
    intptr_t r = (intptr_t)ShellExecuteA(NULL, "open", exe_path, NULL, NULL, SW_SHOWNORMAL);
    if (r <= 32) {
        printf("Failed to launch exe. Error code: %d\n", (int)r);
        return 1;
    }

    return 0;
}