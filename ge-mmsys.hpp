//-------------------- ImGui命名空间 --------------------//

namespace imgui = ImGui;

//----------------------------------------//


//-------------------- DirectX命名空间 --------------------//

namespace d2d = D2D1;
namespace dx = DirectX;
namespace dxlh = DirectX::LoaderHelpers;

//----------------------------------------//


//-------------------- DirectX数据类型 --------------------//

typedef d2d::ColorF     D2DXColor;
typedef d2d::Matrix3x2F D2DXMatrix;

typedef dx::WAVData WAVView;
typedef dx::CommonStates        DXTKCommStates;
typedef dx::GeometricPrimitive  DXTKGeomPrim;
typedef dx::ResourceUploadBatch DXTKUploader;

//----------------------------------------//


//-------------------- GPU资源驻留 --------------------//

// 驻留GPU资源
inline $VOID ResidentGpuResource(ID3D12Device4 *pD3dDev, ID3D12Pageable *pObject){
	pD3dDev->MakeResident(1, &pObject);
}

// 驱逐GPU资源
inline $VOID EvictGpuResource(ID3D12Device4 *pD3dDev, ID3D12Pageable *pObject){
	pD3dDev->Evict(1, &pObject);
}

//----------------------------------------//


//-------------------- 系统提示框 --------------------//

// 弹出提示
$VOID PopupPromptA(HWND hWindow, _in CHAR8 *psFormat, ...){
	va_list pArgs;
	CHAR8 Text[256];

	va_start(pArgs, psFormat);
	vsnprintf(Text, 256, psFormat, pArgs);
	va_end(pArgs);
	MessageBoxA(hWindow, Text, P_Null, MB_OK);
}

// 弹出提示(宽字符)
$VOID PopupPromptW(HWND hWindow, _in WCHAR *psFormat, ...){
	va_list pArgs;
	WCHAR Text[256];

	va_start(pArgs, psFormat);
	_vsnwprintf(Text, 256, psFormat, pArgs);
	va_end(pArgs);
	MessageBoxW(hWindow, Text, P_Null, MB_OK);
}

//----------------------------------------//


//-------------------- 字体家族枚举 --------------------//

// 枚举字体家族
$VOID EnumFontFamily(IDWriteFactory *pDWFactory, _out TSeqList<WCHAR> &rResult){
	IDWriteFontCollection *lpFontColl;
	IDWriteFontFamily *lpFontFamily;
	IDWriteLocalizedStrings *lpFamilyNames;

	IBOOL bValidName;
	WCHAR *psFamilyName;
	WCHAR LocaleName[LOCALE_NAME_MAX_LENGTH];
	UNS32 NameId, NameLen, FamilyId, FamilyCnt;
	const UNS32 MaxNameLen = 32;

	pDWFactory->GetSystemFontCollection(&lpFontColl);
	FamilyCnt = lpFontColl->GetFontFamilyCount();
	rResult.resize(FamilyCnt * MaxNameLen, L'\0');
	psFamilyName = rResult.data();
	GetUserDefaultLocaleName(LocaleName, LOCALE_NAME_MAX_LENGTH);

	for(FamilyId = 0; FamilyId < FamilyCnt; ++FamilyId){
		lpFontColl->GetFontFamily(FamilyId, &lpFontFamily);
		lpFontFamily->GetFamilyNames(&lpFamilyNames);
		lpFamilyNames->FindLocaleName(LocaleName, &NameId, &bValidName);

		if(!bValidName) lpFamilyNames->FindLocaleName(L"en-US", &NameId, &bValidName);
		if(!bValidName) NameId = 0;

		lpFamilyNames->GetStringLength(NameId, &NameLen);
		lpFamilyNames->GetString(NameId, psFamilyName, NameLen + 1);

		lpFontFamily->Release();
		lpFamilyNames->Release();

		psFamilyName += MaxNameLen;
	}

	lpFontColl->Release();
}

//----------------------------------------//


//-------------------- 显示设备枚举 --------------------//

// 枚举显示适配器
$VOID EnumDisplayAdapters(IDXGIFactory6 *pDxgiFactory, _out TSeqList<DXGI_ADAPTER_DESC1> &rResult){
	UNS32 Index = 0;
	HRESULT RetCode;
	IDXGIAdapter1 *lpAdapter;
	DXGI_ADAPTER_DESC1 AdaptDesc;

	while(B_True){
		RetCode = pDxgiFactory->EnumAdapters1(Index++, &lpAdapter);
		if(RetCode == DXGI_ERROR_NOT_FOUND) break;
		lpAdapter->GetDesc1(&AdaptDesc);
		rResult.push_back(AdaptDesc);
		lpAdapter->Release();
	}
}

// 枚举显示输出
$VOID EnumDisplayOutputs(IDXGIAdapter *pAdapter, _out TSeqList<DXGI_OUTPUT_DESC> &rResult){
	UNS32 Index = 0;
	HRESULT RetCode;
	IDXGIOutput *lpDevice;
	DXGI_OUTPUT_DESC DevDesc;

	while(B_True){
		RetCode = pAdapter->EnumOutputs(Index++, &lpDevice);
		if(RetCode == DXGI_ERROR_NOT_FOUND) break;
		lpDevice->GetDesc(&DevDesc);
		rResult.push_back(DevDesc);
		lpDevice->Release();
	}
}

// 枚举显示模式
$VOID EnumDisplayModes(IDXGIOutput *pDevice, DXGI_FORMAT Format, _out TSeqList<DXGI_MODE_DESC> &rResult){
	UNS32 Count = 0;
	pDevice->GetDisplayModeList(Format, 0L, &Count, P_Null);
	rResult.resize(Count);
	pDevice->GetDisplayModeList(Format, 0L, &Count, rResult.data());
}

//----------------------------------------//


//-------------------- 位图绘制 --------------------//

// 绘制位图
$VOID DrawBitmap(ID2D1DeviceContext *pD2dDevCtx, ID2D1Bitmap1 *pBitmap, _in FRect &rcDest, BOXFITMODE FitMode = BFM_Fill){
	D2D1_SIZE_F SrcSize = pBitmap->GetSize();
	FRect rcSource0 = { 0.f, 0.f, SrcSize.width, SrcSize.height };
	FRect rcDest0 = rcDest;
	D2D1_SIZE_F DstSize = RECT_SIZE(rcDest0);

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

// 绘制位图(指定区域)
$VOID DrawBitmap(ID2D1DeviceContext *pD2dDevCtx, ID2D1Bitmap1 *pBitmap, _in FRect &rcSource, _in FRect &rcDest, BOXFITMODE FitMode = BFM_Fill){
	FRect rcDest0 = rcDest;
	FRect rcSource0 = rcSource;
	VFloat2 DstSize = RECT_SIZE(rcDest0);
	VFloat2 SrcSize = RECT_SIZE(rcSource0);

	switch(FitMode){
		case BFM_Fill:
			break;
		case BFM_None:{
			if((SrcSize.x < DstSize.x) &&
				(SrcSize.y < DstSize.y))
			{
				rcDest0.left += (DstSize.x - SrcSize.x) * 0.5f;
				rcDest0.right = rcDest0.left + SrcSize.x;
				rcDest0.top += (DstSize.y - SrcSize.y) * 0.5f;
				rcDest0.bottom = rcDest0.top + SrcSize.y;
				break;
			}
			//否则使用contain模式
		}
		case BFM_Contain:{
			SPFPN SrcRatio = SrcSize.x / SrcSize.y;
			SPFPN DstRatio = DstSize.x / DstSize.y;

			if(SrcRatio > DstRatio){
				SPFPN Height = DstSize.x / SrcRatio;
				rcDest0.top += (DstSize.y - Height) * 0.5f;
				rcDest0.bottom = rcDest0.top + Height;
			} else if(SrcRatio < DstRatio){
				SPFPN Width = DstSize.y * SrcRatio;
				rcDest0.left += (DstSize.x - Width) * 0.5f;
				rcDest0.right = rcDest0.left + Width;
			}

			break;
		}
		case BFM_Cover:{
			SPFPN SrcRatio = SrcSize.x / SrcSize.y;
			SPFPN DstRatio = DstSize.x / DstSize.y;

			if(SrcRatio > DstRatio){
				SPFPN Width = SrcSize.y * DstRatio;
				rcSource0.left += (SrcSize.x - Width) * 0.5f;
				rcSource0.right = rcSource0.left + Width;
			} else if(SrcRatio < DstRatio){
				SPFPN Height = SrcSize.x / DstRatio;
				rcSource0.top += (SrcSize.y - Height) * 0.5f;
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

// 搜索文件块
IBOOL FindFileChunk(HANDLE hFile, DWORD MatchType){
	DWORD ChunkType;
	ULONG ChunkSize, ReadSize;

	while(B_True){
		ReadFile(hFile, &ChunkType, sizeof(DWORD), &ReadSize, P_Null);
		if(ReadSize == 0) return B_False;
		ReadFile(hFile, &ChunkSize, sizeof(ULONG), P_Null, P_Null);
		if(ChunkType == MatchType) return B_True;
		SetFilePointer(hFile, ChunkSize, P_Null, FILE_CURRENT);
	}
}

//----------------------------------------//


//-------------------- 文件按行读取 --------------------//

// 文件按行读取
IBOOL ReadFileByLine(HANDLE hFile, CHAR8 *psDest, UNS32 MaxSize){
	static CHAR8 Buffer[1024];
	static DWORD BufPtr = 0;
	static DWORD BufUsed = 0;

	CHAR8 Char;
	CHAR8 *pDest = psDest;
	CHAR8 *pEnd = pDest + MaxSize - 1;

	while(pDest != pEnd){
		if(BufPtr >= BufUsed){
			ReadFile(hFile, Buffer, sizeof(Buffer), &BufUsed, P_Null);
			if(BufUsed == 0) return(pDest != psDest);
			BufPtr = 0;
		}

		Char = Buffer[BufPtr++];
		if(Char == '\r'){
			continue;
		} else if(Char != '\n'){
			*pDest++ = Char;
		} else{
			*pDest = '\0';
			return B_True;
		}
	}

	*pDest = '\0';
	return B_True;
}

//----------------------------------------//


//-------------------- 尺寸计算 --------------------//

// 计算文本尺寸
VFloat2 CalcTextSize(IDWriteFactory *pDWFactory, IDWriteTextFormat *pFormat, _in WCHAR *psText){
	DWRITE_TEXT_METRICS Metrics;
	IDWriteTextLayout *lpLayout;

	pDWFactory->CreateTextLayout(psText, (UNS32)wcslen(psText), pFormat, 0.f, 0.f, &lpLayout);
	lpLayout->GetMetrics(&Metrics);
	lpLayout->Release();

	return VFloat2(Metrics.widthIncludingTrailingWhitespace, Metrics.height);
}

// 获取纹理尺寸
VUint2 GetTextureSize(ID3D12Resource *pTexture){
	D3D12_RESOURCE_DESC TexDesc = pTexture->GetDesc();
	return VUint2((UNS32)TexDesc.Width, TexDesc.Height);
}

//----------------------------------------//


//-------------------- GPU产家查询 --------------------//

// 查询GPU产家
DWORD GetGpuVendorId(IDXGIFactory4 *pDxgiFactory, ID3D12Device4 *pD3dDev){
	IDXGIAdapter1 *lpAdapter;
	DXGI_ADAPTER_DESC1 AdaptDesc;

	LUID Luid = pD3dDev->GetAdapterLuid();
	HRESULT RetCode = pDxgiFactory->EnumAdapterByLuid(Luid, IID_PPV_ARGS(&lpAdapter));

	if(FAILED_(RetCode)) return 0L;
	lpAdapter->GetDesc1(&AdaptDesc);
	lpAdapter->Release();
	return AdaptDesc.VendorId;
}

//----------------------------------------//


//-------------------- D3D设备检查 --------------------//

// 检查D3D采样质量等级
UNS32 CheckD3dSampleQualityLevel(ID3D12Device4 *pD3dDev, DXGI_FORMAT Format, UNS32 SampleCnt){
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS Feature;

	Feature.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	Feature.Format = Format;
	Feature.SampleCount = SampleCnt;
	Feature.NumQualityLevels = 0;

	pD3dDev->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &Feature, sizeof(Feature));

	return Feature.NumQualityLevels - 1;
}

// 检查D3D特征等级
D3D_FEATURE_LEVEL CheckD3dFeatureLevel(ID3D12Device4 *pD3dDev){
	D3D12_FEATURE_DATA_FEATURE_LEVELS Feature;
	D3D_FEATURE_LEVEL rgLevel[] = {
		D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

	Feature.NumFeatureLevels = ARR_LEN(rgLevel);
	Feature.pFeatureLevelsRequested = rgLevel;

	pD3dDev->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS, &Feature, sizeof(Feature));

	return Feature.MaxSupportedFeatureLevel;
}

// 检查D3D根签名版本
D3D_ROOT_SIGNATURE_VERSION CheckD3dRootSignatureVersion(ID3D12Device4 *pD3dDev){
	HRESULT RetCode;
	D3D12_FEATURE_DATA_ROOT_SIGNATURE Feature;

	Feature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	RetCode = pD3dDev->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &Feature, sizeof(Feature));

	return WORKED_(RetCode) ?
		D3D_ROOT_SIGNATURE_VERSION_1_1 :
		D3D_ROOT_SIGNATURE_VERSION_1_0;
}

// 检查D3D光线追踪等级
D3D12_RAYTRACING_TIER CheckD3dRaytracingTier(ID3D12Device *pD3dDev){
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 Feature = {};
	pD3dDev->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Feature, sizeof(Feature));
	return Feature.RaytracingTier;
}

//----------------------------------------//


//-------------------- 资源创建 --------------------//

// 创建SRV堆
ID3D12DescriptorHeap *CreateSrvHeap(ID3D12Device4 *pD3dDev, UNS32 Size){
	ID3D12DescriptorHeap *lpHeap;
	D3D12_DESCRIPTOR_HEAP_DESC Desc;

	Desc.NodeMask = 0x0;
	Desc.NumDescriptors = Size;
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	pD3dDev->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&lpHeap));

	return lpHeap;
}

// 创建上传缓存区
ID3D12Resource *CreateUploadBuffer(ID3D12Device4 *pD3dDev, UIPTR Size, IBOOL bConst){
	ID3D12Resource *lpBuffer;
	UIPTR AliSize = bConst ? ToPowOf2Mul(Size, 256) : Size;

	pD3dDev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(AliSize), D3D12_RESOURCE_STATE_GENERIC_READ,
		P_Null, IID_PPV_ARGS(&lpBuffer));

	return lpBuffer;
}

// 创建字体格式
IDWriteTextFormat *CreateTextFormat(IDWriteFactory *pDWFactory, _in WCHAR *psFamilyName, SPFPN FontSize){
	IDWriteTextFormat *lpFormat;

	pDWFactory->CreateTextFormat(psFamilyName, P_Null,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		FontSize, L"en-US", &lpFormat);

	lpFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	lpFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	lpFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	return lpFormat;
}

// 加载WIC位图
IWICBitmap *LoadWicBitmap(IWICImagingFactory *pWicFactory, _in WCHAR *psFilePath){
	IWICBitmap *lpBitmap;
	IWICFormatConverter *lpConvter;
	IWICBitmapDecoder *lpDecoder;
	IWICBitmapFrameDecode *lpSource;

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

// 加载D2D位图
ID2D1Bitmap1 *LoadD2dBitmap(IWICImagingFactory *pWicFactory, ID2D1DeviceContext *pD2dDevCtx, _in WCHAR *psFilePath){
	ID2D1Bitmap1 *lpBitmap;
	IWICFormatConverter *lpConvter;
	IWICBitmapDecoder *lpDecoder;
	IWICBitmapFrameDecode *lpSource;

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

// 加载音频
IXAudio2SourceVoice *LoadAudio(IXAudio2 *pXAFactory, IXAudio2VoiceCallback *pCallback, _in WCHAR *psFilePath){
	/// 加载文件

	BYTET *lpsWavData;
	WAVView FileView;
	TUniquePtr<BYTET[]> apFileData;

	dx::LoadWAVAudioFromFileEx(psFilePath, apFileData, FileView);
	lpsWavData = new BYTET[FileView.audioBytes];
	Mem_Copy(lpsWavData, FileView.startAudio, FileView.audioBytes);

	/// 创建音源

	IXAudio2SourceVoice *lpAudio;
	XAUDIO2_BUFFER AudioBuff = {};

	AudioBuff.pContext = lpsWavData; //被接管
	AudioBuff.pAudioData = lpsWavData;
	AudioBuff.AudioBytes = FileView.audioBytes;
	AudioBuff.Flags = XAUDIO2_END_OF_STREAM;

	pXAFactory->CreateSourceVoice(&lpAudio, FileView.wfx, 0L, XAUDIO2_DEFAULT_FREQ_RATIO, pCallback);
	lpAudio->SubmitSourceBuffer(&AudioBuff);

	return lpAudio;
}

// 编译着色器
ID3DBlob *CompileShader(_in WCHAR *psFilePath, _in CHAR8 *psEntryName, _in CHAR8 *psTargetName, _in D3D_SHADER_MACRO *prgMacro){
	ID3DBlob *lpTarget, *lpErrCode;

	D3DCompileFromFile(psFilePath, prgMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		psEntryName, psTargetName, 0L, 0L, &lpTarget, &lpErrCode);

	if(lpErrCode != P_Null){
		MessageBoxA(P_Null, (CHAR8*)lpErrCode->GetBufferPointer(), "Compilation Error", MB_OK);
		lpErrCode->Release();
	}

	return lpTarget;
}

//----------------------------------------//