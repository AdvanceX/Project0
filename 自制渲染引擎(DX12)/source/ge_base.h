//--------------------进程--------------------//

namespace ge{
	MSG Message; //进程消息
	HINSTANCE hInstance; //实例句柄
}

//----------------------------------------//


//--------------------3D设备--------------------//

namespace ge{
	///D3D设备
	ID3DDevice *lpD3DDevice; //D3D设备
	IDXGIFactory4 *lpDXGIFactory; //DXGI工厂
	///命令
	ID3DGfxCmdList *lpCmdList;  //命令列表
	ID3DCmdQueue *lpCmdQueue;   //命令队列
	ID3DCmdAlloc *lpCmdAlloc;   //命令分配器
	ID3DCmdAlloc **arrCmdAlloc; //命令分配器数组
	///围栏
	ULNG MaxFencePoint;  //最大围栏点
	ULNG *arrFencePoint; //围栏点数组
	HANDLE hFenceEvent;  //围栏事件
	ID3DFence *lpFence;  //围栏
	///描述符
	UINT cbRTView;  //RTV字节数
	UINT cbDSView;  //DSV字节数
	UINT cbCSUView; //CBV/SRV/UAV字节数
	///渲染周期
	UINT CurFrame = 0; //当前帧
	UINT MaxFrame = 2; //最大帧

	//预置D3D设备
	VOID InitD3DDevice(){
		RESULT Result = D3D12CreateDevice(
			NULL, D3D_FEATURE_LEVEL_11_0, COM_ARGS(ge::lpD3DDevice));

		if(FAILED(Result)){
			IDXGIAdapter4 *lpAdapter;
			ge::lpDXGIFactory->EnumWarpAdapter(COM_ARGS(lpAdapter));
			D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, COM_ARGS(ge::lpD3DDevice));
			RELEASE_REF(lpAdapter);
		}
	}
	//预置命令对象
	VOID InitCommandObjects(){
		//创建围栏

		ge::MaxFencePoint = 0;
		ge::arrFencePoint = new ULNG[ge::MaxFrame]{ 0 };
		ge::hFenceEvent = CreateEventEx(NULL, NULL, 0x0, EVENT_ALL_ACCESS);
		ge::lpD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, COM_ARGS(ge::lpFence));

		//创建命令队列

		D3DCmdQueueDesc CQDesc;

		CQDesc.NodeMask = 0x0;
		CQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		ge::lpD3DDevice->CreateCommandQueue(&CQDesc, COM_ARGS(ge::lpCmdQueue));

		//创建命令分配器

		ge::arrCmdAlloc = new ID3DCmdAlloc*[ge::MaxFrame];
		for(UINT i = 0; i < ge::MaxFrame; ++i){
			ge::lpD3DDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, COM_ARGS(ge::arrCmdAlloc[i]));
		}
		ge::lpCmdAlloc = ge::arrCmdAlloc[0];

		//创建命令列表

		ge::lpD3DDevice->CreateCommandList1(0x0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_LIST_FLAG_NONE, COM_ARGS(ge::lpCmdList));
	}
	//预置视图句柄移动步幅
	VOID InitViewHandleIncrementSizes(){
		ge::cbRTView = ge::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		ge::cbDSView = ge::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		ge::cbCSUView = ge::lpD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	//预置D3D
	VOID InitDirect3D(){
		CreateDXGIFactory1(COM_ARGS(ge::lpDXGIFactory));
		ge::InitD3DDevice();
		ge::InitCommandObjects();
		ge::InitViewHandleIncrementSizes();
	}
	//终结D3D
	VOID FreeDirect3D(){
		if(!ge::lpD3DDevice) return;

		CloseHandle(ge::hFenceEvent);
		ge::hFenceEvent = NULL;

		for(UINT i = 0; i < ge::MaxFrame; ++i)
			RELEASE_REF(ge::arrCmdAlloc[i]);

		RESET_REF(ge::lpFence);
		RESET_REF(ge::lpCmdList);
		RESET_REF(ge::lpCmdQueue);
		RESET_REF(ge::lpD3DDevice);
		RESET_REF(ge::lpDXGIFactory);

		RESET_ARR(ge::arrCmdAlloc);
		RESET_ARR(ge::arrFencePoint);
	}
	//开始命令
	inline VOID BeginCommand(){
		ge::lpCmdAlloc->Reset();
		ge::lpCmdList->Reset(ge::lpCmdAlloc, NULL);
	}
	//提交命令
	inline VOID SubmitCommand(){
		ge::lpCmdList->Close();
		ge::lpCmdQueue->ExecuteCommandLists(1, (ID3DCmdList**)&ge::lpCmdList);
	}
	//等待GPU
	inline VOID WaitForGPU(){
		ge::lpCmdQueue->Signal(ge::lpFence, ++ge::MaxFencePoint);

		if(ge::lpFence->GetCompletedValue() < ge::MaxFencePoint){
			ge::lpFence->SetEventOnCompletion(ge::MaxFencePoint, ge::hFenceEvent);
			WaitForSingleObject(ge::hFenceEvent, INFINITE);
		}
	}
};

//----------------------------------------//


//--------------------2D设备--------------------//

namespace ge{
	///D3D11设备
	ID3D11DevCtx *lpD3D11DC;
	ID3D11Device *lpD3D11Device;
	ID3D11On12Dev *lpD3D11On12Dev;
	///D2D设备
	ID2DDevCtx *lpD2DDC;
	ID2DDevice *lpD2DDevice;
	ID2DFactory *lpD2DFactory;
	///DWrite工厂
	IDWFactory *lpDWFactory;
	///WIC工厂
	IWICFactory *lpWICFactory;

	//预置D2D
	VOID InitDirect2D(){
		//创建D3D11设备

		D3D11On12CreateDevice(ge::lpD3DDevice, D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, 0,
			(IUnknown**)&ge::lpCmdQueue, 1, 0x0, &ge::lpD3D11Device, &ge::lpD3D11DC, NULL);

		ge::lpD3D11Device->QueryInterface(&ge::lpD3D11On12Dev);

		//创建D2D设备

		IDXGIDevice *lpDXGIDevice;

		ge::lpD3D11On12Dev->QueryInterface(&lpDXGIDevice);
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, COM_ARGS(ge::lpD2DFactory));
		ge::lpD2DFactory->CreateDevice(lpDXGIDevice, &ge::lpD2DDevice);
		ge::lpD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ge::lpD2DDC);

		lpDXGIDevice->Release();

		//创建DWrite工厂

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			clsidof(IDWriteFactory), (IUnknown**)&ge::lpDWFactory);

		//创建WIC工厂

		CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, COM_ARGS(ge::lpWICFactory));
	}
	//终结D2D
	VOID FreeDirect2D(){
		RESET_REF(ge::lpD3D11DC);
		RESET_REF(ge::lpD3D11Device);
		RESET_REF(ge::lpD3D11On12Dev);
		RESET_REF(ge::lpD2DDC);
		RESET_REF(ge::lpD2DDevice);
		RESET_REF(ge::lpD2DFactory);
		RESET_REF(ge::lpDWFactory);
		RESET_REF(ge::lpWICFactory);
	}
}

//----------------------------------------//


//--------------------视频设备--------------------//

namespace ge{
	//预置MF
	VOID InitMediaFoundation(){
		CoInitialize(NULL);
		MFStartup(MF_VERSION, MFSTARTUP_LITE);
	}
	//终结MF
	VOID FreeMediaFoundation(){
		MFShutdown();
	}
}

//----------------------------------------//


//--------------------声音设备--------------------//

namespace ge{
	IXAFactory *lpXAFactory;
	IXADestVoice *lpDestVoice;

	//预置XAudio
	VOID InitXAudio(){
		XAudio2Create(&ge::lpXAFactory, 0x0, XAUDIO2_DEFAULT_PROCESSOR);
		ge::lpXAFactory->CreateMasteringVoice(&ge::lpDestVoice);
	}
	//终结XAudio
	VOID FreeXAudio(){
		RESET_REF(ge::lpXAFactory);
	}
}

//----------------------------------------//


//--------------------输入设备--------------------//

namespace ge{
	INT4 WheelValue;
	INT4 WheelOffset;
	DXTKMouse Mouse;
	DXTKKeyboard Keyboard;
	DXTKKeysState KBState;
	DXTKMouseState MouseState;

	//更新鼠标状态
	inline VOID UpdateMouseState(){
		ge::MouseState = ge::Mouse.GetState();
		ge::WheelOffset = ge::MouseState.scrollWheelValue - ge::WheelValue;
		ge::WheelValue = ge::MouseState.scrollWheelValue;
	}
	//更新键盘状态
	inline VOID UpdateKeyboardState(){
		ge::KBState = ge::Keyboard.GetState();
	}
	//设置输入窗口(窗口)
	inline VOID SetInputWindow(HWND hWindow){
		ge::Mouse.SetWindow(hWindow);
	}
	//设置鼠标模式(模式)
	inline VOID SetMouseMode(DXTKMouse::Mode Mode){
		ge::Mouse.SetMode(Mode);
	}
	//处理消息(消息号,参数0,参数1)
	inline VOID ProcMessage(UINT MsgCode, WPARAM Param0, LPARAM Param1){
		DXTKMouse::ProcessMessage(MsgCode, Param0, Param1);
		DXTKKeyboard::ProcessMessage(MsgCode, Param0, Param1);
	}
}

//----------------------------------------//


//--------------------调试模块--------------------//

namespace ge{
	SPFP DebugWndWidth;
	SPFP DebugPrintPos = 0.0f;
	SPFP DebugFontSize = 20.0f;
	IDWTxtFormat *lpDebugTxtFormat;
	ID2DSolidColorBrush *lpDebugBrush;
	CLinkList DebugList;

	VOID FreeDebug(){
		RESET_REF(ge::lpDebugBrush);
		RESET_REF(ge::lpDebugTxtFormat);
	}
	VOID InitDebug(SPFP WndWidth){
		ge::DebugWndWidth = WndWidth;

		ge::lpD2DDC->CreateSolidColorBrush(d2d::ColorF(d2d::ColorF::Green), &ge::lpDebugBrush);
		ge::lpDWFactory->CreateTextFormat(L"Verdana", NULL,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			ge::DebugFontSize - 2.0f, L"en-us", &ge::lpDebugTxtFormat);

		ge::lpDebugTxtFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		ge::lpDebugTxtFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		ge::lpDebugTxtFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}
	inline VOID AddDebug(WCHR* Format, ...){
		VALIST Args;
		WCHR *Text = new WCHR[128];

		VA_START(Args, Format);
		VWSPrint(Text, Format, Args);
		VA_END(Args);

		ge::DebugList.Insert((LPTR)Text);
	}
	inline VOID PrintDebug(){
		WCHR *Text;
		D2DRectF Area;
		CLinkList::NODE *pText = ge::DebugList.GetFirst();

		for(UINT i = 0; i < ge::DebugList.Size; ++i){
			Text = (WCHR*)pText->Data;

			Area.left = 10.0f;
			Area.top = (ge::DebugPrintPos * ge::DebugFontSize) + 10.0f;
			Area.right = ge::DebugWndWidth;
			Area.bottom = Area.top + ge::DebugFontSize;

			ge::lpD2DDC->SetTransform(d2d::Matrix3x2F::Identity());
			ge::lpD2DDC->DrawText(Text, (UINT)WcsLen(Text),
				ge::lpDebugTxtFormat, Area, ge::lpDebugBrush);

			ge::DebugPrintPos += 1.0f;
			pText = pText->Next;
			delete[] Text;
		}

		ge::DebugList.Clear(FALSE);
		ge::DebugPrintPos = 0.0f;		
	}
	inline VOID AddBlankDebug(){
		ge::DebugPrintPos += 1.0f;
	}
};

//----------------------------------------//


//--------------------窗口--------------------//

namespace ge{
	//窗口描述
	struct WndDesc{
		HWND hWindow;
		UINT WndWidth;
		UINT WndHeight;
		UINT BuffCount = 1;
		UINT MSAACount = 1;
		UINT MSAAQuality = 0;
		BOOL bWindowed = FALSE;
		BOOL bEnable2D = FALSE;
		DXGIFormat SCBFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGIFormat DSBFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};
	//窗口
	class Window{
		///窗口句柄
		open HWND hWindow; //窗口句柄
		///窗口尺寸
		open UINT WndWidth;  //窗口宽
		open UINT WndHeight; //窗口高
		///缓冲层数
		open UINT BuffCount; //缓冲区数量
		hide UINT BuffIndex; //缓冲区索引
		///描述符
		open HD3DVIEW hRTView; //RTV句柄
		open HD3DVIEW hDSView; //DSV句柄
		hide ID3DViewHeap *lpRTVHeap; //RTV堆
		hide ID3DViewHeap *lpDSVHeap; //DSV堆
		///3D缓冲区
		open ID3DResource *lpRTBuffer; //渲染目标缓冲区
		open ID3DResource *lpDSBuffer; //深度模板缓冲区
		///2D缓冲区
		open ID2DBitmap *lpCanvas2D;   //2D画布
		open ID2DBitmap **arrCanvas2D; //2D画布数组
		open ID3D11Resource *lpBuffWrap;   //缓冲区包裹
		open ID3D11Resource **arrBuffWrap; //缓冲区包裹数组
		///交换链
		open IDXGISwapChain *lpSwapChain; //交换链
		///视口/裁剪区
		open D3DRect rcScissor; //裁剪区
		open D3DViewport Viewport; //视口

		//析构//
		open ~Window(){
			self.Finalize();
		}
		//构造//
		open Window(){
			MemZero(this, sizeof(*this));
		}
		//终结
		open VOID Finalize(){
			if(!self.lpSwapChain) return;

			for(UINT i = 0; i < self.BuffCount; ++i){
				self.arrCanvas2D[i]->Release();
				self.arrBuffWrap[i]->Release();
			}

			DELETE_ARR(self.arrBuffWrap);
			DELETE_ARR(self.arrCanvas2D);

			RELEASE_REF(self.lpRTVHeap);
			RELEASE_REF(self.lpDSVHeap);
			RELEASE_REF(self.lpDSBuffer);
			RELEASE_REF(self.lpRTBuffer);
			RELEASE_REF(self.lpSwapChain);

			MemZero(this, sizeof(*this));
		}
		//预置(应用描述)
		open VOID Initialize(ge::WndDesc &WndDesc){
			self.hWindow = WndDesc.hWindow;
			self.WndWidth = WndDesc.WndWidth;
			self.WndHeight = WndDesc.WndHeight;
			self.BuffCount = WndDesc.BuffCount;

			self.InitViewportAndScissorRect();
			self.BuildDescriptorHeaps();
			self.BuildSwapChain(&WndDesc);
			self.BuildDepthStencilBuffer(&WndDesc);

			if(WndDesc.bEnable2D) self.Build2DCanvases();
		}
		//调整窗口(应用描述)
		open VOID ResizeWindow(ge::WndDesc &WndDesc){
			//保存新设置

			self.WndWidth = WndDesc.WndWidth;
			self.WndHeight = WndDesc.WndHeight;
			self.BuffCount = WndDesc.BuffCount;

			//释放旧缓存

			RELEASE_REF(self.lpDSBuffer);
			RELEASE_REF(self.lpRTBuffer);

			//调整交换链

			self.lpSwapChain->ResizeBuffers(self.BuffCount, self.WndWidth, self.WndHeight,
				WndDesc.SCBFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

			//创建描述符

			HD3DVIEW hRTView;
			ID3DResource *lpBuffer;

			hRTView = self.hRTView;
			for(UINT i = 0; i < self.BuffCount; ++i){
				self.lpSwapChain->GetBuffer(i, COM_ARGS(lpBuffer));
				ge::lpD3DDevice->CreateRenderTargetView(lpBuffer, NULL, hRTView);
				hRTView = hRTView.Offset(1, ge::cbRTView);
			}

			//重建深度模板缓存和重置视口

			self.BuildDepthStencilBuffer(&WndDesc);
			self.InitViewportAndScissorRect();
		}
		//设置视口(视口)
		open VOID SetViewport(D3DViewport &Viewport){
			self.Viewport = Viewport;
		}
		//设置裁剪区(裁剪区)
		open VOID SetScissorRect(D3DRect &rcScissor){
			self.rcScissor = rcScissor;
		}
		//构建交换链(应用描述)
		hide VOID BuildSwapChain(ge::WndDesc* pAppDesc){
			//创建交换链

			DXGISwapChainDesc SCDesc;

			SCDesc.BufferDesc.Width = self.WndWidth;
			SCDesc.BufferDesc.Height = self.WndHeight;
			SCDesc.BufferDesc.Format = pAppDesc->SCBFormat;
			SCDesc.BufferDesc.RefreshRate.Numerator = 60;
			SCDesc.BufferDesc.RefreshRate.Denominator = 1;
			SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

			SCDesc.SampleDesc.Count = pAppDesc->MSAACount;
			SCDesc.SampleDesc.Quality = pAppDesc->MSAAQuality;

			SCDesc.Windowed = pAppDesc->bWindowed;
			SCDesc.OutputWindow = self.hWindow;
			SCDesc.BufferCount = self.BuffCount;
			SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SCDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			ge::lpDXGIFactory->CreateSwapChain(ge::lpCmdQueue,
				&SCDesc, (IDXGISwapChain**)&self.lpSwapChain);

			//创建描述符

			HD3DVIEW hRTView;
			ID3DResource *lpSCBuffer;

			hRTView = self.hRTView;
			for(UINT i = 0; i < self.BuffCount; ++i){
				self.lpSwapChain->GetBuffer(i, COM_ARGS(lpSCBuffer));
				ge::lpD3DDevice->CreateRenderTargetView(lpSCBuffer, NULL, hRTView);
				hRTView.Offset(ge::cbRTView);
				lpSCBuffer->Release();
			}

			//设置渲染目标

			self.lpSwapChain->GetBuffer(0, COM_ARGS(self.lpRTBuffer));
		}
		//构建深度模板缓存(应用描述)
		hide VOID BuildDepthStencilBuffer(ge::WndDesc* pAppDesc){
			//创建缓存

			D3DResDesc BuffDesc;
			D3DFillValue FillValue;

			BuffDesc.Alignment = 0;
			BuffDesc.MipLevels = 1;
			BuffDesc.DepthOrArraySize = 1;
			BuffDesc.Width = self.WndWidth;
			BuffDesc.Height = self.WndHeight;
			BuffDesc.SampleDesc.Count = pAppDesc->MSAACount;
			BuffDesc.SampleDesc.Quality = pAppDesc->MSAAQuality;
			BuffDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			BuffDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			BuffDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			BuffDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			FillValue.Format = pAppDesc->DSBFormat;
			FillValue.DepthStencil.Depth = 1.0f;
			FillValue.DepthStencil.Stencil = 0;

			ge::lpD3DDevice->CreateCommittedResource(
				&D3DHeapProps(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE, &BuffDesc,
				D3D12_RESOURCE_STATE_COMMON, &FillValue,
				COM_ARGS(self.lpDSBuffer));

			//创建视图

			D3DDsvDesc ViewDesc;

			ViewDesc.Format = pAppDesc->DSBFormat;
			ViewDesc.Flags = D3D12_DSV_FLAG_NONE;
			ViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			ViewDesc.Texture2D.MipSlice = 0;

			ge::lpD3DDevice->CreateDepthStencilView(self.lpDSBuffer, &ViewDesc, self.hDSView);
		}
		//构建2D画布
		hide VOID Build2DCanvases(){
			SPFP DpiX, DpiY;
			D2DBmpProps BmpProps;
			D3D11ResFlags BuffFlags;
			IDXGISurface *lpSurface;
			ID3DResource *lpSCBuffer;

			ge::lpD2DFactory->GetDesktopDpi(&DpiX, &DpiY);

			BmpProps = d2d::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				d2d::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				DpiX, DpiY
			);

			MemZero(&BuffFlags, sizeof(D3D11ResFlags));
			BuffFlags.BindFlags = D3D11_BIND_RENDER_TARGET;

			self.arrCanvas2D = new ID2DBitmap*[self.BuffCount];
			self.arrBuffWrap = new ID3D11Resource*[self.BuffCount];

			for(UINT i = 0; i < self.BuffCount; ++i){
				self.lpSwapChain->GetBuffer(i, COM_ARGS(lpSCBuffer));

				ge::lpD3D11On12Dev->CreateWrappedResource(lpSCBuffer, &BuffFlags,
					D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
					COM_ARGS(self.arrBuffWrap[i]));

				self.arrBuffWrap[i]->QueryInterface(&lpSurface);

				ge::lpD2DDC->CreateBitmapFromDxgiSurface(
					lpSurface, &BmpProps, &self.arrCanvas2D[i]);

				lpSurface->Release();
				lpSCBuffer->Release();
			}
		}
		//构建描述符堆
		hide VOID BuildDescriptorHeaps(){
			//创建描述符堆

			D3DViewHeapDesc HeapDesc;

			HeapDesc.NodeMask = 0x0;
			HeapDesc.NumDescriptors = self.BuffCount;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			ge::lpD3DDevice->CreateDescriptorHeap(
				&HeapDesc, COM_ARGS(self.lpRTVHeap));

			HeapDesc.NodeMask = 0x0;
			HeapDesc.NumDescriptors = 1;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			ge::lpD3DDevice->CreateDescriptorHeap(
				&HeapDesc, COM_ARGS(self.lpDSVHeap));

			//获取视图句柄

			self.hRTView = self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
			self.hDSView = self.lpDSVHeap->GetCPUDescriptorHandleForHeapStart();
		}
		//预置视口和裁剪区
		hide VOID InitViewportAndScissorRect(){
			//设置视口

			self.Viewport.TopLeftX = 0.0f;
			self.Viewport.TopLeftY = 0.0f;
			self.Viewport.MinDepth = 0.0f;
			self.Viewport.MaxDepth = 1.0f;
			self.Viewport.Width = (SPFP)self.WndWidth;
			self.Viewport.Height = (SPFP)self.WndHeight;

			//设置裁剪区

			self.rcScissor.top = 0;
			self.rcScissor.left = 0;
			self.rcScissor.right = self.WndWidth;
			self.rcScissor.bottom = self.WndHeight;
		}
		//呈现缓存
		open VOID PresentBuffer(){
			self.lpSwapChain->Present(0, 0x0);
			self.FlipPage();
		}
		//翻页
		hide VOID FlipPage(){
			self.BuffIndex++;
			self.BuffIndex %= self.BuffCount;

			self.lpRTBuffer->Release();
			self.lpSwapChain->GetBuffer(self.BuffIndex, COM_ARGS(self.lpRTBuffer));

			self.hRTView = self.lpRTVHeap->GetCPUDescriptorHandleForHeapStart();
			self.hRTView.Offset(ge::cbRTView * self.BuffIndex);

			if(self.lpCanvas2D){
				self.lpCanvas2D = self.arrCanvas2D[self.BuffIndex];
				self.lpBuffWrap = self.arrBuffWrap[self.BuffIndex];
			}
		}
	};

	//开始2D绘制
	inline VOID BeginDraw2D(ge::Window &Window){
		ge::lpD3D11On12Dev->AcquireWrappedResources(&Window.lpBuffWrap, 1);
		ge::lpD2DDC->SetTarget(Window.lpCanvas2D);
		ge::lpD2DDC->BeginDraw();
	}
	//结束2D绘制
	inline VOID EndDraw2D(ge::Window &Window){
		ge::lpD2DDC->EndDraw();
		ge::lpD3D11On12Dev->ReleaseWrappedResources(&Window.lpBuffWrap, 1);
		ge::lpD3D11DC->Flush();
	}
}

//----------------------------------------//