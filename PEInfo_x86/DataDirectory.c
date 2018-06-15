#include "stdafx.h"
#include "resource.h"
#include "DataDirectory.h"
#include "PEFunctions.h"
#include "Export Table.h"
#include "ImportTable.h"
#include "ResourceTable.h"

extern HINSTANCE g_hInst;
extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK DirectoryDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWndDlg, TEXT("Data Directory"));
		InitStaticText(hWndDlg);
		ShowDataDirectoryInfo(hWndDlg);
		if (HasExportTable(mfs.ImageBase))
		{
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_EXPORTTABLE), TRUE);
		}
		if (HasImportTable(mfs.ImageBase))
		{
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_IMPORTTABLE), TRUE);
		}
		if (HasResourceTable(mfs.ImageBase))
		{
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_RESOURCE), TRUE);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_EXPORTTABLE:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_EXPORTTABLE_DIALOG), hWndDlg, (DLGPROC)ExportTableDlgProc);
			break;

		case IDC_BUTTON_IMPORTTABLE:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_IMPORTTABLE_DIALOG), hWndDlg, (DLGPROC)ImportTableDlgProc);
			break;

		case IDC_BUTTON_RESOURCE:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_RESOURCE_DIALOG), hWndDlg, (DLGPROC)ResourceTableDlgProc);
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWndDlg, 0);
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

	// ÉèÖÃÐÂ×ÖÌå
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(font), &font);
	font.lfWeight = FW_BOLD;
	hFont = CreateFontIndirect(&font);
	SendMessage(GetDlgItem(hWnd, IDC_DIRECTORY_TITLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_EXPORTTABLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_IMPORTTABLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_RESOURCES), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_EXCEPTION), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_SECURITY), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_RELOCATION), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_DEBUG), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_COPYRIGHT), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_GLOBALPTR), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_TLSTABLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_LOADCONFIG), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_BOUNDIMPORT), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_IAT), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_DELAYIMPORT), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_COM), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_RESERVED), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_RVA), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_DD_SIZE), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	DeleteObject(hFont);
}

VOID ShowDataDirectoryInfo(HWND hWnd)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOH = &pNtH->OptionalHeader;
	TCHAR szBuffer[128];

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[0].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_EXPORTTABLE_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[0].Size);
	SetDlgItemText(hWnd, IDC_EDIT_EXPORTTABLE_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[1].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_IMPORTTABLE_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[1].Size);
	SetDlgItemText(hWnd, IDC_EDIT_IMPORTTABLE_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[2].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_RESOURCES_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[2].Size);
	SetDlgItemText(hWnd, IDC_EDIT_RESOURCES_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[3].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_EXCEPTION_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[3].Size);
	SetDlgItemText(hWnd, IDC_EDIT_EXCEPTION_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[4].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_SECURITY_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[4].Size);
	SetDlgItemText(hWnd, IDC_EDIT_SECURITY_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[5].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_RELOCATION_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[5].Size);
	SetDlgItemText(hWnd, IDC_EDIT_RELOCATION_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[6].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_DEBUG_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[6].Size);
	SetDlgItemText(hWnd, IDC_EDIT_DEBUG_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[7].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_COPYRIGHT_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[7].Size);
	SetDlgItemText(hWnd, IDC_EDIT_COPYRIGHT_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[8].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_GLOBALPTR_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[8].Size);
	SetDlgItemText(hWnd, IDC_EDIT_GLOBALPTR_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[9].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_TLSTABLE_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[9].Size);
	SetDlgItemText(hWnd, IDC_EDIT_TLSTABLE_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[10].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_LOADCONFIG_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[10].Size);
	SetDlgItemText(hWnd, IDC_EDIT_LOADCONFIG_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[11].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_BOUNDIMPORT_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[11].Size);
	SetDlgItemText(hWnd, IDC_EDIT_BOUNDIMPORT_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[12].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_IAT_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[12].Size);
	SetDlgItemText(hWnd, IDC_EDIT_IAT_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[13].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_DELAYIMPORT_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[13].Size);
	SetDlgItemText(hWnd, IDC_EDIT_DELAYIMPORT_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[14].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_COM_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[14].Size);
	SetDlgItemText(hWnd, IDC_EDIT_COM_SIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[15].VirtualAddress);
	SetDlgItemText(hWnd, IDC_EDIT_RESERVED_RVA, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DataDirectory[15].Size);
	SetDlgItemText(hWnd, IDC_EDIT_RESERVED_SIZE, szBuffer);
}