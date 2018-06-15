#pragma once

typedef struct _MAP_FILE_STRUCT
{
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID ImageBase;
} MAP_FILE_STRUCT, *LPMAP_FILE_STRUCT;

// 将文件映射信息装载到全局变量中
extern BOOL RoadFile(LPCTSTR szFileName, LPMAP_FILE_STRUCT pMFS);

extern BOOL IsPE32File(LPVOID ImageBase);

// 相对虚拟地址转换为原始文件偏移
extern BOOL Rva2offset(LPVOID ImageBase, DWORD VirtualAddress, PDWORD dwOffset);

extern BOOL HasExportTable(LPVOID ImageBase);

extern BOOL HasImportTable(LPVOID ImageBase);

extern BOOL HasResourceTable(LPVOID ImageBase);