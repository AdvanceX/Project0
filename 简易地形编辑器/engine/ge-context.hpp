//-------------------- 引擎上下文 --------------------//

// Game Engine Process(Context)
class GEPrc{
	_open static MSG LastMsg;
	_open static HANDLE hStdIn;
	_open static HANDLE hStdOut;
	_open static HINSTANCE hInstance;

	_open static $VOID InitCom(DWORD Flag){
		CoInitializeEx(P_Null, Flag);
	}
	_open static $VOID UninitCom(){
		CoUninitialize();
	}
	_open static $VOID CreateConsole(){
		AllocConsole();
		GEPrc::hStdIn = GetStdHandle(STD_INPUT_HANDLE);
		GEPrc::hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	_open static $VOID DestroyConsole(){
		FreeConsole();
		GEPrc::hStdIn = P_Null;
		GEPrc::hStdOut = P_Null;
	}
};

// Game Engine Clock(Context)
class GEClk{
	_open static DPFPN TickPeriod; // 单位:秒

	_open static $VOID Initialize(){
		LARGE_INTEGER TickRate;
		QueryPerformanceFrequency(&TickRate);
		GEClk::TickPeriod = 1.0 / (DPFPN)TickRate.QuadPart;
	}
	_open static UNS64 GetTickCount(){
		LARGE_INTEGER TickCnt;
		QueryPerformanceCounter(&TickCnt);
		return TickCnt.QuadPart;
	}
	_open static DPFPN TicksToSeconds(UNS64 TickCnt){
		return GEClk::TickPeriod * TickCnt;
	}
	_open static DPFPN TicksToMillisecs(UNS64 TickCnt){
		return GEClk::TickPeriod * TickCnt * 1000.0;
	}
};

// Game Engine Physics(Context)
class GEPhy{
	_open static btCollisionDispatcher *lpCollisDispr;
	_open static btBroadphaseInterface *lpOverlapCache;
	_open static btDefaultCollisionConfiguration *lpCollisConfig;
	_open static btSequentialImpulseConstraintSolver *lpSICSolver;

	_open static $VOID Initialize(){
		GEPhy::lpSICSolver = new btSequentialImpulseConstraintSolver;
		GEPhy::lpOverlapCache = new btDbvtBroadphase;
		GEPhy::lpCollisConfig = new btDefaultCollisionConfiguration;
		GEPhy::lpCollisDispr = new btCollisionDispatcher(lpCollisConfig);
	}
	_open static $VOID Release(){
		SAFE_DELETE(GEPhy::lpSICSolver);
		SAFE_DELETE(GEPhy::lpCollisDispr);
		SAFE_DELETE(GEPhy::lpCollisConfig);
		SAFE_DELETE(GEPhy::lpOverlapCache);
	}
};

// Game Engine Input(Context)
class GEInp{
	_open enum MOUSEKEY{
		MKEY_Left = 256L,
		MKEY_Right = 257L,
		MKEY_Middle = 258L,
	};
	_open struct NKeysState{
		SPFPN Timers[260];
		BYTET Flags[2][260];
		BYTET *psF0, *psF1;
	};

	_open static IDirectInputDevice8W *lpMouse;
	_open static IDirectInputDevice8W *lpKeyboard;
	_open static IDirectInput8W *lpDIFactory;
	_open static DIMOUSESTATE MouseState;
	_open static NKeysState KeysState;

	_open static $VOID Release(){
		if(GEInp::lpMouse){
			GEInp::lpMouse->Unacquire();
			GEInp::lpMouse->Release();
			GEInp::lpMouse = P_Null;
		}
		if(GEInp::lpKeyboard){
			GEInp::lpKeyboard->Unacquire();
			GEInp::lpKeyboard->Release();
			GEInp::lpKeyboard = P_Null;
		}
		if(GEInp::lpDIFactory){
			GEInp::lpDIFactory->Release();
			GEInp::lpDIFactory = P_Null;
		}
	}
	_open static $VOID Initialize(HWND hWindow){
		DirectInput8Create(GEPrc::hInstance, DIRECTINPUT_VERSION,
			IID_IDirectInput8, ($VOID**)&GEInp::lpDIFactory, P_Null);

		GEInp::KeysState.psF0 = GEInp::KeysState.Flags[0];
		GEInp::KeysState.psF1 = GEInp::KeysState.Flags[1];

		GEInp::lpDIFactory->CreateDevice(GUID_SysMouse, &GEInp::lpMouse, P_Null);
		GEInp::lpDIFactory->CreateDevice(GUID_SysKeyboard, &GEInp::lpKeyboard, P_Null);

		if(GEInp::lpMouse){
			GEInp::lpMouse->SetCooperativeLevel(hWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			GEInp::lpMouse->SetDataFormat(&c_dfDIMouse);
			GEInp::lpMouse->Acquire();
		}
		if(GEInp::lpKeyboard){
			GEInp::lpKeyboard->SetCooperativeLevel(hWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			GEInp::lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
			GEInp::lpKeyboard->Acquire();
		}
	}
	_open static $VOID Update(SPFPN TimeSpan){
		SWAP_(GEInp::KeysState.psF0, GEInp::KeysState.psF1);

		SPFPN *prgTimer = GEInp::KeysState.Timers;
		BYTET *prgState = GEInp::KeysState.psF0;
		BYTET *prgState1 = GEInp::KeysState.psF1;

		if(GEInp::lpKeyboard){
			if(GEInp::lpKeyboard->GetDeviceState(sizeof(BYTET[256]), prgState) == DIERR_INPUTLOST){
				GEInp::lpKeyboard->Acquire();
				return;
			}
			for(UNS32 N = 0; N < 256; ++N){
				if(prgState[N] &= 0x80){
					if(!prgState1[N]) prgTimer[N] = 0.f;
					else prgTimer[N] += TimeSpan;
				}
			}
		}
		if(GEInp::lpMouse){
			if(GEInp::lpMouse->GetDeviceState(sizeof(DIMOUSESTATE), &GEInp::MouseState) == DIERR_INPUTLOST){
				GEInp::lpMouse->Acquire();
				return;
			}
			for(UNS32 N = 256; N < 260; ++N){
				prgState[N] = GEInp::MouseState.rgbButtons[N - 256];
				if(prgState[N] &= 0x80){
					if(!prgState1[N]) prgTimer[N] = 0.f;
					else prgTimer[N] += TimeSpan;
				}
			}
		}
	}
	_open static IBOOL IsLoosed(UNS32 Key){
		return !GEInp::KeysState.psF0[Key];
	}
	_open static IBOOL IsPressed(UNS32 Key){
		return GEInp::KeysState.psF0[Key];
	}
	_open static IBOOL JustLoosed(UNS32 Key){
		return !GEInp::KeysState.psF0[Key] && GEInp::KeysState.psF1[Key];
	}
	_open static IBOOL JustPressed(UNS32 Key){
		return GEInp::KeysState.psF0[Key] && !GEInp::KeysState.psF1[Key];
	}
};

// Game Engine Audio(Context)
class GEAud{
	class CDefCback: public IXAudio2VoiceCallback{
		$VOID OnStreamEnd(){}
		$VOID OnLoopEnd(PVOID pBuffer){}
		$VOID OnBufferEnd(PVOID pBuffer){
			delete[] pBuffer;
		}
		$VOID OnBufferStart(PVOID pBuffer){}
		$VOID OnVoiceError(PVOID pBuffer, HRESULT ErrCode){}
		$VOID OnVoiceProcessingPassStart(UNS32 RequiredSize){}
		$VOID OnVoiceProcessingPassEnd(){}
	};

	_open static CDefCback *lpCback;
	_open static IXAudio2 *lpXAFactory;
	_open static IXAudio2MasteringVoice *lpOutput;

	_open static $VOID Initialize(){
		XAudio2Create(&GEAud::lpXAFactory, 0L, XAUDIO2_DEFAULT_PROCESSOR);
		GEAud::lpXAFactory->CreateMasteringVoice(&GEAud::lpOutput);
		GEAud::lpCback = new CDefCback;
	}
	_open static $VOID Release(){
		SAFE_DELETE(GEAud::lpCback);
		SAFE_DELETE(GEAud::lpOutput);
		SAFE_RELEASE(GEAud::lpXAFactory);
	}
};

// Game Engine 3D(Context)
class GE3d{
	_open enum GPUBRAND{
		GPU_BRAND_None = 0L,
		GPU_BRAND_Amd = 4098L,    // 0x1002
		GPU_BRAND_Intel = 32902L, // 0x8086
		GPU_BRAND_Nvidia = 4318L, // 0x10DE
	};
	_open struct NOptions{
		IBOOL bUseWarp = B_False;
		IBOOL bNeedDxr = B_False;
		GPUBRAND GpuBrand = GPU_BRAND_None;
		DXGI_GPU_PREFERENCE GpuPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
	};

	///视图
	_open static UNS32 cbRTView;
	_open static UNS32 cbDSView;
	_open static UNS32 cbSRView;
	///设备
	_open static ID3D12Device4 *lpD3dDevice;
	_open static IDXGIFactory6 *lpDxgiFactory;
	///命令
	_open static ID3D12CommandQueue *lpGfxQueue;
	_open static ID3D12CommandQueue *lpGpcQueue;
	_open static ID3D12CommandQueue *lpCpyQueue;
	
	_open static $VOID Initialize(_in NOptions &Options = NOptions()){
		GE3d::InitDxgiFactory();
		GE3d::InitD3dDevice(Options);
		GE3d::InitViewByteCounts();
		GE3d::InitCommandQueues();
	}
	_secr static $VOID InitD3dDevice(_in NOptions &Options){
		HRESULT RetCode;
		DXGI_ADAPTER_DESC1 AdaptDesc;

		ID3D12Device4 *lpDevice = P_Null;
		IDXGIAdapter4 *lpAdapter = P_Null;

		if(!Options.bUseWarp){
			for(UNS32 iExe = 0; !GE3d::lpD3dDevice; ++iExe){
				SAFE_RELEASE(lpDevice);
				SAFE_RELEASE(lpAdapter);

				RetCode = GE3d::lpDxgiFactory->EnumAdapterByGpuPreference(
					iExe, Options.GpuPref, IID_PPV_ARGS(&lpAdapter));

				if(RetCode == DXGI_ERROR_NOT_FOUND)
					break;

				lpAdapter->GetDesc1(&AdaptDesc);

				if(AdaptDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if((Options.GpuBrand != GPU_BRAND_None) && (Options.GpuBrand != AdaptDesc.VendorId))
					continue;

				D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&lpDevice));

				if(Options.bNeedDxr && !CheckD3dRaytracingTier(lpDevice))
					continue;

				GE3d::lpD3dDevice = lpDevice;
				lpAdapter->Release();
			}
		}

		if(!GE3d::lpD3dDevice){
			GE3d::lpDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&lpAdapter));
			D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&GE3d::lpD3dDevice));
			lpAdapter->Release();
		}
	}
	_secr static $VOID InitViewByteCounts(){
		GE3d::cbRTView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		GE3d::cbDSView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		GE3d::cbSRView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	_secr static $VOID InitCommandQueues(){
		D3D12_COMMAND_QUEUE_DESC QueDesc;
		QueDesc.NodeMask = 0x0;
		QueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_PPV_ARGS(&GE3d::lpGfxQueue));

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_PPV_ARGS(&GE3d::lpGpcQueue));

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_PPV_ARGS(&GE3d::lpCpyQueue));
	}
	_secr static $VOID InitDxgiFactory(){
		CreateDXGIFactory2(0L, IID_PPV_ARGS(&GE3d::lpDxgiFactory));
	}
	_open static $VOID Release(){
		SAFE_RELEASE(GE3d::lpGfxQueue);
		SAFE_RELEASE(GE3d::lpGpcQueue);
		SAFE_RELEASE(GE3d::lpCpyQueue);
		SAFE_RELEASE(GE3d::lpD3dDevice);
		SAFE_RELEASE(GE3d::lpDxgiFactory);
	}
};

// Game Engine 2D(Context)
class GE2d{
	/// 设备
	_open static ID2D1Device3 *lpD2dDevice;
	_open static ID2D1Factory4 *lpD2dFactory;
	_open static ID2D1DeviceContext *lpD2dDevCtx;
	_open static ID3D11Device *lpD3d11Device;
	_open static ID3D11On12Device *lpD3d11On12Dev;
	_open static ID3D11DeviceContext *lpD3d11DevCtx;
	_open static IDXGIDevice *lpDxgiDevice;
	_open static IDWriteFactory *lpDWFactory;
	_open static IWICImagingFactory *lpWicFactory;
	/// 笔刷
	_open static ID2D1BitmapBrush1 *lpBmpBrush;
	_open static ID2D1SolidColorBrush *lpPureBrush;

	_open static $VOID Release(){
		SAFE_RELEASE(GE2d::lpBmpBrush);
		SAFE_RELEASE(GE2d::lpPureBrush);
		SAFE_RELEASE(GE2d::lpDWFactory);
		SAFE_RELEASE(GE2d::lpD2dDevice);
		SAFE_RELEASE(GE2d::lpD2dDevCtx);
		SAFE_RELEASE(GE2d::lpD2dFactory);
		SAFE_RELEASE(GE2d::lpDxgiDevice);
		SAFE_RELEASE(GE2d::lpD3d11Device);
		SAFE_RELEASE(GE2d::lpD3d11DevCtx);
		SAFE_RELEASE(GE2d::lpD3d11On12Dev);
		//SAFE_RELEASE(GE2d::lpWicFactory);
	}
	_open static $VOID Initialize(){
		//WIC工厂

		CoCreateInstance(CLSID_WICImagingFactory, P_Null,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&GE2d::lpWicFactory));

		//DWrite工厂

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory), (IUnknown**)&GE2d::lpDWFactory);

		//D2D工厂

		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&GE2d::lpD2dFactory));

		//D3D11设备

		D3D11On12CreateDevice(GE3d::lpD3dDevice, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			P_Null, 0, (IUnknown**)&GE3d::lpGfxQueue, 1, 0x0,
			&GE2d::lpD3d11Device, &GE2d::lpD3d11DevCtx, P_Null);

		//DXGI,D2D,D3D11On12设备

		GE2d::lpD3d11Device->QueryInterface(&GE2d::lpD3d11On12Dev);
		GE2d::lpD3d11On12Dev->QueryInterface(&GE2d::lpDxgiDevice);
		GE2d::lpD2dFactory->CreateDevice(GE2d::lpDxgiDevice, &GE2d::lpD2dDevice);
		GE2d::lpD2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &GE2d::lpD2dDevCtx);

		//笔刷

		GE2d::lpD2dDevCtx->CreateBitmapBrush(P_Null, &GE2d::lpBmpBrush);
		GE2d::lpD2dDevCtx->CreateSolidColorBrush(D2DXColor(1.f, 1.f, 1.f), &GE2d::lpPureBrush);
	}
	_open static $VOID EndDraw(ID3D11Resource *pBackBuff){
		GE2d::lpD2dDevCtx->EndDraw();
		GE2d::lpD3d11On12Dev->ReleaseWrappedResources(&pBackBuff, 1);
		GE2d::lpD3d11DevCtx->Flush();
	}
	_open static $VOID BeginDraw(ID3D11Resource *pBackBuff, ID2D1Bitmap1 *pCanvas){
		GE2d::lpD3d11On12Dev->AcquireWrappedResources(&pBackBuff, 1);
		GE2d::lpD2dDevCtx->SetTarget(pCanvas);
		GE2d::lpD2dDevCtx->BeginDraw();
	}
};

// Game Engine GUI(Context)
class GEGui{
	_open static $VOID Initialize(HWND hWindow){
		imgui::CreateContext();
		imgui::StyleColorsDark();
		imgui::SetCustomTextDrawing(P_Null);

		imgui::GetIO().IniFilename = P_Null;
		imgui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		imgui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		imgui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplWin32_Init(hWindow);
		ImGui_ImplDX11_Init(GE2d::lpD3d11Device, GE2d::lpD3d11DevCtx);
	}
	_open static $VOID EndDraw(ID3D11RenderTargetView *pBufView){
		GE2d::lpD3d11DevCtx->OMSetRenderTargets(1, &pBufView, P_Null);
		imgui::Render();
		ImGui_ImplDX11_RenderDrawData(imgui::GetDrawData());
		GE2d::lpD3d11DevCtx->Flush();
	}
	_open static $VOID BeginDraw(){
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		imgui::NewFrame();
	}
	_open static $VOID Release(){
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		imgui::DestroyContext();
	}
};

// Game Engine Command
class GECmd{
	/// 帧数
	_open UNS32 CurFrame;
	_open UNS32 MaxFrame;
	/// 围栏
	_secr UNS64 LastFenceVal;
	_secr UNS64 *lprgFenceVal;
	_secr HANDLE hrFenceEvent;
	_secr ID3D12Fence *lpFence;
	///命令
	_open ID3D12CommandQueue *pCmdQueue;
	_open ID3D12CommandAllocator *pCmdAllctr;
	_secr ID3D12CommandAllocator **lprgCmdAllctr;
	_open ID3D12GraphicsCommandList *lpCmdList;

	_open $VOID Initialize(UNS32 MaxFrame, D3D12_COMMAND_LIST_TYPE Type){
		$m.CurFrame = 0;
		$m.MaxFrame = MaxFrame;
		$m.LastFenceVal = 0;
		$m.lprgFenceVal = new UNS64[MaxFrame]{};
		$m.lprgCmdAllctr = new ID3D12CommandAllocator*[MaxFrame];
		$m.hrFenceEvent = CreateEventExW(P_Null, P_Null, 0L, EVENT_ALL_ACCESS);

		GE3d::lpD3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&$m.lpFence));
		GE3d::lpD3dDevice->CreateCommandList1(0x0, Type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&$m.lpCmdList));

		for(SIPTR iFrm = MaxFrame - 1; iFrm >= 0; --iFrm){
			GE3d::lpD3dDevice->CreateCommandAllocator(Type, IID_PPV_ARGS(&$m.pCmdAllctr));
			$m.lprgCmdAllctr[iFrm] = $m.pCmdAllctr;
		}

		if(Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			$m.pCmdQueue = GE3d::lpGfxQueue;
		else if(Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			$m.pCmdQueue = GE3d::lpGpcQueue;
		else if(Type == D3D12_COMMAND_LIST_TYPE_COPY)
			$m.pCmdQueue = GE3d::lpCpyQueue;
	}
	_open $VOID BeginCommand(){
		$m.pCmdAllctr->Reset();
		$m.lpCmdList->Reset($m.pCmdAllctr, P_Null);
	}
	_open $VOID EndCommand(){
		$m.lpCmdList->Close();
		$m.pCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&$m.lpCmdList);
	}
	_open $VOID WaitForGpu(){
		$m.LastFenceVal += 1;
		$m.pCmdQueue->Signal($m.lpFence, $m.LastFenceVal);

		if($m.lpFence->GetCompletedValue() < $m.LastFenceVal){
			$m.lpFence->SetEventOnCompletion($m.LastFenceVal, $m.hrFenceEvent);
			WaitForSingleObject($m.hrFenceEvent, UINT_MAX);
		}
	}
	_open $VOID BeginFrame(){
		UNS64 ProgVal = $m.lpFence->GetCompletedValue();
		UNS64 FenceVal = $m.lprgFenceVal[$m.CurFrame];

		if(ProgVal < FenceVal){
			$m.lpFence->SetEventOnCompletion(FenceVal, $m.hrFenceEvent);
			WaitForSingleObject($m.hrFenceEvent, UINT_MAX);
		}

		$m.BeginCommand();
	}
	_open $VOID EndFrame(){
		$m.EndCommand();

		$m.LastFenceVal += 1;
		$m.lprgFenceVal[$m.CurFrame] = $m.LastFenceVal;
		$m.pCmdQueue->Signal($m.lpFence, $m.LastFenceVal);

		$m.CurFrame += 1;
		$m.CurFrame %= $m.MaxFrame;
		$m.pCmdAllctr = $m.lprgCmdAllctr[$m.CurFrame];
	}
	_open $VOID Release(){
		for(UNS32 iFrm = 0; iFrm < $m.MaxFrame; ++iFrm)
			$m.lprgCmdAllctr[iFrm]->Release();

		SAFE_RELEASE($m.lpFence);
		SAFE_RELEASE($m.lpCmdList);

		SAFE_DELETEA($m.lprgFenceVal);
		SAFE_DELETEA($m.lprgCmdAllctr);

		SAFE_CLOSE($m.hrFenceEvent);
	}
};

// Game Engine Window
class GEWnd{
	//描述
	_open struct XDesc{
		HWND hWindow = P_Null;
		UNS32 Width = 0;
		UNS32 Height = 0;
		UNS32 RefreshRateD = 1;
		UNS32 RefreshRateN = 60;
		UNS32 MaxFrame = 2;
		IBOOL bWindowed = B_True; //设为TRUE更友好
		IBOOL bEnableD3d11 = B_False; //用于D2D&ImGui
		IBOOL bEnableIdBuf = B_False; //用于对象拾取等
		DXGI_FORMAT RtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};

	///尺寸
	_open UNS32 Width;  // 窗口宽
	_open UNS32 Height; // 窗口高
	///帧计数
	_open UNS32 MaxFrame; // 最大帧
	_secr UNS32 CurFrame; // 当前帧
	///ID缓存区(CPU)
	_open UNS32 *prgIdPixel; // ID缓存区(CPU)
	///视口&裁剪区
	_open D3D12_RECT rcScissor;    // 裁剪区
	_open D3D12_VIEWPORT Viewport; // 视口
	///缓存区视图
	_open D3D12_CPU_DESCRIPTOR_HANDLE hIBView; // ID缓存区视图
	_open D3D12_CPU_DESCRIPTOR_HANDLE hCBView; // 颜色缓存区视图
	_open D3D12_CPU_DESCRIPTOR_HANDLE hDSView; // 深度模板视图
	///缓存区视图堆
	_secr ID3D12DescriptorHeap *lpRtvHeap; // RTV堆
	_secr ID3D12DescriptorHeap *lpDsvHeap; // DSV堆
	///3D缓存区
	_open ID3D12Resource *lpRTBuffer; // 渲染目标缓存区
	_open ID3D12Resource *lpDSBuffer; // 深度模板缓存区
	///ID缓存区
	_secr ID3D12Resource *lpIdReadBuf;   // ID回读缓存区
	_open ID3D12Resource *pIdBuffer;     // 活跃ID缓存区
	_secr ID3D12Resource *rgIdBuffer[3]; // ID缓存区数组
	///2D缓存区
	_open ID2D1Bitmap1 *pD2dTarget;              //活跃D2D画布
	_secr ID2D1Bitmap1 *rgD2dTarget[3];          //D2D画布数组
	_open ID3D11Resource *pD3d11Buff;            //活跃D3D11缓存区
	_secr ID3D11Resource *rgD3d11Buff[3];        //D3D11缓存区数组
	_open ID3D11RenderTargetView *pD3d11Rtv;     //活跃D3D11RTV
	_secr ID3D11RenderTargetView *rgD3d11Rtv[3]; //D3D11RTV数组
	///交换链
	_open IDXGISwapChain4 *lpSwapChain; //交换链

	_open ~GEWnd(){
		$m.Release();
	}
	_open GEWnd(){
		Var_Zero(this);
	}
	_open GEWnd(_in XDesc &Desc){
		$m.Initialize(Desc);
	}
	//////
	_open $VOID Initialize(_in XDesc &Desc){
		Var_Zero(this);

		$m.Width = Desc.Width;
		$m.Height = Desc.Height;
		$m.MaxFrame = Desc.MaxFrame;

		$m.InitViewport(Desc);
		$m.InitViewHeaps(Desc);
		$m.InitSwapChain(Desc);
		$m.InitDepthStencilBuffer(Desc);

		if(Desc.bEnableIdBuf)
			$m.InitIdBuffer(Desc);
		if(Desc.bEnableD3d11)
			$m.InitD3d11Target(Desc);

		$m.FlipPage();
	}
	_secr $VOID InitViewport(_in XDesc &Desc){
		///视口设置

		$m.Viewport.Width = (SPFPN)Desc.Width;
		$m.Viewport.Height = (SPFPN)Desc.Height;
		$m.Viewport.MinDepth = D3D12_MIN_DEPTH;
		$m.Viewport.MaxDepth = D3D12_MAX_DEPTH;
		$m.Viewport.TopLeftX = 0.f;
		$m.Viewport.TopLeftY = 0.f;

		///裁剪区设置

		$m.rcScissor.top = 0;
		$m.rcScissor.left = 0;
		$m.rcScissor.right = Desc.Width;
		$m.rcScissor.bottom = Desc.Height;
	}
	_secr $VOID InitIdBuffer(_in XDesc &Desc){
		D3D12_RESOURCE_DESC BufDesc;
		BufDesc.Alignment = 0;
		BufDesc.MipLevels = 1;
		BufDesc.DepthOrArraySize = 1;
		BufDesc.SampleDesc.Count = 1;
		BufDesc.SampleDesc.Quality = 0;
		BufDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		BufDesc.Format = DXGI_FORMAT_R32_UINT;

		///渲染目标
		{
			BufDesc.Width = Desc.Width;
			BufDesc.Height = Desc.Height;
			BufDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			BufDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			for(UNS32 iBuf = 0; iBuf < Desc.MaxFrame; ++iBuf){
				GE3d::lpD3dDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
					&BufDesc, D3D12_RESOURCE_STATE_RENDER_TARGET,
					P_Null, IID_PPV_ARGS(&$m.rgIdBuffer[iBuf]));
			}
		}

		///回读堆
		{
			BufDesc.Height = 1;
			BufDesc.Width = Desc.Width * Desc.Height * sizeof(UNS32);
			BufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			BufDesc.Format = DXGI_FORMAT_UNKNOWN;
			BufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			BufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

			GE3d::lpD3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK), D3D12_HEAP_FLAG_NONE,
				&BufDesc, D3D12_RESOURCE_STATE_COPY_DEST,
				P_Null, IID_PPV_ARGS(&$m.lpIdReadBuf));

			$m.lpIdReadBuf->Map(0, P_Null, ($VOID**)&$m.prgIdPixel);
		}
	}
	_secr $VOID InitSwapChain(_in XDesc &Desc){
		DXGI_SWAP_CHAIN_DESC1 ChainDesc;

		ChainDesc.Flags = 0L;
		ChainDesc.Width = Desc.Width;
		ChainDesc.Height = Desc.Height;
		ChainDesc.Format = Desc.RtvFormat;
		ChainDesc.Stereo = B_False;
		ChainDesc.Scaling = DXGI_SCALING_STRETCH;
		ChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		ChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		ChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		ChainDesc.BufferCount = Desc.MaxFrame;
		ChainDesc.SampleDesc.Count = 1;
		ChainDesc.SampleDesc.Quality = 0;

		GE3d::lpDxgiFactory->CreateSwapChainForHwnd(
			GE3d::lpGfxQueue, Desc.hWindow, &ChainDesc,
			P_Null, P_Null, (IDXGISwapChain1**)&$m.lpSwapChain);

		$m.lpSwapChain->GetBuffer(0, IID_PPV_ARGS(&$m.lpRTBuffer));
	}
	_secr $VOID InitViewHeaps(_in XDesc &Desc){
		//渲染目标视图
		{
			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
			HeapDesc.NumDescriptors = 2;
			HeapDesc.NodeMask = 0x0;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

			GE3d::lpD3dDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&$m.lpRtvHeap));
			$m.hCBView = $m.lpRtvHeap->GetCPUDescriptorHandleForHeapStart();
			$m.hIBView = CD3DX12_CPU_DESCRIPTOR_HANDLE($m.hCBView, GE3d::cbRTView);
		}

		//深度模板视图
		{
			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;
			HeapDesc.NumDescriptors = 1;
			HeapDesc.NodeMask = 0x0;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

			GE3d::lpD3dDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&$m.lpDsvHeap));
			$m.hDSView = $m.lpDsvHeap->GetCPUDescriptorHandleForHeapStart();
		}
	}
	_secr $VOID InitD3d11Target(_in XDesc &Desc){
		SPFPN WndDpi = (SPFPN)GetDpiForWindow(Desc.hWindow);
		D3D11_RESOURCE_FLAGS BufFlags = { D3D11_BIND_RENDER_TARGET };
		D2D1_BITMAP_OPTIONS BmpOpt = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		D2D1_PIXEL_FORMAT BmpFormat = { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED };
		D2D1_BITMAP_PROPERTIES1 BmpProps = { BmpFormat, WndDpi, WndDpi, BmpOpt };

		IDXGISurface *lpSurface = P_Null;
		ID3D12Resource *lpBackBuff = P_Null;

		for(UNS32 iBuf = 0; iBuf < Desc.MaxFrame; ++iBuf){
			$m.lpSwapChain->GetBuffer(iBuf, IID_PPV_ARGS(&lpBackBuff));

			GE2d::lpD3d11On12Dev->CreateWrappedResource(lpBackBuff, &BufFlags,
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&$m.rgD3d11Buff[iBuf]));

			GE2d::lpD3d11Device->CreateRenderTargetView(
				$m.rgD3d11Buff[iBuf], P_Null, &$m.rgD3d11Rtv[iBuf]);

			$m.rgD3d11Buff[iBuf]->QueryInterface(&lpSurface);

			GE2d::lpD2dDevCtx->CreateBitmapFromDxgiSurface(
				lpSurface, &BmpProps, &$m.rgD2dTarget[iBuf]);

			SAFE_RELEASE(lpSurface);
			SAFE_RELEASE(lpBackBuff);
		}
	}
	_secr $VOID InitDepthStencilBuffer(_in XDesc &Desc){
		D3D12_RESOURCE_DESC BufDesc;
		D3D12_CLEAR_VALUE BufValue;

		BufDesc.Width = Desc.Width;
		BufDesc.Height = Desc.Height;
		BufDesc.Format = Desc.DsvFormat;
		BufDesc.Alignment = 0;
		BufDesc.MipLevels = 1;
		BufDesc.DepthOrArraySize = 1;
		BufDesc.SampleDesc.Count = 1;
		BufDesc.SampleDesc.Quality = 0;
		BufDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		BufDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		BufDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		BufValue.Format = Desc.DsvFormat;
		BufValue.DepthStencil.Depth = 1.f;
		BufValue.DepthStencil.Stencil = 0;

		GE3d::lpD3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&BufDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &BufValue, IID_PPV_ARGS(&$m.lpDSBuffer));

		GE3d::lpD3dDevice->CreateDepthStencilView($m.lpDSBuffer, P_Null, $m.hDSView);
	}
	_open $VOID ReadbackIdBuffer(ID3D12GraphicsCommandList *pCmdList){
		D3D12_TEXTURE_COPY_LOCATION Dest = {};
		D3D12_TEXTURE_COPY_LOCATION Source = {};

		Source.SubresourceIndex = 0;
		Source.pResource = $m.pIdBuffer;
		Source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		Dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		Dest.pResource = $m.lpIdReadBuf;
		Dest.PlacedFootprint.Footprint.Depth = 1;
		Dest.PlacedFootprint.Footprint.Width = $m.Width;
		Dest.PlacedFootprint.Footprint.Height = $m.Height;
		Dest.PlacedFootprint.Footprint.RowPitch = $m.Width * sizeof(UNS32);
		Dest.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R32_UINT;

		pCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition($m.pIdBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

		pCmdList->CopyTextureRegion(&Dest, 0, 0, 0, &Source, P_Null);

		pCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition($m.pIdBuffer,
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}
	_open $VOID ResizeWindow(_in XDesc &Desc){
		$m.Width = Desc.Width;
		$m.Height = Desc.Height;
		$m.MaxFrame = Desc.MaxFrame;

		$m.lpRTBuffer->Release();
		$m.lpDSBuffer->Release();

		$m.InitViewport(Desc);
		$m.InitDepthStencilBuffer(Desc);

		$m.lpSwapChain->ResizeBuffers(Desc.MaxFrame, Desc.Width, Desc.Height,
			Desc.RtvFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		$m.lpSwapChain->GetBuffer(0, IID_PPV_ARGS(&$m.lpRTBuffer));

		$m.FlipPage();
	}
	_open $VOID PresentBuffer(){
		$m.lpSwapChain->Present(0, 0L);
		$m.FlipPage();
	}
	_secr $VOID FlipPage(){
		$m.CurFrame = $m.lpSwapChain->GetCurrentBackBufferIndex();

		$m.pIdBuffer = $m.rgIdBuffer[$m.CurFrame];
		$m.pD3d11Rtv = $m.rgD3d11Rtv[$m.CurFrame];
		$m.pD3d11Buff = $m.rgD3d11Buff[$m.CurFrame];
		$m.pD2dTarget = $m.rgD2dTarget[$m.CurFrame];

		$m.lpRTBuffer->Release();
		$m.lpSwapChain->GetBuffer($m.CurFrame, IID_PPV_ARGS(&$m.lpRTBuffer));

		GE3d::lpD3dDevice->CreateRenderTargetView($m.lpRTBuffer, P_Null, $m.hCBView);
		if($m.pIdBuffer) GE3d::lpD3dDevice->CreateRenderTargetView($m.pIdBuffer, P_Null, $m.hIBView);
	}
	_open $VOID Release(){
		if(!$m.lpSwapChain) return;

		if($m.lpIdReadBuf){
			$m.lpIdReadBuf->Unmap(0, P_Null);
			$m.lpIdReadBuf->Release();
			$m.lpIdReadBuf = P_Null;
		}

		for(UNS32 iBuf = 0; iBuf < $m.MaxFrame; ++iBuf){
			SAFE_RELEASE($m.rgD2dTarget[iBuf]);
			SAFE_RELEASE($m.rgD3d11Buff[iBuf]);
			SAFE_RELEASE($m.rgD3d11Rtv[iBuf]);
			SAFE_RELEASE($m.rgIdBuffer[iBuf]);
		}

		SAFE_RELEASE($m.lpDsvHeap);
		SAFE_RELEASE($m.lpRtvHeap);
		SAFE_RELEASE($m.lpDSBuffer);
		SAFE_RELEASE($m.lpRTBuffer);
		SAFE_RELEASE($m.lpSwapChain);
	}
};

// Timer
class CTimer{
	_secr UNS64 BaseTime;
	_secr UNS64 PrevTime;
	_secr UNS64 LastTime;
	_secr UNS64 StopTime;
	_secr UNS64 LastSpan;
	_secr UNS64 StopSpan;
	_secr IBOOL bStopped;

	_open CTimer(){
		$m.Clear();
	}
	_open $VOID Stop(){
		if(!$m.bStopped){
			$m.LastSpan = 0;
			$m.bStopped = B_True;
			$m.StopTime = GEClk::GetTickCount();
		}
	}
	_open $VOID Start(){
		if($m.bStopped){
			$m.LastTime = GEClk::GetTickCount();
			$m.StopSpan += $m.LastTime - $m.StopTime;
			$m.bStopped = B_False;
		}
	}
	_open $VOID Clear(){
		$m.LastSpan = 0;
		$m.StopSpan = 0;
		$m.StopTime = 0;
		$m.BaseTime = GEClk::GetTickCount();
		$m.LastTime = $m.BaseTime;
		$m.PrevTime = $m.BaseTime;
		$m.bStopped = B_False;
	}
	_open $VOID Check(){
		if(!$m.bStopped){
			$m.PrevTime = $m.LastTime;
			$m.LastTime = GEClk::GetTickCount();
			$m.LastSpan = $m.LastTime - $m.PrevTime;
		}
	}
	_open UNS64 GetLastSpan(){
		return $m.LastSpan;
	}
	_open UNS64 GetTotalSpan(){
		UNS64 LastTime = $m.bStopped ? $m.StopTime : $m.LastTime;
		return LastTime - $m.BaseTime - $m.StopSpan;
	}
};

//----------------------------------------//


//-------------------- 3D上下文 --------------------//

///视图
UNS32 GE3d::cbRTView; //RTV字节数
UNS32 GE3d::cbDSView; //DSV字节数
UNS32 GE3d::cbSRView; //SRV字节数
///设备
ID3D12Device4 *GE3d::lpD3dDevice;   //D3D设备
IDXGIFactory6 *GE3d::lpDxgiFactory; //DXGI工厂
///命令
ID3D12CommandQueue *GE3d::lpGfxQueue; //图形队列
ID3D12CommandQueue *GE3d::lpGpcQueue; //计算队列
ID3D12CommandQueue *GE3d::lpCpyQueue; //复制队列

//----------------------------------------//


//-------------------- 2D上下文 --------------------//

/// WIC
IWICImagingFactory *GE2d::lpWicFactory; // WIC工厂
/// DWrite
IDWriteFactory *GE2d::lpDWFactory; // DWrite工厂
/// DXGI
IDXGIDevice *GE2d::lpDxgiDevice; // DXGI设备
/// D2D
ID2D1Device3 *GE2d::lpD2dDevice;       // D2D设备
ID2D1Factory4 *GE2d::lpD2dFactory;     // D2D工厂
ID2D1DeviceContext *GE2d::lpD2dDevCtx; // D2D设备上下文
/// D3D11
ID3D11Device *GE2d::lpD3d11Device;        // D3D11设备
ID3D11On12Device *GE2d::lpD3d11On12Dev;   // D3D11On12设备
ID3D11DeviceContext *GE2d::lpD3d11DevCtx; // D3D11设备上下文
/// 笔刷
ID2D1BitmapBrush1 *GE2d::lpBmpBrush;     // 位图笔刷
ID2D1SolidColorBrush *GE2d::lpPureBrush; // 纯色笔刷

//----------------------------------------//


//-------------------- 音频上下文 --------------------//

/// 设备
IXAudio2 *GEAud::lpXAFactory;            // XAudio工厂
IXAudio2MasteringVoice *GEAud::lpOutput; // 输出设备
/// 回调
GEAud::CDefCback *GEAud::lpCback;        // 默认回调

//----------------------------------------//


//-------------------- 输入上下文 --------------------//

/// 设备
IDirectInput8W *GEInp::lpDIFactory;      // DInput工厂
IDirectInputDevice8W *GEInp::lpMouse;    // 鼠标
IDirectInputDevice8W *GEInp::lpKeyboard; // 键盘
/// 设备状态
DIMOUSESTATE GEInp::MouseState;          // 鼠标状态
GEInp::NKeysState GEInp::KeysState;      // 按键状态

//----------------------------------------//


//-------------------- 物理上下文 --------------------//

btCollisionDispatcher *GEPhy::lpCollisDispr;             // 碰撞调度
btBroadphaseInterface *GEPhy::lpOverlapCache;            // 粗测接口
btDefaultCollisionConfiguration *GEPhy::lpCollisConfig;  // 碰撞配置
btSequentialImpulseConstraintSolver *GEPhy::lpSICSolver; // PGS解算器

//----------------------------------------//


//-------------------- 时钟上下文 --------------------//

DPFPN GEClk::TickPeriod; // 滴答周期

//----------------------------------------//


//-------------------- 进程上下文 --------------------//

MSG GEPrc::LastMsg;         // 进程消息
HANDLE GEPrc::hStdIn;       // 标准输入
HANDLE GEPrc::hStdOut;      // 标准输出
HINSTANCE GEPrc::hInstance; // 进程实例

//----------------------------------------//