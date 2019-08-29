//#include <Windows.h>
//#include "ModelFactory.h"
#include "Graphics.h"

#pragma region Variables
WNDCLASSEX WndCls;
HWND hWnd;
std::string winTitle = "Caio Tavares GFX2 Project | See KeyBinds.txt for info";
Graphics* Gfx = nullptr;
std::string strIB;
bool isTyping = false;
#define MODEL_COUNT 3
int currModel = 0;
bool ModelSwitched = FALSE;
#define MoveSpeed 1.0f
XMFLOAT2 centerScreen;
float last_X;
float last_Y;
float screenY;
float screenX;
float screenRatio;

int CamInUse = 0;
XMMATRIX* views[2];
XMMATRIX* cams[2];
XMMATRIX* projs[2];
float* fovs[2];
bool lookat = false;
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

	screenY = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
	screenX = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
	centerScreen = { (screenX * 0.5f) ,(screenY * 0.5f) };
	last_X = centerScreen.x;
	last_Y = centerScreen.y;
	screenRatio = (hWndHeight) * (hWndWidth) / (screenY * screenX);
	SetCursorPos(static_cast<int>(centerScreen.x), static_cast<int>(centerScreen.y));

	WNDCLASSEX wc = Init_WindowClass("WndClassEX", hInstance);
	hWnd = Init_Window(hWndWidth, hWndHeight, winTitle, &wc);
	Gfx = new Graphics(hWnd);
	Gfx->InitDevice();
	cams[0] = &Gfx->Camera_1;
	views[0] = &Gfx->globalView_1;
	projs[0] = &Gfx->globalProj_1;
	fovs[0] = &Gfx->FoV_angle_1;
	cams[1] = &Gfx->Camera_2;
	views[1] = &Gfx->globalView_2;
	projs[1] = &Gfx->globalProj_2;
	fovs[1] = &Gfx->FoV_angle_2;


	// Message Loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		//if (ModelSwitched)
		//{
		//	ModelSwitched = FALSE;
		//	ModelDraw_Switch(currModel);
		//	Gfx->InitDevice();
		//}


		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Gfx->Render();
	}

	// CLEAN UP
	if (Gfx != nullptr)
	{
		delete Gfx;
		Gfx = nullptr;
	}
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
		//case VK_RETURN:
		//{
		//
		//}
		//break;
		//case WM_CHAR: // TranslateMessage(); processes WM_CHAR allow for easy key input.
		//{
		//	if (wParam == VK_TAB || isTyping)
		//	{
		//		if (!isTyping)
		//		{
		//			isTyping = true;
		//			break;
		//		}
		//		// While Typing...
		//		// "Enter" to send strIB to string processor
		//		if (wParam == VK_RETURN)
		//		{
		//			if ((strIB = ToolBox::CommandProcesser(strIB)) != "")
		//			{
		//				if (strIB == "cls")
		//				{
		//					PostQuitMessage(0);
		//					isTyping = false;
		//				}
		//				//else if (strIB.substr(0,6) == "alight")
		//				else if (strIB.substr(0, 6) == "dlight")
		//				{
		//					DirLight_ComProc(strIB);
		//					isTyping = false;
		//				}
		//			}
		//			strIB = "";
		//			SetWindowText(hWnd, winTitle.c_str());
		//			break;
		//		}
		//		strIB.push_back((char)wParam);
		//		SetWindowText(hWnd, strIB.c_str());
		//	}
		//
		//}
		//break;
	case WM_LBUTTONDOWN: // Left Click to get coordinate of raster where (0,0) is top left.
	{
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "Cursor Position: (" << pt.x << "," << pt.y << ")" << std::endl;
		OutputDebugString(oss.str().c_str());
		oss.clear();
	}
	break;
	case WM_ENTERSIZEMOVE:
	{
		//RECT* nWnd = new RECT();
		//GetClientRect(hWnd, nWnd);
		//UINT nHeight = nWnd->bottom - nWnd->top;
		//UINT nWidth = nWnd->right - nWnd->left;
		//hWndHeight = nWidth / hWndAspectRatio;
		//hWndWidth = nHeight * hWndAspectRatio;

	}
	break;
	// Camera Controls
	case WM_MOUSEWHEEL:
	{
		//if (/*direction > 0 && */)
		//	//if ((char)wParam == 'f') // Fov Wider // zoom out
		//{
		//	Gfx->FoV_angle += direction;
		//	Gfx->globalProj = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
		//}
		//else if ((char)wParam == 'r') // Fov narrower // zoom in
		if ((*fovs[CamInUse]) <= 90 && (*fovs[CamInUse]) >= 30)
		{
			float direction = GET_WHEEL_DELTA_WPARAM(wParam) * 0.01f;
			(*fovs[CamInUse]) += direction;
			*projs[CamInUse] = XMMatrixPerspectiveFovLH(degToRad((*fovs[CamInUse])), hWndWidth / hWndHeight, Gfx->nearPlane, Gfx->farPlane);
		}
		else
		{
			if ((*fovs[CamInUse]) > 90) (*fovs[CamInUse]) -= 0.05f;
			else if ((*fovs[CamInUse]) < 30) (*fovs[CamInUse]) += 0.05f;

			*projs[CamInUse] = XMMatrixPerspectiveFovLH(degToRad((*fovs[CamInUse])), hWndWidth / hWndHeight, Gfx->nearPlane, Gfx->farPlane);

		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (GetAsyncKeyState(0x46))
		{
			(*fovs[CamInUse]) = 60;
			*projs[CamInUse] = XMMatrixPerspectiveFovLH(degToRad((*fovs[CamInUse])), hWndWidth / hWndHeight, Gfx->nearPlane, Gfx->farPlane);
		}
		if (GetAsyncKeyState(VK_TAB))
		{
			CamInUse++;
			if (CamInUse >= ARRAYSIZE(cams))
				CamInUse = 0;
		}
		if (GetAsyncKeyState(VK_UP))
		{
			XMVECTOR saver = cams[CamInUse]->r[3];
			cams[CamInUse]->r[3] = XMVectorSet(0, 0, 0, 1);
			*cams[CamInUse] = XMMatrixMultiply(XMMatrixRotationX(degToRad(-2.5f)), *cams[CamInUse]);
			cams[CamInUse]->r[3] = saver;
		}
		else if (GetAsyncKeyState(VK_DOWN))
		{
			XMVECTOR saver = cams[CamInUse]->r[3];
			cams[CamInUse]->r[3] = XMVectorSet(0, 0, 0, 1);
			*cams[CamInUse] = XMMatrixMultiply(XMMatrixRotationX(degToRad(2.5f)), *cams[CamInUse]);
			cams[CamInUse]->r[3] = saver;
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			XMVECTOR saver = cams[CamInUse]->r[3];
			cams[CamInUse]->r[3] = XMVectorSet(0, 0, 0, 1);
			*cams[CamInUse] = XMMatrixMultiply(*cams[CamInUse], XMMatrixRotationY(degToRad(-2.5f)));
			cams[CamInUse]->r[3] = saver;
		}
		else if (GetAsyncKeyState(VK_RIGHT))
		{
			XMVECTOR saver = cams[CamInUse]->r[3];
			cams[CamInUse]->r[3] = XMVectorSet(0, 0, 0, 1);
			*cams[CamInUse] = XMMatrixMultiply(*cams[CamInUse], XMMatrixRotationY(degToRad(2.5f)));
			cams[CamInUse]->r[3] = saver;
		}

		// Up
		if (GetAsyncKeyState(VK_SPACE))
		{
			XMMATRIX trans = XMMatrixTranslation(0.0f, MoveSpeed, 0.0f);
			*cams[CamInUse] = XMMatrixMultiply(*cams[CamInUse], trans);
		}
		// Down
		else if (GetAsyncKeyState(VK_LSHIFT))
		{
			XMMATRIX trans = XMMatrixTranslation(0.0f, -MoveSpeed, 0.0f);
			*cams[CamInUse] = XMMatrixMultiply(*cams[CamInUse], trans);
		}
	//}
	//case WM_CHAR:
	//{
		// IF for 1,2,3,4 camera types, camera 1 is free cam
		if (GetAsyncKeyState(0x50))
		//if ((char)wParam == 'p')
		{
			std::ostringstream oss;
			XMFLOAT4 currPos;
			XMStoreFloat4(&currPos, cams[CamInUse]->r[3]);
			oss << "Camera Position: (" << currPos.x << "," << currPos.y << "," << currPos.z << ")" << std::endl;
			OutputDebugString(oss.str().c_str());
			oss.clear();
		}
		if (GetAsyncKeyState(0x4C))
		{
			if (Gfx->PointLight_A < 1.0f)
				Gfx->PointLight_A = 1.0f;
			else
				Gfx->PointLight_A = 0.0f;
		}
		if (GetAsyncKeyState(0x4F)) // Go Home Cam youre drunk
		{
			lookat = !lookat;
		}
		if (GetAsyncKeyState(0x57)) // Forward
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] -= {0.0f, 0.0f, 0.1f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, MoveSpeed);
			*cams[CamInUse] = XMMatrixMultiply(trans, *cams[CamInUse]);
		}
		else if (GetAsyncKeyState(0x53)) // Back
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] += {0.0f, 0.0f, 0.1f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, -MoveSpeed);
			*cams[CamInUse] = XMMatrixMultiply(trans, *cams[CamInUse]);
		}
		if (GetAsyncKeyState(0x41)) // Left
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] += {0.1f, 0.0f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(-MoveSpeed, 0.0f, 0.0f);
			*cams[CamInUse] = XMMatrixMultiply(trans, *cams[CamInUse]);
		}
		else if (GetAsyncKeyState(0x44)) // Right
		{
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			//Gfx->globalView.r[2] -= {0.1f, 0.0f, 0.0f};
			//Gfx->globalView = XMMatrixInverse(&XMMatrixDeterminant(Gfx->globalView), Gfx->globalView);
			XMMATRIX trans = XMMatrixTranslation(MoveSpeed, 0.0f, 0.0f);
			*cams[CamInUse] = XMMatrixMultiply(trans, *cams[CamInUse]);
		}

		//// roll left
		//if ((char)wParam == 'q')
		//{
		//	XMMATRIX rotation = XMMatrixRotationZ(degToRad(2));
		//	XMVECTOR saver = Gfx->Camera.r[3];
		//	Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		//	Gfx->Camera = XMMatrixMultiply(rotation, Gfx->Camera);
		//	Gfx->Camera.r[3] = saver;
		//}
		//// roll right
		//else if ((char)wParam == 'e')
		//{
		//	XMMATRIX rotation = XMMatrixRotationZ(degToRad(-2));
		//	XMVECTOR saver = Gfx->Camera.r[3];
		//	Gfx->Camera.r[3] = XMVectorSet(0, 0, 0, 1);
		//	Gfx->Camera = XMMatrixMultiply(rotation, Gfx->Camera);
		//	Gfx->Camera.r[3] = saver;
		//}

		if ((char)wParam == '-') // Far plane closer
		{
			if (Gfx->farPlane > 50.0f)
			{
				Gfx->farPlane -= 50.0f;
				Gfx->globalProj_1 = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle_1), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == '=') // Far plane further
		{
			if (Gfx->farPlane < 500.0f)
			{
				Gfx->farPlane += 50.0f;
				Gfx->globalProj_1 = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle_1), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == '[') // Near plane closer
		{
			if (Gfx->nearPlane > 0.001)
			{
				Gfx->nearPlane /= 10.0f;
				Gfx->globalProj_1 = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle_1), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		else if ((char)wParam == ']') // Near plane further
		{
			if (Gfx->nearPlane < 25.0f)
			{
				Gfx->nearPlane += 10.0f;
				Gfx->globalProj_1 = XMMatrixPerspectiveFovLH(degToRad(Gfx->FoV_angle_1), 1280.0f / 720.0f, Gfx->nearPlane, Gfx->farPlane);
			}
		}
		if ((char)wParam == ',')
		{
			//if (Gfx->gSpotLight.coneWidth_R.x > 0.5f)
			//	Gfx->gSpotLight.coneWidth_R.x -= 0.01f;
			//std::ostringstream oss;
			//oss << Gfx->gSpotLight.coneWidth_R.x << std::endl;
			//OutputDebugString(oss.str().c_str());
			//oss.clear();
		}
		else if ((char)wParam == '.')
		{
			//if (Gfx->gSpotLight.coneWidth_R.x < 1.0f)
			//	Gfx->gSpotLight.coneWidth_R.x += 0.01f;
			//	//Gfx->SpotLightWidth += 0.01f;
			//std::ostringstream oss;
			//oss << Gfx->gSpotLight.coneWidth_R.x << std::endl;
			//OutputDebugString(oss.str().c_str());
			//oss.clear();
		}
	}
	}
	if (lookat)
	{
		Gfx->At = XMLoadFloat4A(&Gfx->gPointLights[0].pos);
		//Gfx->At.m128_f32[0] *= -10.0f;
		//Gfx->At.m128_f32[1] *= -10.0f;
		//Gfx->At.m128_f32[2] *= -10.0f;
		//*views[CamInUse] = XMMatrixLookAtLH(Gfx->Eye, Gfx->At, Gfx->Up);
		*views[CamInUse] = XMMatrixLookAtLH(cams[CamInUse]->r[3], Gfx->At, Gfx->Up);
		*cams[CamInUse] = XMMatrixInverse(nullptr, *views[CamInUse]);
	}
	if (Gfx)
	{
		*views[CamInUse] = XMMatrixInverse(nullptr, *cams[CamInUse]);
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
	ClientRect.left = labs((long)(centerScreen.x - (_width * 0.5f)));
	ClientRect.right = (long)(ClientRect.left + _width);
	ClientRect.top = labs((long)(centerScreen.y - (_height * 0.5f)));
	ClientRect.bottom = (long)(ClientRect.top + static_cast<long>(_height));
	AdjustWindowRect(&ClientRect, WS_CAPTION | WS_SIZEBOX| WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE);
	HWND hWnd = CreateWindowEx(
		0, _WndClass->lpszClassName, _title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,
		ClientRect.left, ClientRect.top, ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top,
		nullptr, nullptr, _WndClass->hInstance, nullptr);

	if (hWnd == nullptr)
		ToolBox::ThrowErrorMsg("CTERROR: BuildWindow(int,int,string,wndclassex)\nhWnd was null");

	// Draw hWnd.
	ShowWindow(hWnd, SW_SHOW);
	return hWnd;
}

//void ModelDraw_Switch(int modelToDraw)
//{
//	if (modelToDraw == 0)
//	{
//		Gfx->LoadMesh("Cube.fbx", 50.0f, Gfx->gppMesh, 0);
//		OutputDebugString("CUBE.FBX");
//	}
//	else if (modelToDraw == 1)
//	{
//		Gfx->LoadMesh("Cube.fbx", 100.0f, Gfx->gppMesh, 1);
//		OutputDebugString("CUBE.FBX BUT BIGGER");
//	}
//	else if (modelToDraw == 2)
//	{
//		Gfx->LoadMesh("Cube.fbx", 10.0f, Gfx->gppMesh, 2);
//		OutputDebugString("CUBE.FBX BUT smaller");
//	}
//	else
//	{
//		if (modelToDraw < 0) currModel = MODEL_COUNT;
//		else if (modelToDraw > MODEL_COUNT) currModel = 0;
//	}
//}