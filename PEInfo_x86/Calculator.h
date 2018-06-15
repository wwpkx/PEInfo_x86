#ifndef _CLACULATOR_H
#define _CLACULATOR_H

extern INT_PTR CALLBACK CalculatorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern VOID InitStaticText(HWND hWnd);

extern BOOL IsHex(LPCTSTR szBuffer);

#endif // !_CLACULATOR_H

