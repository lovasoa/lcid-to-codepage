# Windows Locale and Character Encoding Mapper

This tool generates a comprehensive CSV file containing Windows locale information, ANSI/OEM code pages, and character encoding details for all locales installed on your system.

If you are only interested in the results, see [**windows_locales_extended.csv**](./windows_locales_extended.csv).

## What Problem Does It Solve?

When working with international text, legacy applications, or character encoding conversions, you often need to know:
- Which code page is used for a specific language/country
- What character encoding corresponds to a specific locale
- How Windows maps between locales and character sets

This program automatically extracts all this information from your Windows system and saves it in an easy-to-use CSV format.

## Key Features

- Lists all installed Windows locales
- Maps locale identifiers (LCID) to their code pages
- Provides both ANSI and OEM code page information
- Includes character set names for each code page
- Shows language names in both English and native script
- Exports everything to a UTF-8 encoded CSV file

## Output Format

The program creates a CSV file with the following columns:
1. `Locale Name` (e.g., "en-US", "ja-JP")
2. `LCID` (Locale Identifier in hexadecimal)
3. `ANSI CodePage` (e.g., 1252, 932)
4. `ANSI Character Set` (e.g., "Windows-1252 Western European")
5. `OEM CodePage` (e.g., 437, 932)
6. `OEM Character Set` (e.g., "OEM United States")
7. `Native Language Name` (Language name in its own script)
8. `English Language Name`
9. `Country` (in English)
10. `Script` (Writing system used)

## Building the Program

### Using Visual Studio

```batch
cl locale_encoding.c
```

### Using MinGW

```batch
gcc locale_encoding.c -o locale_encoding.exe
```

## Usage

1. Simply run the compiled executable:
   ```batch
   locale_encoding.exe
   ```

2. The program will create a file named `windows_locales_extended.csv` in the same directory

3. Open the CSV file with Excel, LibreOffice, or any text editor that supports UTF-8

## Common Use Cases

- Finding the correct code page for legacy system integration
- International software development
- Character encoding conversion tools
- Database collation configuration
- Text file encoding detection
- Legacy system maintenance
- Internationalization (i18n) projects
- Localization (l10n) planning

## Technical Details

The program uses the following Windows APIs:
- `EnumSystemLocalesEx`: Enumerates available locales
- `GetLocaleInfoW`: Retrieves locale-specific information
- `GetCPInfoExW`: Gets code page characteristics
- `LocaleNameToLCID`: Converts locale names to LCIDs

## Search Keywords

Windows, Locale, LCID, Code Page, Character Set, Encoding, ANSI, OEM, UTF-8, ASCII, Windows-1252, Shift-JIS, GBK, Big5, ISO-8859, Character Map, Internationalization, i18n, l10n, GetLocaleInfo, EnumSystemLocales, GetCPInfoEx, National Language Support, NLS, Universal Character Set, UCS, Unicode, Language Code, Country Code, Regional Settings

## Related Concepts

- Unicode transformation
- Character encoding conversion
- Text internationalization
- Windows National Language Support (NLS)
- Regional and Language settings
- Legacy text encoding
- Multi-language support
- Code page detection
- Character set conversion

## Notes

- The program requires Windows Vista or later
- Output is UTF-8 encoded with BOM for Excel compatibility
- Empty or zero values in code page fields indicate Unicode-only locales
- Some locales might share the same code page
