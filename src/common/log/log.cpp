#include "stdafx.h"
#include "log/log.h"

LPTLOGCATEGORYINST g_pSvrLogCat;
LPTLOGCATEGORYINST g_pOssLogCat;

char g_szLogMessage[4096];

#if defined(WIN32)

HANDLE g_hConsole = INVALID_HANDLE_VALUE;

void init_console_window(void)
{
	g_hConsole  = GetStdHandle(STD_OUTPUT_HANDLE);
	LOG_PROCESS_ERROR(g_hConsole != INVALID_HANDLE_VALUE);

	CONSOLE_FONT_INFOEX FontInfo;
	COORD dwSize;
	SetConsoleOutputCP(CP_UTF8);
	FontInfo.cbSize = sizeof(FontInfo);
	FontInfo.dwFontSize.X = 8;
	FontInfo.dwFontSize.Y = 16;
	wcsncpy(FontInfo.FaceName, L"Consolas", LF_FACESIZE);
	FontInfo.FontFamily = FF_DONTCARE;
	FontInfo.FontWeight = FW_BOLD;
	FontInfo.nFont = 1;
	SetCurrentConsoleFontEx(g_hConsole, FALSE, &FontInfo);
	dwSize.X = 80;
	dwSize.Y = 1024;
	SetConsoleScreenBufferSize(g_hConsole, dwSize);
	SetConsoleMode(g_hConsole, ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
Exit0:
	return;
}

#endif	// WIN32
