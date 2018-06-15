#include "stdafx.h"
#include "resource.h"
#include "SectionTable.h"
#include "PEFunctions.h"

extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK SectionTableDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWndDlg, TEXT("Section Table"));
		InitSectionList(hWndDlg);
		ShowSectionTableInfo(hWndDlg);
		break;

	case WM_CLOSE:
		EndDialog(hWndDlg, 0);
		break;

	default:
		break;
	}

	return 0;
}

VOID InitSectionList(HWND hWnd)
{
	HWND         hList;
	LVCOLUMN     lvColumn;
	memset(&lvColumn, 0, sizeof(LVCOLUMN));

	hList = GetDlgItem(hWnd, IDC_LIST_SECTION);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("name");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("VirtAddr");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("VirtSize");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 2, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("raw data offs");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 3, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("raw data size");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 4, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("relocation offs");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 5, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("relocations");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 6, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("flags");
	SendDlgItemMessage(hWnd, IDC_LIST_SECTION, LVM_INSERTCOLUMN, 7, (LPARAM)&lvColumn);
}

VOID ShowSectionTableInfo(HWND hWnd)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_FILE_HEADER pFH = &pNtH->FileHeader;
	PIMAGE_SECTION_HEADER pSH = IMAGE_FIRST_SECTION(pNtH);
	LVITEMA lvItem;
	CHAR szBuffer[128];
	size_t i;

	for (i = 0; i < pFH->NumberOfSections; i++, pSH++)
	{
		memset(&lvItem, 0, sizeof(LVITEMA));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		StringCchPrintfA(szBuffer, 128, "%s", (char*)pSH->Name);
		lvItem.pszText = szBuffer;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_INSERTITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->VirtualAddress);
		lvItem.iSubItem = 1;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->Misc.VirtualSize);
		lvItem.iSubItem = 2;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->PointerToRawData);
		lvItem.iSubItem = 3;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->SizeOfRawData);
		lvItem.iSubItem = 4;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->PointerToRelocations);
		lvItem.iSubItem = 5;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->NumberOfRelocations);
		lvItem.iSubItem = 6;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08XH", pSH->Characteristics);
		lvItem.iSubItem = 7;
		SendDlgItemMessageA(hWnd, IDC_LIST_SECTION, LVM_SETITEMA, (WPARAM)0, (LPARAM)&lvItem);
	}
}