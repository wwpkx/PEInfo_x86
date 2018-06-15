#ifndef _TASKMANAGER_H

#include <CommCtrl.h>

extern INT_PTR CALLBACK TaskMgrDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern VOID InitList(HWND hWnd);

// 显示当前系统内所有进程信息
extern VOID InitProcessInfo(HWND hWnd);

// 显示选中进程的载入模块信息
extern VOID ShowProcessModule(HWND hWnd, int index);

#endif // !_TASKMANAGER_H
