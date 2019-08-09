//#include <Windows.h>
#include "Graphics.h"
#include <string>
#include <sstream> // for ostringstream
#include <time.h>

#pragma region Variables
WNDCLASSEX WndCls;
std::string strIB;
#pragma endregion

#pragma region ForwardDeclarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDCLASSEX Init_WindowClass(const char* _className, HINSTANCE _hInst);
HWND Init_Window(int _width, int _height, std::string _title, WNDCLASSEX* _WndClass);
#pragma endregion

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{

	WNDCLASSEX wc = Init_WindowClass("WndClassEX", hInstance);
	HWND hWnd = Init_Window(1280, 720, "DirectX11 Window", &wc);
	Graphics* Gfx = new Graphics(hWnd);
	Gfx->InitDevice();

	// Message Loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Gfx->Render();
		}
	}

	// CLEAN UP
	delete Gfx;
	ToolBox::CleanUp(&hWnd);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0); // is the arbitrary value we want to return on quit through wParam.
		break;
	case VK_RETURN:
	{
		
	}
		break;
	case WM_CHAR: // TranslateMessage(); processes WM_CHAR allow for easy key input.
	{
		// send strIB to string processor
		if (wParam == VK_RETURN)
		{
			if ((strIB = ToolBox::CommandProcesser(strIB)) != "")
			{
					OutputDebugString(strIB.c_str());
				if (strIB == "grow")
				{
					hWnd = Init_Window(1920, 1080, "NEW WINDOW", &WndCls);
				}
				/*OutputDebugString("BALLER!!!");
				if (strIB.size() == 0)
					OutputDebugString("Cleared");*/
			}
			strIB = "";
			SetWindowText(hWnd, strIB.c_str());
			break;
		}
		strIB.push_back((char)wParam);
		//SetActiveWindow(hWnd);
		//Iss << mySin.push_back((char)wParam);


		/*static std::string title;
		title.push_back((char)wParam);*/
		SetWindowText(hWnd, strIB.c_str());
	}
	break;
	case WM_LBUTTONDOWN: // Left Click to get coordinate of raster where (0,0) is top left.
	{
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "Cursor Position: (" << pt.x << "," << pt.y << ")" << std::endl;
		OutputDebugString(oss.str().c_str());
		oss.clear();
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			// mouse wheel forward/up ie:zoom in
			OutputDebugString("Zoom IN");
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			// mouse wheel back/down ie:zoom out
			OutputDebugString("Zoom OUT");
		}
	}
	break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

WNDCLASSEX Init_WindowClass(const char* _className, HINSTANCE _hInst)
{
	// Register window class.
	WndCls = { 0 };
	WndCls.cbSize = sizeof(WndCls);
	WndCls.style = CS_OWNDC;
	WndCls.lpfnWndProc = WndProc;
	WndCls.cbClsExtra = 0;
	WndCls.cbWndExtra = 0;
	WndCls.hInstance = _hInst;
	WndCls.hIcon = nullptr;
	WndCls.hCursor = nullptr;
	WndCls.hbrBackground = nullptr;
	WndCls.lpszMenuName = nullptr;
	WndCls.lpszClassName = _className;

	if (RegisterClassEx(&WndCls) == 0)
		ToolBox::ThrowErrorMsg("CTERROR: In_WindowClass(const char*, HINSTANCE)\nReturned 0\nWindow Class Failed To Register");

	return WndCls;
}

HWND Init_Window(int _width, int _height, std::string _title, WNDCLASSEX* _WndClass)
{

	// Create window.
	RECT ClientRect;
	ClientRect.left = 100;
	ClientRect.right = ClientRect.left + _width;
	ClientRect.top = 100;
	ClientRect.bottom = ClientRect.top + _height;
	AdjustWindowRect(&ClientRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	HWND hWnd = CreateWindowEx(
		0, _WndClass->lpszClassName, _title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
		nullptr, nullptr, _WndClass->hInstance, nullptr);

	if (hWnd == nullptr)
		ToolBox::ThrowErrorMsg("CTERROR: BuildWindow(int,int,string,wndclassex)\nhWnd was null");

	// Draw hWnd.
	ShowWindow(hWnd, SW_SHOW);
	return hWnd;
}