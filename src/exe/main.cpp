#include <Windows.h>
#include <ctime>
#include <cassert>
#include <random>
#include <string>
#include <filesystem>
#include <algorithm>

std::string generate_random_string(size_t size)
{
    static auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string result(size, 0);
    std::generate_n(result.begin(), size, randchar);
    return result;
}

bool exec_file(const std::filesystem::path& path, const std::wstring& cmdline)
{
    PROCESS_INFORMATION pInfo;
    memset(&pInfo, NULL, sizeof(pInfo));

    STARTUPINFOW sInfo;
    memset(&sInfo, NULL, sizeof(sInfo));
    sInfo.cb = sizeof(sInfo);
    BOOL isOk = CreateProcessW(path.c_str(), (LPWSTR)(path.wstring() + L" " + cmdline).c_str(), NULL, NULL, FALSE, 0, NULL, std::filesystem::absolute(path).parent_path().wstring().c_str(), &sInfo, &pInfo);
    if (!isOk) return false;
    CloseHandle(pInfo.hProcess);
    CloseHandle(pInfo.hThread);
    return true;
}

void rename_if_needed(int argc, char** argv)
{
    if (argc == 3 && strcmp(argv[1], "renamed") == 0) {
        std::error_code ec;
        while (std::filesystem::remove(argv[2], ec)) // here we remove old file
            Sleep(10); // wait while program was closed
        return;
    }
    std::filesystem::path curPath = argv[0];
    std::filesystem::path newPath = std::filesystem::path(curPath).replace_filename(generate_random_string(8)).replace_extension(curPath.extension());
    std::error_code ec;
    assert(std::filesystem::copy_file(curPath, newPath, ec));
    assert(exec_file(newPath, std::wstring(L"renamed ") + curPath.filename().wstring()));
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    rename_if_needed(argc, argv);

    // code
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, sizeof(buf) / sizeof(buf[0]));
    MessageBoxW(NULL, L"Hello, Darwin!", std::filesystem::path(buf).stem().c_str(), MB_OK);

    return EXIT_SUCCESS;
}