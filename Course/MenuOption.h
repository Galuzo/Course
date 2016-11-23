#include<Windows.h>
#pragma once
#ifndef MenuOption_H
#define MenuOption_H
void getMenu(HWND hWnd);
void ButtonWork(HWND hWnd, WPARAM wParam, int lastListBox, HWND hWndListBox1, HWND hWndListBox2);
#endif