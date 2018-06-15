#include "stdafx.h"
#include "resource.h"
#include "PEFunctions.h"
#include "Calculator.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK CalculatorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD VirtualAddress;
	DWORD dwOffset;
	TCHAR szBuffer[128];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Address Calculator"));
		SetDlgItemText(hWnd, IDC_EDIT_RVA, TEXT("0x"));
		InitStaticText(hWnd);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCALC:
			GetDlgItemText(hWnd, IDC_EDIT_RVA, szBuffer, 128);
			if (!IsHex(szBuffer))
			{
				MessageBox(hWnd, TEXT("Not a valid hex address!"), TEXT(""), MB_ICONERROR | MB_OK);
				break;
			}
			// 十六进制字符串转为对应数值
			if (StrToIntEx(szBuffer, STIF_SUPPORT_HEX, &VirtualAddress))
			{
				Rva2offset(mfs.ImageBase, VirtualAddress, &dwOffset);
				if (0xFFFFFFFF == dwOffset)
				{
					MessageBox(hWnd, TEXT("Not a valid address!"), TEXT(""), MB_ICONERROR | MB_OK);
				}
				else
				{
					StringCchPrintf(szBuffer, 128, TEXT("0x%08X"), dwOffset);
					SetDlgItemText(hWnd, IDC_EDIT_RAWOFFSET, szBuffer);
				}
			}
			break;

		default:
			break;
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

VOID InitStaticText(HWND hWnd)
{
	HFONT hFont;
	LOGFONT font;

	// 设置新字体
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(font), &font);
	font.lfWeight = FW_BOLD;
	hFont = CreateFontIndirect(&font);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_RVA), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_RAWOFFSET), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_H1), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	SendMessage(GetDlgItem(hWnd, IDC_STATIC_H2), WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	DeleteObject(hFont);
}

BOOL IsHex(LPCTSTR szBuffer)
{
	size_t i;

	if (szBuffer[0] != TEXT('0') || (szBuffer[1] != TEXT('x') && szBuffer[1] != TEXT('X')))
	{
		return FALSE;
	}
	for (i = 2; i < _tcslen(szBuffer); i++)
	{
		if (szBuffer[i] < TEXT('0') ||
			((TEXT('9') < szBuffer[i]) && (szBuffer[i] < 'A')) ||
			((TEXT('G') < szBuffer[i]) && (szBuffer[i] < 'a')) ||
			TEXT('f') < szBuffer[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}