//--------------------灯光库/材质库--------------------//

//灯光库
class CLitLibrary{
	open UINT Size;
	open UINT Capacity;
	open LIGHT *Items;
	hide ID3DResource *lpBuffer;

	open ~CLitLibrary(){
		self.Finalize();
	}
	open CLitLibrary(){
		ZeroMemory(this, sizeof(*this));
	}
	open CLitLibrary(ID3DDevice* lpD3DDevice, UINT Capacity){
		self.Initialize(lpD3DDevice, Capacity);
	}

	open VOID Finalize(){
		if(self.lpBuffer){
			self.lpBuffer->Unmap(0, NULL);
			self.lpBuffer->Release();
			self.lpBuffer = NULL;
		}
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, UINT Capacity){
		UINT BuffSize = sizeof(LIGHT) * Capacity;

		self.lpBuffer = CreateUploadBuffer(lpD3DDevice, BuffSize, FALSE);
		self.lpBuffer->Map(0, NULL, (VOID**)&self.Items);

		ZeroMemory(self.Items, BuffSize);

		self.Size = 0;
		self.Capacity = Capacity;
	}
	open VOID UpdateSpotLight(const LIGHT &Data, UINT Index = UINT_MAX){
		if(Index == UINT_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		*Light = Data;

		XVECTOR Direction = Vec3Normalize(PackV3(Data.Direction));
		UnpackV3(Light->Direction, Direction);
	}
	open VOID UpdatePointLight(const LIGHT &Data, UINT Index = UINT_MAX){
		if(Index == UINT_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		Light->Intensity = Data.Intensity;
		Light->Position = Data.Position;
		Light->AttStart = Data.AttStart;
		Light->AttEnd = Data.AttEnd;
	}
	open VOID UpdateDirectionalLight(const LIGHT &Data, UINT Index = UINT_MAX){
		if(Index == UINT_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		Light->Intensity = Data.Intensity;
		Light->AttEnd = FLT_INFINITE;

		XVECTOR Direction = Vec3Normalize(PackV3(Data.Direction));
		UnpackV3(Light->Direction, Direction);
	}
	open VOID TurnOnLight(UINT Index){
		SPFP &Switch = self.Items[Index].AttEnd;
		if(Switch < 0.0f) Switch = -Switch;
	}
	open VOID TurnOffLight(UINT Index){
		SPFP &Switch = self.Items[Index].AttEnd;
		if(Switch > 0.0f) Switch = -Switch;
	}
	open HGPUBUFF GetGPUAddress(){
		return self.lpBuffer->GetGPUVirtualAddress();
	}
};

//材质库
class CMtlLibrary{
	struct MATEX{
		UINT Ident;
		MATERIAL Base;
	};
	struct TEXTURE{
		UINT Ident;
		ID3DResource *lpBuffer;
	};

	open UINT MtlCount;
	open UINT TexCount;
	open UINT *arrMtlID;
	open UINT *arrTexID;
	open ID3DViewHeap *lpSRVHeap;
	hide ID3DResource *lpMtlBuff;
	hide ID3DResource **arrTexture;

	open ~CMtlLibrary(){
		self.Finalize();
	}
	open CMtlLibrary(UINT MtlCount = 64, UINT TexCount = 64){
		self.Initialize(MtlCount, TexCount);
	}

	open VOID Finalize(){
		for(UINT i = 0; i < self.TexCount; ++i)
			RELEASE_REF(self.arrTexture[i]);

		DELETE_ARR(self.arrMtlID);
		DELETE_ARR(self.arrTexID);
		DELETE_ARR(self.arrTexture);

		RELEASE_REF(self.lpSRVHeap);
		RELEASE_REF(self.lpMtlBuff);

		ZeroMemory(this, sizeof(*this));
	}
	hide VOID Initialize(UINT MtlCount, UINT TexCount){
		ZeroMemory(this, sizeof(*this));

		CSeqList *pMtlList = new CSeqList(MtlCount, sizeof(MATEX));
		CSeqList *pTexList = new CSeqList(TexCount, sizeof(TEXTURE));

		self.arrMtlID = (UINT*)pMtlList;
		self.arrTexID = (UINT*)pTexList;
	}
	open VOID AddTexture(TCHR* Name, ID3DResource* lpBuffer){
		TEXTURE Texture;
		CSeqList *pTexList;

		Texture.Ident = StrHash4(Name);
		Texture.lpBuffer = lpBuffer;

		pTexList = (CSeqList*)self.arrTexID;
		pTexList->Insert(&Texture);
	}
	open VOID AddTexturesFromDirectory(ID3DDevice* lpD3DDevice, TCHR* Directory, DXTKUploader &Uploader){
		HANDLE hSeeker;
		FILEINFO FileInfo;
		TCHR FilePath[256];

		//搜索第一个DDS文件

		StrCpy(FilePath, Directory);
		StrCat(FilePath, TXT("*.dds"));

		hSeeker = FindFirstFile(FilePath, &FileInfo);
		if(hSeeker == INVALID_HANDLE_VALUE) goto done;

		//处理DDS文件

		BOOL bHasNext = FALSE;
		UPTR DirLength = StrLen(Directory);
		CSeqList *pTexList = (CSeqList*)self.arrTexID;
		TEXTURE Texture;

		do{
			FilePath[DirLength] = TXT('\0');
			StrCat(FilePath, FileInfo.cFileName);

			dx::CreateDDSTextureFromFile(lpD3DDevice, Uploader, FilePath, &Texture.lpBuffer);
			Texture.Ident = StrHash4(FileInfo.cFileName);
			pTexList->Insert(&Texture);

			bHasNext = FindNextFile(hSeeker, &FileInfo);
		} while(bHasNext);
done:
		FindClose(hSeeker);
	}
	open VOID AddMaterialsFromFile(TCHR* FilePath){
		//定位数据

		HANDLE hFile = FindFileChunk(FilePath, ASSET_MATERIAL);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		//读取数据

		UINT MtlCount;
		CSeqList *pMtlList = (CSeqList*)self.arrMtlID;

		ReadFile(hFile, &MtlCount, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < MtlCount; ++i){
			VOID *pMtrl = pMtlList->New();
			ReadFile(hFile, pMtrl, sizeof(MATEX), NULL, NULL);
		}

		//关闭文件
done:
		CloseHandle(hFile);
	}
	hide VOID CreateTextureViews(ID3DDevice* lpD3DDevice){
		//创建视图堆

		D3DViewHeapDesc HeapDesc;

		HeapDesc.NodeMask = 0x0;
		HeapDesc.NumDescriptors = self.TexCount;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		lpD3DDevice->CreateDescriptorHeap(&HeapDesc, COM_ARGS(&self.lpSRVHeap));

		//创建视图

		HD3DVIEW hTexView;
		D3DResDesc TexDesc;
		D3DSrvDesc ViewDesc;

		ViewDesc.Texture2D.PlaneSlice = 0;
		ViewDesc.Texture2D.MostDetailedMip = 0;
		ViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		ViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		hTexView = self.lpSRVHeap->GetCPUDescriptorHandleForHeapStart();
		for(UINT i = 0; i < self.TexCount; ++i){
			TexDesc = (D3DResDesc)self.arrTexture[i]->GetDesc();
			ViewDesc.Format = TexDesc.Format;
			ViewDesc.Texture2D.MipLevels = TexDesc.MipLevels;
			lpD3DDevice->CreateShaderResourceView(self.arrTexture[i], &ViewDesc, hTexView);
			hTexView.Offset(1, GE3D::cbCSUView);
		}
	}
	open VOID SubmitResources(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader){
		CSeqList *pMtlList = (CSeqList*)self.arrMtlID;
		CSeqList *pTexList = (CSeqList*)self.arrTexID;

		//获取纹理和材质数量

		self.MtlCount = pMtlList->Size;
		self.TexCount = pTexList->Size;
		self.arrMtlID = new UINT[self.MtlCount];
		self.arrTexID = new UINT[self.TexCount];
		self.arrTexture = new ID3DResource*[self.TexCount];

		//保存纹理

		for(UINT i = 0; i < self.TexCount; ++i){
			TEXTURE *pTexture = (TEXTURE*)pTexList->Get(i);
			self.arrTexID[i] = pTexture->Ident;
			self.arrTexture[i] = pTexture->lpBuffer;
		}

		//上传材质

		if(self.MtlCount > 0){
			MATERIAL *arrMtrl = new MATERIAL[self.MtlCount];

			for(UINT i = 0; i < self.MtlCount; ++i){
				MATEX *pMtrl = (MATEX*)pMtlList->Get(i);
				self.arrMtlID[i] = pMtrl->Ident;
				arrMtrl[i] = pMtrl->Base;
			}

			dx::CreateStaticBuffer(lpD3DDevice, Uploader, arrMtrl, sizeof(MATERIAL) * self.MtlCount,
				1, D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpMtlBuff);
		}

		//创建纹理视图

		if(self.TexCount > 0)
			self.CreateTextureViews(lpD3DDevice);

		//释放数据

		delete pMtlList;
		delete pTexList;
	}
	open HGPUVIEW GetTextureView(UINT Index){
		HGPUVIEW hView;
		hView = self.lpSRVHeap->GetGPUDescriptorHandleForHeapStart();
		hView.Offset(Index, GE3D::cbCSUView);
		return hView;
	}
	open HGPUBUFF GetMaterialGPUAddress(UINT Index){
		HGPUBUFF Handle = self.lpMtlBuff->GetGPUVirtualAddress();
		Handle += sizeof(MATERIAL) * Index;
		return Handle;
	}
};

//----------------------------------------//


//--------------------模型--------------------//

//网格
class CMesh{
	open CHAR *Name;
	open CMesh *Next;
	open UINT MtlCount;
	open UINT MapCount;
	open UINT cbVtxBuff;
	open UINT cbIdxBuff;
	open SUBMESH *arrSubset;
	open MESHMAP *arrMapping;
	open ID3DResource *lpVtxBuff;
	open ID3DResource *lpIdxBuff;
	open DXGIFormat IdxFormat;
	open VERTFORMAT VtxFormat;

	open ~CMesh(){
		self.Finalize();
	}
	open CMesh(){
		ZeroMemory(this, sizeof(*this));
	}
	open CMesh(ID3DDevice* lpD3DDevice, HANDLE hFile, DXTKUploader &Uploader){
		self.Initialize(lpD3DDevice, hFile, Uploader);
	}

	open VOID Initialize(ID3DDevice* lpD3DDevice, HANDLE hFile, DXTKUploader &Uploader){
		UINT StrSize, BuffSize;
		UINT MtlCount, MapCount;
		UINT VtxCount, IdxCount;
		DWRD VtxFormat, BuffAttr;
		BYTE *VtxBuff, *IdxBuff;

		//读取名称

		ReadFile(hFile, &StrSize, sizeof(UINT), NULL, NULL);
		self.Name = new CHAR[StrSize + 1];
		ReadFile(hFile, self.Name, StrSize, NULL, NULL);
		self.Name[StrSize] = '\0';

		//读取基础信息

		ReadFile(hFile, &IdxCount, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &VtxCount, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &MtlCount, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &MapCount, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &VtxFormat, sizeof(DWRD), NULL, NULL);

		self.MtlCount = MtlCount;
		self.MapCount = MapCount;

		self.VtxFormat = VtxFormat;
		self.IdxFormat = (VtxCount > UINT16_MAX) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

		self.cbVtxBuff = self.GetVertexByteCount() * VtxCount;
		self.cbIdxBuff = self.GetIndexByteCount() * IdxCount;

		//读取顶点和索引

		VtxBuff = new BYTE[self.cbVtxBuff];
		IdxBuff = new BYTE[self.cbIdxBuff];

		ReadFile(hFile, VtxBuff, self.cbVtxBuff, NULL, NULL);
		ReadFile(hFile, IdxBuff, self.cbIdxBuff, NULL, NULL);

		//读取子集

		if(MtlCount == 0){
			UPTR Temp = (UPTR)IdxCount;
			self.arrSubset = (SUBMESH*)Temp;
		} else{
			self.arrSubset = new SUBMESH[MtlCount];
			BuffSize = sizeof(SUBMESH) * MtlCount;
			ReadFile(hFile, self.arrSubset, BuffSize, NULL, NULL);
		}

		//读取纹理信息

		if(MapCount > 0){
			self.arrMapping = new MESHMAP[MapCount];
			BuffSize = sizeof(MESHMAP) * MapCount;
			ReadFile(hFile, self.arrMapping, BuffSize, NULL, NULL);
		}

		//创建缓存区

		BuffAttr = (VtxFormat & 0x80000000) ?
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

		dx::CreateStaticBuffer(lpD3DDevice, Uploader, VtxBuff, self.cbVtxBuff, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpVtxBuff, (D3DResFlag)BuffAttr);
		dx::CreateStaticBuffer(lpD3DDevice, Uploader, IdxBuff, self.cbIdxBuff, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpIdxBuff);
	}
	open VOID Finalize(){
		DELETE_ARR(self.Name);
		DELETE_ARR(self.arrSubset);
		DELETE_ARR(self.arrMapping);

		RELEASE_REF(self.lpVtxBuff);
		RELEASE_REF(self.lpIdxBuff);

		ZeroMemory(this, sizeof(*this));
	}
	open UINT GetIndexCount(){
		return self.cbIdxBuff / self.GetIndexByteCount();
	}
	open UINT GetVertexCount(){
		return self.cbVtxBuff / self.GetVertexByteCount();
	}
	open UINT GetIndexByteCount(){
		return (self.IdxFormat == DXGI_FORMAT_R32_UINT) ? 4 : 2;
	}
	open UINT GetVertexByteCount(){
		CVertView VertDesc(self.VtxFormat);
		return VertDesc.ByteCount;
	}
	open D3DIdxBuffView GetIndexBufferView(){
		D3DIdxBuffView IBView;
		IBView.BufferLocation = self.lpIdxBuff->GetGPUVirtualAddress();
		IBView.SizeInBytes = self.cbIdxBuff;
		IBView.Format = self.IdxFormat;
		return IBView;
	}
	open D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuff->GetGPUVirtualAddress();
		VBView.StrideInBytes = self.GetVertexByteCount();
		VBView.SizeInBytes = self.cbVtxBuff;
		return VBView;
	}
};

//模型
class CModel{
	open UINT MeshCount;
	open CMesh *FirstMesh;

	open ~CModel(){
		self.Finalize();
	}
	open CModel(){
		ZeroMemory(this, sizeof(*this));
	}
	open CModel(ID3DDevice* lpD3DDevice, TCHR* FilePath, DXTKUploader &Uploader){
		self.Initialize(lpD3DDevice, FilePath, Uploader);
	}

	open VOID Finalize(){
		CMesh *pMesh, *NextMesh;

		pMesh = self.FirstMesh;
		self.FirstMesh = NULL;

		while(pMesh){
			NextMesh = pMesh->Next;
			delete pMesh;
			pMesh = NextMesh;
		}
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, TCHR* FilePath, DXTKUploader &Uploader){
		HANDLE hFile = FindFileChunk(FilePath, ASSET_MESH);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		ReadFile(hFile, &self.MeshCount, sizeof(UINT), NULL, NULL);
		self.FirstMesh = new CMesh(lpD3DDevice, hFile, Uploader);

		CMesh *pMesh = self.FirstMesh;
		for(UINT i = 1; i < self.MeshCount; ++i){
			pMesh->Next = new CMesh(lpD3DDevice, hFile, Uploader);
			pMesh = pMesh->Next;
		}
done:
		CloseHandle(hFile);
	}
	open VOID LinkMaterials(CMtlLibrary &Assets){
		for(CMesh *pMesh = self.FirstMesh; pMesh; pMesh = pMesh->Next){
			for(UINT i = 0; i < pMesh->MtlCount; ++i){
				SUBMESH *pSubmesh = &pMesh->arrSubset[i];
				for(UINT j = 0; j < Assets.MtlCount; ++j){
					if(pSubmesh->MtlIdent == Assets.arrMtlID[j]){
						pSubmesh->MtlIdent = j * sizeof(MATERIAL);
						break;
					}
				}
			}

			for(UINT i = 0; i < pMesh->MapCount; ++i){
				MESHMAP *pMapping = &pMesh->arrMapping[i];
				for(UINT j = 0; j < Assets.TexCount; ++j){
					if(pMapping->nDiffuse == Assets.arrTexID[j]){
						pMapping->nDiffuse = j;
						break;
					} else if(pMapping->nBump == Assets.arrTexID[j]){
						pMapping->nBump = j;
						break;
					}
				}
			}
		}
	}
	open VOID LinkTextureByName(CHAR* TexName, UINT Mapping){
		UINT TexIdent = StrHash4A(TexName);
		for(CMesh *pMesh = self.FirstMesh; pMesh; pMesh = pMesh->Next){
			for(UINT i = 0; i < pMesh->MapCount; ++i){
				MESHMAP *pMapping = &pMesh->arrMapping[i];
				if(pMapping->nDiffuse == TexIdent)
					pMapping->nDiffuse = Mapping;
				else if(pMapping->nBump == TexIdent)
					pMapping->nBump = Mapping;
			}
		}
	}
};

//----------------------------------------//


//--------------------骨架--------------------//

//骨架
class CSkeleton{
	open UINT numBone;
	open BONE *arrBone;
	hide MATRIX *arrTrans;
	open ID3DResource *lpTMBuffer;

	open ~CSkeleton(){
		self.Finalize();
	}
	open CSkeleton(){
		ZeroMemory(this, sizeof(*this));
	}
	open CSkeleton(ID3DDevice* lpD3DDevice, TCHR* FilePath){
		self.Initialize(lpD3DDevice, FilePath);
	}

	open VOID Initialize(ID3DDevice* lpD3DDevice, TCHR* FilePath){
		HANDLE hFile = FindFileChunk(FilePath, ASSET_BONE);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		ReadFile(hFile, &self.numBone, sizeof(UINT), NULL, NULL);
		if(self.numBone == 0) return;

		self.arrBone = new BONE[self.numBone];
		self.lpTMBuffer = CreateUploadBuffer(lpD3DDevice, sizeof(MATRIX) * self.numBone, TRUE);
		self.lpTMBuffer->Map(0, NULL, (VOID**)&self.arrTrans);

		for(UINT i = 0; i < self.numBone; ++i){
			UINT StrSize;
			BONE *pBone = &self.arrBone[i];

			ReadFile(hFile, &StrSize, sizeof(UINT), NULL, NULL);
			ReadFile(hFile, pBone->Name, sizeof(CHAR) * StrSize, NULL, NULL);
			ReadFile(hFile, &pBone->nParent, sizeof(UINT), NULL, NULL);
			ReadFile(hFile, &pBone->matOffset, sizeof(MATRIX), NULL, NULL);
			ReadFile(hFile, &pBone->matInitial, sizeof(MATRIX), NULL, NULL);

			pBone->Name[StrSize] = '\0';
			pBone->matCombine = MatIdentity();
			pBone->matTrans = pBone->matInitial;
		}
done:
		CloseHandle(hFile);
	}
	open VOID Finalize(){		
		if(self.lpTMBuffer){
			self.lpTMBuffer->Unmap(0, NULL);
			RESET_REF(self.lpTMBuffer);
			RESET_ARR(self.arrBone);
		}
	}
	open VOID Update(){
		XMATRIX matFinal;
		BONE *pBone, *PrevBone;

		pBone = &self.arrBone[0];
		pBone->matCombine = pBone->matTrans;
		matFinal = MatTranspose(pBone->matCombine);
		UnpackMat(self.arrTrans[0], matFinal);

		for(UINT i = 1; i < self.numBone; ++i){
			pBone = &self.arrBone[i];
			PrevBone = &self.arrBone[pBone->nParent];
			pBone->matCombine = pBone->matTrans * PrevBone->matCombine;
			matFinal = pBone->matOffset * pBone->matCombine;
			UnpackMat(self.arrTrans[i], MatTranspose(matFinal));
		}
	}
	open VOID Reset(){
		BONE *pBone = &self.arrBone[0];
		for(UINT i = 1; i < self.numBone; ++i){
			pBone->matTrans = pBone->matInitial;
			pBone++;
		}
	}
};

//----------------------------------------//


//--------------------变形器--------------------//

//变形PSO
struct CMorphPso{
	ID3DRootSig *lpRootSig;
	ID3DPipeState *lpPipeState;

	static VOID Destroy(CMorphPso &Pso){
		RESET_REF(Pso.lpRootSig);
		RESET_REF(Pso.lpPipeState);
	}
	static VOID Create(ID3DDevice* lpD3DDevice, CMorphPso &Pso){
		ID3DBlob *lpShadeCode;
		ID3DBlob *lpSerialize;
		D3DRootParam arrRootParam[4];
		D3DRootSigDesc RootSigDesc;
		D3DComputePipeStateDesc PSODesc;

		//编译着色器代码

		lpShadeCode = CompileShaderFromFile(
			L"./source/gs_morph.hlsl", NULL, "VertexMorph", "cs_5_1");

		//创建根签名

		arrRootParam[0].InitAsConstants(4, 0);
		arrRootParam[1].InitAsShaderResourceView(0);
		arrRootParam[2].InitAsShaderResourceView(1);
		arrRootParam[3].InitAsUnorderedAccessView(0);

		RootSigDesc.Init(ARR_LEN(arrRootParam), arrRootParam, 0, NULL,
			D3D12_ROOT_SIGNATURE_FLAG_NONE);

		D3D12SerializeRootSignature(&RootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, NULL);

		lpD3DDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), COM_ARGS(&Pso.lpRootSig));

		lpSerialize->Release();

		//创建PSO

		ZeroMemory(&PSODesc, sizeof(D3DComputePipeStateDesc));
		PSODesc.pRootSignature = Pso.lpRootSig;
		PSODesc.CS = { lpShadeCode->GetBufferPointer(), lpShadeCode->GetBufferSize() };
		PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		lpD3DDevice->CreateComputePipelineState(&PSODesc, COM_ARGS(&Pso.lpPipeState));
		lpShadeCode->Release();
	}
};

//变形器
struct CMorpher{
	UINT Ident;
	UINT cbMesh;
	UINT numMesh;
	ID3DResource *arrMesh;

	~CMorpher(){
		self.Finalize();
	}
	CMorpher(){
		ZeroMemory(this, sizeof(*this));
	}
	CMorpher(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, TCHR *FilePath){
		self.Initialize(lpD3DDevice, Uploader, FilePath);
	}

	VOID Finalize(){
		RESET_REF(self.arrMesh);
	}
	VOID Initialize(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, TCHR *FilePath){
		//定位数据

		HANDLE hFile;
		BOOL bSucceed;
		UINT ChunkSize;
		DWRD ChunkType;

		hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		while(TRUE){
			bSucceed = ReadFile(hFile, &ChunkType, sizeof(UINT), NULL, NULL);
			if(!bSucceed) goto done;

			ReadFile(hFile, &ChunkSize, sizeof(UINT), NULL, NULL);
			if(ChunkType == ASSET_BONE) break;
			else SetFilePointer(hFile, ChunkSize, NULL, FILE_CURRENT);
		}

		//读取数据

		BYTE *Buffer;
		UINT cbBuffer;

		ReadFile(hFile, &self.Ident, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &self.cbMesh, sizeof(UINT), NULL, NULL);
		ReadFile(hFile, &self.numMesh, sizeof(UINT), NULL, NULL);

		cbBuffer = self.cbMesh * self.numMesh;
		Buffer = new BYTE[cbBuffer];
		ReadFile(hFile, Buffer, cbBuffer, NULL, NULL);

		dx::CreateStaticBuffer(lpD3DDevice, Uploader, Buffer, cbBuffer, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.arrMesh);
done:
		CloseHandle(hFile);
	}
	VOID UpdateMesh(ID3DGfxCmdList* lpCmdList, CMorphPso &Pso, UINT Targets[2], SPFP Lerp, CMesh* Dest){
		struct SETTING{
			SPFP MorphLerp;
			BOOL bWithNormal;
			UINT cbVertex;
			UINT VertCount;
		};

		HGPUBUFF hSrcBuff = self.arrMesh->GetGPUVirtualAddress();
		HGPUBUFF hSrcBuff0 = hSrcBuff + Targets[0] * self.cbMesh;
		HGPUBUFF hSrcBuff1 = hSrcBuff + Targets[1] * self.cbMesh;
		HGPUBUFF hDestBuff = Dest->lpVtxBuff->GetGPUVirtualAddress();

		SETTING Setting;
		Setting.MorphLerp = Lerp;
		Setting.bWithNormal = Dest->VtxFormat & VERT_NORMAL;
		Setting.cbVertex = Dest->GetVertexByteCount();
		Setting.VertCount = Dest->cbVtxBuff / Setting.cbVertex;
		Setting.cbVertex /= 4;

		lpCmdList->SetPipelineState(Pso.lpPipeState);
		lpCmdList->SetComputeRootSignature(Pso.lpRootSig);
		lpCmdList->SetComputeRoot32BitConstants(0, 4, &Setting, 0);
		lpCmdList->SetComputeRootShaderResourceView(1, hSrcBuff0);
		lpCmdList->SetComputeRootShaderResourceView(2, hSrcBuff1);
		lpCmdList->SetComputeRootUnorderedAccessView(3, hDestBuff);

		lpCmdList->Dispatch(MAX(Setting.VertCount / 32, 1), 1, 1);
	}
};

//----------------------------------------//


//--------------------天空--------------------//

//天空穹
struct CSkyDome{
	UINT cbVtxBuff;
	UINT cbIdxBuff;
	ID3DResource *lpVtxBuff;
	ID3DResource *lpIdxBuff;
	ID3DResource *lpTexture;
	ID3DViewHeap *lpSRVHeap;
	ID3DPipeState *lpPipeState;
	ID3DRootSig *lpRootSig;

	~CSkyDome(){
		self.Finalize();
	}
	CSkyDome(){
		ZeroMemory(this, sizeof(*this));
	}
	CSkyDome(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, WCHR* TexPath, SPFP Diameter, D3DGfxPipeStateDesc &PSODesc){
		self.Initialize(lpD3DDevice, Uploader, TexPath, Diameter, PSODesc);
	}

	VOID Initialize(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, WCHR* TexPath, SPFP Diameter, D3DGfxPipeStateDesc &PSODesc){
		self.CreateMesh(lpD3DDevice, Uploader, Diameter);
		self.CreateTexture(lpD3DDevice, Uploader, TexPath);
		self.CreatePSO(lpD3DDevice, &PSODesc);
	}
	VOID CreatePSO(ID3DDevice* lpD3DDevice, D3DGfxPipeStateDesc* pPSODesc){
		ID3DBlob *lpVSCode;
		ID3DBlob *lpPSCode;
		ID3DBlob *lpSerialize;
		D3DViewRange ViewRange;
		D3DRootParam arrParam[2];
		D3DInputElemDesc InputDesc;
		D3DRootSigDesc RootSigDesc;
		D3DStaticSamplerDesc SamplerDesc;

		//编译着色器代码

		lpVSCode = CompileShaderFromFile(
			L"./source/gs_sky.hlsl", NULL, "VertexShade", "vs_5_1");
		lpPSCode = CompileShaderFromFile(
			L"./source/gs_sky.hlsl", NULL, "PixelShade", "ps_5_1");

		//创建根签名

		SamplerDesc.Init(0, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 8);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		arrParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		arrParam[1].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL);

		RootSigDesc.Init(2, arrParam, 1, &SamplerDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&RootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, NULL);

		lpD3DDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), COM_ARGS(&self.lpRootSig));

		lpSerialize->Release();

		//创建PSO

		InputDesc.InputSlot = 0;
		InputDesc.SemanticIndex = 0;
		InputDesc.AlignedByteOffset = 0;
		InputDesc.InstanceDataStepRate = 0;
		InputDesc.SemanticName = "POSITION";
		InputDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		InputDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		pPSODesc->VS = { lpVSCode->GetBufferPointer(), lpVSCode->GetBufferSize() };
		pPSODesc->PS = { lpPSCode->GetBufferPointer(), lpPSCode->GetBufferSize() };
		pPSODesc->InputLayout = { &InputDesc, 1 };
		pPSODesc->pRootSignature = self.lpRootSig;
		pPSODesc->RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pPSODesc->DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		lpD3DDevice->CreateGraphicsPipelineState(pPSODesc, COM_ARGS(&self.lpPipeState));

		lpVSCode->Release();
		lpPSCode->Release();
	}
	VOID CreateMesh(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, SPFP Diameter){
		USRT *arrIndex;
		VECTOR3 *arrVertex;
		UINT VtxCount, IdxCount;
		STLVector<USRT> IndexList;
		STLVector<DXTKGeomPrim::VertexType> VertList;

		DXTKGeomPrim::CreateSphere(VertList, IndexList, Diameter, 8, FALSE);

		VtxCount = (UINT)VertList.size();
		IdxCount = (UINT)IndexList.size();

		arrVertex = new VECTOR3[VtxCount];
		arrIndex = new USRT[IdxCount];

		for(UINT i = 0; i < VtxCount; ++i)
			arrVertex[i] = VertList[i].position;
		for(UINT i = 0; i < IdxCount; ++i)
			arrIndex[i] = IndexList[i];

		self.cbVtxBuff = sizeof(VECTOR3) * VtxCount;
		self.cbIdxBuff = sizeof(USRT) * VtxCount;

		dx::CreateStaticBuffer(lpD3DDevice, Uploader, arrVertex, self.cbVtxBuff, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpVtxBuff);
		dx::CreateStaticBuffer(lpD3DDevice, Uploader, arrIndex, self.cbIdxBuff, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpIdxBuff);
	}
	VOID CreateTexture(ID3DDevice* lpD3DDevice, DXTKUploader &Uploader, WCHR* FilePath){
		HD3DVIEW hTexView;
		D3DResDesc tTexDesc;
		D3DSrvDesc tViewDesc;
		D3DViewHeapDesc tHeapDesc;

		//创建纹理

		dx::CreateDDSTextureFromFile(lpD3DDevice, Uploader, FilePath, &self.lpTexture);

		//创建视图堆

		tHeapDesc.NodeMask = 0;
		tHeapDesc.NumDescriptors = 1;
		tHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		tHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		lpD3DDevice->CreateDescriptorHeap(&tHeapDesc, COM_ARGS(&self.lpSRVHeap));

		//创建视图

		hTexView = self.lpSRVHeap->GetCPUDescriptorHandleForHeapStart();
		tTexDesc = *(D3DResDesc*)&self.lpTexture->GetDesc();

		tViewDesc.TextureCube.MostDetailedMip = 0;
		tViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		tViewDesc.TextureCube.MipLevels = tTexDesc.MipLevels;
		tViewDesc.Format = tTexDesc.Format;
		tViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		tViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		lpD3DDevice->CreateShaderResourceView(self.lpTexture, &tViewDesc, hTexView);
	}
	VOID Finalize(){
		if(!self.lpVtxBuff) return;

		RELEASE_REF(self.lpRootSig);
		RELEASE_REF(self.lpPipeState);
		RELEASE_REF(self.lpSRVHeap);
		RELEASE_REF(self.lpTexture);
		RELEASE_REF(self.lpVtxBuff);
		RELEASE_REF(self.lpIdxBuff);

		ZeroMemory(this, sizeof(*this));
	}
	HGPUVIEW GetTextureView(){
		return (HGPUVIEW)self.lpSRVHeap->GetGPUDescriptorHandleForHeapStart();
	}
	D3DIdxBuffView GetIndexBufferView(){
		D3DIdxBuffView IBView;
		IBView.BufferLocation = self.lpIdxBuff->GetGPUVirtualAddress();
		IBView.SizeInBytes = self.cbIdxBuff;
		IBView.Format = DXGI_FORMAT_R16_UINT;
		return IBView;
	}
	D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuff->GetGPUVirtualAddress();
		VBView.StrideInBytes = sizeof(VECTOR3);
		VBView.SizeInBytes = self.cbVtxBuff;
		return VBView;
	}
};

//----------------------------------------//


//--------------------粒子系统--------------------//

//粒子属性
enum PTCLATTR{
	///渲染属性
	PARTICLE_COORD = 0x0001,    //坐标
	PARTICLE_SIZE = 0x0002,     //尺寸
	PARTICLE_COLOR32 = 0x0004,  //32b颜色
	PARTICLE_TEXCOORD = 0x0008, //纹理坐标
	///非渲染属性
	PARTICLE_TIMING = 0x0010,   //计时
	PARTICLE_LIFETIME = 0x0020, //寿命
	PARTICLE_DSIZE = 0x0040,    //尺寸差
	PARTICLE_COLOR = 0x0080,    //颜色
	PARTICLE_DCOLOR = 0x0100,   //颜色差
	PARTICLE_VELOC = 0x0200,    //速度
	PARTICLE_ACCEL = 0x0400,    //加速度
	PARTICLE_DACCEL = 0x0800,   //加速度差
};

//粒子视图
struct CPtcView{
	BYTE *Address;
	UINT *pAccount;
	UINT ByteCount;
	UCHR hState;
	UCHR hCoord;
	UCHR hTexCoord;
	UCHR hColor32;
	UCHR hTiming;
	UCHR hLifetime;
	UCHR hSize;
	UCHR hDifSize;
	UCHR hColor;
	UCHR hDifColor;
	UCHR hVeloc;
	UCHR hAccel;
	UCHR hDifAccel;

	VOID Initialize(DWRD Format){
		ZeroMemory(this, sizeof(*this));

		if(Format & PARTICLE_COORD){
			FORCE_CVT(self.hCoord, self.ByteCount);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_SIZE){
			FORCE_CVT(self.hSize, self.ByteCount);
			self.ByteCount += sizeof(VECTOR2);
		}
		if(Format & PARTICLE_COLOR32){
			FORCE_CVT(self.hColor32, self.ByteCount);
			self.ByteCount += sizeof(ARGB);
		}
		if(Format & PARTICLE_TEXCOORD){
			FORCE_CVT(self.hTexCoord, self.ByteCount);
			self.ByteCount += sizeof(VECTOR2);
		}
		if(Format & PARTICLE_TIMING){
			FORCE_CVT(self.hTiming, self.ByteCount);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_LIFETIME){
			FORCE_CVT(self.hLifetime, self.ByteCount);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_DSIZE){
			FORCE_CVT(self.hDifSize, self.ByteCount);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_COLOR){
			FORCE_CVT(self.hColor, self.ByteCount);
			self.ByteCount += sizeof(COLOR128);
		}
		if(Format & PARTICLE_DCOLOR){
			FORCE_CVT(self.hDifColor, self.ByteCount);
			self.ByteCount += sizeof(COLOR128);
		}
		if(Format & PARTICLE_VELOC){
			FORCE_CVT(self.hVeloc, self.ByteCount);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_ACCEL){
			FORCE_CVT(self.hAccel, self.ByteCount);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_DACCEL){
			FORCE_CVT(self.hDifAccel, self.ByteCount);
			self.ByteCount += sizeof(VECTOR3);
		}

		FORCE_CVT(self.hState, self.ByteCount);
		self.ByteCount += sizeof(bool);
	}
	VOID SetTarget(VOID* Pointer){
		self.Address = (BYTE*)Pointer;
	}
	VOID SetAccount(UINT *pAccount){
		self.pAccount = pAccount;
	}
	VOID OffsetPointer(INT4 Offset){
		self.Address += (INT4)self.ByteCount * Offset;
	}
	VOID KillTarget(){
		self.Activity() = FALSE;
		--(*self.pAccount);
	}
	///属性访问
	bool &Activity(){
		BYTE *pElement = self.Address + self.hState;
		return *((bool*)pElement);
	}
	SPFP &Timing(){
		BYTE *pElement = self.Address + self.hTiming;
		return *((SPFP*)pElement);
	}
	SPFP &Lifetime(){
		BYTE *pElement = self.Address + self.hLifetime;
		return *((SPFP*)pElement);
	}
	COLOR32 &Color32(){
		BYTE *pElement = self.Address + self.hColor32;
		return *((COLOR32*)pElement);
	}
	VECTOR3 &Coord(){
		BYTE *pElement = self.Address + self.hCoord;
		return *((VECTOR3*)pElement);
	}
	VECTOR3 &Veloc(){
		BYTE *pElement = self.Address + self.hVeloc;
		return *((VECTOR3*)pElement);
	}
	VECTOR3 &Accel(){
		BYTE *pElement = self.Address + self.hAccel;
		return *((VECTOR3*)pElement);
	}
	VECTOR3 &DifAccel(){
		BYTE *pElement = self.Address + self.hDifAccel;
		return *((VECTOR3*)pElement);
	}
	VECTOR2 &Size(){
		BYTE *pElement = self.Address + self.hSize;
		return *((VECTOR2*)pElement);
	}
	VECTOR2 &DifSize(){
		BYTE *pElement = self.Address + self.hDifSize;
		return *((VECTOR2*)pElement);
	}
	VECTOR2 &TexCoord(){
		BYTE *pElement = self.Address + self.hTexCoord;
		return *((VECTOR2*)pElement);
	}
	COLOR128 &Color(){
		BYTE *pElement = self.Address + self.hColor;
		return *((COLOR128*)pElement);
	}
	COLOR128 &DifColor(){
		BYTE *pElement = self.Address + self.hDifColor;
		return *((COLOR128*)pElement);
	}
	///属性元素访问
	SPFP* SizeElem(UINT Index){
		BYTE *pElement = self.Address + self.hSize;
		return (SPFP*)pElement + Index;
	}
	SPFP* DsizeElem(UINT Index){
		BYTE *pElement = self.Address + self.hDifSize;
		return (SPFP*)pElement + Index;
	}
	SPFP* CoordElem(UINT Index){
		BYTE *pElement = self.Address + self.hCoord;
		return (SPFP*)pElement + Index;
	}
	SPFP* VelocElem(UINT Index){
		BYTE *pElement = self.Address + self.hVeloc;
		return (SPFP*)pElement + Index;
	}
	SPFP* AccelElem(UINT Index){
		BYTE *pElement = self.Address + self.hAccel;
		return (SPFP*)pElement + Index;
	}
	SPFP* DaccelElem(UINT Index){
		BYTE *pElement = self.Address + self.hDifAccel;
		return (SPFP*)pElement + Index;
	}
	SPFP* ColorElem(UINT Index){
		BYTE *pElement = self.Address + self.hColor;
		return (SPFP*)pElement + Index;
	}
	SPFP* DcolorElem(UINT Index){
		BYTE *pElement = self.Address + self.hDifColor;
		return (SPFP*)pElement + Index;
	}
	SPFP* TexcoordElem(UINT Index){
		BYTE *pElement = self.Address + self.hTexCoord;
		return (SPFP*)pElement + Index;
	}
};

//粒子事件
class CPtcEvent{
	open enum TYPE{
		TYPE_TIMING,
		TYPE_LIFE,
		TYPE_SIZE,
		TYPE_VELOC,
		TYPE_ACCEL,
		TYPE_COLOR,
		TYPE_TEXCOORD,
		TYPE_SUBPROP,
		TYPE_SIZE_X,
		TYPE_SIZE_Y,
		TYPE_VELOC_X,
		TYPE_VELOC_Y,
		TYPE_VELOC_Z,
		TYPE_ACCEL_X,
		TYPE_ACCEL_Y,
		TYPE_ACCEL_Z,
		TYPE_COLOR_R,
		TYPE_COLOR_G,
		TYPE_COLOR_B,
		TYPE_COLOR_A,
		TYPE_TEXCOORD_U,
		TYPE_TEXCOORD_V,
	};

	open BOOL bFade; //渐变
	open TYPE Type; //类型
	open SPFP Time; //时间
	open SPFP Times[2]; //时间范围
	open CPtcEvent *Next; //下一个

	open virtual VOID ParseContent(const CHAR* Content) = 0;
	open virtual VOID SetChange(CPtcView &PtcView) = 0;
	open virtual VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff) = 0;
	open virtual VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice) = 0;
	open virtual VOID UpdateParticle(CPtcView &PtcView, SPFP TimeSpan){
		SPFP PrevTime = PtcView.Timing();
		SPFP LastTime = PtcView.Timing() + TimeSpan;

		//时间未落在本片段则给下一个事件处理

		if(self.Next && (PrevTime >= self.Next->Time)){
			self.Next->UpdateParticle(PtcView, TimeSpan);
			return;
		}

		//检查是否有渐变和分片处理

		BOOL bFadeOut = self.Next && self.Next->bFade;
		BOOL bInBatch = self.Next && (LastTime > self.Next->Time);

		//首次进入该事件的粒子需要初始化

		if(PrevTime == self.Time){
			if(!self.bFade)
				self.SetChange(PtcView);
			if(bFadeOut){
				SPFP EndTime = (self.Next->Time != FLT_MAX) ?
					self.Next->Time : PtcView.Lifetime();
				SPFP TimeDiff = EndTime - self.Time;

				self.SetupFading(PtcView, TimeDiff);
			}
		}

		//计算时间切片

		SPFP TimeSlice = bInBatch ?
			(self.Next->Time - PrevTime) : TimeSpan;

		//更新粒子位置

		if(self.Type == TYPE_VELOC)
			self.UpdatePosition(PtcView, TimeSlice);

		//如果有渐变则需要维护

		if(bFadeOut)
			self.UpdateFading(PtcView, TimeSlice);

		//如果有分片则需要继续处理

		if(!bInBatch){
			PtcView.Timing() = LastTime;
		} else{
			PtcView.Timing() = self.Next->Time;
			TimeSpan = LastTime - PtcView.Timing();
			self.Next->UpdateParticle(PtcView, TimeSpan);
		}
	}
	open virtual VOID UpdatePosition(CPtcView &PtcView, SPFP TimeSlice){
		using namespace dx;

		XVECTOR VecStep = PackV3(PtcView.Veloc());
		XVECTOR Vector = PackV3(PtcView.Coord()) + (VecStep * TimeSlice);

		UnpackV3(PtcView.Coord(), Vector);
	}

	open static CPtcEvent* CreateEmpty(TYPE Type);
	open static CPtcEvent* Create(const CHAR *Action, const CHAR *Time, const CHAR* Type, const CHAR* Content);
	open static INT4 Compare(const VOID* pElem0, const VOID* pElem1){
		CPtcEvent *pEvent0 = *(CPtcEvent**)pElem0;
		CPtcEvent *pEvent1 = *(CPtcEvent**)pElem1;

		if(pEvent0->Type != pEvent1->Type)
			return (INT4)pEvent0->Type - (INT4)pEvent1->Type;

		if(pEvent0->Time < pEvent1->Time) return -1;
		else if(pEvent0->Time == pEvent1->Time) return 0;
		else return 1;
	}
	open static CHAR* ParseNumber(const CHAR* Content, SPFP Output[2]){
		CHAR Buffer[32];
		CHAR *pSrcChar, *pDstChar;

		pDstChar = &Buffer[0];
		pSrcChar = (CHAR*)Content;

		if(!IsAlpha(*pSrcChar)){
			while((*pSrcChar == '-') || (*pSrcChar == '.') || IsDigit(*pSrcChar))
				*pDstChar++ = *pSrcChar++;
			*pDstChar = '\0';

			Output[0] = (SPFP)StoF(Buffer);
			Output[1] = Output[0];

			return pSrcChar + 1;
		} else{
			pSrcChar += sizeof("rand");
			pDstChar = &Buffer[0];

			while((*pSrcChar == '-') || (*pSrcChar == '.') || IsDigit(*pSrcChar))
				*pDstChar++ = *pSrcChar++;
			*pDstChar = '\0';
			Output[0] = (SPFP)StoF(Buffer);

			pSrcChar++;
			pDstChar = &Buffer[0];

			while((*pSrcChar == '-') || (*pSrcChar == '.') ||
				IsDigit(*pSrcChar)) *pDstChar++ = *pSrcChar++;
			*pDstChar = '\0';
			Output[1] = (SPFP)StoF(Buffer);

			return pSrcChar + 2;
		}
	}
	open static VOID ParseColor(const CHAR* Content, COLOR128 Output[2]){
		SPFP R[2], G[2], B[2], A[2];

		Content = ParseNumber(Content, R);
		Content = ParseNumber(Content, G);
		Content = ParseNumber(Content, B);
		Content = ParseNumber(Content, A);

		Output[0] = { R[0], G[0], B[0], A[0] };
		Output[1] = { R[1], G[1], B[1], A[1] };
	}
	open static VOID ParseVector2(const CHAR* Content, VECTOR2 Output[2]){
		SPFP X[2], Y[2];

		Content = ParseNumber(Content, X);
		Content = ParseNumber(Content, Y);

		Output[0] = { X[0], Y[0] };
		Output[1] = { X[1], Y[1] };
	}
	open static VOID ParseVector3(const CHAR* Content, VECTOR3 Output[2]){
		SPFP X[2], Y[2], Z[2];

		Content = ParseNumber(Content, X);
		Content = ParseNumber(Content, Y);
		Content = ParseNumber(Content, Z);

		Output[0] = { X[0], Y[0], Z[0] };
		Output[1] = { X[1], Y[1], Z[1] };
	}
	open static VOID ParseBlendMode(const CHAR* Content, DWRD &Output){
		if(StrCmpA(Content, "BLEND_ZERO") == 0)
			Output = D3D12_BLEND_ZERO;
		else if(StrCmpA(Content, "BLEND_ONE") == 0)
			Output = D3D12_BLEND_ONE;
		else if(StrCmpA(Content, "BLEND_SRCCOLOR") == 0)
			Output = D3D12_BLEND_SRC_COLOR;
		else if(StrCmpA(Content, "BLEND_INVSRCCOLOR") == 0)
			Output = D3D12_BLEND_INV_SRC_COLOR;
		else if(StrCmpA(Content, "BLEND_SRCALPHA") == 0)
			Output = D3D12_BLEND_SRC_ALPHA;
		else if(StrCmpA(Content, "BLEND_INVSRCALPHA") == 0)
			Output = D3D12_BLEND_INV_SRC_ALPHA;
		else if(StrCmpA(Content, "BLEND_DESTALPHA") == 0)
			Output = D3D12_BLEND_DEST_ALPHA;
		else if(StrCmpA(Content, "BLEND_INVDESTALPHA") == 0)
			Output = D3D12_BLEND_INV_DEST_ALPHA;
		else if(StrCmpA(Content, "BLEND_DESTCOLOR") == 0)
			Output = D3D12_BLEND_DEST_COLOR;
		else if(StrCmpA(Content, "BLEND_INVDESTCOLOR") == 0)
			Output = D3D12_BLEND_INV_DEST_COLOR;
		else if(StrCmpA(Content, "BLEND_SRCALPHASAT") == 0)
			Output = D3D12_BLEND_SRC_ALPHA_SAT;
		else if(StrCmpA(Content, "BLENDOP_ADD") == 0)
			Output = D3D12_BLEND_OP_ADD;
		else if(StrCmpA(Content, "BLENDOP_SUB") == 0)
			Output = D3D12_BLEND_OP_SUBTRACT;
		else if(StrCmpA(Content, "BLENDOP_REVSUB") == 0)
			Output = D3D12_BLEND_OP_REV_SUBTRACT;
		else if(StrCmpA(Content, "BLENDOP_MIN") == 0)
			Output = D3D12_BLEND_OP_MIN;
		else if(StrCmpA(Content, "BLENDOP_MAX") == 0)
			Output = D3D12_BLEND_OP_MAX;
		else if(StrCmpA(Content, "BLENDOP_OFF") == 0)
			Output = 0x0;
	}
};

//粒子组
class CPtcGroup{
	open UINT Ident; //ID
	open UINT TexIdent; //纹理ID
	open UINT MtlIdent; //材质ID
	///发射器
	hide SPFP EmitValue;    //发射进度
	open SPFP EmitRates[2]; //发射速率
	open VECTOR3 EmitRange; //发射范围
	///粒子
	open DWRD PtcFormat;    //粒子格式
	hide UINT PtcIndex;     //粒子索引
	open UINT PtcCount;     //粒子数量
	open UINT PtcMaxCnt;    //粒子最大数量
	open UINT PtcByteCnt;   //粒子字节数
	hide BYTE *PtcBuff;     //粒子缓存区
	open VECTOR2 PtcSize;   //粒子尺寸
	open COLOR128 PtcColor; //粒子颜色
	hide CPtcView PtcView;  //粒子视图
	///事件
	open UINT numTopEvent;        //顶事件数量
	open CPtcEvent **arrTopEvent; //顶事件数组

	open ~CPtcGroup(){
		self.Finalize();
	}
	open CPtcGroup(){
		ZeroMemory(this, sizeof(*this));
	}

	open VOID Finalize(){
		if(!self.PtcBuff) return;

		CPtcEvent *pEvent, *NextEvent;
		for(UINT i = 0; i < self.numTopEvent; ++i){
			pEvent = self.arrTopEvent[i];
			while(pEvent){
				NextEvent = pEvent->Next;
				delete[] pEvent;
				pEvent = NextEvent;
			}
		}

		DELETE_ARR(self.arrTopEvent);
		DELETE_ARR(self.PtcBuff);

		ZeroMemory(this, sizeof(*this));
	}
	open VOID Initialize(TXMLElement* pXMLNode){
		//解析ID

		TXMLAttribute *pNodeAttr = pXMLNode->FirstAttribute();
		if(StrCmpA(pNodeAttr->Name(), "name") == 0)
			self.Ident = StrHash4A(pNodeAttr->Value());

		//设置默认颜色

		self.PtcColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		//解析其它数据

		VECTOR2 Vec2Range[2];
		VECTOR3 Vec3Range[2];
		COLOR128 ColorRange[2];

		pXMLNode = pXMLNode->FirstChildElement();
		while(pXMLNode){
			if(StrCmpA(pXMLNode->Value(), "numparticles") == 0){
				self.PtcMaxCnt = StoI(pXMLNode->GetText());
			} else if(StrCmpA(pXMLNode->Value(), "texture") == 0){
				self.TexIdent = StrHash4A(pXMLNode->GetText());
			} else if(StrCmpA(pXMLNode->Value(), "events") == 0){
				self.InitEvents(pXMLNode);
			} else if(StrCmpA(pXMLNode->Value(), "emitrate") == 0){
				CPtcEvent::ParseNumber(pXMLNode->GetText(), self.EmitRates);
			} else if(StrCmpA(pXMLNode->Value(), "emitrange") == 0){
				CPtcEvent::ParseVector3(pXMLNode->GetText(), Vec3Range);
				Vec3Sample1(self.EmitRange, Vec3Range);
			} else if(StrCmpA(pXMLNode->Value(), "psize") == 0){
				CPtcEvent::ParseVector2(pXMLNode->GetText(), Vec2Range);
				Vec2Sample1(self.PtcSize, Vec2Range);
			} else if(StrCmpA(pXMLNode->Value(), "color") == 0){
				CPtcEvent::ParseColor(pXMLNode->GetText(), ColorRange);
				ColorSample1(self.PtcColor, ColorRange);
			}
			pXMLNode = pXMLNode->NextSiblingElement();
		}
	}
	hide VOID InitEvents(TXMLElement* pXMLNode){
		TXMLAttribute *pNodeAttr;
		CPtcEvent **arrEvent;
		UINT numEvent, Index;
		const CHAR *Action, *Time, *Type, *Value;

		//解析事件数量

		pNodeAttr = pXMLNode->FirstAttribute();
		numEvent = StoI(pNodeAttr->Value());
		arrEvent = new CPtcEvent*[numEvent];

		//解析事件

		Index = 0;
		pXMLNode = pXMLNode->FirstChildElement();

		while(pXMLNode){
			pNodeAttr = pXMLNode->FirstAttribute();
			while(pNodeAttr){
				if(StrCmpA(pNodeAttr->Name(), "time") == 0)
					Time = pNodeAttr->Value();
				else if(StrCmpA(pNodeAttr->Name(), "type") == 0)
					Type = pNodeAttr->Value();
				pNodeAttr = pNodeAttr->Next();
			}

			Action = pXMLNode->Value();
			Value = pXMLNode->GetText();

			arrEvent[Index++] = CPtcEvent::Create(Action, Time, Type, Value);
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//优化事件列表

		self.InitTopEventList(arrEvent, numEvent);
		DELETE_ARR(arrEvent);
	}
	hide VOID InitTopEventList(CPtcEvent **arrEvent, UINT numEvent){
		DWRD LastType;
		UINT TypeCount;
		CPtcEvent *pEvent;

		//选取事件发生时间

		for(UINT i = 0; i < numEvent; ++i)
			arrEvent[i]->Time = RealSample1(arrEvent[i]->Times);

		//按事件时间和类型排序

		QSort(arrEvent, numEvent, sizeof(CPtcEvent*), CPtcEvent::Compare);

		//统计事件类型

		TypeCount = 0;
		LastType = -1L;

		for(UINT i = 0; i < numEvent; ++i){
			if(arrEvent[i]->Type != LastType){
				LastType = arrEvent[i]->Type;
				TypeCount++;
			}
		}

		//连接同类型事件

		self.numTopEvent = TypeCount;
		self.arrTopEvent = new CPtcEvent*[TypeCount];

		for(UINT n = 0, i = 0; i < self.numTopEvent; ++i){
			self.arrTopEvent[i] = arrEvent[n];
			pEvent = self.arrTopEvent[i];

			for(++n; n < numEvent; ++n){
				if(arrEvent[n]->Type != pEvent->Type){
					break;
				} else{
					pEvent->Next = arrEvent[n];
					pEvent = pEvent->Next;
				}
			}
		}

		//给向量分量类型事件创建起始事件

		for(UINT i = 0; i < self.numTopEvent; ++i){
			if(pEvent->Type > CPtcEvent::TYPE_SUBPROP){
				pEvent = CPtcEvent::CreateEmpty(self.arrTopEvent[i]->Type);
				pEvent->Next = self.arrTopEvent[i];
				self.arrTopEvent[i] = pEvent;
			}
		}

		//创建粒子缓存区

		self.InitParticleBuffer();
	}
	hide VOID InitParticleBuffer(){
		//根据事件类型给粒子分配属性

		self.PtcFormat = PARTICLE_COORD;
		self.PtcFormat |= PARTICLE_TIMING;

		CPtcEvent *pEvent;
		for(UINT i = 0; i < self.numTopEvent; ++i){
			pEvent = self.arrTopEvent[i];
			while(pEvent->Next){
				pEvent = pEvent->Next;
				if(pEvent->bFade) break;
			}

			switch(pEvent->Type){
				case CPtcEvent::TYPE_SIZE:{
					self.PtcFormat |= PARTICLE_SIZE;
					if(pEvent->bFade)
						self.PtcFormat |= PARTICLE_DSIZE;
				} break;
				case CPtcEvent::TYPE_LIFE:{
					self.PtcFormat |= PARTICLE_LIFETIME;
				} break;
				case CPtcEvent::TYPE_TIMING:{
					self.PtcFormat |= PARTICLE_TIMING;
				} break;
				case CPtcEvent::TYPE_TEXCOORD:
				case CPtcEvent::TYPE_TEXCOORD_U:
				case CPtcEvent::TYPE_TEXCOORD_V:{
					self.PtcFormat |= PARTICLE_TEXCOORD;
				} break;
				case CPtcEvent::TYPE_COLOR:
				case CPtcEvent::TYPE_COLOR_R:
				case CPtcEvent::TYPE_COLOR_G:
				case CPtcEvent::TYPE_COLOR_B:
				case CPtcEvent::TYPE_COLOR_A:{
					self.PtcFormat |= PARTICLE_COLOR32;
					self.PtcFormat |= PARTICLE_COLOR;
					if(pEvent->bFade)
						self.PtcFormat |= PARTICLE_DCOLOR;
				} break;
				case CPtcEvent::TYPE_VELOC:
				case CPtcEvent::TYPE_VELOC_X:
				case CPtcEvent::TYPE_VELOC_Y:
				case CPtcEvent::TYPE_VELOC_Z:{
					self.PtcFormat |= PARTICLE_VELOC;
					if(pEvent->bFade)
						self.PtcFormat |= PARTICLE_ACCEL;
				} break;
				case CPtcEvent::TYPE_ACCEL:
				case CPtcEvent::TYPE_ACCEL_X:
				case CPtcEvent::TYPE_ACCEL_Y:
				case CPtcEvent::TYPE_ACCEL_Z:{
					self.PtcFormat |= PARTICLE_ACCEL;
					if(pEvent->bFade)
						self.PtcFormat |= PARTICLE_DACCEL;
				} break;
			}
		}

		//根据粒子属性分配内存空间

		self.PtcView.Initialize(self.PtcFormat);
		self.PtcByteCnt = self.PtcView.ByteCount;
		self.PtcBuff = new BYTE[self.PtcByteCnt * self.PtcMaxCnt]{ 0 };
		self.PtcView.SetTarget(self.PtcBuff);
		self.PtcView.SetAccount(&self.PtcCount);
	}
	hide VOID SetupParticle(){
		CPtcView *pPtcView = self.GetParticleView(self.PtcIndex);
		for(UINT i = 0; i < self.PtcMaxCnt; ++i){
			if(!pPtcView->Activity()) break;

			self.PtcIndex++;
			pPtcView->OffsetPointer(1);

			if(self.PtcIndex >= self.PtcMaxCnt){
				self.PtcIndex = 0;
				pPtcView = self.GetParticleView(0);
			}
		}

		ZeroMemory(pPtcView->Address, self.PtcByteCnt);
		Vec3Sample(pPtcView->Coord(), VECTOR3(0.0f, 0.0f, 0.0f), self.EmitRange);
		pPtcView->Activity() = TRUE;
		self.PtcCount++;
	}
	open VOID Update(SPFP TimeSpan){
		//更新粒子

		CPtcView *pPtcView = self.GetParticleView(0);
		for(UINT i = 0; i < self.PtcMaxCnt; ++i){
			if(pPtcView->Activity()){
				for(UINT j = 0; j < self.numTopEvent; ++j)
					self.arrTopEvent[j]->UpdateParticle(*pPtcView, TimeSpan);
				pPtcView->OffsetPointer(1);
			}
		}

		//创建新粒子

		if(self.PtcCount < self.PtcMaxCnt){
			SPFP EmitRate = RealSample1(self.EmitRates);
			self.EmitValue += EmitRate * TimeSpan;

			if(self.EmitValue >= 1.0f){
				UINT EmitValue = (UINT)self.EmitValue;
				self.EmitValue -= (SPFP)EmitValue;

				if((EmitValue + self.PtcCount) > self.PtcMaxCnt)
					EmitValue = self.PtcMaxCnt - self.PtcCount;

				for(UINT i = 0; i < EmitValue; ++i)
					self.SetupParticle();
			}
		}
	}
	open CPtcView* GetParticleView(UINT Index){
		self.PtcView.Address = self.PtcBuff;
		self.PtcView.Address += self.PtcByteCnt * Index;
		return &self.PtcView;
	}
};

//粒子系统
class CPtcSystem{
	open VECTOR3 Position; //位置
	///混合模式
	open DWRD fBlendOP;   //混合操作
	open DWRD fSrcBlend;  //源混合模式
	open DWRD fDstBlend;  //目标混合模式
	///粒子组
	open UINT numSubset;       //子集数量
	open CPtcGroup *arrSubset; //子集数组
	///顶点缓存区
	open DWRD VtxFormat;   //顶点格式
	open UINT cbVertex;    //顶点字节数
	open UINT cbVtxBuff;   //缓存区大小
	open ID3DResource *lpVtxBuff; //顶点缓存区

	open ~CPtcSystem(){
		self.Finalize();
	}
	open CPtcSystem(){
		ZeroMemory(this, sizeof(*this));
	}
	open CPtcSystem(ID3DDevice* lpD3DDevice, CHAR* ScriptPath){
		self.Initialize(lpD3DDevice, ScriptPath);
	}

	open VOID Finalize(){
		RESET_ARR(self.arrSubset);
		RESET_REF(self.lpVtxBuff);
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, CHAR* ScriptPath){
		//设置默认值

		self.fBlendOP = D3D12_BLEND_OP_ADD;
		self.fSrcBlend = D3D12_BLEND_SRC_ALPHA;
		self.fDstBlend = D3D12_BLEND_INV_SRC_ALPHA;

		//读取脚本

		TXMLElement *pXMLNode;
		TXMLDocument XMLScene(ScriptPath);

		XMLScene.LoadFile();
		pXMLNode = XMLScene.RootElement();
		if(StrCmpA(pXMLNode->Value(), "psystem") != 0) return;
		pXMLNode = pXMLNode->FirstChildElement();

		//解析基本数据

		while(pXMLNode){
			if(StrCmpA(pXMLNode->Value(), "numsubsets") == 0){
				self.numSubset = StoI(pXMLNode->GetText());
				self.arrSubset = new CPtcGroup[self.numSubset];
			} else if(StrCmpA(pXMLNode->Value(), "srcblend") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fSrcBlend);
			} else if(StrCmpA(pXMLNode->Value(), "dstblend") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fDstBlend);
			} else if(StrCmpA(pXMLNode->Value(), "blendop") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fBlendOP);
			} else if(StrCmpA(pXMLNode->Value(), "subset") == 0){
				break;
			}
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//解析粒子组

		for(UINT i = 0; i < self.numSubset; ++i){
			self.arrSubset[i].Initialize(pXMLNode);
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//创建顶点缓存区和材质

		self.InitVertexBuffer(lpD3DDevice);
	}
	hide VOID InitVertexBuffer(ID3DDevice* lpD3DDevice){
		//计算顶点格式

		DWRD PtcFormat = self.arrSubset[0].PtcFormat;
		self.VtxFormat = 0x0;

		if(PtcFormat & PARTICLE_COORD)
			self.VtxFormat |= VERT_COORD;
		if(PtcFormat & PARTICLE_SIZE)
			self.VtxFormat |= VERT_SIZE;
		if(PtcFormat & PARTICLE_COLOR)
			self.VtxFormat |= VERT_COLOR;
		if(PtcFormat & PARTICLE_TEXCOORD)
			self.VtxFormat |= VERT_TEXCOORD;

		//创建顶点缓存区

		CVertView VertView(self.VtxFormat);

		self.cbVertex = VertView.ByteCount;
		self.cbVtxBuff = self.cbVertex * self.GetParticleMaxCount();
		self.lpVtxBuff = CreateUploadBuffer(lpD3DDevice, self.cbVtxBuff, FALSE);
	}
	open VOID LinkMaterials(CMtlLibrary &Assets){
		for(UINT i = 0; i < self.numSubset; ++i){
			for(UINT j = 0; j < Assets.TexCount; ++j){
				if(self.arrSubset[i].TexIdent == Assets.arrTexID[j]){
					self.arrSubset[i].TexIdent = j;
					break;
				}
			}
		}
	}
	open VOID SetPosition(VECTOR3 &Position){
		self.Position = Position;
	}
	open VOID UpdateParticles(SPFP TimeSpan){
		for(UINT i = 0; i < self.numSubset; ++i)
			self.arrSubset[i].Update(TimeSpan);
	}
	open VOID UpdateVertexBuffer(){
		BYTE *VtxBytes;
		self.lpVtxBuff->Map(0, NULL, (VOID**)&VtxBytes);

		for(UINT i = 0; i < self.numSubset; ++i){
			CPtcGroup *pPtcGroup = &self.arrSubset[i];
			if(pPtcGroup->PtcCount == 0) continue;

			CPtcView *pPtcView = pPtcGroup->GetParticleView(0);
			for(UINT j = 0; j < pPtcGroup->PtcMaxCnt; ++j){
				if(pPtcView->Activity()){
					CopyMemory(VtxBytes, pPtcView->Address, self.cbVertex);
					VtxBytes += self.cbVertex;
				}
				pPtcView->OffsetPointer(1);
			}
		}

		self.lpVtxBuff->Unmap(0, NULL);
	}
	open UINT GetParticleCount(){
		UINT Count = 0;
		for(UINT i = 0; i < self.numSubset; ++i)
			Count += self.arrSubset[i].PtcCount;
		return Count;
	}
	open UINT GetParticleMaxCount(){
		UINT Count = 0;
		for(UINT i = 0; i < self.numSubset; ++i)
			Count += self.arrSubset[i].PtcMaxCnt;
		return Count;
	}
	open D3DBuffBlendDesc GetBlendDesc(){
		D3DBuffBlendDesc BlendDesc = { FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };

		if(self.fBlendOP != 0x0){
			BlendDesc.BlendEnable = TRUE;
			BlendDesc.BlendOp = (D3DBlendOP)self.fBlendOP;
			BlendDesc.SrcBlend = (D3DBlendMode)self.fSrcBlend;
			BlendDesc.DestBlend = (D3DBlendMode)self.fDstBlend;
		}

		return BlendDesc;
	}
	open D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuff->GetGPUVirtualAddress();
		VBView.StrideInBytes = self.cbVertex;
		VBView.SizeInBytes = self.cbVtxBuff;
		return VBView;
	}
};

//----------------------------------------//


//--------------------粒子事件--------------------//

//标量型事件
struct PtcScalarEvent: public CPtcEvent{
	SPFP Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseNumber(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &PtcView, SPFP TimeSpan){
		SPFP PrevTime = PtcView.Timing();
		SPFP LastTime = PtcView.Timing() + TimeSpan;

		//时间未落在本片段则给下一个事件处理

		if(self.Next && (PrevTime >= self.Next->Time)){
			self.Next->UpdateParticle(PtcView, TimeSpan);
			return;
		}

		//检查是否有渐变和分片处理

		BOOL bFadeOut = self.Next && self.Next->bFade;
		BOOL bInBatch = self.Next && (LastTime > self.Next->Time);

		//选取要修改的粒子属性

		UINT AttrIndex = 0;
		SPFP *pAttr, *pStep;

		switch(self.Type){
			case TYPE_TEXCOORD_V: AttrIndex++;
			case TYPE_TEXCOORD_U:{
				pAttr = PtcView.TexcoordElem(AttrIndex);
			} break;
			case TYPE_SIZE_Y: AttrIndex++;
			case TYPE_SIZE_X:{
				pAttr = PtcView.SizeElem(AttrIndex);
				pStep = PtcView.DsizeElem(AttrIndex);
			} break;
			case TYPE_VELOC_Z: AttrIndex++;
			case TYPE_VELOC_Y: AttrIndex++;
			case TYPE_VELOC_X:{
				pAttr = PtcView.VelocElem(AttrIndex);
				pStep = PtcView.AccelElem(AttrIndex);
			} break;
			case TYPE_ACCEL_Z: AttrIndex++;
			case TYPE_ACCEL_Y: AttrIndex++;
			case TYPE_ACCEL_X:{
				pAttr = PtcView.AccelElem(AttrIndex);
				pStep = PtcView.DaccelElem(AttrIndex);
			} break;
			case TYPE_COLOR_A: AttrIndex++;
			case TYPE_COLOR_B: AttrIndex++;
			case TYPE_COLOR_G: AttrIndex++;
			case TYPE_COLOR_R:{
				pAttr = PtcView.ColorElem(AttrIndex);
				pStep = PtcView.DaccelElem(AttrIndex);
			} break;
		}

		//首次进入该事件的粒子需要初始化

		if(PrevTime == self.Time){
			if(!self.bFade)
				*pAttr = RealSample1(self.Params);
			if(bFadeOut){
				SPFP EndTime = (self.Next->Time != FLT_MAX) ?
					self.Next->Time : PtcView.Lifetime();
				SPFP TimeDiff = EndTime - self.Time;

				PtcScalarEvent *NextEvent = (PtcScalarEvent*)self.Next;
				SPFP NextValue = RealSample1(NextEvent->Params);

				*pStep = (NextValue - *pAttr) / TimeDiff;
			}
		}

		//计算时间切片

		SPFP TimeSlice = bInBatch ?
			(self.Next->Time - PrevTime) : TimeSpan;

		//更新粒子位置

		if((self.Type >= TYPE_VELOC) && (self.Type <= TYPE_VELOC_Z)){
			SPFP *pCoord = PtcView.CoordElem(AttrIndex);
			*pCoord += *pAttr * TimeSlice;
		}

		//如果有渐变则需要维护

		if(bFadeOut)
			*pAttr += *pStep * TimeSlice;

		//如果有分片则需要继续处理

		if(!bInBatch){
			PtcView.Timing() = LastTime;
		} else{
			PtcView.Timing() = self.Next->Time;
			TimeSpan = LastTime - PtcView.Timing();
			self.Next->UpdateParticle(PtcView, TimeSpan);
		}
	}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){}
	VOID SetChange(CPtcView &PtcView){}
};
//速度事件
struct PtcVelocEvent: public CPtcEvent{
	VECTOR3 Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseVector3(Content, self.Params);
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){
		using namespace dx;

		XVECTOR VecStep = PackV3(PtcView.Accel());
		XVECTOR Vector = PackV3(PtcView.Veloc()) + (VecStep * TimeSlice);

		UnpackV3(PtcView.Veloc(), Vector);
	}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){
		using namespace dx;

		VECTOR3 NextValue;
		PtcVelocEvent *NextEvent = (PtcVelocEvent*)self.Next;
		Vec3Sample1(NextValue, NextEvent->Params);

		XVECTOR Vector0 = PackV3(PtcView.Veloc());
		XVECTOR Vector1 = PackV3(NextValue);
		XVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
		UnpackV3(PtcView.Accel(), VecStep);
	}
	VOID SetChange(CPtcView &PtcView){
		Vec3Sample1(PtcView.Veloc(), self.Params);
	}
};
//加速度事件
struct PtcAccelEvent: public CPtcEvent{
	VECTOR3 Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseVector3(Content, self.Params);
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){
		using namespace dx;

		XVECTOR VecStep = PackV3(PtcView.DifAccel());
		XVECTOR Vector = PackV3(PtcView.Accel()) + (VecStep * TimeSlice);

		UnpackV3(PtcView.Accel(), Vector);
	}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){
		using namespace dx;

		VECTOR3 NextValue;
		PtcAccelEvent *NextEvent = (PtcAccelEvent*)self.Next;
		Vec3Sample1(NextValue, NextEvent->Params);

		XVECTOR Vector0 = PackV3(PtcView.Accel());
		XVECTOR Vector1 = PackV3(NextValue);
		XVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
		UnpackV3(PtcView.DifAccel(), VecStep);
	}
	VOID SetChange(CPtcView &PtcView){
		Vec3Sample1(PtcView.Accel(), self.Params);
	}
};
//颜色事件
struct PtcColorEvent: public CPtcEvent{
	COLOR128 Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseColor(Content, self.Params);
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){
		using namespace dx;

		XVECTOR VecStep = PackF4(PtcView.DifColor());
		XVECTOR Vector = PackF4(PtcView.Color()) + (VecStep * TimeSlice);

		UnpackF4(PtcView.Color(), Vector);
		ColorConvert32(PtcView.Color32(), PackF4(PtcView.Color()));
	}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){
		using namespace dx;

		COLOR128 NextValue;
		PtcColorEvent *NextEvent = (PtcColorEvent*)self.Next;
		ColorSample1(NextValue, NextEvent->Params);

		XVECTOR Vector0 = PackF4(PtcView.Color());
		XVECTOR Vector1 = PackF4(NextValue);
		XVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
		UnpackF4(PtcView.DifColor(), VecStep);
	}
	VOID SetChange(CPtcView &PtcView){
		ColorSample1(PtcView.Color(), self.Params);
		ColorConvert32(PtcView.Color32(), PackF4(PtcView.Color()));
	}
};
//尺寸事件
struct PtcSizeEvent: public CPtcEvent{
	VECTOR2 Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseVector2(Content, self.Params);
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){
		using namespace dx;

		XVECTOR VecStep = PackV2(PtcView.DifSize());
		XVECTOR Vector = PackV2(PtcView.Size()) + (VecStep * TimeSlice);

		UnpackV2(PtcView.Size(), Vector);
	}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){
		using namespace dx;

		VECTOR2 NextValue;
		PtcSizeEvent *NextEvent = (PtcSizeEvent*)self.Next;
		Vec2Sample1(NextValue, NextEvent->Params);

		XVECTOR Vector0 = PackV2(PtcView.Size());
		XVECTOR Vector1 = PackV2(NextValue);
		XVECTOR VecStep = (Vector1 - Vector0) / TimeDiff;
		UnpackV2(PtcView.DifSize(), VecStep);
	}
	VOID SetChange(CPtcView &PtcView){
		Vec2Sample1(PtcView.Size(), self.Params);
	}
};
//时间事件
struct PtcTimeEvent: public CPtcEvent{
	SPFP Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseNumber(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &PtcView, SPFP TimeSpan){
		SPFP PrevTime = PtcView.Timing();
		SPFP CurTime = PtcView.Timing() + TimeSpan;

		if(self.Type == TYPE_LIFE){
			if(PrevTime == 0.0f)
				PtcView.Lifetime() = RealSample1(self.Params);
			if(CurTime >= PtcView.Lifetime())
				PtcView.KillTarget();
			else PtcView.Timing() = CurTime;
		} else if(self.Type == TYPE_TIMING){
			if(CurTime >= self.Time)
				PtcView.Timing() = RealSample1(self.Params);
			else PtcView.Timing() = CurTime;
		}
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){}
	VOID SetChange(CPtcView &PtcView){}
};
//纹理事件
struct PtcTexEvent: public CPtcEvent{
	VECTOR2 Params[2];

	VOID ParseContent(const CHAR* Content){
		CPtcEvent::ParseVector2(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &PtcView, SPFP TimeSpan){
		SPFP PrevTime = PtcView.Timing();
		SPFP CurTime = PtcView.Timing() + TimeSpan;

		if(self.Next && (CurTime >= self.Next->Time)){
			self.Next->UpdateParticle(PtcView, TimeSpan);
			return;
		}

		if(PrevTime <= self.Time)
			Vec2Sample1(PtcView.TexCoord(), self.Params);
		PtcView.Timing() = CurTime;
	}
	VOID UpdateFading(CPtcView &PtcView, SPFP TimeSlice){}
	VOID SetupFading(CPtcView &PtcView, SPFP TimeDiff){}
	VOID SetChange(CPtcView &PtcView){}
};

//创建事件
CPtcEvent* CPtcEvent::Create(const CHAR *Action, const CHAR *Time, const CHAR* Type, const CHAR* Content){
	CPtcEvent *pEvent = NULL;

	if(StrCmpA(Type, "life") == 0){
		pEvent = new PtcTimeEvent;
		pEvent->Type = CPtcEvent::TYPE_LIFE;
	} else if(StrCmpA(Type, "timer") == 0){
		pEvent = new PtcTimeEvent;
		pEvent->Type = CPtcEvent::TYPE_TIMING;
	} else if(StrCmpA(Type, "size") == 0){
		pEvent = new PtcSizeEvent;
		pEvent->Type = CPtcEvent::TYPE_SIZE;
	} else if(StrCmpA(Type, "size-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_SIZE_X;
	} else if(StrCmpA(Type, "size-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_SIZE_Y;
	} else if(StrCmpA(Type, "veloc") == 0){
		pEvent = new PtcVelocEvent;
		pEvent->Type = CPtcEvent::TYPE_VELOC;
	} else if(StrCmpA(Type, "veloc-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_VELOC_X;
	} else if(StrCmpA(Type, "veloc-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_VELOC_Y;
	} else if(StrCmpA(Type, "veloc-z") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_VELOC_Z;
	} else if(StrCmpA(Type, "accel") == 0){
		pEvent = new PtcVelocEvent;
		pEvent->Type = CPtcEvent::TYPE_ACCEL;
	} else if(StrCmpA(Type, "accel-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_ACCEL_X;
	} else if(StrCmpA(Type, "accel-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_ACCEL_Y;
	} else if(StrCmpA(Type, "accel-z") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_ACCEL_Z;
	} else if(StrCmpA(Type, "color") == 0){
		pEvent = new PtcColorEvent;
		pEvent->Type = CPtcEvent::TYPE_COLOR;
	} else if(StrCmpA(Type, "color-a") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_COLOR_A;
	} else if(StrCmpA(Type, "color-r") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_COLOR_R;
	} else if(StrCmpA(Type, "color-g") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_COLOR_G;
	} else if(StrCmpA(Type, "color-b") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_COLOR_B;
	} else if(StrCmpA(Type, "texcoord") == 0){
		pEvent = new PtcTexEvent;
		pEvent->Type = CPtcEvent::TYPE_TEXCOORD;
	} else if(StrCmpA(Type, "texcoord-u") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_TEXCOORD_U;
	} else if(StrCmpA(Type, "texcoord-v") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::TYPE_TEXCOORD_V;
	}

	if(pEvent){
		pEvent->ParseContent(Content);
		pEvent->bFade = FALSE;
		pEvent->Next = NULL;

		if(StrCmpA(Action, "set") == 0){
			CPtcEvent::ParseNumber(Time, pEvent->Times);
		} else if(StrCmpA(Action, "fade") == 0){
			CPtcEvent::ParseNumber(Time, pEvent->Times);
			pEvent->bFade = TRUE;
		} else if(StrCmpA(Action, "init") == 0){
			pEvent->Times[0] = 0.0f;
			pEvent->Times[1] = 0.0f;
		} else if(StrCmpA(Action, "final") == 0){
			pEvent->Times[0] = FLT_MAX;
			pEvent->Times[1] = FLT_MAX;
		}
	}

	return pEvent;
}
//创建空事件
CPtcEvent* CPtcEvent::CreateEmpty(TYPE Type){
	PtcScalarEvent *pEvent = new PtcScalarEvent;
	ZeroMemory(pEvent, sizeof(*pEvent));
	pEvent->Type = Type;
	return pEvent;
}

//----------------------------------------//