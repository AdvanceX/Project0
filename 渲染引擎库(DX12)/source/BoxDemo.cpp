#include "game-engine.h"


CModel Model;
CCamera Camera;
CShader Shader;
CGfxPso GfxPso;
CRenderer Render;
CLitLibrary LitLib;
CMtlLibrary MtlLib;

GEWnd Window;
GEWnd::DESC WndDesc;

MATRIX matTrans = gIdentMat;
SPFP BGColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };


VOID UpdateCamera(){
	GEInput::UpdateKeyboardState();
	GEInput::UpdateMouseState();

	if(GEInput::KBState.W) Camera.Fly(1.0f);
	if(GEInput::KBState.S) Camera.Fly(-1.0f);
	if(GEInput::KBState.D) Camera.Strafe(1.0f);
	if(GEInput::KBState.A) Camera.Strafe(-1.0f);

	if(GEInput::MouseState.x > 0) Camera.HorizontalYaw(0.02f);
	else if(GEInput::MouseState.x < 0) Camera.HorizontalYaw(-0.02f);
	if(GEInput::MouseState.y > 0) Camera.Pitch(0.02f);
	else if(GEInput::MouseState.y < 0) Camera.Pitch(-0.02f);
	if(GEInput::WheelDelta > 0) Camera.Walk(5.0f);
	else if(GEInput::WheelDelta < 0) Camera.Walk(-5.0f);

	Camera.Update();
}

VOID GameInit(){
	GE3D::Initialize(2);
	GE2D::Initialize();
	GEInput::Initialize(WndDesc.Handle, DXTKMouse::MODE_RELATIVE);
	GEDebug::Initialize((SPFP)WndDesc.Width);

	//窗口

	WndDesc.bWindowed = TRUE;
	WndDesc.RefreshRateD = 1;
	WndDesc.RefreshRateN = 60;
	WndDesc.BuffCount = GE3D::MaxFrame;
	Window.Initialize(WndDesc);

	//资源

	DXTKUploader Uploader(GE3D::lpD3DDevice);
	Uploader.Begin();

	MtlLib.AddTexturesFromDirectory(GE3D::lpD3DDevice, TXT("./texture/"), Uploader);
	MtlLib.AddMaterialsFromFile(TXT("./model/Demo.mdl"));
	MtlLib.SubmitResources(GE3D::lpD3DDevice, Uploader);

	Model.Initialize(GE3D::lpD3DDevice, TXT("./model/Demo.mdl"), Uploader);
	Model.LinkMaterials(MtlLib);

	auto Thread = Uploader.End(GE3D::lpCmdQueue);

	//渲染管线

	CMesh *pMesh = Model.FirstMesh;
	DWRD Style = pMesh->VtxFormat;
	if(pMesh->MtlCount > 0) Style |= ASSET_MATERIAL;

	Shader.Initialize(GE3D::lpD3DDevice, Style);
	GfxPso.Initialize(GE3D::lpD3DDevice, Shader, Window);
	Render.Initialize(GE3D::lpD3DDevice, GE3D::MaxFrame, 4);

	//灯光

	LIGHT Light;
	Light.Intensity = { 1.0f, 1.0f, 1.0f };
	Light.Direction = { 1.0f, -1.0f, 1.0f };
	VECTOR3 Ambient = { 0.3f, 0.3f, 0.3f };

	LitLib.Initialize(GE3D::lpD3DDevice, 2);
	LitLib.UpdateDirectionalLight(Light);

	Render.SetAmbientLight(Ambient);
	Render.SetLightLibrary(LitLib);
	Render.SetMaterialLibrary(MtlLib);

	//相机

	VECTOR3 Coord = { 0.0f, 0.0f, -100.0f };
	SPFP Aspect = (SPFP)WndDesc.Width / (SPFP)WndDesc.Height;
	Camera.Initialize(Coord, DEG_TO_RAD(90.0f), Aspect, 0.1f, 1000.0f);

	//等待

	Thread.wait();
}
VOID GameLoop(){
	UpdateCamera();

	GE3D::BeginFrame();

	Render.SetFrame(GE3D::CurFrame);
	Render.SetCamera(Camera);
	Render.SetTransformation(matTrans, matTrans);
	Render.ClearWindow(GE3D::lpCmdList, Window, BGColor);
	Render.SetPSO(GE3D::lpCmdList, GfxPso);
	Render.BeginDraw(GE3D::lpCmdList, Window);
	Render.DrawMeshs(GE3D::lpCmdList, Model.FirstMesh);
	Render.EndDraw(GE3D::lpCmdList, Window);

	GE3D::EndFrame();

	Window.PresentBuffer();
}
VOID GameShut(){
	GE3D::WaitForGPU();
	GE3D::Finalize();
	GE2D::Finalize();

	Model.Finalize();
	Window.Finalize();
	Shader.Finalize();
	GfxPso.Finalize();
	Render.Finalize();
	LitLib.Finalize();
	MtlLib.Finalize();
}

LRESULT CALLBACK WinProc(HWND hWindow, UINT MsgCode, WPARAM Param0, LPARAM Param1){
	GEInput::ProcMessage(MsgCode, Param0, Param1);

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
	//获取基本信息

	GEApp::hInstance = hInstance;
	WndDesc.Width = GetSystemMetrics(SM_CXSCREEN);
	WndDesc.Height = GetSystemMetrics(SM_CYSCREEN);

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

	WndDesc.Handle = CreateWindowEx(0x0,
		TXT("Main"), TXT("Game Demo"),
		WS_POPUP | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WndDesc.Width, WndDesc.Height,
		NULL, NULL, hInstance, NULL);

	//程序运行与结束

	GameInit();
	while(TRUE){
		if(PeekMessage(&GEApp::Message, NULL, 0, 0, PM_REMOVE)){
			if(GEApp::Message.message == WM_QUIT) break;
			TranslateMessage(&GEApp::Message);
			DispatchMessage(&GEApp::Message);
		}
		GameLoop();
	}
	GameShut();

	return 0;
}