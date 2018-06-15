#include "stdafx.h"
#include "resource.h"
#include "PEFunctions.h"
#include "SectionTable.h"
#include "DataDirectory.h"
#include "TaskManager.h"
#include "Calculator.h"

#include <CommCtrl.h>
#pragma comment (lib, "comctl32.lib")

HINSTANCE g_hInst;				// 实例句柄
MAP_FILE_STRUCT mfs;			// 文件映射结构
TCHAR g_szFilePath[MAX_PATH];	// 文件路径

// 主对话框过程函数
INT_PTR CALLBACK MainDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 初始化主对话框字体
static VOID InitStaticText(HWND hWnd);
BOOL SelectFile(HWND hWnd);
VOID UnloadFile(LPMAP_FILE_STRUCT pMFS);
VOID ShowFileHeaderInfo(HWND hWnd, LPVOID ImageBase);
VOID ShowOptionalHeaderInfo(HWND hWnd, LPVOID ImageBase);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PTSTR szCmdLine, int nCmdShow)
{
	g_hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC)MainDialogProc);

	return 0;
}

INT_PTR CALLBACK MainDialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitCommonControls();
		SetWindowText(hWndDlg, TEXT("PEInfo v1.0"));
		// 设置菜单
		if (!SetMenu(hWndDlg, LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU))))
		{
			EndDialog(hWndDlg, 0);
			break;
		}
		GetWindowRect(hWndDlg, &rect);
		// 设置窗口居中显示
		SetWindowPos(hWndDlg, HWND_TOP,
			(GetSystemMetrics(SM_CXSCREEN) - rect.right + rect.left) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top) / 2,
			rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
		InitStaticText(hWndDlg);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_SECTIONTABLE:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SECTION_DIALOG), hWndDlg, (DLGPROC)SectionTableDlgProc);
			break;

		case IDC_BUTTON_DATADIRECTORY:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIRECTORY_DIALOG), hWndDlg, (DLGPROC)DirectoryDlgProc);
			break;

		case IDC_BUTTON_TASKMGR:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_TASKMGR_DIALOG), hWndDlg, (DLGPROC)TaskMgrDlgProc);
			break;

		case IDC_BUTTON_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hWndDlg, (DLGPROC)AboutDlgProc);
			break;

		case IDC_BUTTON_OPEN:
			if (!SelectFile(hWndDlg))
			{
				break;
			}
			if (!SetDlgItemText(hWndDlg, IDC_EDIT_FILEPATH, g_szFilePath))
			{
				break;
			}
			if (!RoadFile(g_szFilePath, &mfs))
			{
				MessageBox(hWndDlg, TEXT("Cannot load the file!"), TEXT("warning"), MB_ICONERROR | MB_OK);
				break;
			}
			if (!IsPE32File(mfs.ImageBase))
			{
				UnloadFile(&mfs);
				MessageBox(hWndDlg, TEXT("Not a valid PE32 file!"), TEXT("warning"), MB_ICONERROR | MB_OK);
				break;
			}
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_DATADIRECTORY), TRUE);
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_SECTIONTABLE), TRUE);
			EnableWindow(GetDlgItem(hWndDlg, IDC_BUTTON_RESOURCE), TRUE);
			EnableMenuItem(GetMenu(hWndDlg), ID_CALC, MF_ENABLED);
			ShowFileHeaderInfo(hWndDlg, mfs.ImageBase);
			ShowOptionalHeaderInfo(hWndDlg, mfs.ImageBase);
			break;

		case ID_CALC:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CALCULATOR_DIALOG), hWndDlg, (DLGPROC)CalculatorDlgProc);
			break;

		case ID_EXIT:
			EndDialog(hWndDlg, 0);
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

VOID InitStaticText(HWND hWnd)
{
	HFONT hFont;
	LOGFONT font;

	// 设置新字体
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(font), &font);
	font.lfWeight = FW_BOLD;
	hFont = CreateFontIndirect(&font);
	SendMessage(GetDlgItem(hWnd, IDC_MAIN_TITLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_MACHINE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_NUMOFSECTIONS), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_TIMEDATESTAMP), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_POINTERTOSYMBOLTABLE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_NUMOFSYMBOLS), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_SIZEOFOPTIONALHEADER), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_FH_CHARACTERISTICS), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_SECTIONALIGNMENT), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_FILEALIGNMENT), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_ENTRYPOINT), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_IMAGEBASE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_CODEBASE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_DATABASE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_IMAGESIZE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_HEADERSSIZE), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_SUBSYSTEM), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_CHECKSUM), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_OH_DLLFLAGS), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	DeleteObject(hFont);
}

BOOL SelectFile(HWND hWnd)
{
	OPENFILENAME ofn = { 0, };
	TCHAR szFilePath[MAX_PATH] = { 0, };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = GetModuleHandle(NULL);
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = TEXT(".");
	ofn.lpstrFile = szFilePath;
	ofn.lpstrTitle = TEXT("Open ...[PEInfo v1.0] by Captain Xia");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrFilter = TEXT("*.*\0*.*");

	if (!GetOpenFileName(&ofn))
	{
		return FALSE;
	}
	else
	{
		if (SUCCEEDED(StringCchPrintf(g_szFilePath, MAX_PATH, ofn.lpstrFile)))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

VOID UnloadFile(LPMAP_FILE_STRUCT pMFS)
{
	if (pMFS->hMapping)
	{
		UnmapViewOfFile(pMFS->ImageBase);
	}
	if (pMFS->ImageBase)
	{
		CloseHandle(pMFS->hMapping);
	}
	if (pMFS->hFile)
	{
		CloseHandle(pMFS->hFile);
	}
}

VOID ShowFileHeaderInfo(HWND hWnd, LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_FILE_HEADER pFH = &pNtH->FileHeader;
	char szBuffer[128] = { 0, };

	SetDlgItemTextA(hWnd, IDC_EDIT_FH_MACHINE, "i386");

	StringCchPrintfA(szBuffer, 128, "%d", pFH->NumberOfSections);
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_NUMOFSECTIONS, szBuffer);

	_ctime32_s(szBuffer, 128, (__time32_t*)&pFH->TimeDateStamp);
	szBuffer[strlen(szBuffer) - 1] = '\0';
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_TIMEDATESTAMP, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pFH->PointerToSymbolTable);
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_POINTERTOSYMBOLTABLE, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pFH->NumberOfSymbols);
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_NUMOFSYMBOLS, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pFH->SizeOfOptionalHeader);
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_SIZEOFOPTIONALHEADER, szBuffer);

	StringCchPrintfA(szBuffer, 128, "%d", pFH->Characteristics);
	SetDlgItemTextA(hWnd, IDC_EDIT_FH_CHARACTERISTICS, szBuffer);
}

VOID ShowOptionalHeaderInfo(HWND hWnd, LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_OPTIONAL_HEADER pOH = &pNtH->OptionalHeader;
	TCHAR szBuffer[128] = { 0, };

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->AddressOfEntryPoint);
	SetDlgItemText(hWnd, IDC_EDIT_OH_ENTRYPOINT, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->ImageBase);
	SetDlgItemText(hWnd, IDC_EDIT_OH_IMAGEBASE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->BaseOfCode);
	SetDlgItemText(hWnd, IDC_EDIT_OH_CODEBASE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->BaseOfData);
	SetDlgItemText(hWnd, IDC_EDIT_OH_DATABASE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->SizeOfImage);
	SetDlgItemText(hWnd, IDC_EDIT_OH_IMAGESIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->SizeOfHeaders);
	SetDlgItemText(hWnd, IDC_EDIT_OH_HEADERSSIZE, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->Subsystem);
	SetDlgItemText(hWnd, IDC_EDIT_OH_SUBSYSTEM, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->SectionAlignment);
	SetDlgItemText(hWnd, IDC_EDIT_OH_SECTIONALIGNMENT, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->CheckSum);
	SetDlgItemText(hWnd, IDC_EDIT_OH_CHECKSUM, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->FileAlignment);
	SetDlgItemText(hWnd, IDC_EDIT_OH_FILEALIGNMENT, szBuffer);

	StringCchPrintf(szBuffer, 128, TEXT("%08XH"), pOH->DllCharacteristics);
	SetDlgItemText(hWnd, IDC_EDIT_OH_DLLFLAGS, szBuffer);
}

INT_PTR CALLBACK AboutDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HFONT hFont;
	LOGFONT font;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWndDlg, TEXT("About"));
		// 设置新字体
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(font), &font);
		font.lfWeight = FW_BOLD;
		hFont = CreateFontIndirect(&font);
		SendMessage(GetDlgItem(hWndDlg, IDC_STATIC_NAME), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
		SendMessage(GetDlgItem(hWndDlg, IDC_STATIC_AUTHOR), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
		SendMessage(GetDlgItem(hWndDlg, IDC_STATIC_EMAIL), WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
		DeleteObject(hFont);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDRETURN:
			EndDialog(hWndDlg, 0);
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return 0;
}