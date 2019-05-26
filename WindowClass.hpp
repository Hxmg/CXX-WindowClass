
#ifndef __WINDOWCLASS_H__
#define __WINDOWCLASS_H__


#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <wchar.h>
#include <stdio.h>
#include "WndProcThunk.hpp"


class Window
{

public:

	Window(HINSTANCE hInst)
		:m_HInstance(hInst),
		m_ClientHeight(0),
		m_ClientWidth(0),
		m_Width(0),
		m_Height(0),
		m_HWnd(0),
		m_X(0),
		m_Y(0)
	{
		m_WndProcThunk.Init((DWORD_PTR)&Window::StaticWndProc,this);
	}

	~Window() {
		if (this->m_HWnd != NULL && IsWindow(this->m_HWnd)) {
			DestroyWindow(m_HWnd);
		}
	}

	inline HWND GetHwnd(){return this->m_HWnd;};


	SIZE GetClientSize(){
		SIZE tSize = { m_ClientWidth , m_ClientHeight };
		return tSize;
	}

	bool SetClientSize(int cWidth = -1,int cHeight = -1){
		RECT tRect = {0,0,m_Width - m_ClientWidth , m_Height - m_ClientHeight};
		tRect.left = m_X;
		tRect.top = m_Y;
		tRect.right += cWidth;
		tRect.bottom += cHeight;
		return MoveWindow(m_HWnd,tRect.left,tRect.top,tRect.right,tRect.bottom,true);

	}
	

	bool Create(
		DWORD exStyle = 0,
		LPCTSTR titleText = TEXT(""),
		DWORD style = WS_OVERLAPPEDWINDOW,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		HWND wndParent = nullptr,
		HMENU menu = nullptr,
		int cmdShow = SW_HIDE,
		WNDCLASSEX* partWndClass = nullptr)
	{
		if (this->m_HWnd != NULL) { return false; }

		WNDCLASSEX wcex;

		if (partWndClass != nullptr){
			wcex = *partWndClass;
		}else{
			wcex.hbrBackground =CreateSolidBrush(RGB(0,0,0));  //(HBRUSH)(COLOR_WINDOW + 1);
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hIcon = nullptr;
			wcex.hIconSm = nullptr;
			wcex.lpszMenuName = nullptr;
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			TCHAR className[21] = TEXT("window");
			wcex.lpszClassName = className;
		}

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = Window::InitWndProc;
		wcex.hInstance = this->m_HInstance;

		if (0 == RegisterClassEx(&wcex)) { return false; }

		HWND tHwnd = nullptr;
		tHwnd = CreateWindowEx(exStyle,
			wcex.lpszClassName,
			titleText, 
			style,
			x,
			y,
			width,
			height,
			wndParent,
			menu,
			this->m_HInstance,
			this);

		if (nullptr == tHwnd) {return false;}		
		this->m_HWnd = tHwnd;

		RECT tmpRect = {0,0,0,0};
		GetClientRect(tHwnd,&tmpRect);
		this->m_ClientWidth = tmpRect.right;
		this->m_ClientHeight = tmpRect.bottom;

		tmpRect = {0,0,0,0};
		GetWindowRect(tHwnd,&tmpRect );
		this->m_X = tmpRect.left;
		this->m_Y = tmpRect.top;
		this->m_Width = tmpRect.right - tmpRect.left;
		this->m_Height = tmpRect.bottom - tmpRect.top;

		ShowWindow(tHwnd,cmdShow);

		return true;
	}
	
	bool Create(int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,int x = CW_USEDEFAULT, int y = CW_USEDEFAULT){
		return Create(0,TEXT("Default Window"), WS_OVERLAPPEDWINDOW,x,y,width,height);
	}

	bool Create(){
		return Create(0,TEXT("Default Window"), WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT);
	}


	void Show(int nCmdShow = SW_SHOW){
		ShowWindow(this->m_HWnd, nCmdShow);
	}

	inline unsigned int GetClientWidth(){return this->m_ClientWidth;}
	inline unsigned int GetClientHeight(){return this->m_ClientHeight;}

protected:
	HWND m_HWnd;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_ClientWidth;
	unsigned int m_ClientHeight;
	int m_X;
	int m_Y;
	HINSTANCE m_HInstance;
	WndProcThunk  m_WndProcThunk;


protected:
	static LRESULT CALLBACK StaticWndProc( HWND hWnd,UINT message ,WPARAM wParam , LPARAM lParam){
	
		Window* pThis = (Window*)hWnd;
		hWnd = pThis->m_HWnd;
		LRESULT returnVal = pThis->WndProc(hWnd, message, wParam, lParam);
		RECT tRect = { 0,0,0,0 };
		TCHAR t[128] = TEXT("");
		switch (message) {
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_SIZE:
				pThis->m_ClientWidth = LOWORD(lParam);
				pThis->m_ClientHeight = HIWORD(lParam);
				GetWindowRect(hWnd,&tRect);
				pThis->m_Width = tRect.right - tRect.left;
				pThis->m_Height = tRect.bottom - tRect.top;
				pThis->m_X = tRect.left;
				pThis->m_Y = tRect.top;
				//_stprintf_s(t, TEXT("X : %d , Y : %d , Width : %d , Height : %d   |   ClientWidth : %d , ClientHeight : %d"), pThis->m_X, pThis->m_Y, pThis->m_Width, pThis->m_Height, pThis->m_ClientWidth, pThis->m_ClientHeight);
				//SetWindowText(hWnd, t);
				break;
			case WM_SIZING:
				pThis->m_Width = ((LPRECT)lParam)->right - ((LPRECT)lParam)->left;
				pThis->m_Height = ((LPRECT)lParam)->bottom - ((LPRECT)lParam)->top;
				//_stprintf_s(t, TEXT("X : %d , Y : %d , Width : %d , Height : %d   |   ClientWidth : %d , ClientHeight : %d"  ), pThis->m_X ,pThis->m_Y, pThis->m_Width, pThis->m_Height,pThis->m_ClientWidth , pThis->m_ClientHeight );
				//SetWindowText(hWnd,t);
				break; 
			case WM_MOVING:
				pThis->m_X = ((LPPOINT)lParam)->x;
				pThis->m_Y = ((LPPOINT)lParam)->y;
				//_stprintf_s(t, TEXT("X : %d , Y : %d , Width : %d , Height : %d   |   ClientWidth : %d , ClientHeight : %d"), pThis->m_X, pThis->m_Y, pThis->m_Width, pThis->m_Height, pThis->m_ClientWidth, pThis->m_ClientHeight);
				//SetWindowText(hWnd, t);
				break; 
		}

		return returnVal;
	}

	static LRESULT CALLBACK InitWndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam){
		switch (message) {
			case WM_CREATE:
				Window* w = NULL;
				w = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
				if (w) {
					w->m_HWnd = hWnd;
					WNDPROC ppp = NULL;
					ppp = Window::StaticWndProc;
#ifdef _WIN64
					SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)w->m_WndProcThunk.GetWndProcThunk());   
#else
					SetWindowLong(hWnd, GWL_WNDPROC, (LONG)w->m_WndProcThunk.GetWndProcThunk());
#endif
					return (*(WNDPROC)(w->m_WndProcThunk.GetWndProcThunk()))(hWnd, message, wParam, lParam);
				
				}
				break;
		}
		return DefWindowProc(hWnd,message,wParam,lParam);
	}

	virtual LRESULT WndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam){
		return DefWindowProc(hWnd,message,wParam,lParam);
	}
	
};





#endif