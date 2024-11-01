#include <windows.h>
#include <stdio.h>

// Helper function to get code page's character set name
void GetCodePageInfo(UINT codePage, LPWSTR charSetName, size_t charSetNameSize) {
    CPINFOEXW cpInfo = {0};
    
    if (GetCPInfoExW(codePage, 0, &cpInfo)) {
        wcscpy_s(charSetName, charSetNameSize, cpInfo.CodePageName);
    } else {
        wcscpy_s(charSetName, charSetNameSize, L"Unknown");
    }
}

// Callback function for EnumSystemLocalesEx
BOOL CALLBACK LocaleEnumProc(LPWSTR localeName, DWORD dwFlags, LPARAM lparam) {
    FILE* fp = (FILE*)lparam;
    WCHAR codePageStr[6] = {0};
    WCHAR nativeDisplayName[256] = {0};
    WCHAR englishLangName[256] = {0};
    WCHAR countryName[256] = {0};
    WCHAR scriptName[256] = {0};
    WCHAR charSetName[256] = {0};
    LCID lcid;
    
    // Convert locale name to LCID
    lcid = LocaleNameToLCID(localeName, 0);
    if (lcid == 0) {
        return TRUE;
    }

    // Get the default ANSI code page
    if (!GetLocaleInfoW(lcid,
                      LOCALE_IDEFAULTANSICODEPAGE,
                      codePageStr,
                      sizeof(codePageStr)/sizeof(WCHAR))) {
        return TRUE;
    }

    // Skip locales without an ANSI code page
    if (wcscmp(codePageStr, L"0") == 0) {
        return TRUE;
    }

    // Get various locale information
    GetLocaleInfoW(lcid, LOCALE_SNATIVELANGNAME, nativeDisplayName, sizeof(nativeDisplayName)/sizeof(WCHAR));
    GetLocaleInfoW(lcid, LOCALE_SENGLISHLANGUAGENAME, englishLangName, sizeof(englishLangName)/sizeof(WCHAR));
    GetLocaleInfoW(lcid, LOCALE_SENGCOUNTRY, countryName, sizeof(countryName)/sizeof(WCHAR));
    GetLocaleInfoW(lcid, LOCALE_SSCRIPTS, scriptName, sizeof(scriptName)/sizeof(WCHAR));

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

    // Print CSV line, escaping quotes in strings
    fprintf(fp, "\"%ls\",\"0x%04X\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\",\"%ls\"\n",
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
    
    // Create or overwrite the CSV file
    fp = fopen("windows_locales_extended.csv", "w, ccs=UTF-8");
    if (!fp) {
        printf("Failed to create output file\n");
        return 1;
    }

    // Write UTF-8 BOM
    fwrite("\xEF\xBB\xBF", 1, 3, fp);

    // Write CSV header
    fprintf(fp, "%s\n", "\"Locale Name\",\"LCID\",\"ANSI CodePage\","
                       "\"ANSI Character Set\",\"OEM CodePage\","
                       "\"OEM Character Set\",\"Native Language Name\","
                       "\"English Language Name\",\"Country\",\"Script\"");

    // Enumerate all system locales
    if (!EnumSystemLocalesEx(LocaleEnumProc, 
                            LOCALE_ALL,
                            (LPARAM)fp,
                            NULL)) {
        printf("Failed to enumerate locales. Error: %lu\n", GetLastError());
        fclose(fp);
        return 1;
    }

    fclose(fp);
    printf("CSV file 'windows_locales_extended.csv' has been created.\n");
    
    return 0;
}