#include "../engine/game_engine.h"
#include "utility.hpp"
#include "struct.hpp"
#include "system.hpp"
#include "asset.hpp"
#include "portal.hpp"
#include "editor.hpp"
#include "master.hpp"
#include "gui.hpp"


LRESULT _CBACK WinProc(HWND hWindow, USINT MsgCode, WPARAM Param0, LPARAM Param1){
	if(((MsgCode >= WM_KEYFIRST) && (MsgCode <= WM_KEYLAST)) ||
		((MsgCode >= WM_MOUSEFIRST) && (MsgCode <= WM_MOUSELAST)))
	{
		if(gMyAppPage->OnMessage(MsgCode, Param0, Param1))
			return 0;
		if(ImguiWinMsgProc(hWindow, MsgCode, Param0, Param1))
			return 0;
		if(gMyGuiRoot->OnMessage(MsgCode, Param0, Param1))
			return 0;
	}

	switch(MsgCode){
		case WM_CLOSE:{
			PostQuitMessage(0);
			return 0;
		}
		case WM_KEYDOWN:{
			if((Param0 == VK_F12) && ConfirmMessage(L"Quit game?"))
				PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWindow, MsgCode, Param0, Param1);
}
INT32 _SYSC WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PSTR psCmdLine, LONG32 ShowCmd){
	//创建主窗口

	GEPrc::hInstance = hInstance;
	MSG &rMessage = GEPrc::LastMsg;

	USINT WndWidth = GetSystemMetrics(SM_CXSCREEN);
	USINT WndHeight = GetSystemMetrics(SM_CYSCREEN);

	HCURSOR hCursor = LoadCursorFromFileW(L".\\icon\\Arrow.cur");
	HICON hIcon = (HICON)LoadImageW(P_Null, L".\\icon\\Globe.ico",
		IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

	WNDCLASSEXW WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.cbSize = sizeof(WNDCLASSEXW);
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hIcon = hIcon;
	WndClass.hIconSm = hIcon;
	WndClass.hCursor = hCursor;
	WndClass.hInstance = hInstance;
	WndClass.hbrBackground = P_Null;
	WndClass.lpfnWndProc = WinProc;
	WndClass.lpszMenuName = P_Null;
	WndClass.lpszClassName = L"MainWnd";

	RegisterClassEx(&WndClass);

	gMyWndW = (SPFPN)WndWidth;
	gMyWndH = (SPFPN)WndHeight;
	gMyWindow = CreateWindowEx(
		0L, L"MainWnd", L"MinStrategy", WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, WndWidth, WndHeight,
		P_Null, P_Null, hInstance, P_Null);

	//程序运行与结束

	mgInitSystem();
	mgInitAsset();
	mgInitGui();

	while(B_True){
		if(PeekMessage(&rMessage, P_Null, 0, 0, PM_REMOVE)){
			if(rMessage.message == WM_QUIT) break;
			TranslateMessage(&rMessage);
			DispatchMessage(&rMessage);
		}

		GEInp::Update(0.f);
		gMyAppPage->Update();
		gMyAppPage->Paint();
	}

	gMyGfxCmd.WaitForGpu();
	mgFreeGui();
	mgFreeAsset();
	mgFreeSystem();
	DestroyWindow(gMyWindow);

	return (INT32)rMessage.wParam;
}