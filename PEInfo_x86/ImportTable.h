#ifndef _IMPORTTABLE_H
#define _IMPORTTABLE_H

#include <CommCtrl.h>

extern INT_PTR CALLBACK ImportTableDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern VOID InitTableList(HWND hWnd);

extern VOID ShowImportTableInfo(HWND hWnd);

extern VOID ShowImportFunctions(HWND hWnd,int index);

#endif // !_IMPORTTABLE_H
