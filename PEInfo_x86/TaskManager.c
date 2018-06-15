#include "stdafx.h"
#include "resource.h"
#include "TaskManager.h"
#include <TlHelp32.h>

INT_PTR CALLBACK TaskMgrDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMLISTVIEW lpnmlv;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Process Information"));
		InitList(hWnd);
		InitProcessInfo(hWnd);
		break;

	case WM_NOTIFY:
		if (LOWORD(wParam) == IDC_LIST_PROCESS)
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case LVN_ITEMCHANGED:
				lpnmlv = (LPNMLISTVIEW)lParam;
				if (lpnmlv->uNewState)
				{
					ShowProcessModule(hWnd, lpnmlv->iItem);
				}
				break;

			default:
				break;
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	default:
		break;
	}

	return 0;
}

VOID InitList(HWND hWnd)
{
	HWND hList;
	LVCOLUMN lvColumn;
	memset(&lvColumn, 0, sizeof(LVCOLUMN));

	hList = GetDlgItem(hWnd, IDC_LIST_PROCESS);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 200;
	lvColumn.pszText = TEXT("Path");
	SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("PID");
	SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Image Base");
	SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_INSERTCOLUMN, 2, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Image Size");
	SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_INSERTCOLUMN, 3, (LPARAM)&lvColumn);

	hList = GetDlgItem(hWnd, IDC_LIST_MODULE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 200;
	lvColumn.pszText = TEXT("Path");
	SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Image Base");
	SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Image Size");
	SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_INSERTCOLUMN, 2, (LPARAM)&lvColumn);
}

VOID InitProcessInfo(HWND hWnd)
{
	HANDLE hSnapShot;
	HANDLE hModule;
	PROCESSENTRY32 pe;
	MODULEENTRY32 me;
	HWND hList;
	LVITEM lvItem;
	TCHAR szBuffer[128];
	size_t i;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapShot)
	{
		return;
	}

	hList = GetDlgItem(hWnd, IDC_LIST_PROCESS);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);
	pe.dwSize = sizeof(pe);
	me.dwSize = sizeof(me);
	Process32First(hSnapShot, &pe);
	i = 0;
	while (Process32Next(hSnapShot, &pe))
	{
		memset(&lvItem, 0, sizeof(LVITEM));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.pszText = szBuffer;
		StringCchPrintf(szBuffer, 128, TEXT("%s"), pe.szExeFile);
		SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

		StringCchPrintf(szBuffer, 128, TEXT("%d"), pe.th32ProcessID);
		lvItem.iSubItem = 1;
		SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_SETITEM, 0, (LPARAM)&lvItem);

		hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
		if (hModule == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		if (Module32First(hModule, &me))
		{
			StringCchPrintf(szBuffer, 128, TEXT("%p"), me.modBaseAddr);
			lvItem.iSubItem = 2;
			SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_SETITEM, 0, (LPARAM)&lvItem);

			StringCchPrintf(szBuffer, 128, TEXT("%08X"), me.modBaseSize);
			lvItem.iSubItem = 3;
			SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_SETITEM, 0, (LPARAM)&lvItem);
		}

		i++;
	}
}

VOID ShowProcessModule(HWND hWnd, int index)
{
	LVITEM lvItem;
	HANDLE hSnapShot;
	MODULEENTRY32 me;
	DWORD dwPID;
	TCHAR szBuffer[128];
	size_t i;

	memset(&lvItem, 0, sizeof(LVITEM));
	memset(szBuffer, 0, 128);
	lvItem.mask = LVIF_TEXT;
	lvItem.iSubItem = 1;
	lvItem.pszText = szBuffer;
	lvItem.cchTextMax = 128;
	SendDlgItemMessage(hWnd, IDC_LIST_PROCESS, LVM_GETITEMTEXT, index, (LPARAM)&lvItem);
	
	dwPID = _ttoi(lvItem.pszText);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (INVALID_HANDLE_VALUE == hSnapShot)
	{
		return;
	}
	me.dwSize = sizeof(me);
	Module32First(hSnapShot, &me);
	
	i = 0;
	do
	{
		memset(&lvItem, 0, sizeof(LVITEM));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.pszText = szBuffer;

		StringCchPrintf(szBuffer, 128, TEXT("%s"), me.szExePath);
		SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

		StringCchPrintf(szBuffer, 128, TEXT("%p"), me.modBaseAddr);
		lvItem.iSubItem = 1;
		SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_SETITEM, 0, (LPARAM)&lvItem);

		StringCchPrintf(szBuffer, 128, TEXT("%08X"), me.modBaseSize);
		lvItem.iSubItem = 2;
		SendDlgItemMessage(hWnd, IDC_LIST_MODULE, LVM_SETITEM, 0, (LPARAM)&lvItem);

		i++;
	} while (Module32Next(hSnapShot, &me));
}