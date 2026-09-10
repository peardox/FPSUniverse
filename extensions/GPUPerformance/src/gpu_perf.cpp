#include "gpu_perf.h"

#include <windows.h>

#include <string>

namespace {

constexpr wchar_t kSubKey[] =
    L"SOFTWARE\\Microsoft\\DirectX\\UserGpuPreferences";

// Convert a UTF-8 / ANSI std::string to a wide string for the registry API.
std::wstring to_wide(const std::string& s) {
    if (s.empty()) {
        return std::wstring();
    }
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                                  static_cast<int>(s.size()), nullptr, 0);
    if (len <= 0) {
        return std::wstring();
    }
    std::wstring result(static_cast<size_t>(len), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()),
                        &result[0], len);
    return result;
}

// Convert a wide string back to a narrow std::string.
std::string to_narrow(const std::wstring& s) {
    if (s.empty()) {
        return std::string();
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, s.c_str(),
                                  static_cast<int>(s.size()), nullptr, 0,
                                  nullptr, nullptr);
    if (len <= 0) {
        return std::string();
    }
    std::string result(static_cast<size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()),
                        &result[0], len, nullptr, nullptr);
    return result;
}

// ---------------------------------------------------------------------------
// Internal (non-exported) implementation functions.
// ---------------------------------------------------------------------------

bool set_gpu_performance(const std::string& exename, int perf) {
    // Validate the performance argument.
    if (perf < 0 || perf > 2) {
        return false;
    }

    // Verify the key exists. Open with write access so we can add/update.
    HKEY hKey = nullptr;
    LONG rc = RegOpenKeyExW(HKEY_CURRENT_USER, kSubKey, 0,
                            KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    if (rc != ERROR_SUCCESS) {
        // Key does not exist (or cannot be opened): do not proceed.
        return false;
    }

    // Build the value name and data.
    std::wstring valueName = to_wide(exename);
    if (valueName.empty() && !exename.empty()) {
        RegCloseKey(hKey);
        return false;
    }

    std::wstring data = L"GpuPreference=" + std::to_wstring(perf) + L";";

    // Size in bytes including the null terminator.
    DWORD dataBytes =
        static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t));

    // Existence check to satisfy the required behaviour. RegSetValueExW
    // will create the value if absent or update it if present.
    DWORD existingType = 0;
    LONG queryRc = RegQueryValueExW(hKey, valueName.c_str(), nullptr,
                                    &existingType, nullptr, nullptr);
    (void)queryRc;  // Set handles both add and update identically.

    rc = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ,
                        reinterpret_cast<const BYTE*>(data.c_str()),
                        dataBytes);

    RegCloseKey(hKey);

    return rc == ERROR_SUCCESS;
}

int get_gpu_performance(const std::string& exename) {
    // Verify the key exists (read-only).
    HKEY hKey = nullptr;
    LONG rc = RegOpenKeyExW(HKEY_CURRENT_USER, kSubKey, 0, KEY_QUERY_VALUE,
                            &hKey);
    if (rc != ERROR_SUCCESS) {
        // Key missing.
        return -1;
    }

    std::wstring valueName = to_wide(exename);

    // First query: obtain the required size.
    DWORD type = 0;
    DWORD bytes = 0;
    rc = RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type, nullptr,
                          &bytes);
    if (rc != ERROR_SUCCESS || type != REG_SZ || bytes == 0) {
        RegCloseKey(hKey);
        return -2;
    }

    // Read the data into a buffer.
    std::wstring buffer(bytes / sizeof(wchar_t), L'\0');
    rc = RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type,
                          reinterpret_cast<BYTE*>(&buffer[0]), &bytes);
    RegCloseKey(hKey);

    if (rc != ERROR_SUCCESS || type != REG_SZ) {
        return -2;
    }

    // Trim any trailing null terminators.
    while (!buffer.empty() && buffer.back() == L'\0') {
        buffer.pop_back();
    }

    std::string value = to_narrow(buffer);

    // Expected format: "GpuPreference=<n>;"
    const std::string prefix = "GpuPreference=";
    std::size_t pos = value.find(prefix);
    if (pos == std::string::npos) {
        return -2;
    }
    pos += prefix.size();

    if (pos >= value.size()) {
        return -2;
    }

    char c = value[pos];
    if (c < '0' || c > '9') {
        return -2;
    }

    int result = c - '0';
    if (result < 0 || result > 2) {
        return -2;
    }

    return result;
}

}  // namespace

// ---------------------------------------------------------------------------
// Exported wrapper functions (C linkage, char* / double signatures).
// ---------------------------------------------------------------------------

extern "C" {

double gpu_get_performance(char* exefile) {
    if (exefile == nullptr) {
        return -2.0;
    }
    int result = get_gpu_performance(std::string(exefile));
    return static_cast<double>(result);
}

double gpu_set_performance(char* exefile, double perf) {
    if (exefile == nullptr) {
        return 0.0;
    }
    int iperf = static_cast<int>(perf);
    bool ok = set_gpu_performance(std::string(exefile), iperf);
    return ok ? 1.0 : 0.0;
}

}  // extern "C"
