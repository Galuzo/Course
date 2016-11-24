#include <Windows.h>
#include "Resource.h"
#include "UserFunc.h"

void getMenu(HWND hWnd)
{
	HMENU menu, submenu;
	menu = CreateMenu();
	submenu = CreateMenu();
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, L"Файл");
	AppendMenu(submenu, MF_STRING, 103, L"Поиск ");

	AppendMenu(submenu, MF_STRING, 100, L"Выход ");

	SetMenu(hWnd, menu);
	SetMenu(hWnd, submenu);
	DestroyMenu(submenu);
	submenu = CreateMenu();
	AppendMenu(menu, MF_STRING | MF_POPUP, (UINT)submenu, L"Справка");
	AppendMenu(submenu, MF_STRING, 101,L"О программе..");
	SetMenu(hWnd, menu);
	SetMenu(hWnd, submenu);
	DestroyMenu(submenu);
}

void ButtonWork(HWND hWnd,WPARAM wParam,int lastListBox,HWND hWndListBox1,HWND hWndListBox2)
{
	switch (LOWORD(wParam))
	{
	case 101:
		MessageBox(hWnd, L"File Manager \nGaluzo Alexandr\n2016", L"О программе", MB_ICONQUESTION);
		break;
	case 100:
		DestroyWindow(hWnd);
		break;
	case 103:
		/*if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogFileSearch) == 1)
		{
		
		}*/
		//SearchFile(L"C:\\", L"3.jpg");
		Search(L"C:", L"*.*");
		break;
	case ID_BUTTON_RENAME:
		switch(lastListBox)
		{
			case 1:	
				SendMessage(hWndListBox1, WM_KEYDOWN, VK_F2, 0);
				break;
			case 2:
				SendMessage(hWndListBox2, WM_KEYDOWN, VK_F2, 0);
				break;
		}
		break;
	case ID_BUTTON_COPY:
		switch (lastListBox)
		{
		case 1:
			SendMessage(hWndListBox1, WM_KEYDOWN, VK_F5, 0);
			break;
		case 2:
			SendMessage(hWndListBox2, WM_KEYDOWN, VK_F5, 0);
			break;
		}
		break;
	case ID_BUTTON_DIR_CREATE:
		switch (lastListBox)
		{
		case 1:
			SendMessage(hWndListBox1, WM_KEYDOWN, VK_F7, 0);
			break;
		case 2:
			SendMessage(hWndListBox2, WM_KEYDOWN, VK_F7, 0);
			break;
		}
		break;
	case ID_BUTTON_DELETE:
		switch (lastListBox)
		{
		case 1:
			SendMessage(hWndListBox1, WM_KEYDOWN, VK_F8, 0);
			break;
		case 2:
			SendMessage(hWndListBox2, WM_KEYDOWN, VK_F8, 0);
			break;
		}
		break;
	case ID_BUTTON_MOVE:
		switch (lastListBox)
		{
		case 1:
			SendMessage(hWndListBox1, WM_KEYDOWN, VK_F6, 0);
			break;
		case 2:
			SendMessage(hWndListBox2, WM_KEYDOWN, VK_F6, 0);
			break;
		}
		break;
	}

}