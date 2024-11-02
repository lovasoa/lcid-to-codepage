#include <windows.h>
#include <stdio.h>

// Helper function to get code page's character set name
void GetCodePageInfo(UINT codePage, LPWSTR charSetName, size_t charSetNameSize) {
    CPINFOEXW cpInfo = {0};
    
    fprintf(stderr, "[DEBUG] Calling GetCPInfoExW for code page: %u\n", codePage);
    if (GetCPInfoExW(codePage, 0, &cpInfo)) {
        fprintf(stderr, "[DEBUG] GetCPInfoExW succeeded\n");
        fprintf(stderr, "[DEBUG] CodePageName: %ls\n", cpInfo.CodePageName);
        fprintf(stderr, "[DEBUG] Copying to charSetName buffer (size: %zu)\n", charSetNameSize);
        wcscpy_s(charSetName, charSetNameSize, cpInfo.CodePageName);
    } else {
        fprintf(stderr, "[ERROR] GetCPInfoExW failed. Error code: %lu\n", GetLastError());
        fprintf(stderr, "[DEBUG] Setting charSetName to 'Unknown'\n");
        wcscpy_s(charSetName, charSetNameSize, L"Unknown");
    }
}

// Callback function for EnumSystemLocalesEx
BOOL CALLBACK LocaleEnumProc(LPWSTR localeName, DWORD dwFlags, LPARAM lparam) {
    UNREFERENCED_PARAMETER(dwFlags);
    FILE* fp = (FILE*)lparam;
    
    if (!localeName || !fp) {
        fprintf(stderr, "[ERROR] Invalid parameters passed to LocaleEnumProc\n");
        return FALSE;
    }

    // Check if this is a replacement locale
    if (dwFlags & LOCALE_REPLACEMENT) {
        fprintf(stderr, "[INFO] Locale %ls is a replacement locale\n", localeName);
    }

    WCHAR codePageStr[16] = {0};
    WCHAR nativeDisplayName[256] = {0};
    WCHAR englishLangName[256] = {0};
    WCHAR countryName[256] = {0};
    WCHAR scriptName[256] = {0};
    WCHAR charSetName[256] = {0};
    LCID lcid;
    
    fprintf(stderr, "\n[DEBUG] Processing locale: %ls\n", localeName);
    
    fprintf(stderr, "[DEBUG] Calling LocaleNameToLCID for: %ls\n", localeName);
    lcid = LocaleNameToLCID(localeName, 0);
    if (lcid == 0) {
        fprintf(stderr, "[ERROR] LocaleNameToLCID failed. Error code: %lu\n", GetLastError());
        return TRUE;
    }
    fprintf(stderr, "[DEBUG] LCID: 0x%04lX\n", lcid);

    fprintf(stderr, "[DEBUG] Calling GetLocaleInfoW for LOCALE_IDEFAULTANSICODEPAGE\n");
    if (!GetLocaleInfoW(lcid, LOCALE_IDEFAULTANSICODEPAGE, codePageStr, sizeof(codePageStr)/sizeof(WCHAR))) {
        fprintf(stderr, "[ERROR] GetLocaleInfoW failed for ANSI codepage. Error code: %lu\n", GetLastError());
        return TRUE;
    }
    fprintf(stderr, "[DEBUG] ANSI CodePage: %ls\n", codePageStr);

    // Skip locales without an ANSI code page
    if (wcscmp(codePageStr, L"0") == 0) {
        return TRUE;
    }

    fprintf(stderr, "[DEBUG] Calling GetLocaleInfoW for LOCALE_SNATIVELANGNAME\n");
    GetLocaleInfoW(lcid, LOCALE_SNATIVELANGNAME, nativeDisplayName, sizeof(nativeDisplayName)/sizeof(WCHAR));
    fprintf(stderr, "[DEBUG] Native display name: %ls\n", nativeDisplayName);

    fprintf(stderr, "[DEBUG] Calling GetLocaleInfoW for LOCALE_SENGLISHLANGUAGENAME\n");
    GetLocaleInfoW(lcid, LOCALE_SENGLISHLANGUAGENAME, englishLangName, sizeof(englishLangName)/sizeof(WCHAR));
    fprintf(stderr, "[DEBUG] English language name: %ls\n", englishLangName);

    fprintf(stderr, "[DEBUG] Calling GetLocaleInfoW for LOCALE_SENGCOUNTRY\n");
    GetLocaleInfoW(lcid, LOCALE_SENGCOUNTRY, countryName, sizeof(countryName)/sizeof(WCHAR));
    fprintf(stderr, "[DEBUG] Country name: %ls\n", countryName);

    fprintf(stderr, "[DEBUG] Calling GetLocaleInfoW for LOCALE_SSCRIPTS\n");
    GetLocaleInfoW(lcid, LOCALE_SSCRIPTS, scriptName, sizeof(scriptName)/sizeof(WCHAR));
    fprintf(stderr, "[DEBUG] Script name: %ls\n", scriptName);

    // Get character set information
    UINT codePage = _wtoi(codePageStr);
    GetCodePageInfo(codePage, charSetName, sizeof(charSetName)/sizeof(WCHAR));

    // Get OEM code page for additional information
    WCHAR oemCodePageStr[6] = {0};
    WCHAR oemCharSetName[256] = {0};
    GetLocaleInfoW(lcid, LOCALE_IDEFAULTCODEPAGE, oemCodePageStr, sizeof(oemCodePageStr)/sizeof(WCHAR));
    if (wcscmp(oemCodePageStr, L"0") != 0) {
        UINT oemCodePage = _wtoi(oemCodePageStr);
        GetCodePageInfo(oemCodePage, oemCharSetName, sizeof(oemCharSetName)/sizeof(WCHAR));
    }

    // Handle specific data types based on flags
    if (dwFlags & LOCALE_NEUTRALDATA) {
        fprintf(stderr, "[INFO] Processing neutral locale data for %ls\n", localeName);
    } else if (dwFlags & LOCALE_SPECIFICDATA) {
        fprintf(stderr, "[INFO] Processing specific locale data for %ls\n", localeName);
    }

    fprintf(stderr, "[DEBUG] Writing CSV line for locale %ls\n", localeName);
    fprintf(fp, "\"%ls\",\"0x%04lX\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\"\n",
            localeName,           // Locale name (e.g., en-US)
            lcid,                 // LCID in hex
            codePageStr,         // ANSI code page
            charSetName,         // ANSI character set name
            oemCodePageStr,      // OEM code page
            oemCharSetName,      // OEM character set name
            nativeDisplayName,    // Language name in native language
            englishLangName,     // Language name in English
            countryName,         // Country name in English
            scriptName);         // Script name

    return TRUE;
}

int main() {
    FILE* fp;
    errno_t err;
    
    fprintf(stderr, "[DEBUG] Opening output file: windows_locales_extended.csv\n");
    err = fopen_s(&fp, "windows_locales_extended.csv", "w");
    if (err != 0 || !fp) {
        fprintf(stderr, "[ERROR] Failed to create output file. Error code: %d\n", err);
        return 1;
    }
    fprintf(stderr, "[DEBUG] File opened successfully\n");

    fprintf(stderr, "[DEBUG] Writing UTF-8 BOM\n");
    fwrite("\xEF\xBB\xBF", 1, 3, fp);

    fprintf(stderr, "[DEBUG] Writing CSV header\n");
    fprintf(fp, "\"Locale Name\",\"LCID\",\"ANSI CodePage\",\"ANSI Character Set\",\"OEM CodePage\",\"OEM Character Set\",\"Native Language Name\",\"English Language Name\",\"Country\",\"Script\"\n");
    if (ferror(fp)) {
        fprintf(stderr, "[ERROR] Failed to write CSV header\n");
        fclose(fp);
        return 1;
    }

    if (fflush(fp) != 0) {
        fprintf(stderr, "[ERROR] Failed to flush file buffer. Error: %d\n", ferror(fp));
        fclose(fp);
        return 1;
    }

    fprintf(stderr, "[DEBUG] Starting locale enumeration\n");
    fflush(stderr);

    DWORD enumFlags = LOCALE_ALL;  // Can be combined with other flags if needed
    // LOCALE_WINDOWS | LOCALE_NEUTRALDATA | LOCALE_SUPPLEMENTAL

    SetLastError(0);
    if (!EnumSystemLocalesEx(LocaleEnumProc, enumFlags, (LPARAM)fp, NULL)) {
        DWORD error = GetLastError();
        fprintf(stderr, "[ERROR] EnumSystemLocalesEx failed. Error code: %lu\n", error);
        switch (error) {
            case ERROR_INVALID_FLAGS:
                fprintf(stderr, "[ERROR] Invalid flags specified\n");
                break;
            case ERROR_INVALID_PARAMETER:
                fprintf(stderr, "[ERROR] Invalid parameter\n");
                break;
            case ERROR_BADDB:
                fprintf(stderr, "[ERROR] Could not access locale database\n");
                break;
            default:
                fprintf(stderr, "[ERROR] Unknown error occurred\n");
        }
        fclose(fp);
        return 1;
    }

    fprintf(stderr, "[DEBUG] Closing output file\n");
    fclose(fp);
    fprintf(stderr, "[DEBUG] Process completed successfully\n");
    
    return 0;
}
