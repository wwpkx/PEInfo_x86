#include "stdafx.h"
#include "resource.h"
#include "PEFunctions.h"
#include "ImportTable.h"

extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK ImportTableDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMLISTVIEW lpnmlv;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Import Table"));
		InitTableList(hWnd);
		ShowImportTableInfo(hWnd);
		break;

	case WM_NOTIFY:
		// 得到选中节点的索引, 后进行响应
		if (LOWORD(wParam) == IDC_LIST_IMPORTTABLE)
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case LVN_ITEMCHANGED:
				lpnmlv = (LPNMLISTVIEW)lParam;
				if (lpnmlv->uNewState)
				{
					ShowImportFunctions(hWnd, lpnmlv->iItem);
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

static VOID InitTableList(HWND hWnd)
{
	HWND hList;
	LVCOLUMN lvColumn;
	memset(&lvColumn, 0, sizeof(LVCOLUMN));

	hList = GetDlgItem(hWnd, IDC_LIST_IMPORTTABLE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("DLL Name");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 100;
	lvColumn.pszText = TEXT("OriginalFirstThunk");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 100;
	lvColumn.pszText = TEXT("TimeDateStamp");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 2, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 100;
	lvColumn.pszText = TEXT("ForwarderChain");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 3, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Name");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 4, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("FirstThunk");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTCOLUMN, 5, (LPARAM)&lvColumn);

	hList = GetDlgItem(hWnd, IDC_LIST_IMPORTFUNCTIONS);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Ordinal/Hint");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("API Name");
	SendDlgItemMessage(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);
}

VOID ShowImportTableInfo(HWND hWnd)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR pID = NULL;
	DWORD dwOffset;
	HWND hList;
	LVITEMA lvItem;
	char szBuffer[128];
	size_t i;

	Rva2offset(mfs.ImageBase, pNtH->OptionalHeader.DataDirectory[1].VirtualAddress, &dwOffset);
	pID = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)mfs.ImageBase + dwOffset);

	hList = GetDlgItem(hWnd, IDC_LIST_IMPORTTABLE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	i = 0;
	while (0 != pID->Name)
	{
		memset(&lvItem, 0, sizeof(LVITEMA));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.pszText = szBuffer;

		Rva2offset(mfs.ImageBase, pID->Name, &dwOffset);
		StringCchPrintfA(szBuffer, 128, "%s", (char*)((DWORD)mfs.ImageBase + dwOffset));
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pID->OriginalFirstThunk);
		lvItem.iSubItem = 1;
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		_ctime32_s(szBuffer, 128, (__time32_t*)&pID->TimeDateStamp);
		szBuffer[strlen(szBuffer) - 1] = '\0';
		lvItem.iSubItem = 2;
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pID->ForwarderChain);
		lvItem.iSubItem = 3;
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pID->Name);
		lvItem.iSubItem = 4;
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pID->FirstThunk);
		lvItem.iSubItem = 5;
		SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		pID++;
		i++;
	}
}

VOID ShowImportFunctions(HWND hWnd,int index)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR pID = NULL;
	PIMAGE_THUNK_DATA pTD = NULL;
	PIMAGE_IMPORT_BY_NAME pIBN = NULL;
	DWORD dwOffset;
	HWND hList;
	LVITEMA lvItem;
	char szBuffer[128];
	size_t i;

	hList = GetDlgItem(hWnd, IDC_LIST_IMPORTFUNCTIONS);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);
	SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);

	Rva2offset(mfs.ImageBase, pNtH->OptionalHeader.DataDirectory[1].VirtualAddress, &dwOffset);
	pID = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)mfs.ImageBase + dwOffset) + index;
	Rva2offset(mfs.ImageBase, pID->OriginalFirstThunk, &dwOffset);
	if (0 == *(PDWORD)((DWORD)mfs.ImageBase + dwOffset))
	{
		Rva2offset(mfs.ImageBase, pID->FirstThunk, &dwOffset);
	}
	pTD = (PIMAGE_THUNK_DATA)((DWORD)mfs.ImageBase + dwOffset);

	i = 0;
	while (0 != pTD->u1.AddressOfData)
	{
		memset(&lvItem, 0, sizeof(LVITEMA));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.pszText = szBuffer;

		if (*(PDWORD)pTD & 0x80000000)
		{
			StringCchPrintfA(szBuffer, 128, "%04XH", *(PDWORD)pTD & 0x7FFFFFFF);
			SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);
			StringCchPrintfA(szBuffer, 128, "-");
			lvItem.iSubItem = 1;
			SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_SETITEMA, 0, (LPARAM)&lvItem);
		}
		else
		{
			Rva2offset(mfs.ImageBase, *(PDWORD)pTD, &dwOffset);
			pIBN = (PIMAGE_IMPORT_BY_NAME)((DWORD)mfs.ImageBase + dwOffset);
			StringCchPrintfA(szBuffer, 128, "%04XH", pIBN->Hint);
			SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);
			StringCchPrintfA(szBuffer, 128, "%s", pIBN->Name);
			lvItem.iSubItem = 1;
			SendDlgItemMessageA(hWnd, IDC_LIST_IMPORTFUNCTIONS, LVM_SETITEMA, 0, (LPARAM)&lvItem);
		}
		
		i++;
		pTD++;
	}
}