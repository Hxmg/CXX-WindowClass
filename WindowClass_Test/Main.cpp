#include <Windows.h>
#include "../WindowClass.hpp"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	class _mainWindow :public Window {
	public:
		_mainWindow(HINSTANCE hInstance):Window(hInstance) {
			this->Create(200,200,0,0);
		}
	}mainWindow(hInstance);

	mainWindow.Show();

	MSG msg{};

	while (0 < GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);    
		DispatchMessage(&msg);     
	}
	return 0;
}