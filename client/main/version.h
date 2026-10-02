#ifndef VERSION_H
#define VERSION_H

// Числовая версия — Windows допускает значения 0..65535 в каждом поле.
// Формат: YYYY.MM.DD.BUILD
#define VERSION_MAJOR   2026
#define VERSION_MINOR   10
#define VERSION_PATCH   2
#define VERSION_BUILD   0

// Числовая версия (для VS_VERSION_INFO)
#define VER_FILEVERSION     VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD
#define VER_PRODUCTVERSION  VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD

// Строковая версия (то, что показывается в «Подробно»)
#define STR_FILEVERSION     "2026.10.2.0"
#define STR_PRODUCTVERSION  "2026.10.2.0"

#define STR_COMPANYNAME      "My Company"
#define STR_FILEDESCRIPTION  "GDD Manager"
#define STR_INTERNALNAME     "gdd-manager"
#define STR_LEGALCOPYRIGHT   "Copyright © 2026 My Company"
#define STR_ORIGINALFILENAME "gdd-manager.exe"
#define STR_PRODUCTNAME      "GDD Manager"

#endif // VERSION_H