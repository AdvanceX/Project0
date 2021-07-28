#include "game-engine.h"


HWND hWindow;
UINT WndWidth;
UINT WndHeight;

CModel Model;
CCamera Camera;
CShader Shader;
CGfxPso GfxPso;
CRenderer Render;
CLitLibrary LitLib;
CMtlLibrary MtlLib;

ge::Window geWindow;
ge::WndDesc geWndDesc;

MATRIX matWorld = gIdentMat;
MATRIX matTexAnim = gIdentMat;
SPFP BGColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };


VOID UpdateCamera(){
	ge::UpdateKeyboardState();

	if(ge::KBState.W) Camera.Fly(1.0f);
	if(ge::KBState.S) Camera.Fly(-1.0f);
	if(ge::KBState.D) Camera.Strafe(1.0f);
	if(ge::KBState.A) Camera.Strafe(-1.0f);

	ge::UpdateMouseState();

	if(ge::MouseState.x > 0) Camera.HorizontalYaw(0.02f);
	else if(ge::MouseState.x < 0) Camera.HorizontalYaw(-0.02f);
	if(ge::MouseState.y > 0) Camera.Pitch(0.02f);
	else if(ge::MouseState.y < 0) Camera.Pitch(-0.02f);
	if(ge::WheelOffset > 0) Camera.Walk(5.0f);
	else if(ge::WheelOffset < 0) Camera.Walk(-5.0f);

	Camera.Update();
}

VOID GameInit(){
	ge::InitDirect3D();
	ge::InitDirect2D();
	ge::InitDebug((SPFP)WndWidth);
	ge::SetInputWindow(hWindow);
	ge::SetMouseMode(DXTKMouse::MODE_RELATIVE);
	geWindow.Initialize(geWndDesc);

	//资源

	DXTKUploadBatch ULBatch(ge::lpD3DDevice);
	ULBatch.Begin();

	MtlLib.AddTexturesFromDirectory(ge::lpD3DDevice, TXT("./texture/"), ULBatch);
	MtlLib.AddMaterialsFromFile(TXT("./model/TexBox.mdl"));
	MtlLib.SubmitResources(ge::lpD3DDevice, ULBatch);

	Model.Initialize(ge::lpD3DDevice, TXT("./model/TexBox.mdl"), ULBatch);
	Model.LinkMaterials(MtlLib);

	auto Thread = ULBatch.End(ge::lpCmdQueue);

	//渲染管线

	CMesh *pMesh = Model.FirstMesh;
	DWRD Style = pMesh->VtxFormat;
	if(pMesh->MtlCount > 0) Style |= ASSET_MATERIAL;

	Shader.Initialize(ge::lpD3DDevice, Style);
	GfxPso.Initialize(ge::lpD3DDevice, Shader, geWindow);
	Render.Initialize(ge::lpD3DDevice, 2, 4);

	//灯光

	LIGHT Light;
	Light.Intensity = { 1.0f, 1.0f, 1.0f };
	Light.Direction = { 1.0f, -1.0f, 1.0f };
	VECTOR3 Ambient = { 0.3f, 0.3f, 0.3f };

	LitLib.Initialize(ge::lpD3DDevice, 2);
	LitLib.UpdateDirectionalLight(Light);

	Render.SetAmbientLight(Ambient);
	Render.SetLightLibrary(LitLib);
	Render.SetMaterialLibrary(MtlLib);

	//相机

	VECTOR3 Coord = { 0.0f, 0.0f, -100.0f };
	SPFP Aspect = (SPFP)WndWidth / (SPFP)WndHeight;
	Camera.Initialize(Coord, DEG_TO_RAD(90.0f), Aspect, 0.1f, 1000.0f);

	//等待

	Thread.wait();
}
VOID GameLoop(){
	UpdateCamera();

	ge::BeginCommand();

	Render.SetCamera(Camera);
	Render.SetTransformation(matWorld, matTexAnim);
	Render.ClearWindow(ge::lpCmdList, geWindow, BGColor);
	Render.SetPSO(ge::lpCmdList, GfxPso);
	Render.BeginDraw(ge::lpCmdList, geWindow);
	Render.DrawMeshs(ge::lpCmdList, Model.FirstMesh);
	Render.EndDraw(ge::lpCmdList, geWindow);

	ge::SubmitCommand();
	ge::WaitForGPU();

	geWindow.PresentBuffer();
}
VOID GameShut(){
	ge::WaitForGPU();
	ge::FreeDebug();
	ge::FreeDirect2D();
	ge::FreeDirect3D();
	geWindow.Finalize();

	Model.Finalize();
	Shader.Finalize();
	GfxPso.Finalize();
	Render.Finalize();
	LitLib.Finalize();
	MtlLib.Finalize();
}

LRESULT CALLBACK WinProc(HWND hWindow, UINT MsgCode, WPARAM Param0, LPARAM Param1){
	ge::ProcMessage(MsgCode, Param0, Param1);

	switch(MsgCode){
		case WM_KEYDOWN:{
			if(Param0 == VK_ESCAPE)
				DestroyWindow(hWindow);
			return 0;
		}
		case WM_DESTROY:{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWindow, MsgCode, Param0, Param1);
}
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, INT fCmdShow){
	//获取屏幕尺寸

	WndWidth = GetSystemMetrics(SM_CXSCREEN);
	WndHeight = GetSystemMetrics(SM_CYSCREEN);

	//创建窗口

	WNDCLASSEX WndClass;
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WinProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = TXT("Main");
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&WndClass);

	hWindow = CreateWindowEx(0x0,
		TXT("Main"), TXT("Game Demo"),
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WndWidth, WndHeight,
		NULL, NULL, hInstance, NULL);

	//设置初始信息

	ge::hInstance = hInstance;
	geWndDesc.bWindowed = TRUE;
	geWndDesc.bEnable2D = TRUE;
	geWndDesc.hWindow = hWindow;	
	geWndDesc.WndWidth = WndWidth;
	geWndDesc.WndHeight = WndHeight;
	geWndDesc.BuffCount = 2;

	//程序运行与结束

	GameInit();
	while(TRUE){
		if(PeekMessage(&ge::Message, NULL, 0, 0, PM_REMOVE)){
			if(ge::Message.message == WM_QUIT) break;
			TranslateMessage(&ge::Message);
			DispatchMessage(&ge::Message);
		}
		GameLoop();
	}
	GameShut();

	return 0;
}