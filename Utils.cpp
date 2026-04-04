#include "Utils.hpp"

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstdlib>
#endif

std::string Utils::GetPersonalPath() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SHGetSpecialFolderPathA(NULL, path, CSIDL_APPDATA, FALSE)) {
        return std::string(path);
    }
    return ".";
#else
    const char* home = std::getenv("HOME");
    if (home) return std::string(home);
    
    struct passwd* pw = getpwuid(getuid());
    if (pw) return std::string(pw->pw_dir);
    return ".";
#endif
}