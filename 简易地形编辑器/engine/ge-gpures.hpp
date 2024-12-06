//-------------------- 上传缓存区 --------------------//

// Upload Buffer
class CUploadBuff{
	_rest USINT Size;             //缓存大小
	_rest USINT Pitch;            //缓存节距
	_rest BYTET *psCpuMap;        //内存映射
	_rest ID3DResource *lpKernel; //核心对象

	_open ~CUploadBuff(){
		$m.Finalize();
	}
	_open CUploadBuff(){
		Var_Zero(this);
	}
	_open CUploadBuff(ID3DDevice *pD3dDevice, USINT Division, USINT Pitch, IBOOL bConst){
		$m.Initialize(pD3dDevice, Division, Pitch, bConst);
	}
	//////
	_open $VOID Finalize(){
		if($m.lpKernel){
			if($m.psCpuMap){
				$m.lpKernel->Unmap(0, P_Null);
				$m.psCpuMap = P_Null;
			}
			$m.lpKernel->Release();
			$m.lpKernel = P_Null;
		}
	}
	_open $VOID Initialize(ID3DDevice *pD3dDevice, USINT Division, USINT Pitch, IBOOL bConst){
		$m.Size = Division * Pitch;
		$m.Pitch = Pitch;
		$m.psCpuMap = P_Null;
		$m.lpKernel = CreateUploadBuffer(pD3dDevice, $m.Size, bConst);
	}
	_open $VOID EnableUpdate(IBOOL bEnable, IBOOL bClear){
		if(!bEnable && $m.psCpuMap){
			$m.lpKernel->Unmap(0, P_Null);
			$m.psCpuMap = P_Null;
		} else if(bEnable && !$m.psCpuMap){
			$m.lpKernel->Map(0, P_Null, ($VOID**)&$m.psCpuMap);
			if(bClear) BStr_Zero($m.psCpuMap, $m.Size);
		}
	}
	_open PVOID GetMemory(USINT UnitIdx){
		USINT Offset = $m.Pitch * UnitIdx;
		return $m.psCpuMap + Offset;
	}
	_open HD3D12GPURES GetVram(USINT UnitIdx){
		HD3D12GPURES hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		USINT Offset = $m.Pitch * UnitIdx;
		return hBuffer + Offset;
	}
};

//----------------------------------------//


//-------------------- 索引、顶点缓存区 --------------------//

// Index Buffer
class CIdxBuff: public CUploadBuff{
	_secr USINT Count;

	_open ~CIdxBuff(){}
	_open CIdxBuff(): CUploadBuff(){}
	_open CIdxBuff(ID3DDevice *pD3dDevice, USINT Division, DXGI1FORMAT Format, BOOL bEnable = B_True): CUploadBuff(pD3dDevice, Division, (USINT)dxlh::BitsPerPixel(Format), B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3DDevice *pD3dDevice, USINT Division, DXGI1FORMAT Format, BOOL bEnable = B_True){
		$sup Initialize(pD3dDevice, Division, (USINT)dxlh::BitsPerPixel(Format), B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	_open $VOID SetCount(USINT Count){
		$m.Count = Count;
	}
	_open USINT GetCount(){
		return $m.Count;
	}
	_open USINT GetDivision(){
		return $m.Size / $m.Pitch;
	}
	_open D3D12IBVIEW GetSrv(){
		USINT Size = $m.Count * $m.Pitch;
		DXGI1FORMAT Format = $m.GetFormat();
		HD3D12GPURES hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		return D3D12IBVIEW{ hBuffer, Size, Format };
	}
	_open DXGI1FORMAT GetFormat(){
		switch($m.Pitch){
			case 16: return DXGI_FORMAT_R16_UINT;
			case 32: return DXGI_FORMAT_R32_UINT;
			default: return DXGI_FORMAT_UNKNOWN;
		}
	}
};

// Vertex Buffer
class CVtxBuff: public CUploadBuff{
	_secr USINT Count;

	_open ~CVtxBuff(){}
	_open CVtxBuff(): CUploadBuff(){}
	_open CVtxBuff(ID3DDevice *pD3dDevice, USINT Division, USINT Pitch, BOOL bEnable = B_True): CUploadBuff(pD3dDevice, Division, Pitch, B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3DDevice *pD3dDevice, USINT Division, USINT Pitch, BOOL bEnable = B_True){
		$sup Initialize(pD3dDevice, Division, Pitch, B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Count = 0;
	}
	_open $VOID SetCount(USINT Count){
		$m.Count = Count;
	}
	_open USINT GetCount(){
		return $m.Count;
	}
	_open USINT GetDivision(){
		return $m.Size / $m.Pitch;
	}
	_open D3D12VBVIEW GetSrv(){
		USINT Size = $m.Count * $m.Pitch;
		HD3D12GPURES hBuffer = $m.lpKernel->GetGPUVirtualAddress();
		return D3D12VBVIEW{ hBuffer, Size, $m.Pitch };
	}
};

//----------------------------------------//


//-------------------- 灯光、材质库 --------------------//

// Light Repository
class CLitRepos: public CUploadBuff{
	_secr USINT Count;
	_secr USINT Division;

	_open ~CLitRepos(){}
	_open CLitRepos(): CUploadBuff(){}
	_open CLitRepos(ID3DDevice *pD3dDevice, USINT Capacity, BOOL bEnable = B_True): CUploadBuff(pD3dDevice, Capacity, sizeof(LIGHT), B_False){
		$m.EnableUpdate(bEnable, B_False);
		$m.Division = Capacity;
		$m.Count = 0;
	}
	//////
	_open $VOID Initialize(ID3DDevice *pD3dDevice, USINT Capacity, BOOL bEnable = B_True){
		$sup Initialize(pD3dDevice, Capacity, sizeof(LIGHT), B_False);
		$m.EnableUpdate(bEnable, B_False);
		$m.Division = Capacity;
		$m.Count = 0;
	}
	_open $VOID UpdateSpotLight(_in LIGHT &Value, USINT Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		LIGHT *pLight = (LIGHT*)$m.GetMemory(Index);
		VECTR3X *pLitDir = (VECTR3X*)&pLight->Direction;

		*pLight = Value;
		pLitDir->Normalize();
	}
	_open $VOID UpdatePointLight(_in LIGHT &Value, USINT Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		LIGHT *pLight = (LIGHT*)$m.GetMemory(Index);
		pLight->AttEnd = Value.AttEnd;
		pLight->AttStart = Value.AttStart;
		pLight->Position = Value.Position;
		pLight->Intensity = Value.Intensity;
		pLight->SpotPower = 0.f;
	}
	_open $VOID UpdateDirectionalLight(_in LIGHT &Value, USINT Index = UINT_MAX){
		if(Index == UINT_MAX){
			if($m.Count == $m.Division) return;
			else Index = $m.Count++;
		}

		LIGHT *pLight = (LIGHT*)$m.GetMemory(Index);
		VECTR3X *pLitDir = (VECTR3X*)&pLight->Direction;

		pLight->AttEnd = SPFP_INF;
		pLight->AttStart = 0;
		pLight->SpotPower = 0.f;
		pLight->Intensity = Value.Intensity;
		pLight->Direction = Value.Direction;
		pLitDir->Normalize();
	}
	_open $VOID TurnOffLight(USINT Index){
		LIGHT *pLight = (LIGHT*)$m.GetMemory(Index);
		if(pLight->AttEnd > 0.f) pLight->AttEnd = -pLight->AttEnd;
	}
	_open $VOID TurnOnLight(USINT Index){
		LIGHT *pLight = (LIGHT*)$m.GetMemory(Index);
		if(pLight->AttEnd < 0.f) pLight->AttEnd = -pLight->AttEnd;
	}
	_open USINT GetCapacity(){
		return $m.Division;
	}
	_open USINT GetCount(){
		return $m.Count;
	}
};

// Material Repository
class CMtlRepos{
	_open struct TEXVIEW{
		USINT Ident;
		ID3DResource *pSource;
	};
	_open struct MATVIEW{
		USINT Ident;
		UHALF iMtlBase;
		UHALF iColorMap;
		UHALF iNormMap;
		UHALF iRoughMap;
		UHALF iAlphaMap;
		UHALF iEmissMap;
		UHALF iOcclusMap;
	};

	_secr USINT MtlCount;
	_secr USINT TexCount;
	_secr MATVIEW *lprgMtlView;
	_secr TEXVIEW *lprgTexView;
	_secr ID3DResource *lpMtlBuff;
	_open ID3DViewHeap *lpTexSrvs;

	_open ~CMtlRepos(){
		$m.Finalize();
	}
	_open CMtlRepos(){
		Var_Zero(this);
	}
	_open CMtlRepos(USINT MtlCount, USINT TexCount){
		$m.Initialize(MtlCount, TexCount);
	}
	//////
	_open $VOID Finalize(){
		if($m.lpMtlBuff || $m.lpTexSrvs){
			for(USINT iTex = 0; iTex < $m.TexCount; ++iTex)
				$m.lprgTexView[iTex].pSource->Release();

			SAFE_DELETEA($m.lprgMtlView);
			SAFE_DELETEA($m.lprgTexView);

			SAFE_RELEASE($m.lpMtlBuff);
			SAFE_RELEASE($m.lpTexSrvs);
			// endif
		} else if($m.lprgMtlView || $m.lprgTexView){
			CSeqList *lpMtlList = (CSeqList*)$m.lprgMtlView;
			CSeqList *lpTexList = (CSeqList*)$m.lprgTexView;

			$m.lprgMtlView = P_Null;
			$m.lprgTexView = P_Null;

			delete lpMtlList;
			delete lpTexList;
		}
	}
	_open $VOID Initialize(USINT MtlCount, USINT TexCount){
		CSeqList *pMtlList = new CSeqList(MtlCount, sizeof(MATERIAL));
		CSeqList *pTexList = new CSeqList(TexCount, sizeof(TEXVIEW));

		$m.lprgMtlView = (MATVIEW*)pMtlList;
		$m.lprgTexView = (TEXVIEW*)pTexList;
		$m.lpMtlBuff = P_Null;
		$m.lpTexSrvs = P_Null;
		$m.MtlCount = 0;
		$m.TexCount = 0;
	}
	_open $VOID UploadToVram(DXTKUploader &Uploader){
		ID3DDevice *pD3dDevice = (ID3DDevice*)Uploader.GetDevice();
		CSeqList *lpTexList = (CSeqList*)$m.lprgTexView;
		CSeqList *lpMtlList = (CSeqList*)$m.lprgMtlView;
		MATERIAL *prgMatl = (MATERIAL*)lpMtlList->GetFront();

		lpTexList->Sort(tlCompare<USINT>);
		lpMtlList->Sort(tlCompare<USINT>);

		$m.TexCount = lpTexList->GetSize();
		$m.MtlCount = lpMtlList->GetSize();

		$m.lprgTexView = P_Null;
		$m.lprgMtlView = P_Null;

		if($m.TexCount != 0){
			$m.lpTexSrvs = CreateSrvHeap(pD3dDevice, $m.TexCount);
			$m.lprgTexView = (TEXVIEW*)lpTexList->Detach();
			$m.CreateTextureSrvs(pD3dDevice);
		}
		if($m.MtlCount != 0){
			$m.lprgMtlView = new MATVIEW[$m.MtlCount];
			$m.CreateMaterialViews(prgMatl);
			$m.CreateMaterialOnVram(Uploader, prgMatl);
		}

		delete lpMtlList;
		delete lpTexList;
	}
	_open $VOID AddMaterial(_in MATERIAL &Material){
		((CSeqList*)$m.lprgMtlView)->PushBack(($VOID*)&Material);
	}
	_open $VOID AddTexture(_in CHAR8 *psName, ID3DResource *pTexture){
		CSeqList *pTexList = (CSeqList*)$m.lprgTexView;
		TEXVIEW *pTexView = (TEXVIEW*)pTexList->New();

		pTexView->Ident = StrHash32(psName);
		pTexView->pSource = pTexture;
	}
	_open $VOID AddMaterialsFromFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		IBOOL bFound = FindFileChunk(hrFile, ASSET_TYPE_Material);

		if(!bFound) goto lbl_end;

		USINT Count;
		ReadFile(hrFile, &Count, sizeof(USINT), P_Null, P_Null);
		PVOID pBuffer = ((CSeqList*)$m.lprgMtlView)->New(Count);
		ReadFile(hrFile, pBuffer, Count * sizeof(MATERIAL), P_Null, P_Null);

	lbl_end:
		CloseHandle(hrFile);
	}
	_open $VOID AddMaterialsFromCatalog(_in WCHAR *psCatalog){
		UPINT CtlgLen;
		HANDLE hrSeeker;
		FINDDATAW FileInfo;
		WCHAR FilePath[MAX_PATH];

		WcsCat(WcsCpy(FilePath, psCatalog), L"*.dat");
		hrSeeker = FindFirstFileW(FilePath, &FileInfo);
		if(INVALID_(hrSeeker)) return;
		CtlgLen = WcsLen(psCatalog);

		do{
			WcsCpy(&FilePath[CtlgLen], FileInfo.cFileName);
			$m.AddMaterialsFromFile(FilePath);
		} while(FindNextFileW(hrSeeker, &FileInfo));

		FindClose(hrSeeker);
	}
	_open $VOID AddTexturesFromCatalog(DXTKUploader &Uploader, _in WCHAR *psCatalog){
		UPINT CtlgLen;
		HANDLE hrSeeker;
		FINDDATAW FileInfo;
		WCHAR FilePath[MAX_PATH];

		WcsCat(WcsCpy(FilePath, psCatalog), L"*.dds");
		hrSeeker = FindFirstFileW(FilePath, &FileInfo);
		if(INVALID_(hrSeeker)) return;
		CtlgLen = WcsLen(psCatalog);

		TEXVIEW *pTexView = P_Null;
		CSeqList *pTexList = (CSeqList*)$m.lprgTexView;

		do{
			WcsCpy(&FilePath[CtlgLen], FileInfo.cFileName);
			*WcsRchr(FileInfo.cFileName, L'.') = L'\0';
			pTexView = (TEXVIEW*)pTexList->New();
			pTexView->Ident = StrHash32(W_TO_A(FileInfo.cFileName));
			dx::CreateDDSTextureFromFile(Uploader.GetDevice(), Uploader, FilePath, &pTexView->pSource);
		} while(FindNextFileW(hrSeeker, &FileInfo));

		FindClose(hrSeeker);
	}
	_secr $VOID CreateMaterialOnVram(DXTKUploader &Uploader, _in MATERIAL *prgMatl){
		USINT Count = 0;
		BYTET *psSource = (BYTET*)&prgMatl[0].Base;
		MATBASE *lprgDest = new MATBASE[$m.MtlCount];

		for(USINT iMtl = 0; iMtl < $m.MtlCount; ++iMtl){
			PVOID pDest = LinSearch(psSource, lprgDest, &Count, sizeof(MATBASE), CMtlRepos::CompareMatl);
			UPINT iDest = (MATBASE*)pDest - lprgDest;
			psSource += sizeof(MATERIAL);
			$m.lprgMtlView[iMtl].iMtlBase = (UHALF)iDest;
		}

		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lprgDest/*被接管*/,
			1, ToPowOf2Mul(Count * sizeof(MATBASE), 256),
			D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpMtlBuff);
	}
	_secr $VOID CreateMaterialViews(_in MATERIAL *prgMatl){
		MATVIEW *pDest = $m.lprgMtlView;
		const MATERIAL *pSource = prgMatl;

		for(USINT iMtl = 0; iMtl < $m.MtlCount; ++iMtl){
			pDest->iMtlBase = iMtl;
			pDest->Ident = pSource->Ident;

			UHALF *prgValue = &pDest->iColorMap;
			const USINT *prgKey = &pSource->Maps.iAlbedo;

			for(USINT iLayer = 0; iLayer < 6; ++iLayer){ //6种贴图
				if(prgKey[iLayer] == 0){
					prgValue[iLayer] = UINT16_MAX;
					continue;
				}
				for(USINT iTex = 0; iTex < $m.TexCount; ++iTex){ //根据ID获取索引
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
	_secr $VOID CreateTextureSrvs(ID3DDevice *pD3dDevice){
		D3DX12CPUVIEWPTR hTexSrv($m.lpTexSrvs->GetCPUDescriptorHandleForHeapStart());
		TEXVIEW *pTexView = $m.lprgTexView;

		for(USINT iTex = 0; iTex < $m.TexCount; ++iTex){
			dx::CreateShaderResourceView(pD3dDevice, pTexView->pSource, hTexSrv);
			hTexSrv.Offset(GE3d::cbSRView);
			pTexView++;
		}
	}
	_open const TEXVIEW *GetTextureById(USINT Ident){
		return (TEXVIEW*)BinSearch(&Ident,
			$m.lprgTexView, $m.TexCount, sizeof(TEXVIEW), tlCompare<USINT>);
	}
	_open const MATVIEW *GetMaterialById(USINT Ident){
		return (MATVIEW*)BinSearch(&Ident,
			$m.lprgMtlView, $m.MtlCount, sizeof(MATVIEW), tlCompare<USINT>);
	}
	_open D3D12GPUVIEWPTR GetTextureSrv(USINT Index){
		D3DX12GPUVIEWPTR hView($m.lpTexSrvs->GetGPUDescriptorHandleForHeapStart());
		return hView.Offset(Index, GE3d::cbSRView);
	}
	_open D3D12GPUVIEWPTR GetTextureSrvById(USINT Ident){
		const TEXVIEW *pView = $m.GetTextureById(Ident);
		const UPINT iView = pView - $m.lprgTexView;
		return $m.GetTextureSrv((USINT)iView);
	}
	_open HD3D12GPURES GetMaterialVram(USINT Index){
		HD3D12GPURES hBuffer = $m.lpMtlBuff->GetGPUVirtualAddress();
		return hBuffer + (Index * sizeof(MATBASE));
	}
	_open HD3D12GPURES GetMaterialVramById(USINT Ident){
		const MATVIEW *pView = $m.GetMaterialById(Ident);
		const UPINT iView = pView - $m.lprgMtlView;
		const USINT iBase = $m.lprgMtlView[iView].iMtlBase;
		return $m.GetMaterialVram(iBase);
	}

	_secr static INT32 CompareMatl(_in $VOID *pData0, _in $VOID *pData1){
		SPFPN *pValue0 = (SPFPN*)pData0;
		SPFPN *pValue1 = (SPFPN*)pData1;

		for(USINT iVal = 0; iVal < DW_CNT(MATBASE); ++iVal){
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
	_secr USINT numBone;
	_secr BONE *lprgBone;
	_secr MATRX4X *prgBoneXform;
	_secr ID3DResource *lpPosture;

	_open ~CSkeleton(){
		$m.Finalize();
	}
	_open CSkeleton(){
		Var_Zero(this);
	}
	_open CSkeleton(ID3DDevice *pD3dDevice, _in WCHAR *psFilePath){
		$m.Initialize(pD3dDevice, psFilePath);
	}
	//////
	_open $VOID Initialize(ID3DDevice *pD3dDevice, _in WCHAR *psFilePath){
		HANDLE hrFile;
		IBOOL bFound;
		USINT BufSize;
		CHAR8 Buffer[128];
		BONE *pBone, *pEndBone;

		hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		bFound = FindFileChunk(hrFile, ASSET_TYPE_Skeleton);

		if(!bFound) goto lbl_end;

		ReadFile(hrFile, &$m.numBone, sizeof(USINT), P_Null, P_Null);
		BufSize = $m.numBone * sizeof(MATRIX4);

		$m.lprgBone = new BONE[$m.numBone];
		$m.lpPosture = CreateUploadBuffer(pD3dDevice, BufSize, B_False);
		$m.lpPosture->Map(0, P_Null, ($VOID**)&$m.prgBoneXform);

		pBone = $m.lprgBone;
		pEndBone = pBone + $m.numBone;

		for(; pBone != pEndBone; ++pBone){
			ReadFile(hrFile, &BufSize, sizeof(USINT), P_Null, P_Null);
			ReadFile(hrFile, Buffer, BufSize, P_Null, P_Null);
			ReadFile(hrFile, &pBone->iParent, sizeof(USINT), P_Null, P_Null);
			ReadFile(hrFile, &pBone->matOffset, sizeof(MATRIX4), P_Null, P_Null);
			ReadFile(hrFile, &pBone->matInitial, sizeof(MATRIX4), P_Null, P_Null);

			Buffer[BufSize] = '\0';
			pBone->Ident = StrHash32(Buffer);
			pBone->matLocal = pBone->matInitial;
			pBone->matCombine = Mat_Identity();
		}

	lbl_end:
		CloseHandle(hrFile);
	}
	_open $VOID Finalize(){
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
		MMATRIX matFinal;
		BONE *pBone, *pSupBone;

		pBone = $m.lprgBone;
		pBone->matCombine = pBone->matLocal;
		matFinal = pBone->matOffset * pBone->matCombine;
		$m.prgBoneXform[0] = Mat_Transpose(matFinal);

		for(USINT iBone = 1; iBone < $m.numBone; ++iBone){
			pBone = $m.lprgBone + iBone;
			pSupBone = $m.lprgBone + pBone->iParent;
			pBone->matCombine = pBone->matLocal * pSupBone->matCombine;
			matFinal = pBone->matOffset * pBone->matCombine;
			$m.prgBoneXform[iBone] = Mat_Transpose(matFinal);
		}
	}
	_open $VOID Reset(){
		BONE *pBone = $m.lprgBone;
		BONE *pEndBone = pBone + $m.numBone;

		for(; pBone != pEndBone; ++pBone)
			pBone->matLocal = pBone->matInitial;
	}
	_open BONE *GetBones(){
		return $m.lprgBone;
	}
	_open USINT GetCount(){
		return $m.numBone;
	}
	_open HD3D12GPURES GetPosture(){
		return $m.lpPosture->GetGPUVirtualAddress();
	}
};

//----------------------------------------//


//-------------------- 3D网格 --------------------//

// 3D Mesh
class CMesh3D{
	_open USINT Ident;
	_open USINT numSubset;
	_open USINT cbIdxBuff;
	_open USINT cbVtxBuff;
	_open DWORD IdxFormat;
	_open DWORD VtxFormat;
	_secr ID3DResource *lpIdxBuff;
	_secr ID3DResource *lpVtxBuff;
	_open SUBMESH *lprgSubset;
	_open CMesh3D *pNext;

	_open ~CMesh3D(){
		$m.Finalize();
	}
	_open CMesh3D(){
		Var_Zero(this);
	}
	_open CMesh3D(DXTKUploader &Uploader, HANDLE hFile){
		$m.Initialize(Uploader, hFile);
	}
	//////
	_open $VOID Initialize(DXTKUploader &Uploader, HANDLE hFile){
		USINT NameLen;
		CHAR8 Name[128];
		BYTET *lpsIndices;
		BYTET *lpsVertexs;

		//头部

		ReadFile(hFile, &NameLen, sizeof(USINT), P_Null, P_Null);
		ReadFile(hFile, Name, NameLen, P_Null, P_Null);
		Name[NameLen] = '\0';

		$m.Ident = StrHash32(Name);
		$m.pNext = P_Null;
		$m.lprgSubset = P_Null;

		//基础信息

		ReadFile(hFile, &$m.IdxFormat, sizeof(DWORD), P_Null, P_Null);
		ReadFile(hFile, &$m.VtxFormat, sizeof(DWORD), P_Null, P_Null);
		ReadFile(hFile, &$m.cbIdxBuff, sizeof(USINT), P_Null, P_Null);
		ReadFile(hFile, &$m.cbVtxBuff, sizeof(USINT), P_Null, P_Null);
		ReadFile(hFile, &$m.numSubset, sizeof(USINT), P_Null, P_Null);

		//顶点和索引

		lpsIndices = new BYTET[$m.cbIdxBuff];
		lpsVertexs = new BYTET[$m.cbVtxBuff];

		ReadFile(hFile, lpsIndices, $m.cbIdxBuff, P_Null, P_Null);
		ReadFile(hFile, lpsVertexs, $m.cbVtxBuff, P_Null, P_Null);

		//子集划分

		if($m.numSubset > 0){
			$m.lprgSubset = new SUBMESH[$m.numSubset];
			ReadFile(hFile, $m.lprgSubset, $m.numSubset * sizeof(SUBMESH), P_Null, P_Null);
		}

		//GPU资源

		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lpsIndices/*被接管*/,
			1, $m.cbIdxBuff, D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpIdxBuff);
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, lpsVertexs/*被接管*/,
			1, $m.cbVtxBuff, D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpVtxBuff);
	}
	_open $VOID Finalize(){
		if($m.lpVtxBuff){
			SAFE_RELEASE($m.lpIdxBuff);
			SAFE_RELEASE($m.lpVtxBuff);
			SAFE_DELETEA($m.lprgSubset);
		}
	}
	_open USINT GetIndexCount(){
		return $m.cbIdxBuff / $m.GetIndexDataSize();
	}
	_open USINT GetVertexCount(){
		return $m.cbVtxBuff / $m.GetVertexDataSize();
	}
	_open USINT GetIndexDataSize(){
		switch($m.IdxFormat){
			case DXGI_FORMAT_R32_UINT: return 4;
			case DXGI_FORMAT_R16_UINT: return 2;
			default: return 0;
		}
	}
	_open USINT GetVertexDataSize(){
		CVtxView VtxView($m.VtxFormat);
		return VtxView.GetDataSize();
	}
	_open D3D12IBVIEW GetIndicesSrv(){
		HD3D12GPURES hBuffer = $m.lpIdxBuff->GetGPUVirtualAddress();
		return D3D12IBVIEW{ hBuffer, $m.cbIdxBuff, (DXGI1FORMAT)$m.IdxFormat };
	}
	_open D3D12VBVIEW GetVertexsSrv(){
		HD3D12GPURES hBuffer = $m.lpVtxBuff->GetGPUVirtualAddress();
		return D3D12VBVIEW{ hBuffer, $m.cbVtxBuff, $m.GetVertexDataSize() };
	}
};

// Mesh Assembly
class CMeshAssy{
	_open USINT Count;
	_open CMesh3D *lpFirst;

	_open ~CMeshAssy(){
		$m.Finalize();
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

		if(!bFound) goto lbl_end;

		ReadFile(hrFile, &$m.Count, sizeof(USINT), P_Null, P_Null);
		CMesh3D *pModel = $m.lpFirst = new CMesh3D(Uploader, hrFile);

		for(USINT iMesh = 1; iMesh < $m.Count; ++iMesh){
			pModel->pNext = new CMesh3D(Uploader, hrFile);
			pModel = pModel->pNext;
		}

	lbl_end:
		CloseHandle(hrFile);
	}
	_open $VOID Finalize(){
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


//-------------------- 栅格 --------------------//

// Grid
class CGrid{
	_open struct DESC{
		IBOOL bSmooth;
		IBOOL bBslash;
		USINT ColCount;
		USINT RowCount;
		SPFPN CellWidth;
		SPFPN CellDepth;
		SPFPN HeightVal;
		SPFPN LowerLeftX;
		SPFPN LowerLeftZ;
	};
	_open union VERTEX{
		struct{
			VECTOR3 Coord;
			VECTOR3 Normal;
		};
		struct{
			SPFPN PosX;
			SPFPN PosY;
			SPFPN PosZ;
			SPFPN NormX;
			SPFPN NormY;
			SPFPN NormZ;
		};
	};

	_open USINT IdxCount;
	_open USINT VtxCount;
	_open USINT ColCount;
	_open USINT RowCount;
	_open SPFPN CellWidth;
	_open SPFPN CellDepth;
	_open SPFPN LowerLeftX;
	_open SPFPN LowerLeftZ;
	_open RGB10 *prgNormal;
	_open INDEX3 *prgIndex;
	_open VERTEX *prgVertex;
	_secr ID3DResource *lpIdxBuff;
	_secr ID3DResource *lpVtxBuff;
	_secr ID3DResource *lpNormBuff;

	_open ~CGrid(){
		$m.Finalize();
	}
	_open CGrid(){
		Var_Zero(this);
	}
	_open CGrid(ID3DDevice *pD3dDevice, _in DESC &Desc){
		$m.Initialize(pD3dDevice, Desc);
	}
	//////
	_open $VOID Finalize(){
		if($m.lpVtxBuff){
			if($m.prgVertex){
				$m.lpIdxBuff->Unmap(0, P_Null);
				$m.lpVtxBuff->Unmap(0, P_Null);
				$m.lpNormBuff->Unmap(0, P_Null);
			}

			$m.lpIdxBuff->Release();
			$m.lpVtxBuff->Release();
			$m.lpNormBuff->Release();

			Var_Zero(this);
		}
	}
	_secr $VOID InitNormals(IBOOL bSmooth){
		USINT Count = $m.IdxCount / 3;
		RGB10 Value = bSmooth ? 0x1FFFFDFF : 0xDFFFFDFF;

		for(USINT iNorm = 0; iNorm < Count; ++iNorm)
			$m.prgNormal[iNorm] = Value;
	}
	_secr $VOID InitIndices(IBOOL bBslash){
		USINT iFace0 = 0, iFace1 = 1;
		USINT iLine0 = 0, iLine1 = $m.GetDensityX();

		if(bBslash){
			for(USINT iRow = 0; iRow < $m.RowCount; ++iRow){
				for(USINT iCol = 0; iCol < $m.ColCount; ++iCol){
					USINT iNextCol = iCol + 1;

					///第一个三角形
					$m.prgIndex[iFace0][0] = iLine0 + iNextCol;
					$m.prgIndex[iFace0][1] = iLine0 + iCol;
					$m.prgIndex[iFace0][2] = iLine1 + iCol;
					///第二个三角形
					$m.prgIndex[iFace1][0] = iLine1 + iCol;
					$m.prgIndex[iFace1][1] = iLine1 + iNextCol;
					$m.prgIndex[iFace1][2] = iLine0 + iNextCol;
				}
			}
		} else{
			for(USINT iRow = 0; iRow < $m.RowCount; ++iRow){
				for(USINT iCol = 0; iCol < $m.ColCount; ++iCol){
					USINT iNextCol = iCol + 1;

					///第一个三角形
					$m.prgIndex[iFace0][0] = iLine0 + iCol;
					$m.prgIndex[iFace0][1] = iLine1 + iCol;
					$m.prgIndex[iFace0][2] = iLine1 + iNextCol;
					///第二个三角形
					$m.prgIndex[iFace1][0] = iLine1 + iNextCol;
					$m.prgIndex[iFace1][1] = iLine0 + iNextCol;
					$m.prgIndex[iFace1][2] = iLine0 + iCol;

					iFace0 += 2;
					iFace1 += 2;
				}

				iLine0 = iLine1;
				iLine1 += $m.GetDensityX();
			}
		}
	}
	_secr $VOID InitVertexs(SPFPN CoordY){
		VERTEX *pVertex = $m.prgVertex;
		for(USINT iRow = 0; iRow < $m.GetDensityZ(); ++iRow){
			for(USINT iCol = 0; iCol < $m.GetDensityX(); ++iCol){
				pVertex->Normal = VECTR3X::Up;
				pVertex->PosX = $m.LowerLeftX + ($m.CellWidth * iCol);
				pVertex->PosZ = $m.LowerLeftZ + ($m.CellDepth * iRow);
				pVertex->PosY = CoordY;
				pVertex++;
			}
		}
	}
	_open $VOID Initialize(ID3DDevice *pD3dDevice, _in DESC &Desc){
		USINT IdxCount = Desc.ColCount * Desc.RowCount * 6;
		USINT VtxCount = (Desc.ColCount + 1) * (Desc.RowCount + 1);
		USINT FaceCount = IdxCount / 3;
		USINT cbVtxBuff = sizeof(VERTEX) * VtxCount;
		USINT cbIdxBuff = sizeof(INDEX3) * FaceCount;
		USINT cbNormBuff = sizeof(RGB10) * FaceCount;

		if(!pD3dDevice){
			$m.prgIndex = new INDEX3[FaceCount];
			$m.prgNormal = new RGB10[FaceCount];
			$m.prgVertex = new VERTEX[VtxCount];
		} else{
			$m.lpIdxBuff = CreateUploadBuffer(pD3dDevice, cbIdxBuff, B_False);
			$m.lpVtxBuff = CreateUploadBuffer(pD3dDevice, cbVtxBuff, B_False);
			$m.lpNormBuff = CreateUploadBuffer(pD3dDevice, cbNormBuff, B_False);
			$m.lpIdxBuff->Map(0, P_Null, ($VOID**)&$m.prgIndex);
			$m.lpVtxBuff->Map(0, P_Null, ($VOID**)&$m.prgVertex);
			$m.lpNormBuff->Map(0, P_Null, ($VOID**)&$m.prgNormal);
		}

		$m.IdxCount = IdxCount;
		$m.VtxCount = VtxCount;
		$m.ColCount = Desc.ColCount;
		$m.RowCount = Desc.RowCount;
		$m.CellWidth = Desc.CellWidth;
		$m.CellDepth = Desc.CellDepth;
		$m.LowerLeftX = Desc.LowerLeftX;
		$m.LowerLeftZ = Desc.LowerLeftZ;

		$m.InitNormals(Desc.bSmooth);
		$m.InitIndices(Desc.bBslash);
		$m.InitVertexs(Desc.HeightVal);
	}
	_open $VOID UploadToVram(DXTKUploader &Uploader){
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, $m.prgIndex, $m.IdxCount,
			sizeof(USINT), D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpIdxBuff);
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, $m.prgVertex, $m.VtxCount,
			sizeof(VERTEX), D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpVtxBuff);
		dx::CreateStaticBuffer(Uploader.GetDevice(), Uploader, $m.prgNormal, $m.IdxCount / 3,
			sizeof(VECTOR3), D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpNormBuff);

		$m.prgIndex = P_Null;
		$m.prgVertex = P_Null;
		$m.prgNormal = P_Null;
	}
	_open $VOID ApplyHeightMap(_in SPFPN *psMap, _in D2D1SIZEU &Size){
		SPFPN ScaleX = (SPFPN)Size.width / (SPFPN)$m.GetDensityX();
		SPFPN ScaleY = (SPFPN)Size.height / (SPFPN)$m.GetDensityZ();
		D2D1RECTF rcDest = { 0.f, $m.GetDepth(), $m.GetWidth(), 0.f };

		for(USINT iDstRow = 0; iDstRow < $m.GetDensityZ(); ++iDstRow){
			USINT iSrcRow = USINT(iDstRow * ScaleY);
			USINT iDstLine = iDstRow * $m.GetDensityX();
			USINT iSrcLine = iSrcRow * Size.width;

			for(USINT iDstCol = 0; iDstCol < $m.GetDensityX(); ++iDstCol){
				USINT iSrcCol = USINT(iDstCol * ScaleX);
				USINT iSource = iSrcLine + iSrcCol;
				USINT iDest = iDstLine + iDstCol;

				$m.prgVertex[iDest].PosY = psMap[iSource];
			}
		}

		$m.UpdateFaceNormals(rcDest);
	}
	_open $VOID UpdateFaceNormals(_in D2D1RECTF &rcScope){
		MVECTOR Vector0, Vector1, Vector;
		VECTOR3 *pPoint0, *pPoint1, *pPoint2;

		INT32 LocatT = INT32(rcScope.top / $m.CellDepth);
		INT32 LocatL = INT32(rcScope.left / $m.CellWidth);
		INT32 LocatR = INT32(rcScope.right / $m.CellWidth);
		INT32 LocatB = INT32(rcScope.bottom / $m.CellDepth);

		USINT ColStart = CLAMP_(LocatL, 0, (INT32)$m.ColCount - 1);
		USINT RowStart = CLAMP_(LocatB, 0, (INT32)$m.RowCount - 1);
		USINT ColEnd = CLAMP_(LocatR, 0, (INT32)$m.ColCount - 1);
		USINT RowEnd = CLAMP_(LocatT, 0, (INT32)$m.RowCount - 1);

		for(USINT iRow = RowStart; iRow <= RowEnd; ++iRow){
			USINT iLine = iRow * $m.ColCount;

			for(USINT iCol = ColStart; iCol <= ColEnd; ++iCol){
				USINT iFace0 = (iLine + iCol) * 2;
				USINT iFace1 = iFace0 + 1;

				RGB10 &rNorm0 = $m.prgNormal[iFace0];
				RGB10 &rNorm1 = $m.prgNormal[iFace1];

				pPoint0 = &$m.prgVertex[$m.prgIndex[iFace0][0]].Coord;
				pPoint1 = &$m.prgVertex[$m.prgIndex[iFace0][1]].Coord;
				pPoint2 = &$m.prgVertex[$m.prgIndex[iFace0][2]].Coord;

				Vector0 = Vec_Sub(Pck_V3(pPoint1), Pck_V3(pPoint0));
				Vector1 = Vec_Sub(Pck_V3(pPoint2), Pck_V3(pPoint0));
				Vector = Vec3_Normalize(Vec3_Cross(Vector0, Vector1));

				rNorm0 = rNorm0 & 0xC0000000 | NormCompression(Vector);

				pPoint0 = &$m.prgVertex[$m.prgIndex[iFace1][0]].Coord;
				pPoint1 = &$m.prgVertex[$m.prgIndex[iFace1][1]].Coord;
				pPoint2 = &$m.prgVertex[$m.prgIndex[iFace1][2]].Coord;

				Vector0 = Vec_Sub(Pck_V3(pPoint1), Pck_V3(pPoint0));
				Vector1 = Vec_Sub(Pck_V3(pPoint2), Pck_V3(pPoint0));
				Vector = Vec3_Normalize(Vec3_Cross(Vector0, Vector1));

				rNorm1 = rNorm1 & 0xC0000000 | NormCompression(Vector);
			}
		}
	}
	_open $VOID UpdateVertexNormals(_in D2D1RECTF &rcScope){
		using namespace dx;

		INT32 LocatL = INT32(rcScope.left / $m.CellWidth);
		INT32 LocatB = INT32(rcScope.bottom / $m.CellDepth);
		INT32 LocatR = INT32(rcScope.right / $m.CellWidth) + 1;
		INT32 LocatT = INT32(rcScope.top / $m.CellDepth) + 1;

		USINT ColStart = CLAMP_(LocatL, 0, (INT32)$m.ColCount);
		USINT RowStart = CLAMP_(LocatB, 0, (INT32)$m.RowCount);
		USINT ColEnd = CLAMP_(LocatR, 0, (INT32)$m.ColCount);
		USINT RowEnd = CLAMP_(LocatT, 0, (INT32)$m.RowCount);

		USINT ColCount = $m.GetDensityX();
		USINT RowCount = $m.GetDensityZ();

		for(USINT iRow = RowStart; iRow <= RowEnd; ++iRow){
			USINT iLine = iRow * ColCount;

			for(USINT iCol = ColStart; iCol <= ColEnd; ++iCol){
				USINT iVert = iLine + iCol;

				POINT3 *pPoint0 = P_Null;
				POINT3 *pPoint1 = P_Null;
				POINT3 *pPoint2 = P_Null;
				POINT3 *pPoint3 = P_Null;
				POINT3 *pPoint = &$m.prgVertex[iVert].Coord;

				MVECTOR Normal = dx::g_XMZero;
				MVECTOR Vector0, Vector1, Vector2, Vector3;

				if(iRow < (RowCount - 1)){
					pPoint0 = &$m.prgVertex[iVert + ColCount].Coord;
					Vector0 = Pck_V3(pPoint0) - Pck_V3(pPoint);
				}
				if(iCol < (ColCount - 1)){
					pPoint1 = &$m.prgVertex[iVert + 1].Coord;
					Vector1 = Pck_V3(pPoint1) - Pck_V3(pPoint);
				}
				if(iRow > 0){
					pPoint2 = &$m.prgVertex[iVert - ColCount].Coord;
					Vector2 = Pck_V3(pPoint2) - Pck_V3(pPoint);
				}
				if(iCol > 0){
					pPoint3 = &$m.prgVertex[iVert - 1].Coord;
					Vector3 = Pck_V3(pPoint3) - Pck_V3(pPoint);
				}

				if(pPoint0 && pPoint1)
					Normal += Vec3_Cross(Vector0, Vector1);
				if(pPoint1 && pPoint2)
					Normal += Vec3_Cross(Vector1, Vector2);
				if(pPoint2 && pPoint3)
					Normal += Vec3_Cross(Vector2, Vector3);
				if(pPoint3 && pPoint0)
					Normal += Vec3_Cross(Vector3, Vector0);

				Upk_V3(&$m.prgVertex[iVert].Normal, Vec3_Normalize(Normal));
			}
		}
	}
	_open IBOOL IntersectRay(_in RAY &Ray, _out SPFPN *pDist = P_Null, SPFPN Step = 0.5f, SPFPN Epsilon = 0.001f){
		///系数计算

		RAY2 Ray2;
		AABB2 Box2;
		SPFPN T0, T1;

		Box2.Min.x = $m.LowerLeftX;
		Box2.Min.y = $m.LowerLeftZ;
		Box2.Max.x = Box2.Min.x + $m.GetWidth();
		Box2.Max.y = Box2.Min.y + $m.GetDepth();

		Ray2.Origin.x = Ray.position.x;
		Ray2.Origin.y = Ray.position.z;
		Ray2.Direction.x = Ray.direction.x;
		Ray2.Direction.y = Ray.direction.z;

		if(!Intersect(Ray2, Box2, &T0, &T1))
			return B_False;

		///交点测试

		VECTR3X RayPt;
		SPFPN Height;
		SPFPN DifT = T1 - T0;
		SPFPN T, MinT, MidT, MaxT;

		Step *= MIN_($m.CellWidth, $m.CellDepth);
		Step = DifT / Ceil(DifT / Step);

		for(T = T0; T <= T1; T += Step){
			RayPt = Ray.position + (Ray.direction * T);
			Height = $m.SampleHeight(RayPt.x, RayPt.z);

			if(RayPt.y < Height){
				MaxT = T;
				MinT = T - Step;

				while((MaxT - MinT) > Epsilon){
					MidT = (MinT + MaxT) * 0.5f;
					RayPt = Ray.position + (Ray.direction * MidT);
					Height = $m.SampleHeight(RayPt.x, RayPt.z);

					if(RayPt.y < Height) MaxT = MidT;
					else MinT = MidT;
				}

				if(pDist) *pDist = MaxT;
				return B_True;
			}
		}

		return B_False;
	}
	_open SPFPN SampleHeight(SPFPN PointX, SPFPN PointZ){
		USINT iCol = USINT((PointX - $m.LowerLeftX) / $m.CellWidth);
		USINT iRow = USINT((PointZ - $m.LowerLeftZ) / $m.CellDepth);
		USINT iCell = ($m.ColCount * iRow) + iCol;
		INDEX3 *pFace = &$m.prgIndex[iCell * 2];

		VECTR3X *pP0 = (VECTR3X*)&$m.prgVertex[pFace[0][0]].Coord;
		VECTR3X *pP1 = (VECTR3X*)&$m.prgVertex[pFace[0][1]].Coord;
		VECTR3X *pP2 = (VECTR3X*)&$m.prgVertex[pFace[0][2]].Coord;
		VECTR3X *pP3 = (VECTR3X*)&$m.prgVertex[pFace[1][1]].Coord;

		VECTR3X *pPx, *pPz, *pPo;
		VECTR3X Vx, Vz, Vt;
		SPFPN Tx, Tz;

		if(pP0->x == pP1->x){
			pPx = pP2;
			pPz = pP0;
		} else{
			pPx = pP0;
			pPz = pP2;
		}

		Tx = (PointX - pP1->x) / (pPx->x - pP1->x);
		Tz = (PointZ - pP1->z) / (pPz->z - pP1->z);

		if((Tx + Tz) < 1.f){
			pPo = pP1;
		} else{
			pPo = pP3;
			Tx = 1.f - Tx;
			Tz = 1.f - Tz;
		}

		Vx = *pPx - *pPo;
		Vz = *pPz - *pPo;
		Vt = (Vx * Tx) + (Vz * Tz);

		return (*pPo + Vt).y;
	}
	_open SPFPN GetVertexHeight(USINT RowId, USINT ColId){
		USINT Index = ($m.ColCount + 1)*RowId + ColId;
		return $m.prgVertex[Index].PosY;
	}
	_open DWORD GetVertexFormat(){
		return VFB_Coord | VFB_Normal;
	}
	_open DWORD GetIndexFormat(){
		return DXGI_FORMAT_R32_UINT;
	}
	_open USINT GetDensityX(){
		return $m.ColCount + 1;
	}
	_open USINT GetDensityZ(){
		return $m.RowCount + 1;
	}
	_open SPFPN GetWidth(){
		return $m.ColCount * $m.CellWidth;
	}
	_open SPFPN GetDepth(){
		return $m.RowCount * $m.CellDepth;
	}
	_open D3D12VBVIEW GetVertexsSrv(){
		USINT cbBuffer = $m.VtxCount * sizeof(VERTEX);
		HD3D12GPURES hBuffer = $m.lpVtxBuff->GetGPUVirtualAddress();
		return D3D12VBVIEW{ hBuffer, cbBuffer, sizeof(VERTEX) };
	}
	_open D3D12IBVIEW GetIndicesSrv(){
		USINT cbBuffer = $m.IdxCount * sizeof(USINT);
		HD3D12GPURES hBuffer = $m.lpIdxBuff->GetGPUVirtualAddress();
		return D3D12IBVIEW{ hBuffer, cbBuffer, DXGI_FORMAT_R32_UINT };
	}
	_open HD3D12GPURES GetNormalsVram(){
		return $m.lpNormBuff->GetGPUVirtualAddress();
	}
	_open HD3D12GPURES GetIndicesVram(){
		return $m.lpIdxBuff->GetGPUVirtualAddress();
	}
};

//----------------------------------------//


//-------------------- 精灵 --------------------//

// Sprite
class CSprite{
	_secr struct FRAME{
		USINT Ident;
		UHALF Left;
		UHALF Top;
		UHALF Right;
		UHALF Bottom;
	};

	_secr USINT numFrame;
	_secr FRAME *lprgFrame;
	_secr ID2DBitmap *lpSheet;

	_open ~CSprite(){
		$m.Finalize();
	}
	_open CSprite(){
		Var_Zero(this);
	}
	_open CSprite(IWICFactory *pWicFactory, ID2DDevCtx *pD2dDevCtx, _in WCHAR *psFilePath){
		$m.Initialize(pWicFactory, pD2dDevCtx, psFilePath);
	}
	//////
	_open $VOID Initialize(IWICFactory *pWicFactory, ID2DDevCtx *pD2dDevCtx, _in WCHAR *psFilePath){
		WCHAR JsonPath[MAX_PATH];
		CJsonNode *pDomFrame, *pDomRect;
		CJsonNode *lpDomRoot;
		FRAME *pFrame;

		WcsCpy(JsonPath, psFilePath);
		WcsCpy(WcsRchr(JsonPath, L'.'), L".json");

		lpDomRoot = CJsonNode::CreateByFile(JsonPath);
		pDomFrame = lpDomRoot->GetChild("frames")->GetChild();

		$m.lpSheet = LoadD2dBitmap(pWicFactory, pD2dDevCtx, psFilePath);
		$m.numFrame = (USINT)lpDomRoot->GetChild("frames")->GetSize();
		$m.lprgFrame = pFrame = new FRAME[$m.numFrame];

		for(USINT iFrame = 0; iFrame < $m.numFrame; ++iFrame){
			pDomRect = pDomFrame->GetChild("frame");
			pFrame->Top = (UHALF)pDomRect->GetMemI("y");
			pFrame->Left = (UHALF)pDomRect->GetMemI("x");
			pFrame->Right = (UHALF)pDomRect->GetMemI("w") + pFrame->Left;
			pFrame->Bottom = (UHALF)pDomRect->GetMemI("h") + pFrame->Top;
			pFrame->Ident = StrHash32(pDomFrame->GetMemS("filename"));

			pDomFrame = pDomFrame->GetNext();
			pFrame++;
		}

		lpDomRoot->Release();
	}
	_open $VOID Finalize(){
		SAFE_RELEASE($m.lpSheet);
		SAFE_DELETEA($m.lprgFrame);
	}
	_open USINT GetFrameCount(){
		return $m.numFrame;
	}
	_open USINT GetFrameIndex(USINT Ident){
		USINT iFrame = 0;
		FRAME *pFrame = $m.lprgFrame;

		while(iFrame < $m.numFrame){
			if(pFrame->Ident == Ident) return iFrame;
			iFrame++;
			pFrame++;
		}

		return UINT_MAX;
	}
	_open D2D1RECTF GetFrame(USINT Index){
		FRAME &rFrame = $m.lprgFrame[Index];

		SPFPN Top = (SPFPN)rFrame.Top;
		SPFPN Left = (SPFPN)rFrame.Left;
		SPFPN Right = (SPFPN)rFrame.Right;
		SPFPN Bottom = (SPFPN)rFrame.Bottom;

		return D2D1RECTF{ Left, Top, Right, Bottom };
	}
	_open D2D1RECTF GetFrameById(USINT Ident){
		FRAME *pFrame = $m.lprgFrame;
		FRAME *pEndFrame = pFrame + $m.numFrame;

		for(; pFrame != pEndFrame; ++pFrame)
			if(pFrame->Ident == Ident) break;

		SPFPN Top = (SPFPN)pFrame->Top;
		SPFPN Left = (SPFPN)pFrame->Left;
		SPFPN Right = (SPFPN)pFrame->Right;
		SPFPN Bottom = (SPFPN)pFrame->Bottom;

		return D2D1RECTF{ Left, Top, Right, Bottom };
	}
	_open ID2DBitmap *GetSheet(){
		return $m.lpSheet;
	}
};

//----------------------------------------//


//-------------------- 动画 --------------------//

// Animation
class CAnimation{
	_secr UHALF iCurRot;
	_secr UHALF iCurScale;
	_secr UHALF iCurTrans;
	_secr UHALF numTrans;
	_secr UHALF numScaling;
	_secr UHALF numRotation;
	_secr ROTFRAME *lprgRotation;
	_secr ANIMFRAME *lprgScaling;
	_secr ANIMFRAME *lprgTrans;

	_open ~CAnimation(){
		$m.Finalize();
	}
	_open CAnimation(){
		Var_Zero(this);
	}
	_open CAnimation(HANDLE hFile){
		$m.Initialize(hFile);
	}
	//////
	_open $VOID Finalize(){
		SAFE_DELETEA($m.lprgTrans);
		SAFE_DELETEA($m.lprgScaling);
		SAFE_DELETEA($m.lprgRotation);
	}
	_open $VOID Initialize(HANDLE hFile){
		USINT Count0, Count1, Count2;

		ReadFile(hFile, &Count0, sizeof(USINT), P_Null, P_Null);
		ReadFile(hFile, &Count1, sizeof(USINT), P_Null, P_Null);
		ReadFile(hFile, &Count2, sizeof(USINT), P_Null, P_Null);

		CAST_($m.numTrans, Count0);
		CAST_($m.numScaling, Count1);
		CAST_($m.numRotation, Count2);

		$m.lprgTrans = new ANIMFRAME[Count0];
		$m.lprgScaling = new ANIMFRAME[Count1];
		$m.lprgRotation = new ROTFRAME[Count2];

		ReadFile(hFile, $m.lprgTrans, sizeof(ANIMFRAME) * Count0, P_Null, P_Null);
		ReadFile(hFile, $m.lprgScaling, sizeof(ANIMFRAME) * Count1, P_Null, P_Null);
		ReadFile(hFile, $m.lprgRotation, sizeof(ROTFRAME) * Count2, P_Null, P_Null);
	}
	_open $VOID Interpolate(SPFPN Time, _out MMATRIX &rXform){
		MVECTOR Center = dx::g_XMZero;
		MVECTOR Trans, Scaling, Rotation;

		if($m.numTrans == 0) Trans = dx::g_XMZero;
		else $m.InterpolateTranslation(Time, Trans);

		if($m.numScaling == 0) Scaling = dx::g_XMOne;
		else $m.InterpolateScaling(Time, Scaling);

		if($m.numRotation == 0) Rotation = dx::g_XMIdentityR3;
		else $m.InterpolateRotation(Time, Rotation);

		rXform = Mat_AffineXform(Center, Scaling, Rotation, Trans);
	}
	_secr $VOID InterpolateScaling(SPFPN Time, _out MVECTOR &rXform){
		USINT iEndFrm = $m.numScaling - 1;
		UHALF &rCurFrm = $m.iCurScale;
		ANIMFRAME *prgFrame = $m.lprgScaling;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Pck_V3(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Pck_V3(&prgFrame[iEndFrm].Xform);
		} else{
			ANIMFRAME *pFrm0 = &prgFrame[rCurFrm];
			ANIMFRAME *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MVECTOR Start = Pck_V3(&pFrm0->Xform);
					MVECTOR End = Pck_V3(&pFrm1->Xform);
					rXform = Vec_Lerp(Start, End, Weight);
					return;
				} else{
					pFrm0 = pFrm1++;
					rCurFrm++;
				}
			}
		}
	}
	_secr $VOID InterpolateRotation(SPFPN Time, _out MVECTOR &rXform){
		USINT iEndFrm = $m.numRotation - 1;
		UHALF &rCurFrm = $m.iCurRot;
		ROTFRAME *prgFrame = $m.lprgRotation;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Pck_V4(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Pck_V4(&prgFrame[iEndFrm].Xform);
		} else{
			ROTFRAME *pFrm0 = &prgFrame[rCurFrm];
			ROTFRAME *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MVECTOR Start = Pck_V4(&pFrm0->Xform);
					MVECTOR End = Pck_V4(&pFrm1->Xform);
					rXform = Quat_Slerp(Start, End, Weight);
					return;
				} else{
					pFrm0 = pFrm1++;
					rCurFrm++;
				}
			}
		}
	}
	_secr $VOID InterpolateTranslation(SPFPN Time, _out MVECTOR &rXform){
		USINT iEndFrm = $m.numTrans - 1;
		UHALF &rCurFrm = $m.iCurTrans;
		ANIMFRAME *prgFrame = $m.lprgTrans;

		if(prgFrame[rCurFrm].Time > Time)
			rCurFrm = 0;

		if(Time <= prgFrame[0].Time){
			rXform = Pck_V3(&prgFrame[0].Xform);
		} else if(Time >= prgFrame[iEndFrm].Time){
			rXform = Pck_V3(&prgFrame[iEndFrm].Xform);
		} else{
			ANIMFRAME *pFrm0 = &prgFrame[rCurFrm];
			ANIMFRAME *pFrm1 = pFrm0 + 1;

			while(rCurFrm < iEndFrm){
				if((Time >= pFrm0->Time) && (Time <= pFrm1->Time)){
					SPFPN Weight = (Time - pFrm0->Time) / (pFrm1->Time - pFrm0->Time);
					MVECTOR Start = Pck_V3(&pFrm0->Xform);
					MVECTOR End = Pck_V3(&pFrm1->Xform);
					rXform = Vec_Lerp(Start, End, Weight);
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
	_secr USINT numElem;
	_secr CAnimation *lprgElem;

	_open ~CAnimAssy(){
		$m.Finalize();
	}
	_open CAnimAssy(){
		Var_Zero(this);
	}
	_open CAnimAssy(_in WCHAR *psFilePath){
		$m.Initialize(psFilePath);
	}
	//////
	_open $VOID Finalize(){
		SAFE_DELETEA($m.lprgElem);
	}
	_open $VOID Initialize(_in WCHAR *psFilePath){
		IBOOL bFound;
		HANDLE hrFile;
		CAnimation *pAnim;
		CAnimation *pEndAnim;

		hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		bFound = FindFileChunk(hrFile, ASSET_TYPE_Anim);

		if(!bFound) goto lbl_end;

		ReadFile(hrFile, &$m.Duration, sizeof(SPFPN), P_Null, P_Null);
		ReadFile(hrFile, &$m.numElem, sizeof(USINT), P_Null, P_Null);

		pAnim = $m.lprgElem = new CAnimation[$m.numElem];
		pEndAnim = pAnim + $m.numElem;

		for(; pAnim != pEndAnim; ++pAnim)
			pAnim->Initialize(hrFile);

	lbl_end:
		CloseHandle(hrFile);
	}
	_open $VOID ApplyToBones(SPFPN Time, _out BONE *prgBone){
		for(USINT iPart = 0; iPart < $m.numElem; ++iPart){
			BONE &rBone = prgBone[iPart];
			CAnimation &rAnim = $m.lprgElem[iPart];
			rAnim.Interpolate(Time, rBone.matLocal);
		}
	}
	_open SPFPN GetDuration(){
		return $m.Duration;
	}
	_open USINT GetCount(){
		return $m.numElem;
	}
};

//----------------------------------------//


//--------------------#天空#--------------------//

////天空穹
//struct CSkyDome{
//	USINT cbVtxBuff;
//	USINT cbIdxBuff;
//	ID3DResource *lpVtxBuff;
//	ID3DResource *lpIdxBuff;
//	ID3DResource *lpTexture;
//	ID3DViewHeap *lpSrvHeap;
//	ID3DPipeState *lpPipeState;
//	ID3DRootSig *lpRootSig;
//
//	~CSkyDome(){
//		$m.Finalize();
//	}
//	CSkyDome(){
//		Var_Zero(this);
//	}
//	CSkyDome(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, WCHAR* TexPath, SPFPN Diameter, D3D12GRAPHICSPIPEDESC &PSODesc){
//		$m.Initialize(lpD3dDevice, Uploader, TexPath, Diameter, PSODesc);
//	}
//
//	$VOID Initialize(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, WCHAR* TexPath, SPFPN Diameter, D3D12GRAPHICSPIPEDESC &PSODesc){
//		$m.CreateMesh(lpD3dDevice, Uploader, Diameter);
//		$m.CreateTexture(lpD3dDevice, Uploader, TexPath);
//		$m.CreatePSO(lpD3dDevice, &PSODesc);
//	}
//	$VOID CreatePSO(ID3DDevice* lpD3dDevice, D3D12GRAPHICSPIPEDESC* pPSODesc){
//		ID3DBlob *lpVSCode;
//		ID3DBlob *lpPSCode;
//		ID3DBlob *lpSerialize;
//		D3DX12VIEWRANGE ViewRange;
//		D3DX12ROOTPARAM arrParam[2];
//		D3D12INPUTELEMDESC InputDesc;
//		D3DX12ROOTSIGDESC RootSigDesc;
//		D3DX12SSAMPLERDESC SamplerDesc;
//
//		//编译着色器代码
//
//		lpVSCode = CompileShaderFile(
//			L"./source/gs_sky.hlsl", "VertexShade", "vs_5_1", P_Null);
//		lpPSCode = CompileShaderFile(
//			L"./source/gs_sky.hlsl", "PixelShade", "ps_5_1", P_Null);
//
//		//创建根签名
//
//		SamplerDesc.Init(0, D3D12_FILTER_ANISOTROPIC,
//			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
//			D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.f, 8);
//
//		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
//
//		arrParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
//		arrParam[1].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL);
//
//		RootSigDesc.Init(2, arrParam, 1, &SamplerDesc,
//			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
//			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
//			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
//			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//		D3D12SerializeRootSignature(&RootSigDesc,
//			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, P_Null);
//
//		lpD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
//			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));
//
//		lpSerialize->Release();
//
//		//创建PSO
//
//		InputDesc.InputSlot = 0;
//		InputDesc.SemanticIndex = 0;
//		InputDesc.AlignedByteOffset = 0;
//		InputDesc.InstanceDataStepRate = 0;
//		InputDesc.SemanticName = "POSITION";
//		InputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
//		InputDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
//
//		pPSODesc->VS = { lpVSCode->GetBufferPointer(), lpVSCode->GetBufferSize() };
//		pPSODesc->PS = { lpPSCode->GetBufferPointer(), lpPSCode->GetBufferSize() };
//		pPSODesc->InputLayout = { &InputDesc, 1 };
//		pPSODesc->pRootSignature = $m.lpRootSig;
//		pPSODesc->RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
//		pPSODesc->DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//
//		lpD3dDevice->CreateGraphicsPipelineState(pPSODesc, IID_AND_PPV($m.lpPipeState));
//
//		lpVSCode->Release();
//		lpPSCode->Release();
//	}
//	$VOID CreateMesh(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, SPFPN Diameter){
//		UHALF *arrIndex;
//		VECTOR3 *arrVertex;
//		USINT VtxCount, IdxCount;
//		STLVector<UHALF> IndexList;
//		STLVector<DXTKGeomPrim::VertexType> VtxList;
//
//		DXTKGeomPrim::CreateSphere(VtxList, IndexList, Diameter, 8, B_False);
//
//		VtxCount = (USINT)VtxList.size();
//		IdxCount = (USINT)IndexList.size();
//
//		arrVertex = new VECTOR3[VtxCount];
//		arrIndex = new UHALF[IdxCount];
//
//		for(USINT iExe = 0; iExe < VtxCount; ++iExe)
//			arrVertex[iExe] = VtxList[iExe].position;
//		for(USINT iExe = 0; iExe < IdxCount; ++iExe)
//			arrIndex[iExe] = IndexList[iExe];
//
//		$m.cbVtxBuff = sizeof(VECTOR3) * VtxCount;
//		$m.cbIdxBuff = sizeof(UHALF) * VtxCount;
//
//		dx::CreateStaticBuffer(lpD3dDevice, Uploader, arrVertex, $m.cbVtxBuff, 1,
//			D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpVtxBuff);
//		dx::CreateStaticBuffer(lpD3dDevice, Uploader, arrIndex, $m.cbIdxBuff, 1,
//			D3D12_RESOURCE_STATE_GENERIC_READ, &$m.lpIdxBuff);
//	}
//	$VOID CreateTexture(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, WCHAR* FilePath){
//		D3D12CPUVIEWPTR hTexView;
//		D3D12RESDESC tTexDesc;
//		D3D12SRVDESC tViewDesc;
//		D3D12VIEWHEAPDESC tHeapDesc;
//
//		//创建纹理
//
//		dx::CreateDDSTextureFromFile(lpD3dDevice, Uploader, FilePath, &$m.lpTexture);
//
//		//创建视图堆
//
//		tHeapDesc.NodeMask = 0;
//		tHeapDesc.NumDescriptors = 1;
//		tHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//		tHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//
//		lpD3dDevice->CreateDescriptorHeap(&tHeapDesc, IID_AND_PPV($m.lpTexViews));
//
//		//创建视图
//
//		hTexView = $m.lpTexViews->GetCPUDescriptorHandleForHeapStart();
//		tTexDesc = *(D3D12RESDESC*)&$m.lpTexture->GetDesc();
//
//		tViewDesc.TextureCube.MostDetailedMip = 0;
//		tViewDesc.TextureCube.ResourceMinLODClamp = 0.f;
//		tViewDesc.TextureCube.MipLevels = tTexDesc.MipLevels;
//		tViewDesc.Format = tTexDesc.Format;
//		tViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
//		tViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//
//		lpD3dDevice->CreateShaderResourceView($m.lpTexture, &tViewDesc, hTexView);
//	}
//	$VOID Finalize(){
//		if(!$m.lpVtxBuff) return;
//
//		SAFE_RELEASE($m.lpRootSig);
//		SAFE_RELEASE($m.lpPipeState);
//		SAFE_RELEASE($m.lpTexViews);
//		SAFE_RELEASE($m.lpTexture);
//		SAFE_RELEASE($m.lpVtxBuff);
//		SAFE_RELEASE($m.lpIdxBuff);
//
//		Var_Zero(this);
//	}
//	D3D12GPUVIEWPTR GetTextureGpuView(){
//		return (D3D12GPUVIEWPTR)$m.lpTexViews->GetGPUDescriptorHandleForHeapStart();
//	}
//	D3D12IBVIEW GetIndexBufferView(){
//		D3D12IBVIEW IBView;
//		IBView.BufferLocation = $m.lpIdxBuff->GetGPUVirtualAddress();
//		IBView.SizeInBytes = $m.cbIdxBuff;
//		IBView.Format = DXGI_FORMAT_R16_UINT;
//		return IBView;
//	}
//	D3D12VBVIEW GetVertexBufferView(){
//		D3D12VBVIEW VBView;
//		VBView.BufferLocation = $m.lpVtxBuff->GetGPUVirtualAddress();
//		VBView.StrideInBytes = sizeof(VECTOR3);
//		VBView.SizeInBytes = $m.cbVtxBuff;
//		return VBView;
//	}
//};

//----------------------------------------//


//--------------------#变形器#--------------------//

////变形PSO
//struct CMorphPso{
//	ID3DRootSig *lpRootSig;
//	ID3DPipeState *lpPipeState;
//
//	$VOID Finalize(){
//		SAFE_RELEASE($m.lpRootSig);
//		SAFE_RELEASE($m.lpPipeState);
//	}
//	$VOID Initialize(ID3DDevice* lpD3dDevice){
//		ID3DBlob *lpShadeCode;
//		ID3DBlob *lpSerialize;
//		D3DX12ROOTPARAM arrRootParam[4];
//		D3DX12ROOTSIGDESC RootSigDesc;
//		D3D12COMPUTEPIPEDESC PSODesc;
//
//		//编译着色器代码
//
//		lpShadeCode = CompileShaderFile(
//			L"./source/gs_morph.hlsl", "VertexMorph", "cs_5_1", P_Null);
//
//		//创建根签名
//
//		arrRootParam[0].InitAsConstants(4, 0);
//		arrRootParam[1].InitAsShaderResourceView(0);
//		arrRootParam[2].InitAsShaderResourceView(1);
//		arrRootParam[3].InitAsUnorderedAccessView(0);
//
//		RootSigDesc.Init(ARR_LEN(arrRootParam), arrRootParam, 0, P_Null,
//			D3D12_ROOT_SIGNATURE_FLAG_NONE);
//
//		D3D12SerializeRootSignature(&RootSigDesc,
//			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, P_Null);
//
//		lpD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
//			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));
//
//		lpSerialize->Release();
//
//		//创建PSO
//
//		Sto_Bytes(&PSODesc, 0, sizeof(D3D12COMPUTEPIPEDESC));
//		PSODesc.pRootSignature = $m.lpRootSig;
//		PSODesc.CS = { lpShadeCode->GetBufferPointer(), lpShadeCode->GetBufferSize() };
//		PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
//
//		lpD3dDevice->CreateComputePipelineState(&PSODesc, IID_AND_PPV($m.lpPipeState));
//		lpShadeCode->Release();
//	}
//};
//
////变形器
//struct CMorpher{
//	USINT Ident;
//	USINT cbMesh;
//	USINT numMesh;
//	ID3DResource *arrMesh;
//
//	~CMorpher(){
//		$m.Finalize();
//	}
//	CMorpher(){
//		Var_Zero(this);
//	}
//	CMorpher(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, WCHAR *FilePath){
//		$m.Initialize(lpD3dDevice, Uploader, FilePath);
//	}
//
//	$VOID Finalize(){
//		SAFE_RELEASE($m.arrMesh);
//	}
//	$VOID Initialize(ID3DDevice* lpD3dDevice, DXTKUploader &Uploader, WCHAR *FilePath){
//		//定位数据
//
//		HANDLE hFile;
//		IBOOL bSucceed;
//		USINT ChunkSize;
//		DWORD ChunkType;
//
//		hFile = CreateFile(FilePath, GENERIC_READ, 0L,
//			P_Null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, P_Null);
//
//		while(B_True){
//			bSucceed = ReadFile(hFile, &ChunkType, sizeof(USINT), P_Null, P_Null);
//			if(!bSucceed) goto done;
//
//			ReadFile(hFile, &ChunkSize, sizeof(USINT), P_Null, P_Null);
//			if(ChunkType == ASSET_TYPE_SKELETON) break;
//			else SetFilePointer(hFile, ChunkSize, P_Null, FILE_CURRENT);
//		}
//
//		//读取数据
//
//		BYTET *Buffer;
//		USINT cbBuffer;
//
//		ReadFile(hFile, &$m.Ident, sizeof(USINT), P_Null, P_Null);
//		ReadFile(hFile, &$m.cbMesh, sizeof(USINT), P_Null, P_Null);
//		ReadFile(hFile, &$m.numMesh, sizeof(USINT), P_Null, P_Null);
//
//		cbBuffer = $m.cbMesh * $m.numMesh;
//		Buffer = new BYTET[cbBuffer];
//		ReadFile(hFile, Buffer, cbBuffer, P_Null, P_Null);
//
//		dx::CreateStaticBuffer(lpD3dDevice, Uploader, Buffer, cbBuffer, 1,
//			D3D12_RESOURCE_STATE_GENERIC_READ, &$m.arrMesh);
//done:
//		CloseHandle(hFile);
//	}
//	$VOID UpdateMesh(ID3DCmdList1* lpCmdList, CMorphPso &Pso, USINT Targets[2], SPFPN Lerp, CMesh3D* Dest){
//		struct SETTING{
//			SPFPN MorphLerp;
//			IBOOL bWithNormal;
//			USINT cbVertex;
//			USINT VtxCount;
//		};
//
//		HD3D12GPURES hSrcBuff = $m.arrMesh->GetGPUVirtualAddress();
//		HD3D12GPURES hSrcBuff0 = hSrcBuff + Targets[0] * $m.cbMesh;
//		HD3D12GPURES hSrcBuff1 = hSrcBuff + Targets[1] * $m.cbMesh;
//		HD3D12GPURES hDestBuff = Dest->lpVtxBuff->GetGPUVirtualAddress();
//
//		SETTING Setting;
//		Setting.MorphLerp = Lerp;
//		Setting.bWithNormal = Dest->VtxFormat & VFB_NORMAL;
//		Setting.cbVertex = Dest->GetVertexByteCount();
//		Setting.VtxCount = Dest->cbVtxBuff / Setting.cbVertex;
//		Setting.cbVertex /= 4;
//
//		lpCmdList->SetPipelineState(Pso.lpPipeState);
//		lpCmdList->SetComputeRootSignature(Pso.lpRootSig);
//		lpCmdList->SetComputeRoot32BitConstants(0, 4, &Setting, 0);
//		lpCmdList->SetComputeRootShaderResourceView(1, hSrcBuff0);
//		lpCmdList->SetComputeRootShaderResourceView(2, hSrcBuff1);
//		lpCmdList->SetComputeRootUnorderedAccessView(3, hDestBuff);
//
//		lpCmdList->Dispatch(MAX_(Setting.VtxCount / 32, 1), 1, 1);
//	}
//};

//----------------------------------------//


//--------------------#粒子系统#--------------------//

////粒子组
//class CPtcGroup{
//	_open USINT Ident; //ID
//	_open USINT TexIdent; //纹理ID
//	_open USINT MtlIdent; //材质ID
//	///发射器
//	_secr SPFPN EmitValue;    //发射进度
//	_open SPFPN EmitRates[2]; //发射速率
//	_open VECTOR3 EmitRange; //发射范围
//	///粒子
//	_open DWORD PtcFormat;    //粒子格式
//	_secr USINT PtcIndex;     //粒子索引
//	_open USINT PtcCount;     //粒子数量
//	_open USINT PtcMaxCnt;    //粒子最大数量
//	_open USINT PtcByteCnt;   //粒子字节数
//	_secr BYTET *PtcBuff;     //粒子缓存区
//	_open VECTOR2 PtcSize;   //粒子尺寸
//	_open COLOR128 PtcColor; //粒子颜色
//	_secr CPtcView PtcView;  //粒子视图
//	///事件
//	_open USINT numTopDriver;        //顶事件数量
//	_open CPtcDriver **arrTopDriver; //顶事件数组
//
//	_open ~CPtcGroup(){
//		$m.Finalize();
//	}
//	_open CPtcGroup(){
//		Var_Zero(this);
//	}
//
//	_open $VOID Finalize(){
//		if(!$m.PtcBuff) return;
//
//		CPtcDriver *pDriver, *NextDriver;
//		for(USINT iExe = 0; iExe < $m.numTopDriver; ++iExe){
//			pDriver = $m.arrTopDriver[iExe];
//			while(pDriver){
//				NextDriver = pDriver->Next;
//				delete[] pDriver;
//				pDriver = NextDriver;
//			}
//		}
//
//		SAFE_DELETEA($m.arrTopDriver);
//		SAFE_DELETEA($m.PtcBuff);
//
//		Var_Zero(this);
//	}
//	_open $VOID Initialize(JSONNODE &pDomRoot){
//		////解析基本数据
//
//		//JSONNODE DomNode;
//		//VECTOR2 Vec2Range[2];
//		//VECTOR3 Vec3Range[2];
//		//COLOR128 ColorRange[2];
//
//		//DomNode = pDomRoot.get_object()["name"];
//		//$m.Ident = StrHash32((CHAR8*)DomNode.get_string().c_str());
//
//		//DomNode = pDomRoot.get_object()["texture"];
//		//$m.TexIdent = StrHash32((CHAR8*)DomNode.get_string().c_str());
//
//		//DomNode = pDomRoot.get_object()["numparticles"];
//		//$m.PtcMaxCnt = (USINT)DomNode.get_uint64();
//
//		//DomNode = pDomRoot.get_object()["emitrate"];
//		//CPtcDriver::ParseNumber((CHAR8*)DomNode.get_string().c_str(), $m.EmitRates);
//
//		//DomNode = pDomRoot.get_object()["emitrange"];
//		//CPtcDriver::ParseVector3((CHAR8*)DomNode.get_string().c_str(), Vec3Range);
//		//Rand_Vec3($m.EmitRange, Vec3Range);
//
//		//DomNode = pDomRoot.get_object()["psize"];
//		//CPtcDriver::ParseVector2((CHAR8*)DomNode.get_string().c_str(), Vec2Range);
//		//Rand_Vec2($m.PtcSize, Vec2Range);
//
//		//DomNode = pDomRoot.get_object()["color"];
//		//CPtcDriver::ParseColor((CHAR8*)DomNode.get_string().c_str(), ColorRange);
//		//Rand_Color($m.PtcColor, ColorRange);
//
//		////解析事件
//
//		//$m.InitDrivers(pDomRoot);
//	}
//	_secr $VOID InitDrivers(JSONNODE &pDomRoot){
//		/*JSONNODE DomNode, DomElem, DomAttr;
//		CPtcDriver **arrDriver;
//		USINT numDriver;
//		const CHAR8 *Type, *SubType, *Time, *Value;*/
//
//		//解析事件数量
//
//		//DomNode = pDomRoot.get_object()["numevents"];
//		//numDriver = (USINT)DomNode.get_uint64();
//		//arrDriver = new CPtcDriver*[numDriver];
//
//		////解析事件
//
//		//DomNode = pDomRoot.get_object()["events"];
//
//		//for(USINT iExe = 0; iExe < numDriver; ++iExe){
//		//	DomElem = DomNode.get_array()[iExe];
//		//	DomAttr = DomElem.get_object()["type"];
//		//	Type = (CHAR8*)DomNode.get_string().c_str();
//		//	DomAttr = DomElem.get_object()["subtype"];
//		//	SubType = (CHAR8*)DomNode.get_string().c_str();
//		//	DomAttr = DomElem.get_object()["time"];
//		//	Time = (CHAR8*)DomNode.get_string().c_str();
//		//	DomAttr = DomElem.get_object()["value"];
//		//	Value = (CHAR8*)DomNode.get_string().c_str();
//
//		//	arrDriver[iExe] = CPtcDriverFactory::Create(Type, Time, SubType, Value);
//		//}
//
//		//优化事件列表
//
//		/*$m.InitTopDriverList(arrDriver, numDriver);
//		SAFE_DELETEA(arrDriver);*/
//	}
//	_secr $VOID InitTopDriverList(CPtcDriver** arrDriver, USINT numDriver){
//		DWORD LastType;
//		USINT TypeCount;
//		CPtcDriver *pDriver;
//
//		//选取事件发生时间
//
//		for(USINT iExe = 0; iExe < numDriver; ++iExe)
//			arrDriver[iExe]->Time = Rand_Real(arrDriver[iExe]->Times);
//
//		//按事件时间和类型排序
//
//		QckSort(arrDriver, numDriver, sizeof(CPtcDriver*), CPtcDriver::Compare);
//
//		//统计事件类型
//
//		TypeCount = 0;
//		LastType = -1L;
//
//		for(USINT iExe = 0; iExe < numDriver; ++iExe){
//			if(arrDriver[iExe]->Type != LastType){
//				LastType = arrDriver[iExe]->Type;
//				TypeCount++;
//			}
//		}
//
//		//连接同类型事件
//
//		$m.numTopDriver = TypeCount;
//		$m.arrTopDriver = new CPtcDriver*[TypeCount];
//
//		for(USINT iExe0 = 0, iExe = 0; iExe < $m.numTopDriver; ++iExe){
//			$m.arrTopDriver[iExe] = arrDriver[iExe0];
//			pDriver = $m.arrTopDriver[iExe];
//
//			for(++iExe0; iExe0 < numDriver; ++iExe0){
//				if(arrDriver[iExe0]->Type != pDriver->Type){
//					break;
//				} else{
//					pDriver->Next = arrDriver[iExe0];
//					pDriver = pDriver->Next;
//				}
//			}
//		}
//
//		//给向量分量类型事件创建起始事件
//
//		for(USINT iExe = 0; iExe < $m.numTopDriver; ++iExe){
//			if(pDriver->Type > CPtcDriver::TYPE_SUBPROP){
//				pDriver = CPtcDriverFactory::CreateEmpty($m.arrTopDriver[iExe]->Type);
//				pDriver->Next = $m.arrTopDriver[iExe];
//				$m.arrTopDriver[iExe] = pDriver;
//			}
//		}
//
//		//创建粒子缓存区
//
//		$m.InitParticleBuffer();
//	}
//	_secr $VOID InitParticleBuffer(){
//		//根据事件类型给粒子分配属性
//
//		$m.PtcFormat = PFB_COORD;
//		$m.PtcFormat |= PFB_AGE;
//
//		CPtcDriver *pDriver;
//		for(USINT iExe = 0; iExe < $m.numTopDriver; ++iExe){
//			pDriver = $m.arrTopDriver[iExe];
//			while(pDriver->Next){
//				pDriver = pDriver->Next;
//				if(pDriver->bFade) break;
//			}
//
//			switch(pDriver->Type){
//				case CPtcDriver::TYPE_SIZE:{
//					$m.PtcFormat |= PFB_SIZE;
//					/*if(pDriver->bFade)
//						$m.PtcFormat |= PFB_SIZESTEP;*/
//				} break;
//				case CPtcDriver::TYPE_MaxTIME:{
//					$m.PtcFormat |= PFB_MAXAGE;
//				} break;
//				case CPtcDriver::TYPE_TIME:{
//					$m.PtcFormat |= PFB_AGE;
//				} break;
//				case CPtcDriver::TYPE_TEXCOORD:
//				case CPtcDriver::TYPE_TEXCOORD_U:
//				case CPtcDriver::TYPE_TEXCOORD_V:{
//					$m.PtcFormat |= PFB_TEXCOORD;
//				} break;
//				case CPtcDriver::TYPE_COLOR:
//				case CPtcDriver::TYPE_COLOR_R:
//				case CPtcDriver::TYPE_COLOR_G:
//				case CPtcDriver::TYPE_COLOR_B:
//				case CPtcDriver::TYPE_COLOR_A:{
//					$m.PtcFormat |= PFB_COLOR32;
//					$m.PtcFormat |= PFB_COLOR;
//					/*if(pDriver->bFade)
//						$m.PtcFormat |= PFB_COLORSTEP;*/
//				} break;
//				case CPtcDriver::TYPE_VELOC:
//				case CPtcDriver::TYPE_VELOC_X:
//				case CPtcDriver::TYPE_VELOC_Y:
//				case CPtcDriver::TYPE_VELOC_Z:{
//					$m.PtcFormat |= PFB_VELOC;
//					if(pDriver->bFade)
//						$m.PtcFormat |= PFB_ACCEL;
//				} break;
//				case CPtcDriver::TYPE_ACCEL:
//				case CPtcDriver::TYPE_ACCEL_X:
//				case CPtcDriver::TYPE_ACCEL_Y:
//				case CPtcDriver::TYPE_ACCEL_Z:{
//					$m.PtcFormat |= PFB_ACCEL;
//					/*if(pDriver->bFade)
//						$m.PtcFormat |= PFB_JERK;*/
//				} break;
//			}
//		}
//
//		//根据粒子属性分配内存空间
//
//		$m.PtcView.Initialize($m.PtcFormat);
//		$m.PtcByteCnt = $m.PtcView.cbPrtcl;
//		$m.PtcBuff = new BYTET[$m.PtcByteCnt * $m.PtcMaxCnt]{};
//		$m.PtcView.SetHandle($m.PtcBuff);
//		$m.PtcView.SetTotal(&$m.PtcCount);
//	}
//	_secr $VOID SetupParticle(){
//		/*CPtcView *pPtcView = $m.GetParticleView($m.PtcIndex);
//		for(USINT iExe = 0; iExe < $m.PtcMaxCnt; ++iExe){
//			if(!pPtcView->IsActive()) break;
//
//			$m.PtcIndex++;
//			pPtcView->OffsetHandle(1);
//
//			if($m.PtcIndex >= $m.PtcMaxCnt){
//				$m.PtcIndex = 0;
//				pPtcView = $m.GetParticleView(0);
//			}
//		}
//
//		Sto_Bytes(pPtcView->rPrtcl, 0, $m.PtcByteCnt);
//		RandVec3(pPtcView->Coord(), VECTOR3(0.f, 0.f, 0.f), $m.EmitRange);
//		$m.PtcCount++;*/
//	}
//	_open $VOID Update(SPFPN TimeSpan){
//		//更新粒子
//
//		CPtcView *pPtcView = $m.GetParticleView(0);
//		for(USINT iExe = 0; iExe < $m.PtcMaxCnt; ++iExe){
//			if(pPtcView->IsActive()){
//				for(USINT M = 0; M < $m.numTopDriver; ++M)
//					$m.arrTopDriver[M]->UpdateParticle(*pPtcView, TimeSpan);
//				pPtcView->OffsetHandle(1);
//			}
//		}
//
//		//创建新粒子
//
//		if($m.PtcCount < $m.PtcMaxCnt){
//			SPFPN EmitRate = Rand_Real($m.EmitRates);
//			$m.EmitValue += EmitRate * TimeSpan;
//
//			if($m.EmitValue >= 1.f){
//				USINT EmitValue = (USINT)$m.EmitValue;
//				$m.EmitValue -= (SPFPN)EmitValue;
//
//				if((EmitValue + $m.PtcCount) > $m.PtcMaxCnt)
//					EmitValue = $m.PtcMaxCnt - $m.PtcCount;
//
//				for(USINT iExe = 0; iExe < EmitValue; ++iExe)
//					$m.SetupParticle();
//			}
//		}
//	}
//	_open CPtcView* GetParticleView(USINT Index){
//		/*$m.PtcView.rPrtcl = $m.PtcBuff;
//		$m.PtcView.rPrtcl += $m.PtcByteCnt * Index;
//		return &$m.PtcView;*/
//	}
//};
//
////粒子系统
//class CPtcSystem{
//	_open VECTOR3 Position; //位置
//	///混合模式
//	_open DWORD fBlendOP;   //混合操作
//	_open DWORD fSrcBlend;  //源混合模式
//	_open DWORD fDstBlend;  //目标混合模式
//	///粒子组
//	_open USINT numSubset;       //子集数量
//	_open CPtcGroup *arrSubset; //子集数组
//	///顶点缓存区
//	_open USINT cbVertex;    //顶点字节数
//	_open USINT cbVtxBuff;   //缓存区大小
//	_open DWORD VtxFormat;  //顶点格式
//	_open ID3DResource *lpVtxBuff; //顶点缓存区
//
//	_open ~CPtcSystem(){
//		$m.Finalize();
//	}
//	_open CPtcSystem(){
//		Var_Zero(this);
//	}
//	_open CPtcSystem(ID3DDevice* lpD3dDevice, WCHAR* ScriptPath){
//		$m.Initialize(lpD3dDevice, ScriptPath);
//	}
//
//	_open $VOID Finalize(){
//		SAFE_DELETEA($m.arrSubset);
//		SAFE_RELEASE($m.lpVtxBuff);
//	}
//	_open $VOID Initialize(ID3DDevice* lpD3dDevice, WCHAR* ScriptPath){
//		//CHAR8 *Script;
//		//JSONNODE pDomRoot, DomNode;
//
//		////读取脚本
//
//		//HANDLE hFile = CreateFile(ScriptPath, GENERIC_READ, 0L,
//		//	P_Null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, P_Null);
//
//		//USINT FileSize = GetFileSize(hFile, P_Null);
//		//Script = new CHAR8[FileSize];
//		//ReadFile(hFile, Script, FileSize, P_Null, P_Null);
//
//		//pDomRoot = boost::json::parse(Script);
//		//delete[] Script;
//
//		////解析混合模式
//
//		//DomNode = pDomRoot.get_object()["blendopr"];
//		//CPtcDriver::ParseBlendMode(DomNode.get_string().c_str(), $m.fBlendOP);
//		//DomNode = pDomRoot.get_object()["srcblend"];
//		//CPtcDriver::ParseBlendMode(DomNode.get_string().c_str(), $m.fSrcBlend);
//		//DomNode = pDomRoot.get_object()["dstblend"];
//		//CPtcDriver::ParseBlendMode(DomNode.get_string().c_str(), $m.fDstBlend);
//
//		////解析粒子组
//
//		//DomNode = pDomRoot.get_object()["numsubsets"];
//		//$m.numSubset = (USINT)DomNode.get_uint64();
//		//$m.arrSubset = new CPtcGroup[$m.numSubset];
//
//		//DomNode = pDomRoot.get_object()["subsets"];
//		//for(USINT iExe = 0; iExe < $m.numSubset; ++iExe)
//		//	$m.arrSubset[iExe].Initialize(DomNode.get_array()[iExe]);
//
//		//创建顶点缓存区
//
//		$m.InitVertexBuffer(lpD3dDevice);
//	}
//	_secr $VOID InitVertexBuffer(ID3DDevice* lpD3dDevice){
//		//计算顶点格式
//
//		DWORD PtcFormat = $m.arrSubset[0].PtcFormat;
//		$m.VtxFormat = 0L;
//
//		if(PtcFormat & PFB_COORD)
//			$m.VtxFormat |= VFB_COORD;
//		if(PtcFormat & PFB_SIZE)
//			$m.VtxFormat |= VFB_PSIZE;
//		if(PtcFormat & PFB_COLOR)
//			$m.VtxFormat |= VFB_COLOR;
//		if(PtcFormat & PFB_TEXCOORD)
//			$m.VtxFormat |= VFB_TEXCOORD;
//
//		//创建顶点缓存区
//
//		CVtxView VtxView($m.VtxFormat);
//
//		$m.cbVertex = VtxView.cbVertex;
//		$m.cbVtxBuff = $m.cbVertex * $m.GetParticleMaxCount();
//		$m.lpVtxBuff = CreateUploadBuffer(lpD3dDevice, $m.cbVtxBuff, B_False);
//	}
//	_open $VOID SetPosition(VECTOR3 &Position){
//		$m.Position = Position;
//	}
//	_open $VOID UpdateParticles(SPFPN TimeSpan){
//		for(USINT iExe = 0; iExe < $m.numSubset; ++iExe)
//			$m.arrSubset[iExe].Update(TimeSpan);
//	}
//	_open $VOID UpdateVertexBuffer(){
//		/*BYTET *VtxBytes;
//		$m.lpVtxBuff->Map(0, P_Null, ($VOID**)&VtxBytes);
//
//		for(USINT iExe = 0; iExe < $m.numSubset; ++iExe){
//			CPtcGroup *pPtcGroup = &$m.arrSubset[iExe];
//			if(pPtcGroup->PtcCount == 0) continue;
//
//			CPtcView *pPtcView = pPtcGroup->GetParticleView(0);
//			for(USINT M = 0; M < pPtcGroup->PtcMaxCnt; ++M){
//				if(pPtcView->IsActive()){
//					BStr_Move(VtxBytes, pPtcView->rPrtcl, $m.cbVertex);
//					VtxBytes += $m.cbVertex;
//				}
//				pPtcView->OffsetHandle(1);
//			}
//		}
//
//		$m.lpVtxBuff->Unmap(0, P_Null);*/
//	}
//	_open USINT GetParticleCount(){
//		USINT Count = 0;
//		for(USINT iExe = 0; iExe < $m.numSubset; ++iExe)
//			Count += $m.arrSubset[iExe].PtcCount;
//		return Count;
//	}
//	_open USINT GetParticleMaxCount(){
//		USINT Count = 0;
//		for(USINT iExe = 0; iExe < $m.numSubset; ++iExe)
//			Count += $m.arrSubset[iExe].PtcMaxCnt;
//		return Count;
//	}
//	_open D3D12BUFBLENDDESC GetBlendDesc(){
//		D3D12BUFBLENDDESC BlendDesc = { B_False, B_False,
//			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
//			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
//			D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
//
//		if($m.fBlendOP != 0L){
//			BlendDesc.BlendEnable = B_True;
//			BlendDesc.BlendOp = (D3D12BLENDOPR)$m.fBlendOP;
//			BlendDesc.SrcBlend = (D3D12BLENDMODE)$m.fSrcBlend;
//			BlendDesc.DestBlend = (D3D12BLENDMODE)$m.fDstBlend;
//		}
//
//		return BlendDesc;
//	}
//	_open D3D12VBVIEW GetVertexBufferView(){
//		D3D12VBVIEW VBView;
//		VBView.BufferLocation = $m.lpVtxBuff->GetGPUVirtualAddress();
//		VBView.StrideInBytes = $m.cbVertex;
//		VBView.SizeInBytes = $m.cbVtxBuff;
//		return VBView;
//	}
//};

//----------------------------------------//