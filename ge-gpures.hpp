//-------------------- 上传缓存区 --------------------//

// Upload Buffer
class CUploadBuff{
	_rest UNS32 Size;               // 缓存大小
	_rest UNS32 Pitch;              // 缓存节距
	_rest BYTET *psCpuMap;          // 内存映射
	_rest ID3D12Resource *lpKernel; // 核心对象

	_open ~CUploadBuff(){
		$m.Release();
	}
	_open CUploadBuff(){
		Var_Zero(this);
	}
	_open CUploadBuff(ID3D12Device4 *pD3dDev, UNS32 Division, UNS32 Pitch, IBOOL bConst){
		$m.Initialize(pD3dDev, Division, Pitch, bConst);
	}
	//////
	_open $VOID Release(){
		if($m.lpKernel){
			if($m.psCpuMap){
				$m.lpKernel->Unmap(0, P_Null);
				$m.psCpuMap = P_Null;
			}
			$m.lpKernel->Release();
			$m.lpKernel = P_Null;
		}
	}
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, UNS32 Division, UNS32 Pitch, IBOOL bConst){
		$m.Size = Division * Pitch;
		$m.Pitch = Pitch;
		$m.psCpuMap = P_Null;
		$m.lpKernel = CreateUploadBuffer(pD3dDev, $m.Size, bConst);
	}
	_open $VOID EnableUpdate(IBOOL bEnable, IBOOL bClear){
		if(!bEnable && $m.psCpuMap){
			$m.lpKernel->Unmap(0, P_Null);
			$m.psCpuMap = P_Null;
		} else if(bEnable && !$m.psCpuMap){
			$m.lpKernel->Map(0, P_Null, ($VOID**)&$m.psCpuMap);
			if(bClear) Mem_Zero($m.psCpuMap, $m.Size);
		}
	}
	_open PVOID GetMemory(UNS32 UnitIdx){
		UNS32 Offset = $m.Pitch * UnitIdx;
		return $m.psCpuMap + Offset;
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetVram(UNS32 UnitIdx){
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		UNS32 Offset = $m.Pitch * UnitIdx;
		return hBuffer + Offset;
	}
};

//----------------------------------------//


//-------------------- 索引、顶点缓存区 --------------------//

// Index Buffer
class CIdxBuff: public CUploadBuff{
	_secr UNS32 Count;

	_open ~CIdxBuff(){}
	_open CIdxBuff(): CUploadBuff(){}
	_open CIdxBuff(ID3D12Device4 *pD3dDev, UNS32 Division, DXGI_FORMAT Format, BOOL bEnable = B_True): CUploadBuff(pD3dDev, Division, (UNS32)dxlh::BitsPerPixel(Format), B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, UNS32 Division, DXGI_FORMAT Format, BOOL bEnable = B_True){
		$sup Initialize(pD3dDev, Division, (UNS32)dxlh::BitsPerPixel(Format), B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	_open $VOID SetCount(UNS32 Count){
		$m.Count = Count;
	}
	_open UNS32 GetCount(){
		return $m.Count;
	}
	_open UNS32 GetDivision(){
		return $m.Size / $m.Pitch;
	}
	_open DXGI_FORMAT GetFormat(){
		switch($m.Pitch){
			case 16: return DXGI_FORMAT_R16_UINT;
			case 32: return DXGI_FORMAT_R32_UINT;
			default: return DXGI_FORMAT_UNKNOWN;
		}
	}
	_open D3D12_INDEX_BUFFER_VIEW GetSrv(){
		UNS32 Size = $m.Count * $m.Pitch;
		DXGI_FORMAT Format = $m.GetFormat();
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		return D3D12_INDEX_BUFFER_VIEW{ hBuffer, Size, Format };
	}
};

// Vertex Buffer
class CVtxBuff: public CUploadBuff{
	_secr UNS32 Count;

	_open ~CVtxBuff(){}
	_open CVtxBuff(): CUploadBuff(){}
	_open CVtxBuff(ID3D12Device4 *pD3dDev, UNS32 Division, UNS32 Pitch, BOOL bEnable = B_True): CUploadBuff(pD3dDev, Division, Pitch, B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, UNS32 Division, UNS32 Pitch, BOOL bEnable = B_True){
		$sup Initialize(pD3dDev, Division, Pitch, B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	_open $VOID SetCount(UNS32 Count){
		$m.Count = Count;
	}
	_open UNS32 GetCount(){
		return $m.Count;
	}
	_open UNS32 GetDivision(){
		return $m.Size / $m.Pitch;
	}
	_open D3D12_VERTEX_BUFFER_VIEW GetSrv(){
		UNS32 Size = $m.Count * $m.Pitch;
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		return D3D12_VERTEX_BUFFER_VIEW{ hBuffer, Size, $m.Pitch };
	}
};

//----------------------------------------//


//-------------------- 灯光、材质库 --------------------//

// Light Repository
class CLitRepos: public CUploadBuff{
	_secr UNS32 Count;
	_secr UNS32 Division;

	_open ~CLitRepos(){}
	_open CLitRepos(): CUploadBuff(){}
	_open CLitRepos(ID3D12Device4 *pD3dDev, UNS32 Capacity, BOOL bEnable = B_True): CUploadBuff(pD3dDev, Capacity, sizeof(GLight), B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Division = Capacity;
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, UNS32 Capacity, BOOL bEnable = B_True){
		$sup Initialize(pD3dDev, Capacity, sizeof(GLight), B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Division = Capacity;
		$m.Count = 0;
	}
	_open $VOID UpdateSpotLight(_in GLight &Value, UNS32 Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		GLight *pLight = (GLight*)$m.GetMemory(Index);
		FVector3 *pLitDir = (FVector3*)&pLight->Direction;

		*pLight = Value;
		pLitDir->Normalize();
	}
	_open $VOID UpdatePointLight(_in GLight &Value, UNS32 Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		GLight *pLight = (GLight*)$m.GetMemory(Index);
		pLight->AttEnd = Value.AttEnd;
		pLight->AttStart = Value.AttStart;
		pLight->Position = Value.Position;
		pLight->Intensity = Value.Intensity;
		pLight->SpotPower = 0.f;
	}
	_open $VOID UpdateDirectionalLight(_in GLight &Value, UNS32 Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		GLight *pLight = (GLight*)$m.GetMemory(Index);
		FVector3 *pLitDir = (FVector3*)&pLight->Direction;

		pLight->AttEnd = FLT_INFINITY;
		pLight->AttStart = 0;
		pLight->SpotPower = 0.f;
		pLight->Intensity = Value.Intensity;
		pLight->Direction = Value.Direction;
		pLitDir->Normalize();
	}
	_open $VOID TurnOffLight(UNS32 Index){
		GLight *pLight = (GLight*)$m.GetMemory(Index);
		if(pLight->AttEnd > 0.f) pLight->AttEnd = -pLight->AttEnd;
	}
	_open $VOID TurnOnLight(UNS32 Index){
		GLight *pLight = (GLight*)$m.GetMemory(Index);
		if(pLight->AttEnd < 0.f) pLight->AttEnd = -pLight->AttEnd;
	}
	_open UNS32 GetCapacity(){
		return $m.Division;
	}
	_open UNS32 GetCount(){
		return $m.Count;
	}
};

// Material Repository
class CMtlRepos{
	_open struct NTexView{
		UNS32 Ident;
		ID3D12Resource *pSource;
	};
	_open struct NMtlView{
		UNS32 Ident;
		UNS16 iMtlBase;
		UNS16 iColorMap;
		UNS16 iNormMap;
		UNS16 iRoughMap;
		UNS16 iAlphaMap;
		UNS16 iEmissMap;
		UNS16 iOcclusMap;
	};

	_secr UNS32 MtlCount;
	_secr UNS32 TexCount;
	_secr NMtlView *lprgMtlView;
	_secr NTexView *lprgTexView;
	_secr ID3D12Resource *lpMtlBuff;
	_open ID3D12DescriptorHeap *lpTexSrvs;

	_open ~CMtlRepos(){
		$m.Release();
	}
	_open CMtlRepos(){
		Var_Zero(this);
	}
	_open CMtlRepos(UNS32 MtlCount, UNS32 TexCount){
		$m.Initialize(MtlCount, TexCount);
	}
	//////
	_open $VOID Release(){
		if($m.TexCount > 0){
			for(UNS32 iTex = 0; iTex < $m.TexCount; ++iTex)
				$m.lprgTexView[iTex].pSource->Release();
		}

		SAFE_RELEASE($m.lpMtlBuff);
		SAFE_RELEASE($m.lpTexSrvs);

		SAFE_FREE($m.lprgMtlView);
		SAFE_FREE($m.lprgTexView);
	}
	_open $VOID Initialize(UNS32 MtlCount, UNS32 TexCount){
		GMaterial *prgMtrl;
		NTexView *prgViewT;

		DYNARR_ALLOC(prgMtrl, MtlCount);
		DYNARR_ALLOC(prgViewT, TexCount);

		$m.lprgMtlView = (NMtlView*)prgMtrl;
		$m.lprgTexView = (NTexView*)prgViewT;
		$m.lpMtlBuff = P_Null;
		$m.lpTexSrvs = P_Null;
		$m.MtlCount = 0;
		$m.TexCount = 0;
	}
	_open $VOID UploadToVram(DXTKUploader &Uploader){
		ID3D12Device4 *pD3dDev = (ID3D12Device4*)Uploader.GetDevice();
		GMaterial *lprgMtrl = (GMaterial*)$m.lprgMtlView;

		if($m.TexCount != 0){
			DYNARR_REALLOC($m.lprgTexView, $m.TexCount);
			qsort($m.lprgTexView, $m.TexCount, sizeof(NTexView), tlCompareAsc<UNS32>);
			$m.CreateTextureSrvs(pD3dDev);
		} else{
			free($m.lprgTexView);
		}

		if($m.MtlCount != 0){
			qsort(lprgMtrl, $m.MtlCount, sizeof(GMaterial), tlCompareAsc<UNS32>);
			$m.CreateMaterialViews(lprgMtrl);
			$m.CreateMaterialOnVram(Uploader, lprgMtrl);
		}

		free(lprgMtrl);
	}
	_open $VOID AddMaterial(_in GMaterial &Material){
		GMaterial *prgMtrl = (GMaterial*)$m.lprgMtlView;
		DynArrAppend(prgMtrl, $m.MtlCount, Material);
	}
	_open $VOID AddTexture(_in CHAR8 *psName, ID3D12Resource *pTexture){
		NTexView *pTexView = DynArrReserve($m.lprgTexView, $m.TexCount, 1);
		pTexView->Ident = StrHash32(psName);
		pTexView->pSource = pTexture;
	}
	_open $VOID AddMaterialsFromFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		IBOOL bFound = FindFileChunk(hrFile, ASSET_TYPE_Material);

		if(!bFound) goto label_0;

		UNS32 Count;
		ReadFile(hrFile, &Count, sizeof(UNS32), P_Null, P_Null);
		GMaterial *prgMtrl = (GMaterial*)$m.lprgMtlView;
		prgMtrl = DynArrReserve(prgMtrl, $m.MtlCount, Count);
		ReadFile(hrFile, prgMtrl, Count * sizeof(GMaterial), P_Null, P_Null);

	label_0:
		CloseHandle(hrFile);
	}
	_open $VOID AddMaterialsFromCatalog(_in WCHAR *psCatalog){
		UIPTR CtlgLen;
		HANDLE hrSeeker;
		WCHAR FilePath[MAX_PATH];
		WIN32_FIND_DATAW FileInfo;

		wcscat(wcscpy(FilePath, psCatalog), L"*.dat");
		hrSeeker = FindFirstFileW(FilePath, &FileInfo);
		if(INVALID_(hrSeeker)) return;
		CtlgLen = wcslen(psCatalog);

		do{
			wcscpy(&FilePath[CtlgLen], FileInfo.cFileName);
			$m.AddMaterialsFromFile(FilePath);
		} while(FindNextFileW(hrSeeker, &FileInfo));

		FindClose(hrSeeker);
	}
	_open $VOID AddTexturesFromCatalog(DXTKUploader &Uploader, _in WCHAR *psCatalog){
		UIPTR CtlgLen;
		HANDLE hrSeeker;
		WCHAR FilePath[MAX_PATH];
		WIN32_FIND_DATAW FileInfo;

		wcscat(wcscpy(FilePath, psCatalog), L"*.dds");
		hrSeeker = FindFirstFileW(FilePath, &FileInfo);
		if(INVALID_(hrSeeker)) return;
		CtlgLen = wcslen(psCatalog);

		//////

		NTexView *pTexView;

		do{
			wcscpy(&FilePath[CtlgLen], FileInfo.cFileName);
			*wcsrchr(FileInfo.cFileName, L'.') = L'\0';
			pTexView = DynArrReserve($m.lprgTexView, $m.TexCount, 1);
			pTexView->Ident = StrHash32(W_TO_A(FileInfo.cFileName));
			dx::CreateDDSTextureFromFile(Uploader.GetDevice(), Uploader, FilePath, &pTexView->pSource);
		} while(FindNextFileW(hrSeeker, &FileInfo));

		FindClose(hrSeeker);
	}
	_secr $VOID CreateMaterialOnVram(DXTKUploader &Uploader, _in GMaterial *prgMatl){
		UNS32 Count = 0;
		BYTET *psSource = (BYTET*)&prgMatl[0].Base;
		GMtlBase *lprgDest = new GMtlBase[$m.MtlCount];

		for(UNS32 iMtl = 0; iMtl < $m.MtlCount; ++iMtl){
			PVOID pDest = lsearch(psSource, lprgDest, &Count, sizeof(GMtlBase), CMtlRepos::CompareMatl);
			UIPTR iDest = (GMtlBase*)pDest - lprgDest;
			psSource += sizeof(GMaterial);
			$m.lprgMtlView[iMtl].iMtlBase = (UNS16)iDest;
		}

		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lprgDest/*被接管*/,
			1, ToPowOf2Mul(Count * sizeof(GMtlBase), 256),
			D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpMtlBuff);
	}
	_secr $VOID CreateMaterialViews(_in GMaterial *prgMatl){
		$m.lprgMtlView = new NMtlView[$m.MtlCount];

		NMtlView *pDest = $m.lprgMtlView;
		const GMaterial *pSource = prgMatl;

		for(UNS32 iMtl = 0; iMtl < $m.MtlCount; ++iMtl){
			pDest->iMtlBase = iMtl;
			pDest->Ident = pSource->Ident;

			UNS16 *prgValue = &pDest->iColorMap;
			const UNS32 *prgKey = &pSource->Maps.iAlbedo;

			for(UNS32 iLayer = 0; iLayer < 6; ++iLayer){ //6种贴图
				if(prgKey[iLayer] == 0){
					prgValue[iLayer] = UINT16_MAX;
					continue;
				}
				for(UNS32 iTex = 0; iTex < $m.TexCount; ++iTex){ //根据ID获取索引
					if(prgKey[iLayer] == $m.lprgTexView[iTex].Ident){
						prgValue[iLayer] = iTex;
						break;
					}
				}
			}

			pDest++;
			pSource++;
		}
	}
	_secr $VOID CreateTextureSrvs(ID3D12Device4 *pD3dDev){
		$m.lpTexSrvs = CreateSrvHeap(pD3dDev, $m.TexCount);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE hTexSrv($m.lpTexSrvs->GetCPUDescriptorHandleForHeapStart());
		NTexView *pTexView = $m.lprgTexView;

		for(UNS32 iTex = 0; iTex < $m.TexCount; ++iTex){
			dx::CreateShaderResourceView(pD3dDev, pTexView->pSource, hTexSrv);
			hTexSrv.Offset(GE3d::cbSRView);
			pTexView++;
		}
	}
	_open const NTexView *GetTextureById(UNS32 Ident){
		return (NTexView*)bsearch(&Ident,
			$m.lprgTexView, $m.TexCount, sizeof(NTexView), tlCompareAsc<UNS32>);
	}
	_open const NMtlView *GetMaterialById(UNS32 Ident){
		return (NMtlView*)bsearch(&Ident,
			$m.lprgMtlView, $m.MtlCount, sizeof(NMtlView), tlCompareAsc<UNS32>);
	}
	_open D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrv(UNS32 Index){
		CD3DX12_GPU_DESCRIPTOR_HANDLE hView($m.lpTexSrvs->GetGPUDescriptorHandleForHeapStart());
		return hView.Offset(Index, GE3d::cbSRView);
	}
	_open D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvById(UNS32 Ident){
		const NTexView *pView = $m.GetTextureById(Ident);
		const UIPTR iView = pView - $m.lprgTexView;
		return $m.GetTextureSrv((UNS32)iView);
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetMaterialVram(UNS32 Index){
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpMtlBuff->GetGPUVirtualAddress();
		return hBuffer + (Index * sizeof(GMtlBase));
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetMaterialVramById(UNS32 Ident){
		const NMtlView *pView = $m.GetMaterialById(Ident);
		const UIPTR iView = pView - $m.lprgMtlView;
		const UNS32 iBase = $m.lprgMtlView[iView].iMtlBase;
		return $m.GetMaterialVram(iBase);
	}

	_secr static INT32 CompareMatl(_in $VOID *pData0, _in $VOID *pData1){
		SPFPN *pValue0 = (SPFPN*)pData0;
		SPFPN *pValue1 = (SPFPN*)pData1;

		for(UNS32 iVal = 0; iVal < DW_CNT(GMtlBase); ++iVal){
			if(*pValue0 != *pValue1) return -1;
			pValue0++;
			pValue1++;
		}

		return 0;
	}
};

//----------------------------------------//


//-------------------- 骨架 --------------------//

// Skeleton
class CSkeleton{
	_secr UNS32 numBone;
	_secr MXBone *lprgBone;
	_secr FMatrix4 *prgBoneXform;
	_secr ID3D12Resource *lpPosture;

	_open ~CSkeleton(){
		$m.Release();
	}
	_open CSkeleton(){
		Var_Zero(this);
	}
	_open CSkeleton(ID3D12Device4 *pD3dDev, _in WCHAR *psFilePath){
		$m.Initialize(pD3dDev, psFilePath);
	}
	//////
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, _in WCHAR *psFilePath){
		HANDLE hrFile;
		IBOOL bFound;
		UNS32 BufSize;
		CHAR8 Buffer[128];
		MXBone *pBone, *pEndBone;

		hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		bFound = FindFileChunk(hrFile, ASSET_TYPE_Skeleton);

		if(!bFound) goto label_0;

		ReadFile(hrFile, &$m.numBone, sizeof(UNS32), P_Null, P_Null);
		BufSize = $m.numBone * sizeof(MFlt4x4);

		$m.lprgBone = new MXBone[$m.numBone];
		$m.lpPosture = CreateUploadBuffer(pD3dDev, BufSize, B_False);
		$m.lpPosture->Map(0, P_Null, ($VOID**)&$m.prgBoneXform);

		pBone = $m.lprgBone;
		pEndBone = pBone + $m.numBone;

		for(; pBone != pEndBone; ++pBone){
			ReadFile(hrFile, &BufSize, sizeof(UNS32), P_Null, P_Null);
			ReadFile(hrFile, Buffer, BufSize, P_Null, P_Null);
			ReadFile(hrFile, &pBone->iParent, sizeof(UNS32), P_Null, P_Null);
			ReadFile(hrFile, &pBone->matOffset, sizeof(MFlt4x4), P_Null, P_Null);
			ReadFile(hrFile, &pBone->matInitial, sizeof(MFlt4x4), P_Null, P_Null);

			Buffer[BufSize] = '\0';
			pBone->Ident = StrHash32(Buffer);
			pBone->matLocal = pBone->matInitial;
			pBone->matCombine = dx::XMMatrixIdentity();
		}

	label_0:
		CloseHandle(hrFile);
	}
	_open $VOID Release(){
		if($m.lprgBone){
			delete[] $m.lprgBone;

			$m.lpPosture->Unmap(0, P_Null);
			$m.lpPosture->Release();

			$m.lprgBone = P_Null;
			$m.prgBoneXform = P_Null;
			$m.lpPosture = P_Null;
		}
	}
	_open $VOID Update(){
		MXMATRIX matFinal;
		MXBone *pBone, *pSupBone;

		pBone = $m.lprgBone;
		pBone->matCombine = pBone->matLocal;
		matFinal = pBone->matOffset * pBone->matCombine;
		$m.prgBoneXform[0] = dx::XMMatrixTranspose(matFinal);

		for(UNS32 iBone = 1; iBone < $m.numBone; ++iBone){
			pBone = $m.lprgBone + iBone;
			pSupBone = $m.lprgBone + pBone->iParent;
			pBone->matCombine = pBone->matLocal * pSupBone->matCombine;
			matFinal = pBone->matOffset * pBone->matCombine;
			$m.prgBoneXform[iBone] = dx::XMMatrixTranspose(matFinal);
		}
	}
	_open $VOID Reset(){
		MXBone *pBone = $m.lprgBone;
		MXBone *pEndBone = pBone + $m.numBone;

		for(; pBone != pEndBone; ++pBone)
			pBone->matLocal = pBone->matInitial;
	}
	_open MXBone *GetBones(){
		return $m.lprgBone;
	}
	_open UNS32 GetCount(){
		return $m.numBone;
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetPosture(){
		return $m.lpPosture->GetGPUVirtualAddress();
	}
};

//----------------------------------------//


//-------------------- 动画 --------------------//

// Animation
class CAnimation{
	_secr UNS16 iCurRot;
	_secr UNS16 iCurScale;
	_secr UNS16 iCurTrans;
	_secr UNS16 numTrans;
	_secr UNS16 numScaling;
	_secr UNS16 numRotation;
	_secr GRotFrame *lprgRotation;
	_secr GAnimFrame *lprgScaling;
	_secr GAnimFrame *lprgTrans;

	_open ~CAnimation(){
		$m.Release();
	}
	_open CAnimation(){
		Var_Zero(this);
	}
	_open CAnimation(HANDLE hFile){
		$m.Initialize(hFile);
	}
	//////
	_open $VOID Release(){
		SAFE_DELETEA($m.lprgTrans);
		SAFE_DELETEA($m.lprgScaling);
		SAFE_DELETEA($m.lprgRotation);
	}
	_open $VOID Initialize(HANDLE hFile){
		UNS32 Count0, Count1, Count2;

		ReadFile(hFile, &Count0, sizeof(UNS32), P_Null, P_Null);
		ReadFile(hFile, &Count1, sizeof(UNS32), P_Null, P_Null);
		ReadFile(hFile, &Count2, sizeof(UNS32), P_Null, P_Null);

		CAST_($m.numTrans, Count0);
		CAST_($m.numScaling, Count1);
		CAST_($m.numRotation, Count2);

		$m.lprgTrans = new GAnimFrame[Count0];
		$m.lprgScaling = new GAnimFrame[Count1];
		$m.lprgRotation = new GRotFrame[Count2];

		ReadFile(hFile, $m.lprgTrans, sizeof(GAnimFrame) * Count0, P_Null, P_Null);
		ReadFile(hFile, $m.lprgScaling, sizeof(GAnimFrame) * Count1, P_Null, P_Null);
		ReadFile(hFile, $m.lprgRotation, sizeof(GRotFrame) * Count2, P_Null, P_Null);
	}
	_open $VOID Interpolate(SPFPN Time, _out MXMATRIX &rXform){
		MXVECTOR Center = dx::g_XMZero;
		MXVECTOR Trans, Scaling, Rotation;

		if($m.numTrans == 0) Trans = dx::g_XMZero;
		else $m.InterpolateTranslation(Time, Trans);

		if($m.numScaling == 0) Scaling = dx::g_XMOne;
		else $m.InterpolateScaling(Time, Scaling);

		if($m.numRotation == 0) Rotation = dx::g_XMIdentityR3;
		else $m.InterpolateRotation(Time, Rotation);

		rXform = dx::XMMatrixAffineTransformation(Center, Scaling, Rotation, Trans);
	}
	_secr $VOID InterpolateScaling(SPFPN Time, _out MXVECTOR &rXform){
		UNS32 iEndFrm = $m.numScaling - 1;
		UNS16 &rCurFrm = $m.iCurScale;
		GAnimFrame *prgFrame = $m.lprgScaling;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Ld_V3(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Ld_V3(&prgFrame[iEndFrm].Xform);
		} else{
			GAnimFrame *pFrm0 = &prgFrame[rCurFrm];
			GAnimFrame *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MXVECTOR Start = Ld_V3(&pFrm0->Xform);
					MXVECTOR End = Ld_V3(&pFrm1->Xform);
					rXform = dx::XMVectorLerp(Start, End, Weight);
					return;
				} else{
					pFrm0 = pFrm1++;
					rCurFrm++;
				}
			}
		}
	}
	_secr $VOID InterpolateRotation(SPFPN Time, _out MXVECTOR &rXform){
		UNS32 iEndFrm = $m.numRotation - 1;
		UNS16 &rCurFrm = $m.iCurRot;
		GRotFrame *prgFrame = $m.lprgRotation;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Ld_V4(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Ld_V4(&prgFrame[iEndFrm].Xform);
		} else{
			GRotFrame *pFrm0 = &prgFrame[rCurFrm];
			GRotFrame *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MXVECTOR Start = Ld_V4(&pFrm0->Xform);
					MXVECTOR End = Ld_V4(&pFrm1->Xform);
					rXform = dx::XMQuaternionSlerp(Start, End, Weight);
					return;
				} else{
					pFrm0 = pFrm1++;
					rCurFrm++;
				}
			}
		}
	}
	_secr $VOID InterpolateTranslation(SPFPN Time, _out MXVECTOR &rXform){
		UNS32 iEndFrm = $m.numTrans - 1;
		UNS16 &rCurFrm = $m.iCurTrans;
		GAnimFrame *prgFrame = $m.lprgTrans;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Ld_V3(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Ld_V3(&prgFrame[iEndFrm].Xform);
		} else{
			GAnimFrame *pFrm0 = &prgFrame[rCurFrm];
			GAnimFrame *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MXVECTOR Start = Ld_V3(&pFrm0->Xform);
					MXVECTOR End = Ld_V3(&pFrm1->Xform);
					rXform = dx::XMVectorLerp(Start, End, Weight);
					return;
				} else{
					pFrm0 = pFrm1++;
					rCurFrm++;
				}
			}
		}
	}
};

// Animation Assembly
class CAnimAssy{
	_secr SPFPN Duration;
	_secr UNS32 numElem;
	_secr CAnimation *lprgElem;

	_open ~CAnimAssy(){
		$m.Release();
	}
	_open CAnimAssy(){
		Var_Zero(this);
	}
	_open CAnimAssy(_in WCHAR *psFilePath){
		$m.Initialize(psFilePath);
	}
	//////
	_open $VOID Release(){
		SAFE_DELETEA($m.lprgElem);
	}
	_open $VOID Initialize(_in WCHAR *psFilePath){
		IBOOL bFound;
		HANDLE hrFile;
		CAnimation *pAnim;
		CAnimation *pAEnd;

		hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		bFound = FindFileChunk(hrFile, ASSET_TYPE_Anim);

		if(!bFound) goto label_0;

		ReadFile(hrFile, &$m.Duration, sizeof(SPFPN), P_Null, P_Null);
		ReadFile(hrFile, &$m.numElem, sizeof(UNS32), P_Null, P_Null);

		pAnim = $m.lprgElem = new CAnimation[$m.numElem];
		pAEnd = pAnim + $m.numElem;

		for(; pAnim != pAEnd; ++pAnim)
			pAnim->Initialize(hrFile);

	label_0:
		CloseHandle(hrFile);
	}
	_open $VOID ApplyToBones(SPFPN Time, _out MXBone *prgBone){
		for(UNS32 iElm = 0; iElm < $m.numElem; ++iElm){
			MXBone &rBone = prgBone[iElm];
			CAnimation &rAnim = $m.lprgElem[iElm];
			rAnim.Interpolate(Time, rBone.matLocal);
		}
	}
	_open SPFPN GetDuration(){
		return $m.Duration;
	}
	_open UNS32 GetCount(){
		return $m.numElem;
	}
};

//----------------------------------------//


//-------------------- 3D网格 --------------------//

// 3D Mesh
class CMesh3D{
	_open UNS32 Ident;
	_open UNS32 numSubset;
	_open UNS32 cbIdxBuff;
	_open UNS32 cbVtxBuff;
	_open DWORD IdxFormat;
	_open DWORD VtxFormat;
	_secr ID3D12Resource *lpIdxBuff;
	_secr ID3D12Resource *lpVtxBuff;
	_open GSubMesh *lprgSubset;
	_open CMesh3D *pNext;

	_open ~CMesh3D(){
		$m.Release();
	}
	_open CMesh3D(){
		Var_Zero(this);
	}
	_open CMesh3D(DXTKUploader &Uploader, HANDLE hFile){
		$m.Initialize(Uploader, hFile);
	}
	//////
	_open $VOID Initialize(DXTKUploader &Uploader, HANDLE hFile){
		UNS32 NameLen;
		CHAR8 Name[128];
		BYTET *lpsIndices;
		BYTET *lpsVertexs;

		//头部

		ReadFile(hFile, &NameLen, sizeof(UNS32), P_Null, P_Null);
		ReadFile(hFile, Name, NameLen, P_Null, P_Null);
		Name[NameLen] = '\0';

		$m.Ident = StrHash32(Name);
		$m.pNext = P_Null;
		$m.lprgSubset = P_Null;

		//基础信息

		ReadFile(hFile, &$m.IdxFormat, sizeof(DWORD), P_Null, P_Null);
		ReadFile(hFile, &$m.VtxFormat, sizeof(DWORD), P_Null, P_Null);
		ReadFile(hFile, &$m.cbIdxBuff, sizeof(UNS32), P_Null, P_Null);
		ReadFile(hFile, &$m.cbVtxBuff, sizeof(UNS32), P_Null, P_Null);
		ReadFile(hFile, &$m.numSubset, sizeof(UNS32), P_Null, P_Null);

		//顶点和索引

		lpsIndices = new BYTET[$m.cbIdxBuff];
		lpsVertexs = new BYTET[$m.cbVtxBuff];

		ReadFile(hFile, lpsIndices, $m.cbIdxBuff, P_Null, P_Null);
		ReadFile(hFile, lpsVertexs, $m.cbVtxBuff, P_Null, P_Null);

		//子集划分

		if($m.numSubset > 0){
			$m.lprgSubset = new GSubMesh[$m.numSubset];
			ReadFile(hFile, $m.lprgSubset, $m.numSubset * sizeof(GSubMesh), P_Null, P_Null);
		}

		//GPU资源

		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lpsIndices/*被接管*/,
			1, $m.cbIdxBuff, D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpIdxBuff);
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lpsVertexs/*被接管*/,
			1, $m.cbVtxBuff, D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpVtxBuff);
	}
	_open $VOID Release(){
		if($m.lpVtxBuff){
			SAFE_RELEASE($m.lpIdxBuff);
			SAFE_RELEASE($m.lpVtxBuff);
			SAFE_DELETEA($m.lprgSubset);
		}
	}
	_open UNS32 GetIndexCount(){
		return $m.cbIdxBuff / $m.GetIndexStride();
	}
	_open UNS32 GetVertexCount(){
		return $m.cbVtxBuff / $m.GetVertexStride();
	}
	_open UNS32 GetIndexStride(){
		switch($m.IdxFormat){
			case DXGI_FORMAT_R32_UINT: return 4;
			case DXGI_FORMAT_R16_UINT: return 2;
			default: return 0;
		}
	}
	_open UNS32 GetVertexStride(){
		CVtxView VtxView($m.VtxFormat);
		return VtxView.GetStride();
	}
	_open D3D12_INDEX_BUFFER_VIEW GetIndicesSrv(){
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpIdxBuff->GetGPUVirtualAddress();
		return D3D12_INDEX_BUFFER_VIEW{ hBuffer, $m.cbIdxBuff, (DXGI_FORMAT)$m.IdxFormat };
	}
	_open D3D12_VERTEX_BUFFER_VIEW GetVertexsSrv(){
		D3D12_GPU_VIRTUAL_ADDRESS hBuffer = $m.lpVtxBuff->GetGPUVirtualAddress();
		return D3D12_VERTEX_BUFFER_VIEW{ hBuffer, $m.cbVtxBuff, $m.GetVertexStride() };
	}
};

// Mesh Assembly
class CMeshAssy{
	_open UNS32 Count;
	_open CMesh3D *lpFirst;

	_open ~CMeshAssy(){
		$m.Release();
	}
	_open CMeshAssy(){
		Var_Zero(this);
	}
	_open CMeshAssy(DXTKUploader &Uploader, _in WCHAR *psFilePath){
		$m.Initialize(Uploader, psFilePath);
	}
	//////
	_open $VOID Initialize(DXTKUploader &Uploader, _in WCHAR *psFilePath){
		HANDLE hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		IBOOL bFound = FindFileChunk(hrFile, ASSET_TYPE_Model);

		if(!bFound) goto label_0;

		ReadFile(hrFile, &$m.Count, sizeof(UNS32), P_Null, P_Null);
		CMesh3D *pModel = $m.lpFirst = new CMesh3D(Uploader, hrFile);

		for(UNS32 iMesh = 1; iMesh < $m.Count; ++iMesh){
			pModel->pNext = new CMesh3D(Uploader, hrFile);
			pModel = pModel->pNext;
		}

	label_0:
		CloseHandle(hrFile);
	}
	_open $VOID Release(){
		CMesh3D *pNext;
		CMesh3D *pNode = $m.lpFirst;

		while(pNode){
			pNext = pNode->pNext;
			delete pNode;
			pNode = pNext;
		}

		$m.Count = 0;
		$m.lpFirst = P_Null;
	}
};

//----------------------------------------//


//-------------------- 高度场 --------------------//

// Height Field
class CHeightField{
	_open struct NDesc{
		IBOOL bSmooth;
		IBOOL bBslash;
		UNS32 ColCount;
		UNS32 RowCount;
		SPFPN CellWidth;
		SPFPN CellDepth;
		SPFPN HeightVal;
		SPFPN LowerLeftX;
		SPFPN LowerLeftZ;
	};
	_open struct NVertex{
		SPFPN CoordY;
		RGB10 Normal;
	};
	_open enum FACEFLAG{
		FACE_FLAG_Bslash = 0x80000000,
		FACE_FLAG_Smooth = 0x40000000,
		FACE_FLAG_Discard = 0x20000000,
	};

	_open UNS32 ColCount;
	_open UNS32 RowCount;
	_open SPFPN CellWidth;
	_open SPFPN CellDepth;
	_open SPFPN LowerLeftX;
	_open SPFPN LowerLeftZ;
	_open DWORD *prgTriFeat;
	_open NVertex *prgVertex;
	_secr ID3D12Resource *lpFeatBuff;
	_secr ID3D12Resource *lpVertBuff;

	_open ~CHeightField(){
		$m.Release();
	}
	_open CHeightField(){
		Var_Zero(this);
	}
	_open CHeightField(ID3D12Device4 *pD3dDev, _in NDesc &Desc){
		$m.Initialize(pD3dDev, Desc);
	}
	//////
	_open $VOID Release(){
		if($m.lpVertBuff){
			if($m.prgVertex){
				$m.lpFeatBuff->Unmap(0, P_Null);
				$m.lpVertBuff->Unmap(0, P_Null);
			}

			$m.lpFeatBuff->Release();
			$m.lpVertBuff->Release();

			Var_Zero(this);
			// endif
		} else if($m.prgVertex){
			delete[] $m.prgVertex;
			delete[] $m.prgTriFeat;
			$m.prgVertex = P_Null;
			$m.prgTriFeat = P_Null;
		}
	}
	_secr $VOID InitVertexs(SPFPN CoordY){
		RGB10 DefNorm = PVDec3N(0.f, 1.f, 0.f, 0.f);
		NVertex *pVertex = $m.prgVertex;
		for(UNS32 iRow = 0; iRow < $m.GetDensityZ(); ++iRow){
			for(UNS32 iCol = 0; iCol < $m.GetDensityX(); ++iCol){
				pVertex->Normal = DefNorm;
				pVertex->CoordY = CoordY;
				pVertex++;
			}
		}
	}
	_secr $VOID InitFeatures(IBOOL bSmooth, IBOOL bBslash){
		DWORD Value = 0x0;
		UNS32 Count = $m.ColCount * $m.RowCount * 2;

		if(bSmooth) Value |= FACE_FLAG_Smooth;
		if(bBslash) Value |= FACE_FLAG_Bslash;

		for(UNS32 iTri = 0; iTri < Count; ++iTri)
			$m.prgTriFeat[iTri] = Value;
	}
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, _in NDesc &Desc){
		UNS32 TriCount = Desc.ColCount * Desc.RowCount * 2;
		UNS32 VtxCount = (Desc.ColCount + 1) * (Desc.RowCount + 1);
		UNS32 cbFlagBuff = sizeof(DWORD) * TriCount;
		UNS32 cbVertBuff = sizeof(NVertex) * VtxCount;

		if(!pD3dDev){
			$m.lpFeatBuff = P_Null;
			$m.lpVertBuff = P_Null;
			$m.prgTriFeat = new DWORD[TriCount];
			$m.prgVertex = new NVertex[VtxCount];
		} else{
			$m.lpFeatBuff = CreateUploadBuffer(pD3dDev, cbFlagBuff, B_False);
			$m.lpVertBuff = CreateUploadBuffer(pD3dDev, cbVertBuff, B_False);
			$m.lpFeatBuff->Map(0, P_Null, ($VOID**)&$m.prgTriFeat);
			$m.lpVertBuff->Map(0, P_Null, ($VOID**)&$m.prgVertex);
		}

		$m.ColCount = Desc.ColCount;
		$m.RowCount = Desc.RowCount;
		$m.CellWidth = Desc.CellWidth;
		$m.CellDepth = Desc.CellDepth;
		$m.LowerLeftX = Desc.LowerLeftX;
		$m.LowerLeftZ = Desc.LowerLeftZ;

		$m.InitVertexs(Desc.HeightVal);
		$m.InitFeatures(Desc.bSmooth, Desc.bBslash);
	}
	_open $VOID UploadToVram(DXTKUploader &Uploader){
		UNS32 TriCount = $m.ColCount * $m.RowCount * 2;
		UNS32 VtxCount = ($m.ColCount + 1) * ($m.RowCount + 1);

		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, $m.prgTriFeat, TriCount,
			sizeof(DWORD), D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpFeatBuff);
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, $m.prgVertex, VtxCount,
			sizeof(NVertex), D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpVertBuff);

		$m.prgVertex = P_Null;
		$m.prgTriFeat = P_Null;
	}
	_open $VOID UpdateNormals(_in FRect &rcScope){
		INT32 DivL = INT32((rcScope.left - $m.LowerLeftX) / $m.CellWidth);
		INT32 DivB = INT32((rcScope.bottom - $m.LowerLeftZ) / $m.CellDepth);
		INT32 DivR = INT32((rcScope.right - $m.LowerLeftX) / $m.CellWidth) + 1;
		INT32 DivT = INT32((rcScope.top - $m.LowerLeftZ) / $m.CellDepth) + 1;

		INT32 ColStart = CLAMP_(DivL, 0, (INT32)$m.ColCount);
		INT32 RowStart = CLAMP_(DivB, 0, (INT32)$m.RowCount);
		INT32 ColEnd = CLAMP_(DivR, 0, (INT32)$m.ColCount);
		INT32 RowEnd = CLAMP_(DivT, 0, (INT32)$m.RowCount);

		INT32 ColMax = (INT32)$m.ColCount;
		INT32 RowMax = (INT32)$m.RowCount;
		INT32 ColCnt = ColEnd - ColStart + 1;
		INT32 RowCnt = RowEnd - RowStart + 1;

		UNS32 RowPitch = $m.GetDensityX();

		SPFPN DifY0, DifY1;

		NVertex *prgVert;
		FVector3 *prgNorm, *pNormal, Normal;
		FVector3 *lprgTemp = new FVector3[(RowCnt + 2) * (ColCnt + 2)];

		///相邻点采样

		pNormal = lprgTemp;

		for(INT32 iRow = RowStart - 1; iRow <= RowEnd + 1; ++iRow){
			for(INT32 iCol = ColStart - 1; iCol <= ColEnd + 1; ++iCol){
				if((iCol < 0) || (iRow < 0) || (iCol > ColMax) || (iRow > RowMax)){
					*pNormal = { 0.f, 0.f, 0.f };
					pNormal++;
					continue;
				}

				if((iCol == 0) || (iCol == ColMax)){
					DifY0 = 0.f;
				} else{
					DifY0 = $m.GetVertexHeight(iRow, iCol + 1);
					DifY0 -= $m.GetVertexHeight(iRow, iCol - 1);
					DifY0 /= $m.CellWidth;
				}

				if((iRow == 0) || (iRow == RowMax)){
					DifY1 = 0.f;
				} else{
					DifY1 = $m.GetVertexHeight(iRow + 1, iCol);
					DifY1 -= $m.GetVertexHeight(iRow - 1, iCol);
					DifY1 /= $m.CellDepth;
				}

				pNormal->x = -DifY0;
				pNormal->z = -DifY1;
				pNormal->y = 1.f;
				pNormal->Normalize();
				pNormal++;
			}
		}

		///法线平滑

		prgVert = $m.prgVertex + ColStart;
		prgVert += (RowStart - 1) * RowPitch;

		for(INT32 iRow = 1; iRow <= RowCnt; ++iRow){
			prgVert += RowPitch;
			for(INT32 iCol = 1; iCol <= ColCnt; ++iCol){
				Normal = { 0.f, 0.f, 0.f };

				for(INT32 K = -1; K <= 1; ++K){
					prgNorm = lprgTemp + (ColCnt + 2)*(iRow + K);
					for(INT32 L = -1; L <= 1; ++L)
						Normal += prgNorm[iCol + L];
				}

				PVDec3N *pDest = (PVDec3N*)&prgVert[iCol - 1].Normal;
				Normal.Normalize();
				dxpv::XMStoreDecN4(pDest, Normal);
			}
		}

		///内存释放

		delete[] lprgTemp;
	}
	_open $VOID ApplyHeightMap(_in SPFPN *psMap, _in VUint2 &Size){
		SPFPN ScaleX = (SPFPN)Size.x / (SPFPN)$m.GetDensityX();
		SPFPN ScaleY = (SPFPN)Size.y / (SPFPN)$m.GetDensityZ();

		FRect rcDest = { 0.f, $m.GetDepth(), $m.GetWidth(), 0.f };
		NVertex *prgDest = $m.prgVertex;

		for(UNS32 iRow = 0; iRow < $m.GetDensityZ(); ++iRow){
			UNS32 iSrcRow = UNS32(iRow * ScaleY);
			UNS32 iLeftSrc = iSrcRow * Size.x;

			for(UNS32 iCol = 0; iCol < $m.GetDensityX(); ++iCol){
				UNS32 iSrcCol = UNS32(iCol * ScaleX);
				UNS32 iSource = iLeftSrc + iSrcCol;

				prgDest[iCol].CoordY = psMap[iSource];
			}

			prgDest += $m.GetDensityX();
		}

		$m.UpdateNormals(rcDest);
	}
	_open IBOOL IntersectRay(_in FRay3 &Ray, _out SPFPN *pDist = P_Null, SPFPN Step = 0.5f, SPFPN Epsilon = 0.001f){
		///系数计算

		FRay2 Ray2;
		FBox2 Box2;
		SPFPN T0, T1;

		Box2.Min.x = $m.LowerLeftX;
		Box2.Min.y = $m.LowerLeftZ;
		Box2.Max.x = Box2.Min.x + $m.GetWidth();
		Box2.Max.y = Box2.Min.y + $m.GetDepth();

		Ray2.Origin.x = Ray.position.x;
		Ray2.Origin.y = Ray.position.z;
		Ray2.Direction.x = Ray.direction.x;
		Ray2.Direction.y = Ray.direction.z;

		if(!Intersects(Ray2, Box2, &T0, &T1))
			return B_False;

		///交点测试

		FVector3 Point;
		SPFPN Height;
		SPFPN DifT = T1 - T0;
		SPFPN T, MinT, MidT, MaxT;

		Step *= MIN_($m.CellWidth, $m.CellDepth);
		Step = DifT / ceil(DifT / Step);

		for(T = T0; T <= T1; T += Step){
			Point = Ray.position + (Ray.direction * T);
			Height = $m.SampleHeight(Point.x, Point.z);

			if(Point.y < Height){
				MaxT = T;
				MinT = T - Step;

				while((MaxT - MinT) > Epsilon){
					MidT = (MinT + MaxT) * 0.5f;
					Point = Ray.position + (Ray.direction * MidT);
					Height = $m.SampleHeight(Point.x, Point.z);

					if(Point.y < Height) MaxT = MidT;
					else MinT = MidT;
				}

				if(pDist) *pDist = MaxT;
				return B_True;
			}
		}

		return B_False;
	}
	_open SPFPN SampleHeight(SPFPN CoordX, SPFPN CoordZ){
		CoordX -= $m.LowerLeftX;
		CoordZ -= $m.LowerLeftZ;
		CoordX /= $m.CellWidth;
		CoordZ /= $m.CellDepth;

		UNS32 iCol = (UNS32)CoordX;
		UNS32 iRow = (UNS32)CoordZ;
		UNS32 iFace = (($m.ColCount * iRow) + iCol) * 2;

		UNS32 iP0 = ($m.GetDensityX() * iRow) + iCol;
		UNS32 iP1 = iP0 + $m.GetDensityX();
		UNS32 iP2 = iP1 + 1;
		UNS32 iP3 = iP0 + 1;

		SPFPN H0 = $m.prgVertex[iP0].CoordY;
		SPFPN H1 = $m.prgVertex[iP1].CoordY;
		SPFPN H2 = $m.prgVertex[iP2].CoordY;
		SPFPN H3 = $m.prgVertex[iP3].CoordY;

		SPFPN Tx = CoordX - iCol;
		SPFPN Tz = CoordZ - iRow;

		if($m.prgTriFeat[iFace] & FACE_FLAG_Bslash){
			if(Tx < Tz){
				return H0 + (H3 - H0)*Tx + (H1 - H0)*Tz;
			} else{
				Tx = 1.f - Tx;
				Tz = 1.f - Tz;
				return H2 + (H1 - H2)*Tx + (H3 - H2)*Tz;
			}
		} else{
			if(Tx < Tz){
				Tz = 1.f - Tz;
				return H1 + (H2 - H1)*Tx + (H0 - H1)*Tz;
			} else{
				Tx = 1.f - Tx;
				return H3 + (H0 - H3)*Tx + (H2 - H3)*Tz;
			}
		}
	}
	_open SPFPN GetVertexHeight(UNS32 RowId, UNS32 ColId){
		UNS32 Index = ($m.ColCount + 1)*RowId + ColId;
		return $m.prgVertex[Index].CoordY;
	}
	_open UNS32 GetVertexCount(){
		return ($m.RowCount + 1) * ($m.ColCount + 1);
	}
	_open UNS32 GetCellCount(){
		return $m.RowCount * $m.ColCount;
	}
	_open UNS32 GetDensityX(){
		return $m.ColCount + 1;
	}
	_open UNS32 GetDensityZ(){
		return $m.RowCount + 1;
	}
	_open SPFPN GetWidth(){
		return $m.ColCount * $m.CellWidth;
	}
	_open SPFPN GetDepth(){
		return $m.RowCount * $m.CellDepth;
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetVertexsVram(){
		return $m.lpVertBuff->GetGPUVirtualAddress();
	}
	_open D3D12_GPU_VIRTUAL_ADDRESS GetFeaturesVram(){
		return $m.lpFeatBuff->GetGPUVirtualAddress();
	}
};

//----------------------------------------//


//-------------------- 精灵 --------------------//

// Sprite
class CSprite{
	_secr struct NFrame{
		UNS32 Ident;
		UNS16 Left;
		UNS16 Top;
		UNS16 Right;
		UNS16 Bottom;
	};

	_secr UNS32 numFrame;
	_secr NFrame *lprgFrame;
	_secr ID2D1Bitmap1 *lpSheet;

	_open ~CSprite(){
		$m.Release();
	}
	_open CSprite(){
		Var_Zero(this);
	}
	_open CSprite(IWICImagingFactory *pWicFactory, ID2D1DeviceContext *pD2dDevCtx, _in WCHAR *psFilePath){
		$m.Initialize(pWicFactory, pD2dDevCtx, psFilePath);
	}
	//////
	_open $VOID Initialize(IWICImagingFactory *pWicFactory, ID2D1DeviceContext *pD2dDevCtx, _in WCHAR *psFilePath){
		WCHAR JsonPath[MAX_PATH];
		CJsonNode *pDomFrame, *pDomRect;
		CJsonNode *lpDomRoot;
		NFrame *pFrame;

		wcscpy(JsonPath, psFilePath);
		wcscpy(wcsrchr(JsonPath, L'.'), L".json");

		lpDomRoot = CJsonNode::CreateByFile(JsonPath);
		pDomFrame = lpDomRoot->GetChild("frames")->GetChild();

		$m.lpSheet = LoadD2dBitmap(pWicFactory, pD2dDevCtx, psFilePath);
		$m.numFrame = (UNS32)lpDomRoot->GetChild("frames")->GetSize();
		$m.lprgFrame = pFrame = new NFrame[$m.numFrame];

		for(UNS32 iFrame = 0; iFrame < $m.numFrame; ++iFrame){
			pDomRect = pDomFrame->GetChild("frame");
			pFrame->Top = (UNS16)pDomRect->GetMemI("y");
			pFrame->Left = (UNS16)pDomRect->GetMemI("x");
			pFrame->Right = (UNS16)pDomRect->GetMemI("w") + pFrame->Left;
			pFrame->Bottom = (UNS16)pDomRect->GetMemI("h") + pFrame->Top;
			pFrame->Ident = StrHash32(pDomFrame->GetMemS("filename"));

			pDomFrame = pDomFrame->GetNext();
			pFrame++;
		}

		lpDomRoot->Release();
	}
	_open $VOID Release(){
		SAFE_RELEASE($m.lpSheet);
		SAFE_DELETEA($m.lprgFrame);
	}
	_open UNS32 GetFrameCount(){
		return $m.numFrame;
	}
	_open UNS32 GetFrameIndex(UNS32 Ident){
		UNS32 Index = 0;
		NFrame *pFrame = $m.lprgFrame;

		while(Index < $m.numFrame){
			if(pFrame->Ident == Ident) return Index;
			pFrame++;
			Index++;
		}

		return UINT_MAX;
	}
	_open FRect GetFrameById(UNS32 Ident){
		NFrame *pFrame = $m.lprgFrame;
		NFrame *pFEnd = pFrame + $m.numFrame;

		for(; pFrame != pFEnd; ++pFrame)
			if(pFrame->Ident == Ident) break;

		SPFPN Top = (SPFPN)pFrame->Top;
		SPFPN Left = (SPFPN)pFrame->Left;
		SPFPN Right = (SPFPN)pFrame->Right;
		SPFPN Bottom = (SPFPN)pFrame->Bottom;

		return FRect{ Left, Top, Right, Bottom };
	}
	_open FRect GetFrame(UNS32 Index){
		NFrame &rFrame = $m.lprgFrame[Index];

		SPFPN Top = (SPFPN)rFrame.Top;
		SPFPN Left = (SPFPN)rFrame.Left;
		SPFPN Right = (SPFPN)rFrame.Right;
		SPFPN Bottom = (SPFPN)rFrame.Bottom;

		return FRect{ Left, Top, Right, Bottom };
	}
	_open ID2D1Bitmap1 *GetSheet(){
		return $m.lpSheet;
	}
};

//----------------------------------------//