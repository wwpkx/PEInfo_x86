#include "stdafx.h"
#include "PEFunctions.h"

BOOL RoadFile(LPCTSTR szFileName, LPMAP_FILE_STRUCT pMFS)
{
	HANDLE hFile;
	HANDLE hMapFile;
	LPVOID ImageBase;

	hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hMapFile)
	{
		return FALSE;
	}

	ImageBase = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	if (!ImageBase)
	{
		return FALSE;
	}

	pMFS->hFile = hFile;
	pMFS->hMapping = hMapFile;
	pMFS->ImageBase = ImageBase;
	return TRUE;
}

BOOL IsPE32File(LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_FILE_HEADER pFH = &pNtH->FileHeader;

	if (!pDH)
	{
		return FALSE;
	}
	if (pDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}
	if (pNtH->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}
	if (pFH->Machine == IMAGE_FILE_MACHINE_I386)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Rva2offset(LPVOID ImageBase, DWORD VirtualAddress, PDWORD dwOffset)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_SECTION_HEADER pSH = IMAGE_FIRST_SECTION(pNtH);
	size_t i = 0;
	for (i = 0; i < pNtH->FileHeader.NumberOfSections; i++, pSH++)
	{
		if (pSH->VirtualAddress <= VirtualAddress && VirtualAddress < pSH->VirtualAddress + pSH->SizeOfRawData)
		{
			*dwOffset = VirtualAddress - pSH->VirtualAddress + pSH->PointerToRawData;
			return TRUE;
		}
	}

	*dwOffset = 0xFFFFFFFF;
	return FALSE;
}

BOOL HasExportTable(LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDD = &pNtH->OptionalHeader.DataDirectory[0];

	if (0 == pDD->Size)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL HasImportTable(LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDD = &pNtH->OptionalHeader.DataDirectory[1];

	if (0 == pDD->Size)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL HasResourceTable(LPVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)ImageBase;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
	PIMAGE_DATA_DIRECTORY pDD = &pNtH->OptionalHeader.DataDirectory[2];

	if (0 == pDD->Size)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}