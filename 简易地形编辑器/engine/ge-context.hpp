//-------------------- 引擎上下文 --------------------//

// Game Engine Process(Context)
class GEPrc{
	_open static MSG LastMsg;
	_open static HANDLE hStdInput;
	_open static HANDLE hStdOutput;
	_open static HINSTANCE hInstance;

	_open static $VOID InitCom(DWORD Flag){
		CoInitializeEx(P_Null, Flag);
	}
	_open static $VOID UninitCom(){
		CoUninitialize();
	}
	_open static $VOID CreateConsole(){
		AllocConsole();
		GEPrc::hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		GEPrc::hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	_open static $VOID DestroyConsole(){
		FreeConsole();
		GEPrc::hStdInput = P_Null;
		GEPrc::hStdOutput = P_Null;
	}
};

// Game Engine Clock(Context)
class GEClk{
	_open static DPFPN TickPeriod; //单位:秒

	_open static $VOID Initialize(){
		LARGEINT TickRate;
		QueryPerformanceFrequency(&TickRate);
		GEClk::TickPeriod = 1.0 / (DPFPN)TickRate.QuadPart;
	}
	_open static ULEXT GetTickCount(){
		LARGEINT TickCnt;
		QueryPerformanceCounter(&TickCnt);
		return TickCnt.QuadPart;
	}
	_open static DPFPN TicksToSeconds(ULEXT TickCnt){
		return GEClk::TickPeriod * TickCnt;
	}
	_open static DPFPN TicksToMillisecs(ULEXT TickCnt){
		return GEClk::TickPeriod * TickCnt * 1000.0;
	}
};

// Game Engine Physics(Context)
class GEPhy{
	_open static BPCollisDispr *lpCollisDispr;
	_open static BPBroadphaseIface *lpOverlapCache;
	_open static BPDefCollisConfig *lpCollisConfig;
	_open static BPSeqImpulseCstrSolver *lpSICSolver;

	_open static $VOID Initialize(){
		GEPhy::lpSICSolver = new BPSeqImpulseCstrSolver;
		GEPhy::lpOverlapCache = new BPDbvtBroadphase;
		GEPhy::lpCollisConfig = new BPDefCollisConfig;
		GEPhy::lpCollisDispr = new BPCollisDispr(lpCollisConfig);
	}
	_open static $VOID Finalize(){
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
	_open struct KEYSSTATE{
		SPFPN Timers[260];
		BYTET Flags[2][260];
		BYTET *psF0, *psF1;
	};

	_open static IDIDevice *lpMouse;
	_open static IDIDevice *lpKeyboard;
	_open static IDIFactory *lpDIFactory;
	_open static DI8MOUSESTATE MouseState;
	_open static KEYSSTATE KeysState;

	_open static $VOID Finalize(){
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
			for(USINT iExe = 0; iExe < 256; ++iExe){
				if(prgState[iExe] &= 0x80){
					if(!prgState1[iExe]) prgTimer[iExe] = 0.f;
					else prgTimer[iExe] += TimeSpan;
				}
			}
		}
		if(GEInp::lpMouse){
			if(GEInp::lpMouse->GetDeviceState(sizeof(DI8MOUSESTATE), &GEInp::MouseState) == DIERR_INPUTLOST){
				GEInp::lpMouse->Acquire();
				return;
			}
			for(USINT iExe = 256; iExe < 260; ++iExe){
				prgState[iExe] = GEInp::MouseState.rgbButtons[iExe - 256];
				if(prgState[iExe] &= 0x80){
					if(!prgState1[iExe]) prgTimer[iExe] = 0.f;
					else prgTimer[iExe] += TimeSpan;
				}
			}
		}
	}
	_open static IBOOL IsLoosed(USINT Key){
		return !GEInp::KeysState.psF0[Key];
	}
	_open static IBOOL IsPressed(USINT Key){
		return GEInp::KeysState.psF0[Key];
	}
	_open static IBOOL JustLoosed(USINT Key){
		return !GEInp::KeysState.psF0[Key] && GEInp::KeysState.psF1[Key];
	}
	_open static IBOOL JustPressed(USINT Key){
		return GEInp::KeysState.psF0[Key] && !GEInp::KeysState.psF1[Key];
	}
};

// Game Engine Audio(Context)
class GEAud{
	class CDefCback: public IXAVoiceCback{
		$VOID OnStreamEnd(){}
		$VOID OnLoopEnd(PVOID pBuffer){}
		$VOID OnBufferEnd(PVOID pBuffer){
			delete[] pBuffer;
		}
		$VOID OnBufferStart(PVOID pBuffer){}
		$VOID OnVoiceError(PVOID pBuffer, HRESULT ErrCode){}
		$VOID OnVoiceProcessingPassStart(USINT RequiredSize){}
		$VOID OnVoiceProcessingPassEnd(){}
	};

	_open static CDefCback *lpCback;
	_open static IXAOutput *lpOutput;
	_open static IXAFactory *lpXAFactory;

	_open static $VOID Initialize(){
		XAudio2Create(&GEAud::lpXAFactory, 0L, XAUDIO2_DEFAULT_PROCESSOR);
		GEAud::lpXAFactory->CreateMasteringVoice(&GEAud::lpOutput);
		GEAud::lpCback = new CDefCback;
	}
	_open static $VOID Finalize(){
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
	_open struct OPTIONS{
		IBOOL bUseWarp = B_False;
		IBOOL bNeedDxr = B_False;
		GPUBRAND GpuBrand = GPU_BRAND_None;
		DXGI1GPUPREFERENCE GpuPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
	};

	///视图
	_open static USINT cbRTView;
	_open static USINT cbDSView;
	_open static USINT cbSRView;
	///命令
	_open static ID3DCmdQueue *lpGfxQueue;
	_open static ID3DCmdQueue *lpGpcQueue;
	_open static ID3DCmdQueue *lpCpyQueue;
	///设备
	_open static ID3DDevice *lpD3dDevice;
	_open static IDXGIFactory6 *lpDxgiFactory;

	_open static $VOID Initialize(_in OPTIONS &Options = OPTIONS()){
		GE3d::InitDxgiFactory();
		GE3d::InitD3dDevice(Options);
		GE3d::InitViewByteCounts();
		GE3d::InitCommandQueues();
	}
	_secr static $VOID InitD3dDevice(_in OPTIONS &Options){
		HRESULT RetCode;
		DXGI1ADAPTERDESC1 AdaptDesc;

		ID3DDevice *lpDevice = P_Null;
		IDXGIAdapter4 *lpAdapter = P_Null;

		if(!Options.bUseWarp){
			for(USINT iExe = 0; !GE3d::lpD3dDevice; ++iExe){
				SAFE_RELEASE(lpDevice);
				SAFE_RELEASE(lpAdapter);

				RetCode = GE3d::lpDxgiFactory->EnumAdapterByGpuPreference(
					iExe, Options.GpuPref, IID_AND_PPV(lpAdapter));

				if(RetCode == DXGI_ERROR_NOT_FOUND)
					break;

				lpAdapter->GetDesc1(&AdaptDesc);

				if(AdaptDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if((Options.GpuBrand != GPU_BRAND_None) && (Options.GpuBrand != AdaptDesc.VendorId))
					continue;

				D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, IID_AND_PPV(lpDevice));

				if(Options.bNeedDxr && !CheckD3dRaytracingTier(lpDevice))
					continue;

				GE3d::lpD3dDevice = lpDevice;
				lpAdapter->Release();
			}
		}

		if(!GE3d::lpD3dDevice){
			GE3d::lpDxgiFactory->EnumWarpAdapter(IID_AND_PPV(lpAdapter));
			D3D12CreateDevice(lpAdapter, D3D_FEATURE_LEVEL_11_0, IID_AND_PPV(GE3d::lpD3dDevice));
			lpAdapter->Release();
		}
	}
	_secr static $VOID InitViewByteCounts(){
		GE3d::cbRTView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		GE3d::cbDSView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		GE3d::cbSRView = GE3d::lpD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	_secr static $VOID InitCommandQueues(){
		D3D12COMMANDQUEUEDESC QueDesc;
		QueDesc.NodeMask = 0x0;
		QueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_AND_PPV(GE3d::lpGfxQueue));

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_AND_PPV(GE3d::lpGpcQueue));

		QueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		GE3d::lpD3dDevice->CreateCommandQueue(&QueDesc, IID_AND_PPV(GE3d::lpCpyQueue));
	}
	_secr static $VOID InitDxgiFactory(){
		CreateDXGIFactory2(0L, IID_AND_PPV(GE3d::lpDxgiFactory));
	}
	_open static $VOID Finalize(){
		SAFE_RELEASE(GE3d::lpGfxQueue);
		SAFE_RELEASE(GE3d::lpGpcQueue);
		SAFE_RELEASE(GE3d::lpCpyQueue);
		SAFE_RELEASE(GE3d::lpD3dDevice);
		SAFE_RELEASE(GE3d::lpDxgiFactory);
	}
};

// Game Engine 2D(Context)
class GE2d{
	///设备
	_open static IDWFactory *lpDWFactory;
	_open static ID2DDevice *lpD2dDevice;
	_open static ID2DDevCtx *lpD2dDevCtx;
	_open static ID2DFactory *lpD2dFactory;
	_open static IWICFactory *lpWicFactory;
	_open static IDXGIDevice *lpDxgiDevice;
	_open static ID3D11Device *lpD3d11Device;
	_open static ID3D11DevCtx *lpD3d11DevCtx;
	_open static ID3D11On12Dev *lpD3d11On12Dev;
	///笔刷
	_open static ID2DBmpBrush *lpBmpBrush;
	_open static ID2DPureColorBrush *lpPureBrush;

	_open static $VOID Finalize(){
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
			CLSCTX_INPROC_SERVER, IID_AND_PPV(GE2d::lpWicFactory));

		//DWrite工厂

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory), (IUnknown**)&GE2d::lpDWFactory);

		//D2D工厂

		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_AND_PPV(GE2d::lpD2dFactory));

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
		GE2d::lpD2dDevCtx->CreateSolidColorBrush(D2DX1COLOR(1.f, 1.f, 1.f), &GE2d::lpPureBrush);
	}
	_open static $VOID EndDraw(ID3D11Resource *pBackBuff){
		GE2d::lpD2dDevCtx->EndDraw();
		GE2d::lpD3d11On12Dev->ReleaseWrappedResources(&pBackBuff, 1);
		GE2d::lpD3d11DevCtx->Flush();
	}
	_open static $VOID BeginDraw(ID3D11Resource *pBackBuff, ID2DBitmap *pCanvas){
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

		ImguiWinInit(hWindow);
		ImguiD3dInit(GE2d::lpD3d11Device, GE2d::lpD3d11DevCtx);
	}
	_open static $VOID EndDraw(ID3D11RtbView *pBufView){
		GE2d::lpD3d11DevCtx->OMSetRenderTargets(1, &pBufView, P_Null);
		imgui::Render();
		ImguiD3dRendering(imgui::GetDrawData());
		GE2d::lpD3d11DevCtx->Flush();
	}
	_open static $VOID BeginDraw(){
		ImguiD3dNewFrame();
		ImguiWinNewFrame();
		imgui::NewFrame();
	}
	_open static $VOID Finalize(){
		ImguiD3dShutdown();
		ImguiWinShutdown();
		imgui::DestroyContext();
	}
};

// Game Engine Command
class GECmd{
	///帧数
	_open USINT CurFrame;
	_open USINT MaxFrame;
	///围栏
	_secr ULEXT LastFenceVal;
	_secr ULEXT *lprgFenceVal;
	_secr HANDLE hrFenceEvent;
	_secr ID3DFence *lpFence;
	///命令
	_open ID3DCmdList1 *lpCmdList;
	_open ID3DCmdQueue *pCmdQueue;
	_open ID3DCmdAllctr *pCmdAllctr;
	_secr ID3DCmdAllctr **lprgCmdAllctr;

	_open $VOID Initialize(USINT MaxFrame, D3D12COMMANDLISTTYPE Type){
		$m.CurFrame = 0;
		$m.MaxFrame = MaxFrame;
		$m.LastFenceVal = 0;
		$m.lprgFenceVal = new ULEXT[MaxFrame]{};
		$m.lprgCmdAllctr = new ID3DCmdAllctr*[MaxFrame];
		$m.hrFenceEvent = CreateEventExW(P_Null, P_Null, 0L, EVENT_ALL_ACCESS);

		GE3d::lpD3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_AND_PPV($m.lpFence));
		GE3d::lpD3dDevice->CreateCommandList1(0x0, Type, D3D12_COMMAND_LIST_FLAG_NONE, IID_AND_PPV($m.lpCmdList));

		for(SPINT iFrame = MaxFrame - 1; iFrame >= 0; --iFrame){
			GE3d::lpD3dDevice->CreateCommandAllocator(Type, IID_AND_PPV($m.pCmdAllctr));
			$m.lprgCmdAllctr[iFrame] = $m.pCmdAllctr;
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
		$m.pCmdQueue->ExecuteCommandLists(1, (ID3DCmdList**)&$m.lpCmdList);
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
		ULEXT ProgVal = $m.lpFence->GetCompletedValue();
		ULEXT FenceVal = $m.lprgFenceVal[$m.CurFrame];

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
	_open $VOID Finalize(){
		for(USINT iFrame = 0; iFrame < $m.MaxFrame; ++iFrame)
			$m.lprgCmdAllctr[iFrame]->Release();

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
	_open struct DESC{
		HWND hWindow = P_Null;
		USINT Width = 0;
		USINT Height = 0;
		USINT RefreshRateD = 1;
		USINT RefreshRateN = 60;
		USINT MaxFrame = 2;
		IBOOL bWindowed = B_True; //设为TRUE更友好
		IBOOL bEnableD3d11 = B_False; //用于D2D&ImGui
		IBOOL bEnableIdBuf = B_False; //用于对象拾取等
		DXGI1FORMAT RtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI1FORMAT DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};

	///尺寸
	_open USINT Width;  //窗口宽
	_open USINT Height; //窗口高
	///帧计数
	_open USINT MaxFrame; //最大帧
	_secr USINT CurFrame; //当前帧
	///ID缓存区(CPU)
	_open USINT *prgIdPixel; //ID缓存区(CPU)
	///视口&裁剪区
	_open D3D12RECT rcScissor;    //裁剪区
	_open D3D12VIEWPORT Viewport; //视口
	///缓存区视图
	_open D3D12CPUVIEWPTR hIBView; //ID缓存区视图
	_open D3D12CPUVIEWPTR hCBView; //颜色缓存区视图
	_open D3D12CPUVIEWPTR hDSView; //深度模板视图
	///缓存区视图堆
	_secr ID3DViewHeap *lpRtvHeap; //RTV堆
	_secr ID3DViewHeap *lpDsvHeap; //DSV堆
	///3D缓存区
	_open ID3DResource *lpRTBuffer; //渲染目标缓存区
	_open ID3DResource *lpDSBuffer; //深度模板缓存区
	///ID缓存区
	_secr ID3DResource *lpIdReadBuf;   //ID回读缓存区
	_open ID3DResource *pIdBuffer;     //活跃ID缓存区
	_secr ID3DResource *rgIdBuffer[3]; //ID缓存区数组
	///2D缓存区
	_open ID2DBitmap *pD2dTarget;         //活跃D2D画布
	_secr ID2DBitmap *rgD2dTarget[3];     //D2D画布数组
	_open ID3D11RtbView *pD3d11Rtv;       //活跃D3D11RTV
	_secr ID3D11RtbView *rgD3d11Rtv[3];   //D3D11RTV数组
	_open ID3D11Resource *pD3d11Buff;     //活跃D3D11缓存区
	_secr ID3D11Resource *rgD3d11Buff[3]; //D3D11缓存区数组
	///交换链
	_open IDXGISwapChain4 *lpSwapChain;  //交换链

	_open ~GEWnd(){
		$m.Finalize();
	}
	_open GEWnd(){
		Var_Zero(this);
	}
	_open GEWnd(_in DESC &Desc){
		$m.Initialize(Desc);
	}
	//////
	_open $VOID Initialize(_in DESC &Desc){
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
	_secr $VOID InitViewport(_in DESC &Desc){
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
	_secr $VOID InitIdBuffer(_in DESC &Desc){
		D3D12RESDESC BufDesc;
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

			for(USINT iBuf = 0; iBuf < Desc.MaxFrame; ++iBuf){
				GE3d::lpD3dDevice->CreateCommittedResource(
					&D3DX12HEAPPROPS(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
					&BufDesc, D3D12_RESOURCE_STATE_RENDER_TARGET,
					P_Null, IID_AND_PPV($m.rgIdBuffer[iBuf]));
			}
		}

		///回读堆
		{
			BufDesc.Height = 1;
			BufDesc.Width = Desc.Width * Desc.Height * sizeof(USINT);
			BufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			BufDesc.Format = DXGI_FORMAT_UNKNOWN;
			BufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			BufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

			GE3d::lpD3dDevice->CreateCommittedResource(
				&D3DX12HEAPPROPS(D3D12_HEAP_TYPE_READBACK), D3D12_HEAP_FLAG_NONE,
				&BufDesc, D3D12_RESOURCE_STATE_COPY_DEST,
				P_Null, IID_AND_PPV($m.lpIdReadBuf));

			$m.lpIdReadBuf->Map(0, P_Null, ($VOID**)&$m.prgIdPixel);
		}
	}
	_secr $VOID InitSwapChain(_in DESC &Desc){
		DXGI1SWAPCHAINDESC1 ChainDesc;

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

		$m.lpSwapChain->GetBuffer(0, IID_AND_PPV($m.lpRTBuffer));
	}
	_secr $VOID InitViewHeaps(_in DESC &Desc){
		//渲染目标视图
		{
			D3D12VIEWHEAPDESC HeapDesc;
			HeapDesc.NumDescriptors = 2;
			HeapDesc.NodeMask = 0x0;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

			GE3d::lpD3dDevice->CreateDescriptorHeap(&HeapDesc, IID_AND_PPV($m.lpRtvHeap));
			$m.hCBView = $m.lpRtvHeap->GetCPUDescriptorHandleForHeapStart();
			$m.hIBView = D3DX12CPUVIEWPTR($m.hCBView, GE3d::cbRTView);
		}

		//深度模板视图
		{
			D3D12VIEWHEAPDESC HeapDesc;
			HeapDesc.NumDescriptors = 1;
			HeapDesc.NodeMask = 0x0;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

			GE3d::lpD3dDevice->CreateDescriptorHeap(&HeapDesc, IID_AND_PPV($m.lpDsvHeap));
			$m.hDSView = $m.lpDsvHeap->GetCPUDescriptorHandleForHeapStart();
		}
	}
	_secr $VOID InitD3d11Target(_in DESC &Desc){
		SPFPN WndDpi = (SPFPN)GetDpiForWindow(Desc.hWindow);
		D3D11RESFLAGS BufFlags = { D3D11_BIND_RENDER_TARGET };
		D2D1BITMAPOPT BmpOpt = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		D2D1PIXELFORMAT BmpFormat = { DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED };
		D2D1BITMAPPROPS BmpProps = { BmpFormat, WndDpi, WndDpi, BmpOpt };

		IDXGISurface *lpSurface = P_Null;
		ID3DResource *lpBackBuff = P_Null;

		for(USINT iBuf = 0; iBuf < Desc.MaxFrame; ++iBuf){
			$m.lpSwapChain->GetBuffer(iBuf, IID_AND_PPV(lpBackBuff));

			GE2d::lpD3d11On12Dev->CreateWrappedResource(lpBackBuff, &BufFlags,
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
				IID_AND_PPV($m.rgD3d11Buff[iBuf]));

			GE2d::lpD3d11Device->CreateRenderTargetView(
				$m.rgD3d11Buff[iBuf], P_Null, &$m.rgD3d11Rtv[iBuf]);

			$m.rgD3d11Buff[iBuf]->QueryInterface(&lpSurface);

			GE2d::lpD2dDevCtx->CreateBitmapFromDxgiSurface(
				lpSurface, &BmpProps, &$m.rgD2dTarget[iBuf]);

			SAFE_RELEASE(lpSurface);
			SAFE_RELEASE(lpBackBuff);
		}
	}
	_secr $VOID InitDepthStencilBuffer(_in DESC &Desc){
		D3D12RESDESC BufDesc;
		D3D12CLEARVAL BufValue;

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
			&D3DX12HEAPPROPS(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&BufDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &BufValue, IID_AND_PPV($m.lpDSBuffer));

		GE3d::lpD3dDevice->CreateDepthStencilView($m.lpDSBuffer, P_Null, $m.hDSView);
	}
	_open $VOID ReadbackIdBuffer(ID3DCmdList1 *pCmdList){
		D3D12TEXLOCATION Dest = {};
		D3D12TEXLOCATION Source = {};

		Source.SubresourceIndex = 0;
		Source.pResource = $m.pIdBuffer;
		Source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		Dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		Dest.pResource = $m.lpIdReadBuf;
		Dest.PlacedFootprint.Footprint.Depth = 1;
		Dest.PlacedFootprint.Footprint.Width = $m.Width;
		Dest.PlacedFootprint.Footprint.Height = $m.Height;
		Dest.PlacedFootprint.Footprint.RowPitch = $m.Width * sizeof(USINT);
		Dest.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R32_UINT;

		pCmdList->ResourceBarrier(1, &D3DX12RESBARRIER::Transition($m.pIdBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

		pCmdList->CopyTextureRegion(&Dest, 0, 0, 0, &Source, P_Null);

		pCmdList->ResourceBarrier(1, &D3DX12RESBARRIER::Transition($m.pIdBuffer,
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}
	_open $VOID ResizeWindow(_in DESC &Desc){
		$m.Width = Desc.Width;
		$m.Height = Desc.Height;
		$m.MaxFrame = Desc.MaxFrame;

		$m.lpRTBuffer->Release();
		$m.lpDSBuffer->Release();

		$m.InitViewport(Desc);
		$m.InitDepthStencilBuffer(Desc);

		$m.lpSwapChain->ResizeBuffers(Desc.MaxFrame, Desc.Width, Desc.Height,
			Desc.RtvFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		$m.lpSwapChain->GetBuffer(0, IID_AND_PPV($m.lpRTBuffer));

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
		$m.lpSwapChain->GetBuffer($m.CurFrame, IID_AND_PPV($m.lpRTBuffer));

		GE3d::lpD3dDevice->CreateRenderTargetView($m.lpRTBuffer, P_Null, $m.hCBView);
		if($m.pIdBuffer) GE3d::lpD3dDevice->CreateRenderTargetView($m.pIdBuffer, P_Null, $m.hIBView);
	}
	_open $VOID Finalize(){
		if(!$m.lpSwapChain) return;

		if($m.lpIdReadBuf){
			$m.lpIdReadBuf->Unmap(0, P_Null);
			$m.lpIdReadBuf->Release();
			$m.lpIdReadBuf = P_Null;
		}

		for(USINT iBuf = 0; iBuf < $m.MaxFrame; ++iBuf){
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
	_secr ULEXT BaseTime;
	_secr ULEXT PrevTime;
	_secr ULEXT LastTime;
	_secr ULEXT StopTime;
	_secr ULEXT LastSpan;
	_secr ULEXT StopSpan;
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
	_open ULEXT GetLastSpan(){
		return $m.LastSpan;
	}
	_open ULEXT GetTotalSpan(){
		ULEXT LastTime = $m.bStopped ? $m.StopTime : $m.LastTime;
		return LastTime - $m.BaseTime - $m.StopSpan;
	}
};

//----------------------------------------//


//-------------------- 3D上下文 --------------------//

///视图
USINT GE3d::cbRTView; //RTV字节数
USINT GE3d::cbDSView; //DSV字节数
USINT GE3d::cbSRView; //SRV字节数
///命令
ID3DCmdQueue *GE3d::lpGfxQueue; //图形队列
ID3DCmdQueue *GE3d::lpGpcQueue; //计算队列
ID3DCmdQueue *GE3d::lpCpyQueue; //复制队列
///设备
ID3DDevice *GE3d::lpD3dDevice;      //D3D设备
IDXGIFactory6 *GE3d::lpDxgiFactory; //DXGI工厂

//----------------------------------------//


//-------------------- 2D上下文 --------------------//

///DWrite
IDWFactory *GE2d::lpDWFactory; //DWrite工厂
///WIC
IWICFactory *GE2d::lpWicFactory; //WIC工厂
///DXGI
IDXGIDevice *GE2d::lpDxgiDevice; //DXGI设备
///D2D
ID2DDevCtx *GE2d::lpD2dDevCtx;   //D2D设备上下文
ID2DDevice *GE2d::lpD2dDevice;   //D2D设备
ID2DFactory *GE2d::lpD2dFactory; //D2D工厂
///D3D11
ID3D11DevCtx *GE2d::lpD3d11DevCtx;   //D3D11设备上下文
ID3D11Device *GE2d::lpD3d11Device;   //D3D11设备
ID3D11On12Dev *GE2d::lpD3d11On12Dev; //D3D11On12设备
///笔刷
ID2DBmpBrush *GE2d::lpBmpBrush;        //位图笔刷
ID2DPureColorBrush *GE2d::lpPureBrush; //纯色笔刷

//----------------------------------------//


//-------------------- 音频上下文 --------------------//

IXAOutput *GEAud::lpOutput;       //输出设备
IXAFactory *GEAud::lpXAFactory;   //XAudio工厂
GEAud::CDefCback *GEAud::lpCback; //默认回调

//----------------------------------------//


//-------------------- 输入上下文 --------------------//

IDIDevice *GEInp::lpMouse;         //鼠标
IDIDevice *GEInp::lpKeyboard;      //键盘
IDIFactory *GEInp::lpDIFactory;    //DInput工厂
DI8MOUSESTATE GEInp::MouseState;   //鼠标状态
GEInp::KEYSSTATE GEInp::KeysState; //按键状态

//----------------------------------------//


//-------------------- 物理上下文 --------------------//

BPCollisDispr *GEPhy::lpCollisDispr;        //碰撞调度
BPDefCollisConfig *GEPhy::lpCollisConfig;   //碰撞配置
BPBroadphaseIface *GEPhy::lpOverlapCache;   //粗测接口
BPSeqImpulseCstrSolver *GEPhy::lpSICSolver; //PGS解算器

//----------------------------------------//


//-------------------- 时钟上下文 --------------------//

DPFPN GEClk::TickPeriod; //滴答周期

//----------------------------------------//


//-------------------- 进程上下文 --------------------//

MSG GEPrc::LastMsg;         //进程消息
HANDLE GEPrc::hStdInput;    //标准输入
HANDLE GEPrc::hStdOutput;   //标准输出
HINSTANCE GEPrc::hInstance; //进程实例

//----------------------------------------//