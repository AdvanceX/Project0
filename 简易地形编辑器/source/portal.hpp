class MGPortal: public IAppBase{
	_open $VOID Initialize(){}
	_open $VOID Finalize(){}
	_open $VOID Update(){}
	_open $VOID Paint(){
		ID2DBitmap *lpBgImage = mgGetImage("Pic-Home");
		RECTF rcWindow = { 0.f, 0.f, gMyWndW, gMyWndH };

		GE2d::BeginDraw(gMyWndEx.pD3d11Buff, gMyWndEx.pD2dTarget);
		DrawBitmap(GE2d::lpD2dDevCtx, lpBgImage, rcWindow, BFM_Cover);
		gMyGuiRoot->OnMessage(WM_PAINT, 0, 0);
		GE2d::EndDraw(gMyWndEx.pD3d11Buff);
		gMyWndEx.PresentBuffer();
	}
	_open IBOOL OnMessage(DWORD, WPARAM, LPARAM){
		return B_False;
	}
};