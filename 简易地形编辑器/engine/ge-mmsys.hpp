//-------------------- DirectX命名空间 --------------------//

namespace d2d = D2D1;
namespace dx = DirectX;
namespace dxlh = DirectX::LoaderHelpers;

//----------------------------------------//


//-------------------- DirectX数据类型 --------------------//

typedef D2D1::ColorF     D2DX1COLOR;
typedef D2D1::Matrix3x2F D2DX1MATRIX;

typedef DirectX::WAVData WAVVIEW;
typedef DirectX::CommonStates        DXTKCommStates;
typedef DirectX::GeometricPrimitive  DXTKGeomPrim;
typedef DirectX::ResourceUploadBatch DXTKUploader;

typedef WICDecodeOptions           WIC1DECODEOPT;
typedef WICBitmapDitherType        WIC1DITHERTYPE;
typedef WICBitmapPaletteType       WIC1PALETTETYPE;
typedef WICBitmapCreateCacheOption WIC1BITMAPCACHEOPT;

typedef XAUDIO2_BUFFER XA2BUFFER;
typedef DWRITE_TEXT_METRICS DW1TEXTMETRICS;

typedef DIMOUSESTATE      DI8MOUSESTATE;
typedef DIPROPHEADER      DI8PROPHEADER;
typedef DIPROPCAL         DI8CALPROP;
typedef DIPROPDWORD       DI8DWORDPROP;
typedef DIPROPRANGE       DI8RANGEPROP;
typedef DIPROPCALPOV      DI8CALPOVPROP;
typedef DIPROPSTRING      DI8STRINGPROP;
typedef DIPROPCPOINTS     DI8CPOINTSPROP;
typedef DIPROPPOINTER     DI8POINTERPROP;
typedef DIPROPGUIDANDPATH DI8GUIDANDPATHPROP;

typedef DXGI_FORMAT           DXGI1FORMAT;
typedef DXGI_MODE_DESC        DXGI1MODEDESC;
typedef DXGI_SAMPLE_DESC      DXGI1SAMPLEDESC;
typedef DXGI_OUTPUT_DESC      DXGI1OUTPUTDESC;
typedef DXGI_ADAPTER_DESC     DXGI1ADAPTERDESC;
typedef DXGI_ADAPTER_DESC1    DXGI1ADAPTERDESC1;
typedef DXGI_ADAPTER_DESC2    DXGI1ADAPTERDESC2;
typedef DXGI_ADAPTER_DESC3    DXGI1ADAPTERDESC3;
typedef DXGI_GPU_PREFERENCE   DXGI1GPUPREFERENCE;
typedef DXGI_SWAP_CHAIN_DESC  DXGI1SWAPCHAINDESC;
typedef DXGI_SWAP_CHAIN_DESC1 DXGI1SWAPCHAINDESC1;

typedef D2D1_SIZE_F   D2D1SIZEF;
typedef D2D1_SIZE_U   D2D1SIZEU;
typedef D2D1_RECT_F   D2D1RECTF;
typedef D2D1_RECT_U   D2D1RECTU;
typedef D2D1_COLOR_F  D2D1COLOR;
typedef D2D1_POINT_2F D2D1POINTF;
typedef D2D1_POINT_2U D2D1POINTU;
typedef D2D1_MAPPED_RECT                      D2D1MAPPEDMAT;
typedef D2D1_ROUNDED_RECT                     D2D1ROUNDEDRECT;
typedef D2D1_PIXEL_FORMAT                     D2D1PIXELFORMAT;
typedef D2D1_GRADIENT_STOP                    D2D1GRADIENTSTOP;
typedef D2D1_BITMAP_OPTIONS                   D2D1BITMAPOPT;
typedef D2D1_BITMAP_PROPERTIES1               D2D1BITMAPPROPS;
typedef D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES D2D1LGBRUSHPROPS;
typedef D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES D2D1RGBRUSHPROPS;

typedef D3D_SHADER_MACRO           D3D1SHADERMACRO;
typedef D3D_SHADER_MODEL           D3D1SHADERMODEL;
typedef D3D_FEATURE_LEVEL          D3D1FEATURELEVEL;
typedef D3D_PRIMITIVE_TOPOLOGY     D3D1PRIMTOPO;
typedef D3D_ROOT_SIGNATURE_VERSION D3D1ROOTSIGVERSION;

typedef D3D11_RESOURCE_FLAGS D3D11RESFLAGS;

typedef D3D12_FEATURE                                 D3D12FEATURETYPE;
typedef D3D12_BLEND                                   D3D12BLENDMODE;
typedef D3D12_BLEND_OP                                D3D12BLENDOPR;
typedef D3D12_LOGIC_OP                                D3D12LOGICOPR;
typedef D3D12_STENCIL_OP                              D3D12STENCILOPR;
typedef D3D12_HEAP_FLAGS                              D3D12HEAPFLAG;
typedef D3D12_FENCE_FLAGS                             D3D12FENCEFLAG;
typedef D3D12_RESOURCE_FLAGS                          D3D12RESFLAG;
typedef D3D12_RESOURCE_STATES                         D3D12RESSTATE;
typedef D3D12_COMPARISON_FUNC                         D3D12COMPARISONMODE;
typedef D3D12_RAYTRACING_TIER                         D3D12RAYTRACINGTIER;
typedef D3D12_RENDER_PASS_TIER                        D3D12RENDERPASSTIER;
typedef D3D12_COMMAND_LIST_TYPE                       D3D12COMMANDLISTTYPE;
typedef D3D12_ROOT_SIGNATURE_FLAGS                    D3D12ROOTSIGFLAG;
typedef D3D12_TEXTURE_ADDRESS_MODE                    D3D12TEXADDRMODE;
typedef D3D12_PRIMITIVE_TOPOLOGY_TYPE                 D3D12PRIMTYPE;
typedef D3D12_GPU_VIRTUAL_ADDRESS                     HD3D12GPURES;
typedef D3D12_BOX                                     D3D12BOX;
typedef D3D12_RECT                                    D3D12RECT;
typedef D3D12_RANGE                                   D3D12RANGE;
typedef D3D12_VIEWPORT                                D3D12VIEWPORT;
typedef D3D12_CLEAR_VALUE                             D3D12CLEARVAL;
typedef D3D12_HEAP_PROPERTIES                         D3D12HEAPPROPS;
typedef D3D12_SHADER_BYTECODE                         D3D12SHADERBYTECODE;
typedef D3D12_SUBRESOURCE_DATA                        D3D12SUBRESDATA;
typedef D3D12_RESOURCE_BARRIER                        D3D12RESBARRIER;
typedef D3D12_DESCRIPTOR_RANGE                        D3D12VIEWRANGE;
typedef D3D12_DESCRIPTOR_RANGE1                       D3D12VIEWRANGE1;
typedef D3D12_INDEX_BUFFER_VIEW                       D3D12IBVIEW;
typedef D3D12_VERTEX_BUFFER_VIEW                      D3D12VBVIEW;
typedef D3D12_SUBRESOURCE_FOOTPRINT                   D3D12SUBRESLAYOUT;
typedef D3D12_TEXTURE_COPY_LOCATION                   D3D12TEXLOCATION;
typedef D3D12_PLACED_SUBRESOURCE_FOOTPRINT            D3D12PLACEDSUBRESLAYOUT;
typedef D3D12_BLEND_DESC                              D3D12BLENDDESC;
typedef D3D12_SAMPLER_DESC                            D3D12SAMPLERDESC;
typedef D3D12_RASTERIZER_DESC                         D3D12RASTERIZERDESC;
typedef D3D12_COMMAND_QUEUE_DESC                      D3D12COMMANDQUEUEDESC;
typedef D3D12_STATIC_SAMPLER_DESC                     D3D12SSAMPLERDESC;
typedef D3D12_DESCRIPTOR_HEAP_DESC                    D3D12VIEWHEAPDESC;
typedef D3D12_RENDER_TARGET_BLEND_DESC                D3D12BUFBLENDDESC;
typedef D3D12_RESOURCE_DESC                           D3D12RESDESC;
typedef D3D12_RESOURCE_DESC1                          D3D12RESDESC1;
typedef D3D12_INPUT_LAYOUT_DESC                       D3D12INPUTLAYOUTDESC;
typedef D3D12_INPUT_ELEMENT_DESC                      D3D12INPUTELEMDESC;
typedef D3D12_DEPTH_STENCIL_DESC                      D3D12DEPTHSTENCILDESC;
typedef D3D12_DEPTH_STENCIL_DESC1                     D3D12DEPTHSTENCILDESC1;
typedef D3D12_ROOT_SIGNATURE_DESC                     D3D12ROOTSIGDESC;
typedef D3D12_ROOT_SIGNATURE_DESC1                    D3D12ROOTSIGDESC1;
typedef D3D12_DEPTH_STENCIL_VIEW_DESC                 D3D12DSVDESC;
typedef D3D12_RENDER_TARGET_VIEW_DESC                 D3D12RTVDESC;
typedef D3D12_CONSTANT_BUFFER_VIEW_DESC               D3D12CBVDESC;
typedef D3D12_SHADER_RESOURCE_VIEW_DESC               D3D12SRVDESC;
typedef D3D12_UNORDERED_ACCESS_VIEW_DESC              D3D12UAVDESC;
typedef D3D12_COMPUTE_PIPELINE_STATE_DESC             D3D12COMPUTEPIPEDESC;
typedef D3D12_GRAPHICS_PIPELINE_STATE_DESC            D3D12GRAPHICSPIPEDESC;
typedef D3D12_VERSIONED_ROOT_SIGNATURE_DESC           D3D12VERSIONEDROOTSIGDESC;
typedef D3D12_ROOT_CONSTANTS                          D3D12ROOTCONST;
typedef D3D12_ROOT_PARAMETER                          D3D12ROOTPARAM;
typedef D3D12_ROOT_PARAMETER1                         D3D12ROOTPARAM1;
typedef D3D12_ROOT_DESCRIPTOR                         D3D12ROOTVIEW;
typedef D3D12_ROOT_DESCRIPTOR1                        D3D12ROOTVIEW1;
typedef D3D12_ROOT_DESCRIPTOR_TABLE                   D3D12ROOTVIEWTABLE;
typedef D3D12_ROOT_DESCRIPTOR_TABLE1                  D3D12ROOTVIEWTABLE1;
typedef D3D12_CPU_DESCRIPTOR_HANDLE                   D3D12CPUVIEWPTR;
typedef D3D12_GPU_DESCRIPTOR_HANDLE                   D3D12GPUVIEWPTR;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS              D3D12OPTIONSFEATURE;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS1             D3D12OPTIONSFEATURE1;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS2             D3D12OPTIONSFEATURE2;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS3             D3D12OPTIONSFEATURE3;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS4             D3D12OPTIONSFEATURE4;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS5             D3D12OPTIONSFEATURE5;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS6             D3D12OPTIONSFEATURE6;
typedef D3D12_FEATURE_DATA_D3D12_OPTIONS7             D3D12OPTIONSFEATURE7;
typedef D3D12_FEATURE_DATA_SHADER_MODEL               D3D12SHADERFEATURE;
typedef D3D12_FEATURE_DATA_FEATURE_LEVELS             D3D12LEVELFEATURE;
typedef D3D12_FEATURE_DATA_ROOT_SIGNATURE             D3D12ROOTSIGFEATURE;
typedef D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS D3D12MULTISAMPLEFEATURE;

typedef CD3DX12_BOX                           D3DX12BOX;
typedef CD3DX12_RECT                          D3DX12RECT;
typedef CD3DX12_RANGE                         D3DX12RANGE;
typedef CD3DX12_VIEWPORT                      D3DX12VIEWPORT;
typedef CD3DX12_CLEAR_VALUE                   D3DX12CLEARVAL;
typedef CD3DX12_HEAP_PROPERTIES               D3DX12HEAPPROPS;
typedef CD3DX12_SHADER_BYTECODE               D3DX12SHADERBYTECODE;
typedef CD3DX12_RESOURCE_BARRIER              D3DX12RESBARRIER;
typedef CD3DX12_DESCRIPTOR_RANGE              D3DX12VIEWRANGE;
typedef CD3DX12_DESCRIPTOR_RANGE1             D3DX12VIEWRANGE1;
typedef CD3DX12_SUBRESOURCE_FOOTPRINT         D3DX12SUBRESLAYOUT;
typedef CD3DX12_TEXTURE_COPY_LOCATION         D3DX12TEXLOCATION;
typedef CD3DX12_BLEND_DESC                    D3DX12BLENDDESC;
typedef CD3DX12_RESOURCE_DESC                 D3DX12RESDESC;
typedef CD3DX12_RESOURCE_DESC1                D3DX12RESDESC1;
typedef CD3DX12_RASTERIZER_DESC               D3DX12RASTERIZERDESC;
typedef CD3DX12_DEPTH_STENCIL_DESC            D3DX12DEPTHSTENCILDESC;
typedef CD3DX12_DEPTH_STENCIL_DESC1           D3DX12DEPTHSTENCILDESC1;
typedef CD3DX12_STATIC_SAMPLER_DESC           D3DX12SSAMPLERDESC;
typedef CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC D3DX12VERSIONEDROOTSIGDESC;
typedef CD3DX12_ROOT_SIGNATURE_DESC           D3DX12ROOTSIGDESC;
typedef CD3DX12_ROOT_CONSTANTS                D3DX12ROOTCONST;
typedef CD3DX12_ROOT_PARAMETER                D3DX12ROOTPARAM;
typedef CD3DX12_ROOT_PARAMETER1               D3DX12ROOTPARAM1;
typedef CD3DX12_ROOT_DESCRIPTOR               D3DX12ROOTVIEW;
typedef CD3DX12_ROOT_DESCRIPTOR1              D3DX12ROOTVIEW1;
typedef CD3DX12_ROOT_DESCRIPTOR_TABLE         D3DX12ROOTVIEWTABLE;
typedef CD3DX12_ROOT_DESCRIPTOR_TABLE1        D3DX12ROOTVIEWTABLE1;
typedef CD3DX12_CPU_DESCRIPTOR_HANDLE         D3DX12CPUVIEWPTR;
typedef CD3DX12_GPU_DESCRIPTOR_HANDLE         D3DX12GPUVIEWPTR;

typedef ID3D12Fence               ID3DFence;
typedef ID3D12Device4             ID3DDevice;
typedef ID3D12Pageable            ID3DPageable;
typedef ID3D12Resource            ID3DResource;
typedef ID3D12PipelineState       ID3DPipeState;
typedef ID3D12RootSignature       ID3DRootSig;
typedef ID3D12DescriptorHeap      ID3DViewHeap;
typedef ID3D12CommandList         ID3DCmdList;
typedef ID3D12CommandQueue        ID3DCmdQueue;
typedef ID3D12CommandAllocator    ID3DCmdAllctr;
typedef ID3D12GraphicsCommandList ID3DCmdList1;

typedef ID3D11On12Device       ID3D11On12Dev;
typedef ID3D11DeviceContext    ID3D11DevCtx;
typedef ID3D11DepthStencilView ID3D11DsbView;
typedef ID3D11RenderTargetView ID3D11RtbView;

typedef ID2D1Bitmap1                ID2DBitmap;
typedef ID2D1Device3                ID2DDevice;
typedef ID2D1Factory4               ID2DFactory;
typedef ID2D1RenderTarget           ID2DRenderTarget;
typedef ID2D1DeviceContext          ID2DDevCtx;
typedef ID2D1GradientStopCollection ID2DGradientStops;
typedef ID2D1Brush                  ID2DBrush;
typedef ID2D1ImageBrush             ID2DImgBrush;
typedef ID2D1BitmapBrush1           ID2DBmpBrush;
typedef ID2D1SolidColorBrush        ID2DPureColorBrush;
typedef ID2D1LinearGradientBrush    ID2DLinearGradientBrush;
typedef ID2D1RadialGradientBrush    ID2DRadialGradientBrush;

typedef IWICPalette           IWICPalette;
typedef IWICBitmap            IWICBitmap;
typedef IWICBitmapLock        IWICBmpLock;
typedef IWICImagingFactory    IWICFactory;
typedef IWICFormatConverter   IWICFmtConvter;
typedef IWICBitmapDecoder     IWICBmpDecoder;
typedef IWICBitmapFrameDecode IWICBmpFrameDecoder;

typedef IXAudio2               IXAFactory;
typedef IXAudio2SourceVoice    IXASource;
typedef IXAudio2SubmixVoice    IXASubmix;
typedef IXAudio2MasteringVoice IXAOutput;
typedef IXAudio2VoiceCallback  IXAVoiceCback;
typedef IXAudio2EngineCallback IXAEngineCback;

typedef IDWriteFactory          IDWFactory;
typedef IDWriteFont             IDWFont;
typedef IDWriteFontFace         IDWFontFace;
typedef IDWriteFontFamily       IDWFontFamily;
typedef IDWriteFontCollection   IDWFontColl;
typedef IDWriteTextFormat       IDWTxtFormat;
typedef IDWriteTextLayout       IDWTxtLayout;
typedef IDWriteLocalizedStrings IDWLocalStrings;

typedef IDirectInput8       IDIFactory;
typedef IDirectInputDevice8 IDIDevice;

//----------------------------------------//


//-------------------- Windows数据类型 --------------------//

typedef LARGE_INTEGER  LARGEINT;
typedef ULARGE_INTEGER ULARGEINT;
typedef WIN32_FIND_DATA  FINDDATA;
typedef WIN32_FIND_DATAA FINDDATAA;
typedef WIN32_FIND_DATAW FINDDATAW;

//----------------------------------------//


//-------------------- GPU资源驻留 --------------------//

//驻留GPU资源
inline $VOID ResidentGpuResource(ID3DDevice *pD3dDevice, ID3DPageable *pObject){
	pD3dDevice->MakeResident(1, &pObject);
}

//驱逐GPU资源
inline $VOID EvictGpuResource(ID3DDevice *pD3dDevice, ID3DPageable *pObject){
	pD3dDevice->Evict(1, &pObject);
}

//----------------------------------------//


//-------------------- 系统提示框 --------------------//

//弹出提示
$VOID PopupPromptA(HWND hWindow, _in CHAR8 *psFormat, ...){
	PVALIST pArgs;
	CHAR8 Text[256];

	VA_START(pArgs, psFormat);
	Avnprintf(Text, 256, psFormat, pArgs);
	VA_END(pArgs);
	MessageBoxA(hWindow, Text, P_Null, MB_OK);
}

//弹出提示(宽字符)
$VOID PopupPromptW(HWND hWindow, _in WCHAR *psFormat, ...){
	PVALIST pArgs;
	WCHAR Text[256];

	VA_START(pArgs, psFormat);
	Wvnprintf(Text, 256, psFormat, pArgs);
	VA_END(pArgs);
	MessageBoxW(hWindow, Text, P_Null, MB_OK);
}

//----------------------------------------//


//-------------------- 字体家族枚举 --------------------//

//枚举字体家族
$VOID EnumFontFamily(IDWFactory *pDWFactory, _out STLVector<WCHAR> &rResult){
	IDWFontColl *lpFontColl;
	IDWFontFamily *lpFontFamily;
	IDWLocalStrings *lpFamilyNames;

	IBOOL bValidName;
	WCHAR *psFamilyName;
	WCHAR LocaleName[LOCALE_NAME_MAX_LENGTH];
	USINT NameIdx, NameLen, FamilyIdx, FamilyCnt;
	const USINT MaxNameLen = 32;

	pDWFactory->GetSystemFontCollection(&lpFontColl);
	FamilyCnt = lpFontColl->GetFontFamilyCount();
	rResult.resize(FamilyCnt * MaxNameLen, L'\0');
	psFamilyName = rResult.data();
	GetUserDefaultLocaleName(LocaleName, LOCALE_NAME_MAX_LENGTH);

	for(FamilyIdx = 0; FamilyIdx < FamilyCnt; ++FamilyIdx){
		lpFontColl->GetFontFamily(FamilyIdx, &lpFontFamily);
		lpFontFamily->GetFamilyNames(&lpFamilyNames);
		lpFamilyNames->FindLocaleName(LocaleName, &NameIdx, &bValidName);

		if(!bValidName) lpFamilyNames->FindLocaleName(L"en-US", &NameIdx, &bValidName);
		if(!bValidName) NameIdx = 0;

		lpFamilyNames->GetStringLength(NameIdx, &NameLen);
		lpFamilyNames->GetString(NameIdx, psFamilyName, NameLen + 1);

		lpFontFamily->Release();
		lpFamilyNames->Release();

		psFamilyName += MaxNameLen;
	}

	lpFontColl->Release();
}

//----------------------------------------//


//-------------------- 显示设备枚举 --------------------//

//枚举显示适配器
$VOID EnumDisplayAdapters(IDXGIFactory6 *pDxgiFactory, _out STLVector<DXGI1ADAPTERDESC1> &rResult){
	USINT Index = 0;
	HRESULT RetCode;
	IDXGIAdapter1 *lpAdapter;
	DXGI1ADAPTERDESC1 AdaptDesc;

	while(B_True){
		RetCode = pDxgiFactory->EnumAdapters1(Index++, &lpAdapter);
		if(RetCode == DXGI_ERROR_NOT_FOUND) break;
		lpAdapter->GetDesc1(&AdaptDesc);
		rResult.push_back(AdaptDesc);
		lpAdapter->Release();
	}
}

//枚举显示输出
$VOID EnumDisplayOutputs(IDXGIAdapter *pAdapter, _out STLVector<DXGI1OUTPUTDESC> &rResult){
	USINT Index = 0;
	HRESULT RetCode;
	IDXGIOutput *lpDevice;
	DXGI1OUTPUTDESC DevDesc;

	while(B_True){
		RetCode = pAdapter->EnumOutputs(Index++, &lpDevice);
		if(RetCode == DXGI_ERROR_NOT_FOUND) break;
		lpDevice->GetDesc(&DevDesc);
		rResult.push_back(DevDesc);
		lpDevice->Release();
	}
}

//枚举显示模式
$VOID EnumDisplayModes(IDXGIOutput *pDevice, DXGI1FORMAT Format, _out STLVector<DXGI1MODEDESC> &rResult){
	USINT Count = 0;
	pDevice->GetDisplayModeList(Format, 0L, &Count, P_Null);
	rResult.resize(Count);
	pDevice->GetDisplayModeList(Format, 0L, &Count, rResult.data());
}

//----------------------------------------//


//-------------------- 位图绘制 --------------------//

//绘制位图
$VOID DrawBitmap(ID2DDevCtx *pD2dDevCtx, ID2DBitmap *pBitmap, _in D2D1RECTF &rcDest, BOXFITMODE FitMode = BFM_Fill){
	D2D1SIZEF SrcSize = pBitmap->GetSize();
	D2D1RECTF rcSource0 = { 0.f, 0.f, SrcSize.width, SrcSize.height };
	D2D1RECTF rcDest0 = rcDest;
	D2D1SIZEF DstSize = RECT_SIZE(rcDest0);

	switch(FitMode){
		case BFM_Fill:
			break;
		case BFM_None:{
			if((SrcSize.width < DstSize.width) &&
				(SrcSize.height < DstSize.height))
			{
				rcDest0.left += (DstSize.width - SrcSize.width) * 0.5f;
				rcDest0.right = rcDest0.left + SrcSize.width;
				rcDest0.top += (DstSize.height - SrcSize.height) * 0.5f;
				rcDest0.bottom = rcDest0.top + SrcSize.height;
				break;
			}
			//否则使用contain模式
		}
		case BFM_Contain:{
			SPFPN SrcRatio = SrcSize.width / SrcSize.height;
			SPFPN DstRatio = DstSize.width / DstSize.height;

			if(SrcRatio > DstRatio){
				SPFPN Height = DstSize.width / SrcRatio;
				rcDest0.top += (DstSize.height - Height) * 0.5f;
				rcDest0.bottom = rcDest0.top + Height;
			} else if(SrcRatio < DstRatio){
				SPFPN Width = DstSize.height * SrcRatio;
				rcDest0.left += (DstSize.width - Width) * 0.5f;
				rcDest0.right = rcDest0.left + Width;
			}

			break;
		}
		case BFM_Cover:{
			SPFPN SrcRatio = SrcSize.width / SrcSize.height;
			SPFPN DstRatio = DstSize.width / DstSize.height;

			if(SrcRatio > DstRatio){
				SPFPN Width = SrcSize.height * DstRatio;
				rcSource0.left = (SrcSize.width - Width) * 0.5f;
				rcSource0.right = rcSource0.left + Width;
			} else if(SrcRatio < DstRatio){
				SPFPN Height = SrcSize.width / DstRatio;
				rcSource0.top = (SrcSize.height - Height) * 0.5f;
				rcSource0.bottom = rcSource0.top + Height;
			}

			break;
		}
	}

	pD2dDevCtx->DrawBitmap(pBitmap, rcDest0,
		1.f, D2D1_INTERPOLATION_MODE_LINEAR, &rcSource0);
}

//绘制位图(指定区域)
$VOID DrawBitmap(ID2DDevCtx *pD2dDevCtx, ID2DBitmap *pBitmap, _in D2D1RECTF &rcSource, _in D2D1RECTF &rcDest, BOXFITMODE FitMode = BFM_Fill){
	D2D1RECTF rcDest0 = rcDest;
	D2D1RECTF rcSource0 = rcSource;
	D2D1SIZEF DstSize = RECT_SIZE(rcDest0);
	D2D1SIZEF SrcSize = RECT_SIZE(rcSource0);

	switch(FitMode){
		case BFM_Fill:
			break;
		case BFM_None:{
			if((SrcSize.width < DstSize.width) &&
				(SrcSize.height < DstSize.height))
			{
				rcDest0.left += (DstSize.width - SrcSize.width) * 0.5f;
				rcDest0.right = rcDest0.left + SrcSize.width;
				rcDest0.top += (DstSize.height - SrcSize.height) * 0.5f;
				rcDest0.bottom = rcDest0.top + SrcSize.height;
				break;
			}
			//否则使用contain模式
		}
		case BFM_Contain:{
			SPFPN SrcRatio = SrcSize.width / SrcSize.height;
			SPFPN DstRatio = DstSize.width / DstSize.height;

			if(SrcRatio > DstRatio){
				SPFPN Height = DstSize.width / SrcRatio;
				rcDest0.top += (DstSize.height - Height) * 0.5f;
				rcDest0.bottom = rcDest0.top + Height;
			} else if(SrcRatio < DstRatio){
				SPFPN Width = DstSize.height * SrcRatio;
				rcDest0.left += (DstSize.width - Width) * 0.5f;
				rcDest0.right = rcDest0.left + Width;
			}

			break;
		}
		case BFM_Cover:{
			SPFPN SrcRatio = SrcSize.width / SrcSize.height;
			SPFPN DstRatio = DstSize.width / DstSize.height;

			if(SrcRatio > DstRatio){
				SPFPN Width = SrcSize.height * DstRatio;
				rcSource0.left += (SrcSize.width - Width) * 0.5f;
				rcSource0.right = rcSource0.left + Width;
			} else if(SrcRatio < DstRatio){
				SPFPN Height = SrcSize.width / DstRatio;
				rcSource0.top += (SrcSize.height - Height) * 0.5f;
				rcSource0.bottom = rcSource0.top + Height;
			}

			break;
		}
	}

	pD2dDevCtx->DrawBitmap(pBitmap, rcDest0,
		1.f, D2D1_INTERPOLATION_MODE_LINEAR, &rcSource0);
}

//----------------------------------------//


//-------------------- 文件块搜索 --------------------//

//搜索文件块
IBOOL FindFileChunk(HANDLE hFile, DWORD MatchType){
	DWORD ChunkType;
	ULONG ChunkSize, SizeOfRead;

	while(B_True){
		ReadFile(hFile, &ChunkType, sizeof(DWORD), &SizeOfRead, P_Null);
		if(SizeOfRead == 0) return B_False;
		ReadFile(hFile, &ChunkSize, sizeof(ULONG), P_Null, P_Null);
		if(ChunkType == MatchType) return B_True;
		SetFilePointer(hFile, ChunkSize, P_Null, FILE_CURRENT);
	}
}

//----------------------------------------//


//-------------------- 文件按行读取 --------------------//

IBOOL ReadFileByLine(HANDLE hFile, CHAR8 *psBuffer, USINT BufSize){
	SPINT LineSize;
	ULONG SizeRead;
	INT32 SizeBack;
	CHAR8 *pLineEnd;

	ReadFile(hFile, psBuffer, BufSize, &SizeRead, P_Null);
	if(SizeRead == 0) return B_False;

	if(SizeRead < BufSize)
		psBuffer[SizeRead] = '\n';
	if(SizeRead == BufSize)
		psBuffer[BufSize - 1] = '\n';

	pLineEnd = MbsChr(psBuffer, '\n');
	*pLineEnd = '\0';

	LineSize = pLineEnd - psBuffer + 1;
	SizeBack = (INT32)LineSize - (INT32)SizeRead;

	SetFilePointer(hFile, SizeBack, P_Null, FILE_CURRENT);

	return B_False;
}

//----------------------------------------//


//-------------------- 尺寸计算 --------------------//

//计算文本尺寸
D2D1SIZEF CalcTextSize(IDWFactory *pDWFactory, IDWTxtFormat *pFormat, _in WCHAR *psText){
	DW1TEXTMETRICS Metrics;
	IDWTxtLayout *lpLayout;

	pDWFactory->CreateTextLayout(psText, (USINT)WcsLen(psText), pFormat, 0.f, 0.f, &lpLayout);
	lpLayout->GetMetrics(&Metrics);
	lpLayout->Release();

	return D2D1SIZEF{ Metrics.widthIncludingTrailingWhitespace, Metrics.height };
}

//获取纹理尺寸
D2D1SIZEU GetTextureSize(ID3DResource *pTexture){
	D3D12RESDESC TexDesc = pTexture->GetDesc();
	return D2D1SIZEU{ (USINT)TexDesc.Width, TexDesc.Height };
}

//----------------------------------------//


//-------------------- GPU产家查询 --------------------//

//查询GPU产家
DWORD GetGpuVendorId(IDXGIFactory4 *pDxgiFactory, ID3DDevice *pD3dDevice){
	IDXGIAdapter1 *lpAdapter;
	DXGI1ADAPTERDESC1 AdaptDesc;

	LUID Luid = pD3dDevice->GetAdapterLuid();
	HRESULT RetCode = pDxgiFactory->EnumAdapterByLuid(Luid, IID_AND_PPV(lpAdapter));

	if(FAILED_(RetCode)) return 0L;
	lpAdapter->GetDesc1(&AdaptDesc);
	lpAdapter->Release();
	return AdaptDesc.VendorId;
}

//----------------------------------------//


//-------------------- D3D设备检查 --------------------//

//检查D3D采样质量等级
USINT CheckD3dSampleQualityLevel(ID3DDevice *pD3dDevice, DXGI1FORMAT Format, USINT SampleCnt){
	D3D12MULTISAMPLEFEATURE Feature;

	Feature.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	Feature.Format = Format;
	Feature.SampleCount = SampleCnt;
	Feature.NumQualityLevels = 0;

	pD3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &Feature, sizeof(Feature));

	return Feature.NumQualityLevels - 1;
}

//检查D3D特征等级
D3D1FEATURELEVEL CheckD3dFeatureLevel(ID3DDevice *pD3dDevice){
	D3D12LEVELFEATURE Feature;
	D3D1FEATURELEVEL rgLevel[] = {
		D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

	Feature.NumFeatureLevels = ARR_LEN(rgLevel);
	Feature.pFeatureLevelsRequested = rgLevel;

	pD3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS, &Feature, sizeof(Feature));

	return Feature.MaxSupportedFeatureLevel;
}

//检查D3D根签名版本
D3D1ROOTSIGVERSION CheckD3dRootSignatureVersion(ID3DDevice *pD3dDevice){
	HRESULT RetCode;
	D3D12ROOTSIGFEATURE Feature;

	Feature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	RetCode = pD3dDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &Feature, sizeof(Feature));

	return WORKED_(RetCode) ?
		D3D_ROOT_SIGNATURE_VERSION_1_1 :
		D3D_ROOT_SIGNATURE_VERSION_1_0;
}

//检查D3D光线追踪等级
D3D12RAYTRACINGTIER CheckD3dRaytracingTier(ID3D12Device *pD3dDevice){
	D3D12OPTIONSFEATURE5 Feature = {};
	pD3dDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Feature, sizeof(Feature));
	return Feature.RaytracingTier;
}

//----------------------------------------//


//-------------------- 资源创建 --------------------//

//创建SRV堆
ID3DViewHeap *CreateSrvHeap(ID3DDevice *pD3dDevice, USINT Size){
	ID3DViewHeap *lpHeap;
	D3D12VIEWHEAPDESC Desc;

	Desc.NodeMask = 0x0;
	Desc.NumDescriptors = Size;
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	pD3dDevice->CreateDescriptorHeap(&Desc, IID_AND_PPV(lpHeap));

	return lpHeap;
}

//创建上传缓存区
ID3DResource *CreateUploadBuffer(ID3DDevice *pD3dDevice, UPINT Size, IBOOL bConst){
	ID3DResource *lpBuffer;
	UPINT AdjSize = bConst ? ToPowOf2Mul(Size, 256) : Size;

	pD3dDevice->CreateCommittedResource(
		&D3DX12HEAPPROPS(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&D3DX12RESDESC::Buffer(AdjSize), D3D12_RESOURCE_STATE_GENERIC_READ,
		P_Null, IID_AND_PPV(lpBuffer));

	return lpBuffer;
}

//创建字体格式
IDWTxtFormat *CreateTextFormat(IDWFactory *pDWFactory, _in WCHAR *psFamilyName, SPFPN FontSize){
	IDWTxtFormat *lpFormat;

	pDWFactory->CreateTextFormat(psFamilyName, P_Null,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		FontSize, L"en-US", &lpFormat);

	lpFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	lpFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	lpFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	return lpFormat;
}

//加载WIC位图
IWICBitmap *LoadWicBitmap(IWICFactory *pWicFactory, _in WCHAR *psFilePath){
	IWICBitmap *lpBitmap;
	IWICFmtConvter *lpConvter;
	IWICBmpDecoder *lpDecoder;
	IWICBmpFrameDecoder *lpSource;

	pWicFactory->CreateDecoderFromFilename(psFilePath, P_Null,
		GENERIC_READ, WICDecodeMetadataCacheOnDemand, &lpDecoder);

	lpDecoder->GetFrame(0, &lpSource);

	pWicFactory->CreateFormatConverter(&lpConvter);

	lpConvter->Initialize(lpSource, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, P_Null, 0.f, WICBitmapPaletteTypeMedianCut);

	pWicFactory->CreateBitmapFromSource(lpConvter, WICBitmapCacheOnDemand, &lpBitmap);

	lpSource->Release();
	lpConvter->Release();
	lpDecoder->Release();

	return lpBitmap;
}

//加载D2D位图
ID2DBitmap *LoadD2dBitmap(IWICFactory *pWicFactory, ID2DDevCtx *pD2dDevCtx, _in WCHAR *psFilePath){
	ID2DBitmap *lpBitmap;
	IWICFmtConvter *lpConvter;
	IWICBmpDecoder *lpDecoder;
	IWICBmpFrameDecoder *lpSource;

	pWicFactory->CreateDecoderFromFilename(psFilePath, P_Null,
		GENERIC_READ, WICDecodeMetadataCacheOnLoad, &lpDecoder);

	lpDecoder->GetFrame(0, &lpSource);

	pWicFactory->CreateFormatConverter(&lpConvter);

	lpConvter->Initialize(lpSource, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, P_Null, 0.f, WICBitmapPaletteTypeMedianCut);

	pD2dDevCtx->CreateBitmapFromWicBitmap(lpConvter, P_Null, &lpBitmap);

	lpSource->Release();
	lpConvter->Release();
	lpDecoder->Release();

	return lpBitmap;
}

//加载音频
IXASource *LoadAudio(IXAudio2 *pXAFactory, IXAVoiceCback *pCallback, _in WCHAR *psFilePath){
	//加载文件

	BYTET *lpsWavData;
	WAVVIEW FileView;
	TUniquePtr<BYTET[]> apFileData;

	dx::LoadWAVAudioFromFileEx(psFilePath, apFileData, FileView);
	lpsWavData = new BYTET[FileView.audioBytes];
	BStr_Move(lpsWavData, FileView.startAudio, FileView.audioBytes);

	//创建音源

	IXASource *lpAudio;
	XA2BUFFER AudioBuff = {};

	AudioBuff.pContext = lpsWavData; //被接管
	AudioBuff.pAudioData = lpsWavData;
	AudioBuff.AudioBytes = FileView.audioBytes;
	AudioBuff.Flags = XAUDIO2_END_OF_STREAM;

	pXAFactory->CreateSourceVoice(&lpAudio, FileView.wfx, 0L, XAUDIO2_DEFAULT_FREQ_RATIO, pCallback);
	lpAudio->SubmitSourceBuffer(&AudioBuff);

	return lpAudio;
}

//编译着色器
ID3DBlob *CompileShader(_in WCHAR *psFilePath, _in CHAR8 *psEntryName, _in CHAR8 *psTargetName, _in D3D1SHADERMACRO *prgMacro){
	ID3DBlob *lpObjCode, *lpErrCode;

	D3DCompileFromFile(psFilePath, prgMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		psEntryName, psTargetName, 0L, 0L, &lpObjCode, &lpErrCode);

	if(lpErrCode != P_Null){
		MessageBoxA(P_Null, (CHAR8*)lpErrCode->GetBufferPointer(), "Compilation Error", MB_OK);
		lpErrCode->Release();
	}

	return lpObjCode;
}

//----------------------------------------//