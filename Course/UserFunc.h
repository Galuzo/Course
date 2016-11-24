#include<Windows.h>
#pragma once
#ifndef UserFunc_H
#define UserFunc_H
	HWND CreateListBox(int x, int y, int width, int heigth, HWND hWnd, HMENU id);
	void LoadFileList(HWND hWndListBox, TCHAR *path);
	int CALLBACK SortUpDir(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void AddIconToListBox(HWND hWndListBox, int size, TCHAR c_dir[MAX_PATH]);
	int FileOperation(TCHAR *from, TCHAR *to, UINT func);

	INT_PTR CALLBACK DialogFileSearch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	void Search(TCHAR * Dir, TCHAR * Mask);

	void SearchFile(TCHAR * path, TCHAR * name);
	

#endif

