#include"Lib.h"
#include <stdio.h>
#include "UserFunc.h"
int CALLBACK SortUpDir(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
void AddIconToListBox(HWND hWndListBox, int size, TCHAR c_dir[MAX_PATH]);

HWND CreateListBox(int x, int y, int width, int heigth, HWND hWnd, HMENU id)
{
	HWND hWndListBox;
	LVCOLUMN lvc;	

	hWndListBox = CreateWindow(WC_LISTVIEW,
		_T(""),
		WS_CHILD
		| WS_VISIBLE	// Видимый
		| WS_BORDER		// Рамка
		| ES_READONLY
		| LVS_REPORT	// Стиль таблицы
		,
		x, y,
		width, heigth,
		hWnd,
		id,
		0, 
		0);

	ListView_SetExtendedListViewStyle(hWndListBox,
		ListView_GetExtendedListViewStyle(hWndListBox)
		| LVS_EX_FULLROWSELECT	
		);
	//--Настройка цветов--//
	ListView_SetBkColor(hWndListBox, RGB(255,255,255));	
	ListView_SetTextColor(hWndListBox, RGB(0,0,0));
	ListView_SetTextBkColor(hWndListBox, RGB(255, 255, 255));
	//--Добавление колонки--//
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;			
	lvc.pszText = _T("Имя");
	lvc.cx = 200;               
	lvc.fmt = LVCFMT_LEFT;		
	ListView_InsertColumn(hWndListBox, 0, &lvc);

	lvc.iSubItem = 1;			
	lvc.pszText = _T("Размер");	
	lvc.cx = 90;				
	lvc.fmt = LVCFMT_RIGHT;		

	ListView_InsertColumn(hWndListBox, 1, &lvc);

	lvc.iSubItem = 2;			
	lvc.pszText = _T("Дата");	
	lvc.cx = 100;				
	ListView_InsertColumn(hWndListBox, 2, &lvc);

	return hWndListBox;
}



void LoadFileList(HWND hWndListBox, TCHAR *path)
{
	LVITEM lvi;					// структура текста в колонке
	WIN32_FIND_DATA fileInfo;	
	HANDLE findFile;			
	int i, j, k, iTmp;
	LARGE_INTEGER fileSize;		
	SYSTEMTIME fileDate;		
	TCHAR cTmp[256], cTmp2[256];
	TCHAR path2[MAX_PATH];

	SendMessage(hWndListBox, LVM_DELETEALLITEMS, 0, 0);

	path2[0] = 0;
	_tcscat_s(path2, path);
	_tcscat_s(path2, _T("*"));

	//--Добавление текста--//
	memset(&lvi, 0, sizeof(lvi));	
	lvi.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;;			
	lvi.cchTextMax = 256;			

	findFile = FindFirstFile(path2, &fileInfo);	
	if (findFile != INVALID_HANDLE_VALUE)
	{
		i = 0;
		do
		{
			lvi.iItem = i;							
			lvi.iImage = i;
			lvi.iSubItem = 0;						
			lvi.pszText = fileInfo.cFileName;	

			lvi.lParam = i;
			ListView_InsertItem(hWndListBox, &lvi);

			//--Вывод размера файла--//
			if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					ListView_SetItemText(hWndListBox, i, 1, _T("<Ссылка>"));
				}
				else
				{
					ListView_SetItemText(hWndListBox, i, 1, _T("<Папка>"));
				}
			}
			else
			{
				fileSize.LowPart = fileInfo.nFileSizeLow;
				fileSize.HighPart = fileInfo.nFileSizeHigh;
				_stprintf_s(cTmp, 256, _T("%lld"), fileSize);
				iTmp = _tcslen(cTmp);
				if (iTmp > 3)
				{
					_tcscpy_s(cTmp2, cTmp);
					k = 0;
					for (j = 0; j < iTmp; j++)
					{
						if ((iTmp - j) % 3 == 0 && j)
						{
							cTmp[k] = ' ';
							k++;
						}
						cTmp[k] = cTmp2[j];
						k++;
					}
					cTmp[k] = 0;
				}
				ListView_SetItemText(hWndListBox, i, 1, cTmp);
			}
			
			FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &fileDate);		
			_stprintf_s(cTmp, 256, _T("%02d.%02d.%04d %02d:%02d"), fileDate.wDay, fileDate.wMonth, fileDate.wYear, fileDate.wHour, fileDate.wMinute);
			ListView_SetItemText(hWndListBox, i, 2, cTmp);

			i++;
		} while (FindNextFile(findFile, &fileInfo));		
	}
	AddIconToListBox(hWndListBox, i, path2);
	ListView_SortItemsEx(hWndListBox, SortUpDir, hWndListBox);
}

int CALLBACK SortUpDir(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR word1[256], word2[256];
	LVFINDINFO find;
	int i1, i2;

	if (lParam1 == 0) return -1;
	else if (lParam2 == 0) return 1;

	find.flags = LVFI_PARAM;
	find.lParam = lParam1;
	ListView_GetItemText((HWND)lParamSort,
		i1 = ListView_FindItem((HWND)lParamSort, -1, &find),
		1, word1, 256);

	find.lParam = lParam2;
	ListView_GetItemText((HWND)lParamSort,
		i2 = ListView_FindItem((HWND)lParamSort, -1, &find),
		1, word2, 256);

	if (word1[0] == '<' && word2[0] == '<')
	{
		return 0;
	}
	else if (word1[0] == '<')
	{
		return -1;
	}
	else if (word2[0] == '<')
	{
		return 1;
	}
	else return 0;

}

void AddIconToListBox(HWND hWndListBox, int size, TCHAR c_dir[MAX_PATH])
{
	HIMAGELIST hSmall;
	SHFILEINFO lp;
	TCHAR buf1[MAX_PATH];
	DWORD num;

	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR32, size + 2, 1);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(c_dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, _T("Ошибка"), _T("Не найден"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		do
		{//присваеваем атрибуты
			if (_tcscmp(FindFileData.cFileName, _T(".")) == 0)
			{//если диск
				_tcscpy_s(buf1, c_dir);
				_tcscat_s(buf1, FindFileData.cFileName);
				SHGetFileInfo(_T(""), FILE_ATTRIBUTE_DEVICE, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);

			}
			if (_tcscmp(FindFileData.cFileName, _T("..")) == 0)
			{//если фаилы,папки
				_tcscpy_s(buf1, c_dir);
				_tcscat_s(buf1, FindFileData.cFileName);
				SHGetFileInfo(_T(""), FILE_ATTRIBUTE_DIRECTORY, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			//присваеваем иконки
			_tcscpy_s(buf1, c_dir);
			buf1[_tcslen(buf1) - 1] = 0;

			_tcscat_s(buf1, FindFileData.cFileName);
			num = GetFileAttributes(buf1);
			SHGetFileInfo(buf1, num, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

			ImageList_AddIcon(hSmall, lp.hIcon);
			DestroyIcon(lp.hIcon);

		} while (FindNextFile(hFind, &FindFileData) != 0);

		FindClose(hFind);
	}
	ListView_SetImageList(hWndListBox, hSmall, LVSIL_SMALL);
}

int FileOperation(TCHAR *from, TCHAR *to, UINT func)
{
	SHFILEOPSTRUCT shFileOpStr = { 0 };
	int i;

	i = 0;
	while (from[i]) i++;
	from[i + 1] = 0;

	if (to)
	{
		i = 0;
		while (to[i]) i++;
		to[i + 1] = 0;
	}

	shFileOpStr.hwnd = 0;
	shFileOpStr.wFunc = func;
	shFileOpStr.pFrom = from;
	shFileOpStr.pTo = to;
	shFileOpStr.fFlags = FOF_NOCONFIRMMKDIR;
	shFileOpStr.fAnyOperationsAborted = 0;
	shFileOpStr.hNameMappings = 0;
	shFileOpStr.lpszProgressTitle = 0;

	return SHFileOperation(&shFileOpStr);
}


