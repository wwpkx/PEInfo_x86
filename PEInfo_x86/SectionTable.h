#ifndef _SECTIONTABLE_H
#define _SECTIONTABLE_H

#include <CommCtrl.h>

extern INT_PTR CALLBACK SectionTableDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern VOID InitSectionList(HWND hWnd);

extern VOID ShowSectionTableInfo(HWND hWnd);

#endif // !_SECTIONTABLE_H