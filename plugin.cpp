#include <string>
#include <json/json.h>
#include <fstream>
#include <Urlmon.h>
#include <iostream>
#include <filesystem>

#include <RE/C/ConsoleLog.h>

#include "logger.h"

#pragma comment (lib, "urlmon.lib")

/**
 * The function `downloader` takes a URL and a filename as input and downloads the file from the URL to
 * the specified filename.
 * 
 * @param url The `url` parameter is a string that represents the URL of the file to be downloaded. It
 * should be in the format of a valid URL, such as "https://example.com/file.txt".
 * @param filename The filename parameter is a string that represents the name of the file that will be
 * downloaded from the specified URL.
 */
void downloader(std::string url, std::string filename) {

    std::wstring urlstr = std::wstring(url.begin(), url.end());
    const wchar_t* urlcstr = urlstr.c_str();

    std::wstring fnamestr = std::wstring(filename.begin(), filename.end());
    const wchar_t* fnamecstr = fnamestr.c_str();

    RE::ConsoleLog::GetSingleton()->Print("Update");

    URLDownloadToFile(0, urlcstr, fnamecstr, 0, 0);
}


/**
 * The function `checkUpdate` compares the version numbers of a local and server file and returns 1 if
 * the server version is higher, otherwise it returns 0.
 * 
 * @param  The function `checkUpdate` takes a single parameter of type `RE::StaticFunctionTag*`. This
 * parameter is not used within the function and is only present to indicate that the function is
 * intended to be used as a static function in the context of the `RE` namespace.
 * 
 * @return an integer value. If the server version of the DLL or ESP files is greater than the local
 * version, it returns 1. Otherwise, it returns 0.
 */
int checkUpdate(RE::StaticFunctionTag*) {

    std::string url = "https://raw.githubusercontent.com/ac973k/Plottering/master/version.json";

    downloader(url, "Data\\SKSE\\Plugins\\Plottering\\tmp\\version.json");

    std::ifstream server_file("Data\\SKSE\\Plugins\\Plottering\\tmp\\version.json", std::ifstream::binary);
    std::ifstream local_file("Data\\SKSE\\Plugins\\Plottering\\version.json", std::ifstream::binary);

    Json::Value server;
    server_file >> server;

    Json::Value local;
    local_file >> local;

    int serverVersion_dll = server["Plottering_dll"]["version"].asInt();
    int serverVersion_esp = server["Plottering_esp"]["version"].asInt();

    int localversion_dll = local["Plottering_dll"]["version"].asInt();
    int localversion_esp = local["Plottering_esp"]["version"].asInt();

    if (serverVersion_dll > localversion_dll || serverVersion_esp > localversion_esp) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * The function "bindPapyrusCheckUpdate" registers a function named "checkUpdate" from the
 * "aaaCheckUpdateFunctions" namespace with the given virtual machine.
 * 
 * @param a_vm a pointer to an instance of the RE::BSScript::IVirtualMachine class. This class
 * represents the virtual machine used to execute Papyrus scripts in the game.
 * 
 * @return a boolean value of true.
 */
bool bindPapyrusCheckUpdate(RE::BSScript::IVirtualMachine* a_vm) {
    a_vm->RegisterFunction("checkUpdate", "aaaCheckUpdateFunctions", checkUpdate);
    return true;
}


/**
 * The function `SKSEPluginLoad` initializes and sets up logging, creates directories if they don't
 * exist, registers a listener for a specific message type, and binds a Papyrus function.
 * 
 * @param skse The skse parameter is a pointer to the SKSE::LoadInterface object, which is used to
 * initialize the SKSE library.
 * 
 * @return a boolean value, specifically `true`.
 */
SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    SetupLog();
    
    if(std::filesystem::exists("Data\\SKSE\\Plugins\\Plottering\\tmp")) {
        
    } else {
        std::filesystem::create_directory("Data\\SKSE\\Plugins\\Plottering");
        std::filesystem::create_directory("Data\\SKSE\\Plugins\\Plottering\\tmp");
    }

    SKSE::GetPapyrusInterface()->Register(bindPapyrusCheckUpdate);

    return true;
}
