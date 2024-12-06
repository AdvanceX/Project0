//-------------------- 全局变量 --------------------//

HWND gMyWindow;
SPFPN gMyWndW;
SPFPN gMyWndH;
GEWnd gMyWndEx;
GECmd gMyGfxCmd;
GECmd gMyHpcCmd;
GECmd gMyCpyCmd;

//----------------------------------------//


//-------------------- 模块初始化 --------------------//

$VOID mgInitSystem(){
	//系统模块

	GE3d::Initialize();
	GE2d::Initialize();
	GEGui::Initialize(gMyWindow);
	GEInp::Initialize(gMyWindow);
	CWidget::InitContext(GE2d::lpDWFactory, GE2d::lpD2dDevCtx, GE2d::lpPureBrush);

	//命令工具

	gMyGfxCmd.Initialize(2, D3D12_COMMAND_LIST_TYPE_DIRECT);
	gMyHpcCmd.Initialize(2, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	gMyCpyCmd.Initialize(2, D3D12_COMMAND_LIST_TYPE_COPY);

	//窗口

	GEWnd::DESC WndDesc;
	WndDesc.Width = (USINT)gMyWndW;
	WndDesc.Height = (USINT)gMyWndH;
	WndDesc.hWindow = gMyWindow;
	WndDesc.MaxFrame = gMyGfxCmd.MaxFrame;
	WndDesc.bEnableD3d11 = B_True;
	WndDesc.bEnableIdBuf = B_True;

	gMyWndEx.Initialize(WndDesc);
}
$VOID mgFreeSystem(){
	gMyWndEx.Finalize();
	gMyGfxCmd.Finalize();
	gMyHpcCmd.Finalize();
	gMyCpyCmd.Finalize();

	GE2d::Finalize();
	GE3d::Finalize();
	GEGui::Finalize();
	GEInp::Finalize();
};

//----------------------------------------//