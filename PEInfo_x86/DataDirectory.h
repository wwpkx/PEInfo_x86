#ifndef _DATADIRECTORY_H
#define _DATADIRECTORY_H

extern INT_PTR CALLBACK DirectoryDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 初始化对话框字体
extern VOID InitStaticText(HWND hWnd);

// 解析数据目录表
extern VOID ShowDataDirectoryInfo(HWND hWnd);

#endif // !_DATADIRECTORY_H