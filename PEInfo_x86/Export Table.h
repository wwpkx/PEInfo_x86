#ifndef _EXPORTTABLE_H
#define _EXPORTTABLE_H

#include <CommCtrl.h>

extern INT_PTR CALLBACK ExportTableDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern VOID InitStaticText(HWND hWnd);

extern VOID InitTableList(HWND hWnd);

extern VOID ShowExportTableInfo(HWND hWnd);

#endif // !_EXPORTTABLE_H