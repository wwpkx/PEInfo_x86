#include "stdafx.h"
#include "resource.h"
#include "PEFunctions.h"
#include "ResourceTable.h"

extern MAP_FILE_STRUCT mfs;

INT_PTR CALLBACK ResourceTableDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, TEXT("Resource"));
		ShowResourceInfo(hWnd);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;

	default:
		break;
	}

	return 0;
}

VOID ShowResourceInfo(HWND hWnd)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)mfs.ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)mfs.ImageBase + pDH->e_lfanew);
	PIMAGE_RESOURCE_DIRECTORY pRD_1 = NULL;
	PIMAGE_RESOURCE_DIRECTORY pRD_2 = NULL;
	PIMAGE_RESOURCE_DIRECTORY pRD_3 = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pRDE_1 = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pRDE_2 = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pRDE_3 = NULL;
	PIMAGE_RESOURCE_DATA_ENTRY pRDE_source = NULL;
	DWORD dwOffset;
	HWND hTree;
	HTREEITEM hNode;
	TVINSERTSTRUCTW tvs;;
	WCHAR szBuffer[128];
	WCHAR szInfo[128];
	size_t i, j, k;

	hTree = GetDlgItem(hWnd, IDC_TREE_RESOURCE);
	Rva2offset(mfs.ImageBase, pNtH->OptionalHeader.DataDirectory[2].VirtualAddress, &dwOffset);
	pRD_1 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)mfs.ImageBase + dwOffset);

	pRDE_1 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRD_1 + sizeof(IMAGE_RESOURCE_DIRECTORY));
	for (i = 0; i < (size_t)(pRD_1->NumberOfNamedEntries + pRD_1->NumberOfIdEntries); i++, pRDE_1++)
	{
		tvs.item.mask = TVIF_TEXT;
		tvs.hInsertAfter = TVI_LAST;
		tvs.item.pszText = szBuffer;
		tvs.hParent = NULL;
		switch (pRDE_1->Name)
		{
		case 0x01:
			StringCchPrintfW(szBuffer, 128, L"Cursor");
			break;

		case 0x02:
			StringCchPrintfW(szBuffer, 128, L"Bitmap");
			break;

		case 0x03:
			StringCchPrintfW(szBuffer, 128, L"Icon");
			break;

		case 0x04:
			StringCchPrintfW(szBuffer, 128, L"Menu");
			break;

		case 0x05:
			StringCchPrintfW(szBuffer, 128, L"Dialog");
			break;

		case 0x06:
			StringCchPrintfW(szBuffer, 128, L"String");
			break;

		case 0x07:
			StringCchPrintfW(szBuffer, 128, L"Font Directory");
			break;

		case 0x08:
			StringCchPrintfW(szBuffer, 128, L"Font");
			break;

		case 0x09:
			StringCchPrintfW(szBuffer, 128, L"Accelerators");
			break;

		case 0x0A:
			StringCchPrintfW(szBuffer, 128, L"Unformatted");
			break;

		case 0x0B:
			StringCchPrintfW(szBuffer, 128, L"MessageTable");
			break;

		case 0x0C:
			StringCchPrintfW(szBuffer, 128, L"Group Cursor");
			break;

		case 0x0E:
			StringCchPrintfW(szBuffer, 128, L"Group Icon");
			break;

		case 0x10:
			StringCchPrintfW(szBuffer, 128, L"Version Information");
			break;

		default:
			StringCchPrintfW(szBuffer, 128, L"Unknown Resource");
			break;
		}
		hNode = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
		tvs.hParent = hNode;

		// 最高位为1时，指向下一层目录块的起始地址
		if (pRDE_1->OffsetToData & 0x80000000)
		{
			pRD_2 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pRD_1 + (pRDE_1->OffsetToData & 0x7FFFFFFF));
			pRDE_2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRD_2 + sizeof(IMAGE_RESOURCE_DIRECTORY));
			for (j = 0; j < (size_t)(pRD_2->NumberOfNamedEntries + pRD_2->NumberOfIdEntries); j++, pRDE_2++)
			{
				// 最高位为1时, Name字段低位作指针用
				if (pRDE_2->Name & 0x80000000)
				{
					StringCchPrintfW(szBuffer, ((PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)pRD_1 + (pRDE_2->Name & 0x7FFFFFFF)))->Length, L"%s", ((PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)pRD_1 + (pRDE_2->Name & 0x7FFFFFFF)))->NameString);
				}
				// 最高位为0时, Name字段作ID用
				else
				{
					StringCchPrintfW(szBuffer, 128, L"%d", pRDE_2->Name);
				}
				// 最高位为1时, 低位数据指向下一层目录块的起始地址
				if (pRDE_2->OffsetToData & 0x80000000)
				{
					pRD_3 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pRD_1 + (pRDE_2->OffsetToData & 0x7FFFFFFF));
					pRDE_3 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pRD_3 + sizeof(IMAGE_RESOURCE_DIRECTORY));
					for (k = 0; k < (size_t)(pRD_3->NumberOfNamedEntries + pRD_3->NumberOfIdEntries); k++, pRDE_3++)
					{
						if (!(pRDE_3->OffsetToData & 0x80000000))
						{
							// StringCchPrintf 好像不能输入 \t 制表符, 所以我只能用空格代替
							pRDE_source = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRD_1 + (pRDE_3->OffsetToData & 0x7FFFFFFF));
							StringCchPrintfW(szInfo, 128, L"        RVA to Data: %08XH        Size: %08XH", pRDE_source->OffsetToData, pRDE_source->Size);
							StringCchCatW(szBuffer, 128, szInfo);
							SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
							continue;
						}
					}
				}
				else
				{
					pRDE_source = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRD_1 + (pRDE_2->OffsetToData & 0x7FFFFFFF));
					StringCchPrintfW(szInfo, 128, L"        RVA to Data: %08XH        Size: %08XH", pRDE_source->OffsetToData, pRDE_source->Size);
					StringCchCatW(szBuffer, 128, szInfo);
					SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
				}
			}
		}
		else
		{
			pRDE_source = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRD_1 + (pRDE_1->OffsetToData & 0x7FFFFFFF));
			StringCchPrintfW(szInfo, 128, L"        RVA to Data: %08XH        Size: %08XH", pRDE_source->OffsetToData, pRDE_source->Size);
			StringCchCatW(szBuffer, 128, szInfo);
			SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
		}
	}
}