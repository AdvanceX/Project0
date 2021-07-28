//--------------------调试--------------------//

//弹出调试框(文本格式,文本参数...)
VOID PopDebug(CHAR* Format, ...){
	VALIST Args;
	CHAR Text[128];

	VA_START(Args, Format);
	VSPrint(Text, Format, Args);
	VA_END(Args);

	MessageBoxA(NULL, Text, NULL, MB_OK);
}
//弹出调试框(文本格式,文本参数...)
VOID PopDebugW(WCHR* Format, ...){
	VALIST Args;
	WCHR Text[128];

	VA_START(Args, Format);
	VWSPrint(Text, Format, Args);
	VA_END(Args);

	MessageBoxW(NULL, Text, NULL, MB_OK);
}

//----------------------------------------//


//--------------------资源加载--------------------//

//创建上传缓冲区(D3D设备,大小,常量标记)[缓冲区]
ID3DResource* CreateUploadBuffer(ID3DDevice* lpD3DDevice, UINT Size, BOOL bConst){
	ID3DResource *lpBuffer;

	if(bConst) Size = GetMulOf2Pow(Size, 256);

	lpD3DDevice->CreateCommittedResource(
		&D3DHeapProps(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&D3DResDesc::Buffer(Size), D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, COM_ARGS(lpBuffer));

	return lpBuffer;
}

//创建源声(XAudio工厂,文件路径)[源声]
IXASrcVoice* CreateSourceVoiceFromFile(IXAudio2 *lpXAFactory, CHAR* FilePath){
	enum FourCC{
		RIFF = 'FFIR',
		DATA = 'atad',
		FMT = ' tmf',
		WAVE = 'EVAW',
		XWMA = 'AMWX',
		DPDS = 'sdpd',
	};

	HANDLE hFile;
	BYTE *AudioBytes;
	DWRD FileType, ChunkType;
	UINT ChunkSize, ReadSize = 0;
	XABuffer AudioBuff;
	IXASrcVoice *lpSrcVoice;
	WAVEFORMATEXTENSIBLE WaveFormat;

	//打开文件

	hFile = CreateFileA(FilePath, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0x0, NULL);

	while(TRUE){
		ReadFile(hFile, &ChunkType, sizeof(DWRD), (DWRD*)&ReadSize, NULL);
		ReadFile(hFile, &ChunkSize, sizeof(DWRD), (DWRD*)&ReadSize, NULL);

		if(ReadSize == 0) break;

		switch(ChunkType){
			case RIFF:{
				ReadFile(hFile, &FileType, sizeof(DWRD), NULL, NULL);
				if(FileType != WAVE) return NULL;
			} break;
			case FMT:{
				ReadFile(hFile, &WaveFormat, ChunkSize, NULL, NULL);
			} break;
			case DATA:{
				AudioBytes = new BYTE[ChunkSize];
				ReadFile(hFile, AudioBytes, ChunkSize, NULL, NULL);
			} break;
			default:{
				SetFilePointer(hFile, ChunkSize, NULL, FILE_CURRENT);
			} break;
		}
	}

	CloseHandle(hFile);

	//创建源声音

	MemZero(&AudioBuff, sizeof(XABuffer));
	AudioBuff.AudioBytes = ChunkSize;
	AudioBuff.pAudioData = AudioBytes;
	AudioBuff.Flags = XAUDIO2_END_OF_STREAM;

	lpXAFactory->CreateSourceVoice(&lpSrcVoice, (WAVEFORMATEX*)&WaveFormat);
	lpSrcVoice->SubmitSourceBuffer(&AudioBuff);

	return lpSrcVoice;
}

//创建位图(D2DDC,WIC工厂,文件路径)[位图]
ID2DBitmap* CreateBitmapFromFile(ID2DDevCtx* lpD2DDC, IWICFactory* lpWICFactory, WCHR* FilePath){
	ID2DBitmap *lpBitmap;
	IWICBmpDecode *lpDecoder;
	IWICBmpFrameDecode *lpSource;
	IWICFormatConvert *lpConverter;

	lpWICFactory->CreateDecoderFromFilename(
		FilePath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &lpDecoder);

	lpDecoder->GetFrame(0, &lpSource);

	lpWICFactory->CreateFormatConverter(&lpConverter);

	lpConverter->Initialize(lpSource, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);

	lpD2DDC->CreateBitmapFromWicBitmap(lpConverter, NULL, &lpBitmap);

	RELEASE_REF(lpSource);
	RELEASE_REF(lpDecoder);
	RELEASE_REF(lpConverter);

	return lpBitmap;
}

//编译着色器(文件路径,宏定义,入口函数,目标类型)[二进制码]
ID3DBlob* CompileShaderFromFile(WCHR* FilePath, D3DShaderMacro* arrMacro, CHAR* Entrance, CHAR* Target){
	ID3DBlob *lpObjCode, *lpErrCode;

	D3DCompileFromFile(FilePath, arrMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		Entrance, Target, 0x0, 0x0, &lpObjCode, &lpErrCode);

	if(lpErrCode != NULL){
		MessageBoxA(NULL, (CHAR*)lpErrCode->GetBufferPointer(), "Compile Error", MB_OK);
		lpErrCode->Release();
	}

	return lpObjCode;
}

//搜索文件块(文件句柄,块类型)[文件句柄]
HANDLE FindFileChunk(HANDLE hFile, DWRD ChunkType){
	DWRD nRead;
	UINT Offset;
	DWRD CurType;

	while(TRUE){
		ReadFile(hFile, &CurType, sizeof(UINT), &nRead, NULL);
		if(nRead == 0) return INVALID_HANDLE_VALUE;

		ReadFile(hFile, &Offset, sizeof(UINT), NULL, NULL);
		if(CurType == ChunkType) return hFile;
		SetFilePointer(hFile, Offset, NULL, FILE_CURRENT);
	}
}

//----------------------------------------//


//--------------------设备检查--------------------//

//枚举显示适配器(DXGI工厂,@枚举信息)
VOID EnumDisplayAdapters(IDXGIFactory6* ipDXGIFactory, std::vector<DXGIAdaptDesc> &Output){
	RESULT Result;
	IDXGIAdapter1 *lpAdapter;
	DXGIAdaptDesc AdaptDesc;

	for(UINT i = 0; TRUE; ++i){
		Result = ipDXGIFactory->EnumAdapters1(i, &lpAdapter);
		if(Result == DXGI_ERROR_NOT_FOUND) break;

		lpAdapter->GetDesc1(&AdaptDesc);
		Output.push_back(AdaptDesc);
		lpAdapter->Release();
	}
}
//枚举显示器(显示适配器,@枚举信息)
VOID EnumMonitors(IDXGIAdapter* lpAdapter, std::vector<DXGIOutputDesc> &Output){
	RESULT Result;
	IDXGIOutput *lpMonitor;
	DXGIOutputDesc MonitorDesc;

	for(UINT i = 0; TRUE; ++i){
		Result = lpAdapter->EnumOutputs(i, &lpMonitor);
		if(Result == DXGI_ERROR_NOT_FOUND) break;

		lpMonitor->GetDesc(&MonitorDesc);
		Output.push_back(MonitorDesc);
		lpMonitor->Release();
	}
}
//枚举显示模式(显示设备,像素格式,@枚举信息)
VOID EnumDisplayModes(IDXGIOutput* lpMonitor, DXGIFormat Format, DXGIModeDesc Output[]){
	UINT ModeCount = 0;
	lpMonitor->GetDisplayModeList(Format, 0x0, &ModeCount, NULL);
	lpMonitor->GetDisplayModeList(Format, 0x0, &ModeCount, Output);
}

//检查最大采样质量(D3D设备,纹理格式,采样次数)[最大采样质量]
inline UINT CheckMaxSampleQuality(ID3DDevice* lpD3DDevice, DXGIFormat Format, UINT SampleCount){
	D3DSampleFeature Feature;

	Feature.Format = Format;
	Feature.SampleCount = SampleCount;
	Feature.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	Feature.NumQualityLevels = 0;

	lpD3DDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&Feature, sizeof(Feature));

	return(Feature.NumQualityLevels - 1);
}
//检查D3D特征等级(D3D设备)[特征等级]
inline D3DFeatureLevel CheckD3DFeatureLevel(ID3DDevice* lpD3DDevice){
	D3DFeature Feature;
	D3DFeatureLevel arrLevel[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	Feature.NumFeatureLevels = ARR_LEN(arrLevel);
	Feature.pFeatureLevelsRequested = arrLevel;

	lpD3DDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
		&Feature, sizeof(D3DFeature));

	return Feature.MaxSupportedFeatureLevel;
}

//----------------------------------------//


//--------------------文本信息--------------------//

VOID GetTextSize(IDWFactory* lpDWFactory, IDWTxtFormat* lpFormat, WCHR* Text, out D2DSizeF &Size){
	DWTxtMetrics TxtMetrics;
	IDWTxtLayout *lpTxtLayout;

	lpDWFactory->CreateTextLayout(Text, (UINT)WcsLen(Text), lpFormat, 0.0f, 0.0f, &lpTxtLayout);
	lpTxtLayout->GetMetrics(&TxtMetrics);

	Size.width = TxtMetrics.widthIncludingTrailingWhitespace;
	Size.height = TxtMetrics.height;

	lpTxtLayout->Release();
}

//----------------------------------------//