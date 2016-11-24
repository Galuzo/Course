#include "Lib.h"
#include "UserFunc.h"
#include "MenuOption.h"
#define MAX_PATH		512


typedef struct _REPARSE_DATA_BUFFER
{
	ULONG ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;
	union
	{
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			ULONG Flags;
			WCHAR PathBuffer[1];
		}
		SymbolicLinkReparseBuffer;
		struct
		{
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			WCHAR PathBuffer[1];
		}
		MountPointReparseBuffer;
		struct
		{
			UCHAR  DataBuffer[1];
		}
		GenericReparseBuffer;
	};
}
REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcListView1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProcListView2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE hInstance;
static HWND hComboBox_1, hComboBox_2;
static HWND hListView_1, hListView_2;
TCHAR selectedFile1[MAX_PATH], selectedFile2[MAX_PATH];
WNDPROC origWndProcListView;
TCHAR path1[MAX_PATH], path2[MAX_PATH];
TCHAR fullPathToFile[MAX_PATH];
HWND hWndListBox = 0;
HWND hWndListBox1, hWndListBox2;
HWND hWndEdit1, hWndEdit2;
static int lastListBox = 0;


int id_button = ID_BUTTON_START;

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"HelloWorldClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow(L"HelloWorldClass", L"File Manager", WS_BORDER | WS_SYSMENU | WS_VISIBLE,
		CW_USEDEFAULT, 0, 1000, 740, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmHdr = (LPNMHDR)lParam;
	LPNMLISTVIEW pnmLV = (LPNMLISTVIEW)lParam;
	HWND hWndEdit = 0;
	TCHAR *selectedFile=0;
	TCHAR *path = 0;
	TCHAR selectedFileSize[MAX_PATH];
	bool reloadFileList = 1;
		


	switch (message)
	{
	case WM_CREATE:
		int x, y;
		int k;
		int dx, width;
		
		TCHAR *disk;
		InitCommonControls();
	    disk = new TCHAR[256];
		GetLogicalDrives();
		GetLogicalDriveStrings(256, (LPTSTR)disk);
		x = 10;
		y = 10;
		dx = 200;
		width = 170;
		k = _tcslen(disk) + 1;
		while (*disk != '\0')
		{
			disk[1] = 0;
			CreateWindow(_T("BUTTON"), disk, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				x, y, 30, 20, hWnd, (HMENU)id_button++, NULL, NULL);

			CreateWindow(_T("BUTTON"), disk, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				x + 490, y, 30, 20, hWnd, (HMENU)id_button++, NULL, NULL);
			x += 40;
			disk += k;
		}
		x = 10;
		y += 30;
		
		hWndEdit1 = CreateWindow(_T("EDIT"), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,
			x, y, 480, 20, hWnd, (HMENU)ID_EDIT_1, NULL, NULL);
		hWndEdit2 = CreateWindow(_T("EDIT"), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,
			x + 490, y, 480, 20, hWnd, (HMENU)ID_EDIT_2, NULL, NULL);
		y += 30;
		hWndListBox1 = CreateListBox(
			x, y,
			480, 550,
			hWnd,
			(HMENU)ID_LISTBOX_1);
		hWndListBox2 = CreateListBox(
			x + 490, y,
			480, 550,
			hWnd,
			(HMENU)ID_LISTBOX_2);
		y += 560;
		origWndProcListView = (WNDPROC)SetWindowLong(hWndListBox1,
			GWL_WNDPROC, (LONG)WndProcListView1);
		origWndProcListView = (WNDPROC)SetWindowLong(hWndListBox2,
			GWL_WNDPROC, (LONG)WndProcListView2);
		path1[0] = 0;
		path2[0] = 0;
		_tcscat_s(path1, _T("C:\\"));
		_tcscat_s(path2, _T("C:\\"));
		LoadFileList(hWndListBox1, path1);
		LoadFileList(hWndListBox2, path2);
		SetWindowText(hWndEdit1, path1);
		SetWindowText(hWndEdit2, path2);
		CreateWindow(_T("BUTTON"), _T("F2 Переименование"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, width, 30, hWnd, (HMENU)ID_BUTTON_RENAME, NULL, NULL);
		x += dx;
		CreateWindow(_T("BUTTON"), _T("F5 Копирование"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, width, 30, hWnd, (HMENU)ID_BUTTON_COPY, NULL, NULL);
		x += dx;
		CreateWindow(_T("BUTTON"), _T("F6 Перемещение"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, width, 30, hWnd, (HMENU)ID_BUTTON_MOVE, NULL, NULL);
		x += dx;
		CreateWindow(_T("BUTTON"), _T("F7 Каталог"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, width, 30, hWnd, (HMENU)ID_BUTTON_DIR_CREATE, NULL, NULL);
		x += dx;
		CreateWindow(_T("BUTTON"), _T("F8 Удаление"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, width, 30, hWnd, (HMENU)ID_BUTTON_DELETE, NULL, NULL);
		x += dx;
		getMenu(hWnd);
		break;
	case WM_NOTIFY:
		switch (lpnmHdr->code)
		{
		case NM_CLICK:
			switch (lpnmHdr->idFrom)
			{
			case ID_LISTBOX_1:
				hWndListBox = hWndListBox1;
				selectedFile = selectedFile1;
				lastListBox = 1;
				break;
			case ID_LISTBOX_2:
				hWndListBox = hWndListBox2;
				selectedFile = selectedFile2;
				lastListBox = 2;
				break;
			}
			if (hWndListBox)
			{
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 0, selectedFile, MAX_PATH);			
			}
		break;
		case NM_RCLICK:
			switch (lpnmHdr->idFrom)
			{
			case ID_LISTBOX_1:
				hWndListBox = hWndListBox1;
				hWndEdit = hWndEdit1;
				path = path1;
				break;
			case ID_LISTBOX_2:
				hWndListBox = hWndListBox2;
				hWndEdit = hWndEdit2;
				path = path2;
				break;
			}
			if (hWndListBox)
			{				
				selectedFile = new TCHAR[MAX_PATH];
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 0, selectedFile, MAX_PATH);
				if (_tcslen(selectedFile) <= 255)
				{
					SHELLEXECUTEINFO fileInfo;

					_tcscpy_s(fullPathToFile, path);
					_tcscat_s(fullPathToFile, selectedFile);

					ZeroMemory(&fileInfo, sizeof(SHELLEXECUTEINFO));
					fileInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					fileInfo.lpVerb = _T("properties");
					fileInfo.lpFile = fullPathToFile;
					fileInfo.nShow = SW_SHOW;
					fileInfo.fMask = SEE_MASK_INVOKEIDLIST;
					ShellExecuteEx(&fileInfo);

				}
			}
			break;
		case NM_DBLCLK:
			switch (lpnmHdr->idFrom)
			{
			case ID_LISTBOX_1:
				hWndListBox = hWndListBox1;
				hWndEdit = hWndEdit1;
				path = path1;
				break;
			case ID_LISTBOX_2:
				hWndListBox = hWndListBox2;
				hWndEdit = hWndEdit2;
				path = path2;
				break;
			default:
				break;
			}
			if (hWndListBox)
			{
				selectedFile = new TCHAR[MAX_PATH];
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 0, selectedFile, MAX_PATH);

				if (_tcscmp(selectedFile, _T("..")) == 0)	// Вверх на одну дирректорию
				{
					path[_tcslen(path) - 1] = 0;
					for (int i = _tcslen(path) - 1; i > 0; i--)
					{
						TCHAR s;
						s = path[i];
						if (s == '\\')
						{
							k = i;
							break;
						}
					}
					path[k + 1] = 0;
				}
				else if (_tcscmp(selectedFile, _T(".")) == 0)	// В корень диска
				{
					path[3] = 0;
				}
				else
				{
					ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 1, selectedFileSize, MAX_PATH);
					if (_tcscmp(selectedFileSize, _T("<Папка>")) == 0)
					{
						_tcscat_s(path, MAX_PATH, _T("\\"));
						path[_tcslen(path) - 1] = 0;
						_tcscat_s(path, MAX_PATH, selectedFile);
						_tcscat_s(path, MAX_PATH, _T("\\"));
					}
					else if (_tcscmp(selectedFileSize, _T("<Ссылка>")) == 0)
					{
						HANDLE reparsePoint;
						BYTE *pBuffer;
						PREPARSE_DATA_BUFFER pReparseBuffer = NULL;
						DWORD dwRetCode;

						fullPathToFile[0] = 0;
						_tcscpy_s(fullPathToFile, _T("\\??\\"));
						//_tcscpy_s(fullPathToFile, _T("\\\\.\\"));
						_tcscat_s(fullPathToFile, path);
						_tcscat_s(fullPathToFile, selectedFile);

						//MessageBox(0, fullPathToFile, L"", 0);

						reparsePoint = CreateFile(fullPathToFile,
							READ_CONTROL,
							FILE_SHARE_READ,
							0,
							OPEN_EXISTING,
							FILE_FLAG_OPEN_REPARSE_POINT |
							FILE_FLAG_BACKUP_SEMANTICS |
							0,
							0);

						if (reparsePoint == INVALID_HANDLE_VALUE)
						{
							//DisplayError(_T("reParsePoint 1"));
							CloseHandle(reparsePoint);
						}
						else
						{
							pBuffer = new BYTE[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
							pReparseBuffer = (PREPARSE_DATA_BUFFER)pBuffer;
							if (DeviceIoControl(
								reparsePoint,
								FSCTL_GET_REPARSE_POINT,
								NULL,
								0,
								pReparseBuffer,
								MAXIMUM_REPARSE_DATA_BUFFER_SIZE,
								&dwRetCode,
								NULL))
							{
								if (pReparseBuffer->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT)
								{
									if (*pReparseBuffer->MountPointReparseBuffer.PathBuffer)
									{
										TCHAR tmp;
										_tcscpy_s(path, MAX_PATH, &(pReparseBuffer->MountPointReparseBuffer.PathBuffer)[4]);
										_tcscat_s(path, MAX_PATH, _T("\\"));
									}
									else
										MessageBox(0, _T("Ошибка при получении пути по ссылке"), _T("Ошибка"), 0);
								}
								else
								{
									_tcscat_s(path, MAX_PATH, _T("\\"));
									path[_tcslen(path) - 1] = 0;
									_tcscat_s(path, MAX_PATH, selectedFile);
									_tcscat_s(path, MAX_PATH, _T("\\"));
								}
							}
							//else DisplayError(_T("DeviceIoControl 1"));
							delete pBuffer;
							CloseHandle(reparsePoint);
						}
					}
					else
					{
						fullPathToFile[0] = 0;
						_tcscpy_s(fullPathToFile, path);
						_tcscat_s(fullPathToFile, selectedFile);
						ShellExecute(0, _T("open"), fullPathToFile, NULL, NULL, SW_SHOWNORMAL);
						//reloadFileList = 0;
					}
				}
				if (reloadFileList)
				{
					lastListBox = 0;
					SetWindowText(hWndEdit, path);
					LoadFileList(hWndListBox, path);
					switch (lpnmHdr->idFrom)
					{
					case ID_LISTBOX_1:
						selectedFile1[0] = 0;
						break;
					case ID_LISTBOX_2:
						selectedFile2[0] = 0;
						break;
					default:
						break;
					}

				}
				delete[]selectedFile;
			}
		}
		break;
	

	case WM_COMMAND:
	    ButtonWork( hWnd, wParam, lastListBox,  hWndListBox1,hWndListBox2);
		if (LOWORD(wParam) >= ID_BUTTON_START && LOWORD(wParam) < id_button)
		{
			if (LOWORD(wParam) % 2 == 0)
			{
				hWndListBox = hWndListBox1;
				hWndEdit = hWndEdit1;
				selectedFile = selectedFile1;
				path = path1;
			}
			else
			{
				hWndListBox = hWndListBox2;
				hWndEdit = hWndEdit2;
				selectedFile = selectedFile2;
				path = path2;
			}
			GetWindowText(GetDlgItem(hWnd, LOWORD(wParam)), path, MAX_PATH);
			_tcscat_s(path, MAX_PATH, _T(":\\"));
			SetWindowText(hWndEdit, path);
			LoadFileList(hWndListBox, path);
			lastListBox = 0;
			selectedFile[0] = 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


INT_PTR CALLBACK DialogRename1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, ID_DEDIT), selectedFile1);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			TCHAR from[MAX_PATH], to[MAX_PATH], buf[MAX_PATH];

			buf[GetWindowText(GetDlgItem(hDlg, ID_DEDIT), buf, MAX_PATH)] = 0;

			_tcscpy_s(from, path1);
			_tcscat_s(from, selectedFile1);
			_tcscpy_s(to, path1);
			_tcscat_s(to, buf);

			EndDialog(hDlg, LOWORD(FileOperation(from, to, FO_RENAME)));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DialogRename2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hDlg, ID_DEDIT), selectedFile2);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			TCHAR from[MAX_PATH], to[MAX_PATH], buf[MAX_PATH];

			buf[GetWindowText(GetDlgItem(hDlg, ID_DEDIT), buf, MAX_PATH)] = 0;

			_tcscpy_s(from, path2);
			_tcscat_s(from, selectedFile2);
			_tcscpy_s(to, path2);
			_tcscat_s(to, buf);

			EndDialog(hDlg, LOWORD(FileOperation(from, to, FO_RENAME)));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DialogCreateDir1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			TCHAR to[MAX_PATH], buf[MAX_PATH];

			buf[GetWindowText(GetDlgItem(hDlg, ID_DEDIT), buf, MAX_PATH)] = 0;

			_tcscpy_s(to, path1);
			_tcscat_s(to, buf);

			EndDialog(hDlg, LOWORD(CreateDirectory(to, 0)));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(0));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DialogCreateDir2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			TCHAR to[MAX_PATH], buf[MAX_PATH];

			buf[GetWindowText(GetDlgItem(hDlg, ID_DEDIT), buf, MAX_PATH)] = 0;

			_tcscpy_s(to, path2);
			_tcscat_s(to, buf);

			EndDialog(hDlg, LOWORD(CreateDirectory(to, 0)));
			return (INT_PTR)TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(0));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}




LRESULT CALLBACK WndProcListView1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		TCHAR from[MAX_PATH], to[MAX_PATH];

		_tcscpy_s(from, path1);
		_tcscat_s(from, selectedFile1);
		_tcscpy_s(to, path2);
		_tcscat_s(to, selectedFile1);

		switch (wParam)
		{
		case VK_F2:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_RENAME), hWnd, DialogRename1) == 0)
			{
				LoadFileList(hWndListBox1, path1);
				if (_tcscmp(path2, path1) == 0)
					LoadFileList(hWndListBox2, path2);
			}

			break;
		case VK_F5:
			if (FileOperation(from, to, FO_COPY) == 0)
			{
				LoadFileList(hWndListBox2, path2);
			}
			break;

		case VK_F6:
			if (FileOperation(from, to, FO_MOVE) == 0)
			{
				LoadFileList(hWndListBox1, path1);
				LoadFileList(hWndListBox2, path2);
			}
			break;

		case VK_F7:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_CREATE_DIR), hWnd, DialogCreateDir1) == 1)
			{
				LoadFileList(hWndListBox1, path1);
			}
			break;

		case VK_F8:
			if (FileOperation(from, 0, FO_DELETE) == 0)
			{
				LoadFileList(hWndListBox1, path1);
			}
			break;
		}
	break;
	default:
		return CallWindowProc(origWndProcListView, hWnd, message, wParam, lParam);
	}
}


LRESULT CALLBACK WndProcListView2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RBUTTONUP:

		break;
	case WM_KEYDOWN:
		TCHAR from[MAX_PATH], to[MAX_PATH];

		_tcscpy_s(from, path2);
		_tcscat_s(from, selectedFile2);
		_tcscpy_s(to, path1);
		_tcscat_s(to, selectedFile2);

		switch (wParam)
		{
		case VK_F2:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_RENAME), hWnd, DialogRename2) == 0)
			{
				LoadFileList(hWndListBox2, path2);
				if (_tcscmp(path2, path1) == 0)
					LoadFileList(hWndListBox1, path1);
			}
			break;

		case VK_F5:
			if (FileOperation(from, to, FO_COPY) == 0)
			{
				LoadFileList(hWndListBox1, path1);
			}
			break;

		case VK_F6:
			if (FileOperation(from, to, FO_MOVE) == 0)
			{
				LoadFileList(hWndListBox1, path1);
				LoadFileList(hWndListBox2, path2);
			}
			break;

		case VK_F7:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_CREATE_DIR), hWnd, DialogCreateDir2) == 1)
			{
				LoadFileList(hWndListBox2, path2);
			}
			break;

		case VK_DELETE:
		case VK_F8:
			if (FileOperation(from, 0, FO_DELETE) == 0)
			{
				LoadFileList(hWndListBox2, path2);
			}
			break;

		default:
			break;
		}
	default:
		return CallWindowProc(origWndProcListView, hWnd, message, wParam, lParam);
	}
}




