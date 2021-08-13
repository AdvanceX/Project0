//--------------------引擎基础--------------------//

//进程基础
class GEApp{
	open static MSG Message;
	open static HINSTANCE hInstance;
};

//3D基础
class GE3D{
	open static UINT CurFrame;
	open static UINT MaxFrame;
	open static UINT cbRTView;
	open static UINT cbDSView;
	open static UINT cbCSUView;
	hide static ULNG MaxFencePoint;
	hide static ULNG *arrFencePoint;
	hide static HANDLE hFenceEvent;
	hide static ID3DFence *lpFence;
	open static ID3DDevice *lpD3DDevice;
	open static IDXGIFactory6 *lpDXGIFactory;
	open static ID3DGfxCmdList *lpCmdList;
	open static ID3DCmdQueue *lpCmdQueue;
	open static ID3DCmdAlloc *CurCmdAlloc;
	hide static ID3DCmdAlloc **arrCmdAlloc;

	open static VOID Initialize(UINT MaxFrame){
		CreateDXGIFactory1(COM_ARGS(&GE3D::lpDXGIFactory));

		GE3D::CurFrame = 0;
		GE3D::MaxFrame = MaxFrame;

		GE3D::InitD3DDevice();
		GE3D::InitCommandObjects();
		GE3D::InitViewHandleIncrementSizes();
	}
	hide static VOID InitD3DDevice(){
		HRESULT Result = D3D12CreateDevice(
			NULL, D3D_FEATURE_LEVEL_11_0, COM_ARGS(&GE3D::lpD3DDevice));

		if(FAILED(Result)){
			IDXGIAdapter4 *lpAdapter;
			GE3D::lpDXGIFactory->EnumWarpAdapter(COM_ARGS(&lpAdapter));
			D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, COM_ARGS(&GE3D::lpD3DDevice));
			lpAdapter->Release();
		}
	}
	hide static VOID InitCommandObjects(){
		//创建围栏

		GE3D::MaxFencePoint = 0;
		GE3D::arrFencePoint = new ULNG[GE3D::MaxFrame]{ 0 };
		GE3D::hFenceEvent = CreateEventEx(NULL, NULL, 0x0, EVENT_ALL_ACCESS);
		GE3D::lpD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, COM_ARGS(&GE3D::lpFence));

		//创建命令队列

		D3DCmdQueueDesc CQDesc;

		CQDesc.NodeMask = 0x0;
		CQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		GE3D::lpD3DDevice->CreateCommandQueue(&CQDesc, COM_ARGS(&GE3D::lpCmdQueue));

		//创建命令分配器

		GE3D::arrCmdAlloc = new ID3DCmdAlloc*[GE3D::MaxFrame];
		for(UINT i = 0; i < GE3D::MaxFrame; ++i){
			GE3D::lpD3DDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, COM_ARGS(&GE3D::arrCmdAlloc[i]));
		}

		GE3D::CurCmdAlloc = GE3D::arrCmdAlloc[0];

		//创建命令列表

		GE3D::lpD3DDevice->CreateCommandList1(0x0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_LIST_FLAG_NONE, COM_ARGS(&GE3D::lpCmdList));
	}
	hide static VOID InitViewHandleIncrementSizes(){
		GE3D::cbRTView = GE3D::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		GE3D::cbDSView = GE3D::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		GE3D::cbCSUView = GE3D::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	open static VOID Finalize(){
		for(UINT i = 0; i < GE3D::MaxFrame; ++i)
			RELEASE_REF(GE3D::arrCmdAlloc[i]);

		RESET_ARR(GE3D::arrCmdAlloc);
		RESET_ARR(GE3D::arrFencePoint);

		RESET_REF(GE3D::lpFence);
		RESET_REF(GE3D::lpCmdList);
		RESET_REF(GE3D::lpCmdQueue);
		RESET_REF(GE3D::lpD3DDevice);
		RESET_REF(GE3D::lpDXGIFactory);

		CloseHandle(GE3D::hFenceEvent);

		GE3D::MaxFrame = 0;
		GE3D::hFenceEvent = NULL;
	}
	open static VOID EndFrame(){
		GE3D::SubmitCommand();

		GE3D::arrFencePoint[GE3D::CurFrame] = ++GE3D::MaxFencePoint;
		GE3D::lpCmdQueue->Signal(GE3D::lpFence, GE3D::MaxFencePoint);

		GE3D::CurFrame = (GE3D::CurFrame + 1) % GE3D::MaxFrame;
		GE3D::CurCmdAlloc = GE3D::arrCmdAlloc[GE3D::CurFrame];
	}
	open static VOID BeginFrame(){
		ULNG CompletedValue = GE3D::lpFence->GetCompletedValue();
		ULNG FencePoint = GE3D::arrFencePoint[GE3D::CurFrame];

		if(CompletedValue < FencePoint){
			GE3D::lpFence->SetEventOnCompletion(FencePoint, GE3D::hFenceEvent);
			WaitForSingleObject(GE3D::hFenceEvent, INFINITE);
		}

		GE3D::BeginCommand();
	}
	open static VOID WaitForGPU(){
		GE3D::lpCmdQueue->Signal(GE3D::lpFence, ++GE3D::MaxFencePoint);

		if(GE3D::lpFence->GetCompletedValue() < GE3D::MaxFencePoint){
			GE3D::lpFence->SetEventOnCompletion(GE3D::MaxFencePoint, GE3D::hFenceEvent);
			WaitForSingleObject(GE3D::hFenceEvent, INFINITE);
		}
	}
	open static VOID BeginCommand(){
		GE3D::CurCmdAlloc->Reset();
		GE3D::lpCmdList->Reset(GE3D::CurCmdAlloc, NULL);
	}
	open static VOID SubmitCommand(){
		GE3D::lpCmdList->Close();
		GE3D::lpCmdQueue->ExecuteCommandLists(1, (ID3DCmdList**)&GE3D::lpCmdList);
	}
};

//2D基础
class GE2D{
	open static ID2DDevCtx *lpD2DDC;
	open static ID2DDevice *lpD2DDevice;
	open static ID2DFactory *lpD2DFactory;
	open static ID3D11DevCtx *lpD3D11DC;
	open static ID3D11Device *lpD3D11Device;
	open static ID3D11On12Dev *lpD3D11On12Dev;
	open static IDWFactory *lpDWFactory;
	open static IWICFactory *lpWICFactory;

	open static VOID Initialize(){
		//创建D3D11设备

		D3D11On12CreateDevice(GE3D::lpD3DDevice, D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, 0,
			(IUnknown**)&GE3D::lpCmdQueue, 1, 0x0, &GE2D::lpD3D11Device, &GE2D::lpD3D11DC, NULL);

		GE2D::lpD3D11Device->QueryInterface(&GE2D::lpD3D11On12Dev);

		//创建D2D设备

		IDXGIDevice *lpDXGIDevice;

		GE2D::lpD3D11On12Dev->QueryInterface(&lpDXGIDevice);
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, COM_ARGS(&GE2D::lpD2DFactory));
		GE2D::lpD2DFactory->CreateDevice(lpDXGIDevice, &GE2D::lpD2DDevice);
		GE2D::lpD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &GE2D::lpD2DDC);

		lpDXGIDevice->Release();

		//创建DWrite工厂

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			clsidof(IDWriteFactory), (IUnknown**)&GE2D::lpDWFactory);

		//创建WIC工厂

		CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, COM_ARGS(&GE2D::lpWICFactory));
	}
	open static VOID Finalize(){
		RESET_REF(GE2D::lpD3D11DC);
		RESET_REF(GE2D::lpD3D11Device);
		RESET_REF(GE2D::lpD3D11On12Dev);
		RESET_REF(GE2D::lpD2DDC);
		RESET_REF(GE2D::lpD2DDevice);
		RESET_REF(GE2D::lpD2DFactory);
		RESET_REF(GE2D::lpDWFactory);
		RESET_REF(GE2D::lpWICFactory);
	}
	open static VOID BeginDraw(ID3D11Resource* lpBuffWrap, ID2DBitmap* lpCanvas){
		GE2D::lpD3D11On12Dev->AcquireWrappedResources(&lpBuffWrap, 1);
		GE2D::lpD2DDC->SetTarget(lpCanvas);
		GE2D::lpD2DDC->BeginDraw();
	}
	open static VOID EndDraw(ID3D11Resource* lpBuffWrap){
		GE2D::lpD2DDC->EndDraw();
		GE2D::lpD3D11On12Dev->ReleaseWrappedResources(&lpBuffWrap, 1);
		GE2D::lpD3D11DC->Flush();
	}
};

//窗口基础
class GEWnd{
	//窗口描述
	open struct DESC{
		HWND Handle;
		UINT Width;
		UINT Height;
		UINT BuffCount = 2;
		UINT MSAACount = 1;
		UINT MSAAQuality = 0;
		UINT RefreshRateD = 1;
		UINT RefreshRateN = 60;
		BOOL bWindowed = FALSE;
		DXGIFormat SCBFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGIFormat DSBFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};

	///窗口句柄
	open HWND Handle; //窗口句柄
	///窗口尺寸
	open UINT Width;  //窗口宽
	open UINT Height; //窗口高
	///缓冲层数
	hide UINT BuffIndex; //缓存区索引
	open UINT BuffCount; //缓存区数量
	///视口/裁剪区
	open D3DRect rcScissor;    //裁剪区
	open D3DViewport Viewport; //视口
	///描述符
	open HD3DVIEW hDSView; //DSV句柄
	open HD3DVIEW hRTView; //RTV句柄
	hide ID3DViewHeap *lpDSVHeap; //DSV堆
	hide ID3DViewHeap *lpRTVHeap; //RTV堆
	///3D缓存区
	open ID3DResource *lpDSBuffer;   //深度模板缓存区
	open ID3DResource *CurBackBuff;  //当前后备缓存区
	hide ID3DResource **arrBackBuff; //后备缓存区数组
	///2D缓存区
	open ID2DBitmap *CurCanvas2D;      //当前2D画布
	hide ID2DBitmap **arrCanvas2D;     //2D画布数组
	open ID3D11Resource *CurBuffWrap;  //当前缓存区包裹
	hide ID3D11Resource **arrBuffWrap; //缓存区包裹数组
	///交换链
	open IDXGISwapChain4 *lpSwapChain; //交换链

	open ~GEWnd(){
		self.Finalize();
	}
	open GEWnd(){
		ZeroMemory(this, sizeof(*this));
	}
	open GEWnd(DESC &Desc){
		self.Initialize(Desc);
	}

	open VOID Initialize(const DESC &Desc){
		self.Width = Desc.Width;
		self.Height = Desc.Height;
		self.Handle = Desc.Handle;
		self.BuffCount = Desc.BuffCount;
		self.BuffIndex = 0;

		self.InitViewportAndScissorRect();
		self.InitDescriptorHeaps();
		self.InitSwapChain(Desc);
		self.InitDepthStencilBuffer(Desc);
		self.Init2DCanvases();
		self.UpdatePage();
	}
	hide VOID InitSwapChain(const DESC &WndDesc){
		//创建交换链

		DXGISwapChainDesc SCDesc;

		SCDesc.BufferDesc.Width = self.Width;
		SCDesc.BufferDesc.Height = self.Height;
		SCDesc.BufferDesc.Format = WndDesc.SCBFormat;
		SCDesc.BufferDesc.RefreshRate.Numerator = WndDesc.RefreshRateN;
		SCDesc.BufferDesc.RefreshRate.Denominator = WndDesc.RefreshRateD;
		SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		SCDesc.SampleDesc.Count = WndDesc.MSAACount;
		SCDesc.SampleDesc.Quality = WndDesc.MSAAQuality;

		SCDesc.Windowed = WndDesc.bWindowed;
		SCDesc.OutputWindow = self.Handle;
		SCDesc.BufferCount = self.BuffCount;
		SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		GE3D::lpDXGIFactory->CreateSwapChain(GE3D::lpCmdQueue,
			&SCDesc, (IDXGISwapChain**)&self.lpSwapChain);

		//创建描述符

		HD3DVIEW hRTView = (HD3DVIEW)self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
		self.arrBackBuff = new ID3DResource*[self.BuffCount];

		for(UINT i = 0; i < self.BuffCount; ++i){
			self.lpSwapChain->GetBuffer(i, COM_ARGS(&self.arrBackBuff[i]));
			GE3D::lpD3DDevice->CreateRenderTargetView(self.arrBackBuff[i], NULL, hRTView);
			hRTView.Offset(GE3D::cbRTView);
		}
	}
	hide VOID InitDepthStencilBuffer(const DESC &WndDesc){
		//创建缓存

		D3DResDesc BuffDesc;
		D3DFillValue FillValue;

		BuffDesc.Alignment = 0;
		BuffDesc.MipLevels = 1;
		BuffDesc.DepthOrArraySize = 1;
		BuffDesc.Width = self.Width;
		BuffDesc.Height = self.Height;
		BuffDesc.SampleDesc.Count = WndDesc.MSAACount;
		BuffDesc.SampleDesc.Quality = WndDesc.MSAAQuality;
		BuffDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		BuffDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		BuffDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		BuffDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		FillValue.Format = WndDesc.DSBFormat;
		FillValue.DepthStencil.Depth = 1.0f;
		FillValue.DepthStencil.Stencil = 0;

		GE3D::lpD3DDevice->CreateCommittedResource(
			&D3DHeapProps(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &BuffDesc,
			D3D12_RESOURCE_STATE_COMMON, &FillValue,
			COM_ARGS(&self.lpDSBuffer));

		//创建视图

		D3DDsvDesc ViewDesc;

		ViewDesc.Format = WndDesc.DSBFormat;
		ViewDesc.Flags = D3D12_DSV_FLAG_NONE;
		ViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		ViewDesc.Texture2D.MipSlice = 0;

		GE3D::lpD3DDevice->CreateDepthStencilView(self.lpDSBuffer, &ViewDesc, self.hDSView);
	}
	hide VOID InitViewportAndScissorRect(){
		//设置视口

		self.Viewport.TopLeftX = 0.0f;
		self.Viewport.TopLeftY = 0.0f;
		self.Viewport.MinDepth = 0.0f;
		self.Viewport.MaxDepth = 1.0f;
		self.Viewport.Width = (SPFP)self.Width;
		self.Viewport.Height = (SPFP)self.Height;

		//设置裁剪区

		self.rcScissor.top = 0;
		self.rcScissor.left = 0;
		self.rcScissor.right = self.Width;
		self.rcScissor.bottom = self.Height;
	}
	hide VOID InitDescriptorHeaps(){
		//创建描述符堆

		D3DViewHeapDesc HeapDesc;

		HeapDesc.NodeMask = 0x0;
		HeapDesc.NumDescriptors = self.BuffCount;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		GE3D::lpD3DDevice->CreateDescriptorHeap(
			&HeapDesc, COM_ARGS(&self.lpRTVHeap));

		HeapDesc.NodeMask = 0x0;
		HeapDesc.NumDescriptors = 1;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		GE3D::lpD3DDevice->CreateDescriptorHeap(
			&HeapDesc, COM_ARGS(&self.lpDSVHeap));

		//获取视图句柄

		self.hRTView = self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
		self.hDSView = self.lpDSVHeap->GetCPUDescriptorHandleForHeapStart();
	}
	hide VOID Init2DCanvases(){
		SPFP DpiX, DpiY;
		D2DBmpProps BmpProps;
		D3D11ResFlags BuffFlags;
		IDXGISurface *lpSurface;

		GE2D::lpD2DFactory->GetDesktopDpi(&DpiX, &DpiY);

		BmpProps = d2d::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			d2d::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			DpiX, DpiY
		);

		ZeroMemory(&BuffFlags, sizeof(BuffFlags));
		BuffFlags.BindFlags = D3D11_BIND_RENDER_TARGET;

		self.arrCanvas2D = new ID2DBitmap*[self.BuffCount];
		self.arrBuffWrap = new ID3D11Resource*[self.BuffCount];

		for(UINT i = 0; i < self.BuffCount; ++i){
			GE2D::lpD3D11On12Dev->CreateWrappedResource(self.arrBackBuff[i], &BuffFlags,
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
				COM_ARGS(&self.arrBuffWrap[i]));

			self.arrBuffWrap[i]->QueryInterface(&lpSurface);

			GE2D::lpD2DDC->CreateBitmapFromDxgiSurface(
				lpSurface, &BmpProps, &self.arrCanvas2D[i]);

			lpSurface->Release();
		}
	}
	open VOID Finalize(){
		for(UINT i = 0; i < self.BuffCount; ++i){
			RELEASE_REF(self.arrBackBuff[i]);
			RELEASE_REF(self.arrCanvas2D[i]);
			RELEASE_REF(self.arrBuffWrap[i]);
		}

		DELETE_ARR(self.arrBackBuff);
		DELETE_ARR(self.arrBuffWrap);
		DELETE_ARR(self.arrCanvas2D);

		RELEASE_REF(self.lpRTVHeap);
		RELEASE_REF(self.lpDSVHeap);
		RELEASE_REF(self.lpDSBuffer);
		RELEASE_REF(self.lpSwapChain);

		ZeroMemory(this, sizeof(*this));
	}
	hide VOID UpdatePage(){
		self.hRTView = self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
		self.hRTView.Offset(GE3D::cbRTView * self.BuffIndex);

		self.CurBackBuff = self.arrBackBuff[self.BuffIndex];
		self.CurBuffWrap = self.arrBuffWrap[self.BuffIndex];
		self.CurCanvas2D = self.arrCanvas2D[self.BuffIndex];
	}
	open VOID PresentBuffer(){
		self.lpSwapChain->Present(0, 0x0);
		self.BuffIndex = (self.BuffIndex + 1) % self.BuffCount;
		self.UpdatePage();
	}
	open VOID ResizeWindow(const DESC &Desc){
		//保存新设置

		self.Width = Desc.Width;
		self.Height = Desc.Height;
		self.BuffCount = Desc.BuffCount;

		//释放旧缓存

		RELEASE_REF(self.lpDSBuffer);
		for(UINT i = 0; i < self.BuffCount; ++i)
			RELEASE_REF(self.arrBackBuff[i]);

		//调整交换链

		self.lpSwapChain->ResizeBuffers(self.BuffCount, self.Width, self.Height,
			Desc.SCBFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		//创建描述符

		HD3DVIEW hRTView = (HD3DVIEW)self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for(UINT i = 0; i < self.BuffCount; ++i){
			GE3D::lpD3DDevice->CreateRenderTargetView(self.arrBackBuff[i], NULL, hRTView);
			hRTView = hRTView.Offset(1, GE3D::cbRTView);
		}

		//重建深度模板缓存和重置视口

		self.InitDepthStencilBuffer(Desc);
		self.InitViewportAndScissorRect();
	}
	open VOID SetViewport(const D3DViewport &Viewport){
		self.Viewport = Viewport;
	}
	open VOID SetScissorRect(const D3DRect &rcScissor){
		self.rcScissor = rcScissor;
	}
};

//视频基础
class GEVideo{
	open static VOID Initialize(){
		CoInitialize(NULL);
		MFStartup(MF_VERSION, MFSTARTUP_LITE);
	}
	open static VOID Finalize(){
		MFShutdown();
	}
};

//音频基础
class GEAudio{
	open static IXAFactory *lpXAFactory;
	open static IXADestVoice *lpDestVoice;

	open static VOID Initialize(){
		XAudio2Create(&GEAudio::lpXAFactory, 0x0, XAUDIO2_DEFAULT_PROCESSOR);
		GEAudio::lpXAFactory->CreateMasteringVoice(&GEAudio::lpDestVoice);
	}
	open static VOID Finalize(){
		RESET_REF(GEAudio::lpXAFactory);
	}
};

//输入基础
class GEInput{
	open static INT4 WheelValue;
	open static INT4 WheelDelta;
	open static DXTKMouse Mouse;
	open static DXTKKeyboard Keyboard;
	open static DXTKKeysState KBState;
	open static DXTKMouseState MouseState;

	open static VOID Initialize(HWND hWindow, DXTKMouse::Mode Mode){
		GEInput::WheelValue = 0;
		GEInput::WheelDelta = 0;

		GEInput::Mouse.SetWindow(hWindow);
		GEInput::Mouse.SetMode(Mode);
	}
	open static VOID ProcMessage(UINT MsgCode, WPARAM Param0, LPARAM Param1){
		DXTKMouse::ProcessMessage(MsgCode, Param0, Param1);
		DXTKKeyboard::ProcessMessage(MsgCode, Param0, Param1);
	}
	open static VOID SetMouseMode(DXTKMouse::Mode Mode){
		GEInput::Mouse.SetMode(Mode);
	}
	open static VOID SetInputWindow(HWND hWindow){
		GEInput::Mouse.SetWindow(hWindow);
	}
	open static VOID UpdateKeyboardState(){
		GEInput::KBState = GEInput::Keyboard.GetState();
	}
	open static VOID UpdateMouseState(){
		GEInput::MouseState = GEInput::Mouse.GetState();
		GEInput::WheelDelta = GEInput::MouseState.scrollWheelValue - GEInput::WheelValue;
		GEInput::WheelValue = GEInput::MouseState.scrollWheelValue;
	}
};

//调试基础
class GEDebug{
	hide static SPFP BoxWidth;
	hide static SPFP FontSize;
	hide static SPFP PrintPos;
	open static IDWTxtFormat *lpTxtFormat;
	open static ID2DSolidColorBrush *lpBrush;
	hide static CLinkList LogList;

	open static VOID Finalize(){
		RESET_REF(GEDebug::lpBrush);
		RESET_REF(GEDebug::lpTxtFormat);
	}
	open static VOID Initialize(SPFP BoxWidth, SPFP FontSize = 20.0f){
		GEDebug::PrintPos = 0.0f;
		GEDebug::FontSize = FontSize;
		GEDebug::BoxWidth = BoxWidth;

		GE2D::lpD2DDC->CreateSolidColorBrush(d2d::ColorF(d2d::ColorF::Green), &GEDebug::lpBrush);
		GE2D::lpDWFactory->CreateTextFormat(L"Verdana", NULL,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			GEDebug::FontSize - 2.0f, L"en-us", &GEDebug::lpTxtFormat);

		GEDebug::lpTxtFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		GEDebug::lpTxtFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		GEDebug::lpTxtFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	open static VOID AddLog(const WCHR* Format, ...){
		VALIST Args;
		WCHR *Text = new WCHR[128];

		VA_START(Args, Format);
		VWSPrint(Text, Format, Args);
		VA_END(Args);

		GEDebug::LogList.Insert((LPTR)Text);
	}
	open static VOID AddBlank(){
		GEDebug::PrintPos += 1.0f;
	}
	open static VOID PrintLogs(){
		D2DRectF Rect = { 10.0f, 0.0f, GEDebug::BoxWidth, 0.0f };
		GE2D::lpD2DDC->SetTransform(d2d::Matrix3x2F::Identity());

		auto Node = GEDebug::LogList.GetFirst();
		while(Node != GEDebug::LogList.GetLast()){
			WCHR *Text = (WCHR*)Node->Data;

			Rect.top = (GEDebug::PrintPos * GEDebug::FontSize) + 10.0f;
			Rect.bottom = Rect.top + GEDebug::FontSize;

			GE2D::lpD2DDC->DrawText(Text, (UINT)StrLenW(Text),
				GEDebug::lpTxtFormat, Rect, GEDebug::lpBrush);

			GEDebug::PrintPos += 1.0f;
			Node = Node->Next;

			delete[] Text;
		}

		GEDebug::LogList.Clear(FALSE);
		GEDebug::PrintPos = 0.0f;
	}
};

//----------------------------------------//


//--------------------进程基础--------------------//

MSG GEApp::Message;         //进程消息
HINSTANCE GEApp::hInstance; //实例句柄

//----------------------------------------//


//--------------------3D基础--------------------//

///帧周期
UINT GE3D::CurFrame; //当前帧
UINT GE3D::MaxFrame; //最大帧
///描述符
UINT GE3D::cbRTView;  //RTV字节数
UINT GE3D::cbDSView;  //DSV字节数
UINT GE3D::cbCSUView; //CBV/SRV/UAV字节数
///围栏
ULNG GE3D::MaxFencePoint;  //最大围栏点
ULNG *GE3D::arrFencePoint; //围栏点数组
HANDLE GE3D::hFenceEvent;  //围栏事件
ID3DFence *GE3D::lpFence;  //围栏
///命令
ID3DGfxCmdList *GE3D::lpCmdList;  //命令列表
ID3DCmdQueue *GE3D::lpCmdQueue;   //命令队列
ID3DCmdAlloc *GE3D::CurCmdAlloc;  //当前命令分配器
ID3DCmdAlloc **GE3D::arrCmdAlloc; //命令分配器数组
///设备
ID3DDevice *GE3D::lpD3DDevice;      //D3D设备
IDXGIFactory6 *GE3D::lpDXGIFactory; //DXGI工厂

//----------------------------------------//


//--------------------2D基础--------------------//

///D2D
ID2DDevCtx *GE2D::lpD2DDC;       //D2D设备上下文
ID2DDevice *GE2D::lpD2DDevice;   //D2D设备
ID2DFactory *GE2D::lpD2DFactory; //D2D工厂
///D3D11
ID3D11DevCtx *GE2D::lpD3D11DC;       //D3D11设备上下文
ID3D11Device *GE2D::lpD3D11Device;   //D3D11设备
ID3D11On12Dev *GE2D::lpD3D11On12Dev; //D3D11On12设备
///DWrite
IDWFactory *GE2D::lpDWFactory; //DWrite工厂
///WIC
IWICFactory *GE2D::lpWICFactory; //WIC工厂

//----------------------------------------//


//--------------------声音基础--------------------//

IXAFactory *GEAudio::lpXAFactory;   //XAudio工厂
IXADestVoice *GEAudio::lpDestVoice; //目标音源

//----------------------------------------//


//--------------------输入基础--------------------//

INT4 GEInput::WheelValue; //滚轮值
INT4 GEInput::WheelDelta; //滚轮差值
DXTKMouse GEInput::Mouse;       //鼠标
DXTKKeyboard GEInput::Keyboard; //键盘
DXTKKeysState GEInput::KBState;     //键盘状态
DXTKMouseState GEInput::MouseState; //鼠标状态

//----------------------------------------//


//--------------------调试基础--------------------//

SPFP GEDebug::BoxWidth; //窗口宽度
SPFP GEDebug::FontSize; //字体大小
SPFP GEDebug::PrintPos; //打印位置
CLinkList GEDebug::LogList; //日志列表
IDWTxtFormat *GEDebug::lpTxtFormat;    //文本格式
ID2DSolidColorBrush *GEDebug::lpBrush; //文本笔刷

//----------------------------------------//