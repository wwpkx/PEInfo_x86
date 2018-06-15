#include "stdafx.h"
#include "resource.h"
#include "Export Table.h"
#include "PEFunctions.h"

extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK ExportTableDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Export Table"));
		InitStaticText(hWnd);
		InitTableList(hWnd);
		ShowExportTableInfo(hWnd);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	default:
		break;
	}

	return 0;
}

static VOID InitStaticText(HWND hWnd)
{
	HFONT hFont;
	LOGFONT font;

	// 设置新字体
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(font), &font);
	font.lfWeight = FW_BOLD;
	hFont = CreateFontIndirect(&font);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_CHARACTERISTICS), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_TIMEDATESTAMP), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_MAJORVERSION), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_MINORVERSION), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_NAME), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_DLLNAME), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_BASE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_NUMBEROFFUNCTIONS), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_NUMBEROFNAMES), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_ADDRESSOFFUNCTIONS), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_ADDRESSOFNAMES), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_ET_ADDRESSOFNAMEORDINALS), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	DeleteObject(hFont);
}

static VOID InitTableList(HWND hWnd)
{
	HWND         hList;
	LVCOLUMN     lvColumn;
	memset(&lvColumn, 0, sizeof(LVCOLUMN));

	hList = GetDlgItem(hWnd, IDC_LIST_EXPORTTABLE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("Ordinal");
	SendDlgItemMessage(hWnd, IDC_LIST_EXPORTTABLE, LVM_INSERTCOLUMN, 0, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 80;
	lvColumn.pszText = TEXT("RVA");
	SendDlgItemMessage(hWnd, IDC_LIST_EXPORTTABLE, LVM_INSERTCOLUMN, 1, (LPARAM)&lvColumn);

	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.cx = 160;
	lvColumn.pszText = TEXT("Name");
	SendDlgItemMessage(hWnd, IDC_LIST_EXPORTTABLE, LVM_INSERTCOLUMN, 2, (LPARAM)&lvColumn);
}

VOID ShowExportTableInfo(HWND hWnd)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNth = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDD = &pNth->OptionalHeader.DataDirectory[0];
	PIMAGE_EXPORT_DIRECTORY pED = NULL;
	BOOL bExportByName;	// 函数是否以名称输出
	DWORD dwEAT;		// 输出地址表, 是一个RVA数组
	DWORD dwENT;		// 输出函数名称表, 是一个指向ASCII字符串的RVA数组
	DWORD dwOrdinal;	// 函数序号
	DWORD dwOffset;

	HWND hList;
	LVITEMA lvItem;
	char szBuffer[128];
	size_t i, j;

	Rva2offset(mfs.ImageBase, pDD->VirtualAddress, &dwOffset);
	pED = (PIMAGE_EXPORT_DIRECTORY)((DWORD)mfs.ImageBase + dwOffset);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->Characteristics);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_CHARACTERISTICS, szBuffer);

	// 格林威治时间转为本地时间
	_ctime32_s(szBuffer, 128, (__time32_t*)&pED->TimeDateStamp);
	szBuffer[strlen(szBuffer) - 1] = '\0';
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_TIMEDATESTAMP, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pED->MajorVersion);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_MAJORVERSION, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pED->MinorVersion);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_MINORVERSION, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->Name);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_NAME, szBuffer);

	Rva2offset(mfs.ImageBase, pED->Name, &dwOffset);
	StringCchPrintfA(szBuffer, 128, "%s", (char*)(((DWORD)mfs.ImageBase + dwOffset)));
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_DLLNAME, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->Base);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_BASE, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->NumberOfFunctions);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_NUMBEROFFUNCTIONS, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->NumberOfNames);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_NUMBEROFNAMES, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->AddressOfFunctions);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_ADDRESSOFFUNCTIONS, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->AddressOfNames);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_ADDRESSOFNAMES, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%08XH", pED->AddressOfNameOrdinals);
	SetDlgItemTextA(hWnd, IDC_EDIT_ET_ADDRESSOFNAMEORDINALS, szBuffer);

	hList = GetDlgItem(hWnd, IDC_LIST_EXPORTTABLE);
	SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)LVS_EX_FULLROWSELECT);

	Rva2offset(mfs.ImageBase, pED->AddressOfFunctions, &dwOffset);
	dwEAT = (DWORD)mfs.ImageBase + dwOffset;

	Rva2offset(mfs.ImageBase, pED->AddressOfNames, &dwOffset);
	dwENT = (DWORD)mfs.ImageBase + dwOffset;

	Rva2offset(mfs.ImageBase, pED->AddressOfNameOrdinals, &dwOffset);
	dwOrdinal = (DWORD)mfs.ImageBase + dwOffset;
	for (i = 0; i < pED->NumberOfFunctions; i++)
	{
		memset(&lvItem, 0, sizeof(LVITEMA));
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.pszText = szBuffer;

		StringCchPrintfA(szBuffer, 128, "%04X", pED->Base + i);
		SendDlgItemMessageA(hWnd, IDC_LIST_EXPORTTABLE, LVM_INSERTITEMA, 0, (LPARAM)&lvItem);

		StringCchPrintfA(szBuffer, 128, "%08X", *(PDWORD)(dwEAT + i * sizeof(DWORD)));
		lvItem.iSubItem = 1;
		SendDlgItemMessageA(hWnd, IDC_LIST_EXPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);

		bExportByName = FALSE;
		for (j = 0; j < pED->NumberOfNames; j++)
		{
			if (i == *(PWORD)(dwOrdinal + j * sizeof(WORD)))
			{
				bExportByName = TRUE;
				break;
			}
		}
		if (bExportByName)
		{
			Rva2offset(mfs.ImageBase, *(PDWORD)(dwENT + j * sizeof(DWORD)), &dwOffset);
			StringCchPrintfA(szBuffer, 128, "%s", (char*)((DWORD)mfs.ImageBase + dwOffset));
		}
		else
		{
			StringCchPrintfA(szBuffer, 128, "-");
		}
		lvItem.iSubItem = 2;
		SendDlgItemMessageA(hWnd, IDC_LIST_EXPORTTABLE, LVM_SETITEMA, 0, (LPARAM)&lvItem);
	}
}