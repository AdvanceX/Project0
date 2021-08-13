//--------------------资源创建--------------------//

//创建上传缓存区(D3D设备,大小,常量标记)[缓存区]
ID3DResource* CreateUploadBuffer(ID3DDevice* lpD3DDevice, UINT Size, BOOL bConst){
	ID3DResource *lpBuffer;

	if(bConst) Size = GetMulOf2Pow(Size, 256);

	lpD3DDevice->CreateCommittedResource(
		&D3DHeapProps(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&D3DResDesc::Buffer(Size), D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL, COM_ARGS(&lpBuffer));

	return lpBuffer;
}

//创建源声(XAudio工厂,文件路径)[源声]
IXASrcVoice* CreateSourceVoiceFromFile(IXAudio2 *lpXAFactory, const CHAR* FilePath){
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

	ZeroMemory(&AudioBuff, sizeof(XABuffer));
	AudioBuff.AudioBytes = ChunkSize;
	AudioBuff.pAudioData = AudioBytes;
	AudioBuff.Flags = XAUDIO2_END_OF_STREAM;

	lpXAFactory->CreateSourceVoice(&lpSrcVoice, (WAVEFORMATEX*)&WaveFormat);
	lpSrcVoice->SubmitSourceBuffer(&AudioBuff);

	return lpSrcVoice;
}

//创建位图(D2DDC,WIC工厂,文件路径)[位图]
ID2DBitmap* CreateBitmapFromFile(ID2DDevCtx* lpD2DDC, IWICFactory* lpWICFactory, const WCHR* FilePath){
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
ID3DBlob* CompileShaderFromFile(const WCHR* FilePath, const D3DShaderMacro* arrMacro, const CHAR* Entrance, const CHAR* Target){
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
	UINT Offset;
	DWRD SrcType;
	DWRD ReadSize;

	while(TRUE){
		ReadFile(hFile, &SrcType, sizeof(UINT), &ReadSize, NULL);
		if(ReadSize == 0) return INVALID_HANDLE_VALUE;

		ReadFile(hFile, &Offset, sizeof(UINT), NULL, NULL);
		if(SrcType == ChunkType) return hFile;
		SetFilePointer(hFile, Offset, NULL, FILE_CURRENT);
	}
}

//搜索文件块(文件路径,块类型)[文件句柄]
HANDLE FindFileChunk(const TCHR* FilePath, DWRD ChunkType){
	HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	return FindFileChunk(hFile, ChunkType);
}

//----------------------------------------//


//--------------------设备检查--------------------//

//枚举显示适配器(DXGI工厂,@枚举信息)
VOID EnumDisplayAdapters(IDXGIFactory6* lpDXGIFactory, STLList<DXGIAdapterDesc> &Output){
	IDXGIAdapter1 *lpAdapter;
	DXGIAdapterDesc AdaptDesc;

	for(UINT i = 0; TRUE; ++i){
		HRESULT Result = lpDXGIFactory->EnumAdapters1(i, &lpAdapter);
		if(Result == DXGI_ERROR_NOT_FOUND) break;

		lpAdapter->GetDesc1(&AdaptDesc);
		Output.push_back(AdaptDesc);
		lpAdapter->Release();
	}
}
//枚举显示输出(显示适配器,@枚举信息)
VOID EnumDisplayOutputs(IDXGIAdapter* lpAdapter, STLList<DXGIOutputDesc> &Output){
	IDXGIOutput *lpDevice;
	DXGIOutputDesc DevDesc;

	for(UINT i = 0; TRUE; ++i){
		HRESULT Result = lpAdapter->EnumOutputs(i, &lpDevice);
		if(Result == DXGI_ERROR_NOT_FOUND) break;

		lpDevice->GetDesc(&DevDesc);
		Output.push_back(DevDesc);
		lpDevice->Release();
	}
}
//枚举显示模式(显示设备,像素格式,@枚举信息)[枚举数量]
UINT EnumDisplayModes(IDXGIOutput* lpDevice, DXGIFormat Format, SharedPtr<DXGIModeDesc> &Output){
	UINT ModeCount;

	lpDevice->GetDisplayModeList(Format, 0x0, &ModeCount, NULL);
	Output.reset(new DXGIModeDesc[ModeCount]);
	lpDevice->GetDisplayModeList(Format, 0x0, &ModeCount, Output.get());

	return ModeCount;
}

//----------------------------------------//

//--------------------设备检查--------------------//

//检查采样质量等级(D3D设备,纹理格式,采样次数)[质量等级]
UINT CheckSampleQualityLevel(ID3DDevice* lpD3DDevice, DXGIFormat Format, UINT SampleCount){
	D3DSampleFeature Feature;

	Feature.Format = Format;
	Feature.SampleCount = SampleCount;
	Feature.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	Feature.NumQualityLevels = 0;

	lpD3DDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&Feature, sizeof(Feature));

	return Feature.NumQualityLevels - 1;
}
//检查D3D特征等级(D3D设备)[特征等级]
D3DFeatureLevel CheckD3DFeatureLevel(ID3DDevice* lpD3DDevice){
	D3DLevelFeature Feature;
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
		&Feature, sizeof(Feature));

	return Feature.MaxSupportedFeatureLevel;
}

//----------------------------------------//


//--------------------文本信息--------------------//

VOID GetTextSize(IDWFactory* lpDWFactory, IDWTxtFormat* lpFormat, WCHR* Text, D2DSizeF &Size){
	DWTxtMetrics TxtMetrics;
	IDWTxtLayout *lpTxtLayout;

	lpDWFactory->CreateTextLayout(Text, (UINT)StrLenW(Text), lpFormat, 0.0f, 0.0f, &lpTxtLayout);
	lpTxtLayout->GetMetrics(&TxtMetrics);

	Size.width = TxtMetrics.widthIncludingTrailingWhitespace;
	Size.height = TxtMetrics.height;

	lpTxtLayout->Release();
}

//----------------------------------------//