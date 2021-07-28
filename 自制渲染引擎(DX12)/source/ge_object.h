//--------------------灯光库/材质库--------------------//

//灯光库
class CLitLibrary{
	open UINT Size;
	open LIGHT *Items;
	hide ID3DResource *lpBuffer;

	open ~CLitLibrary(){
		self.Finalize();
	}
	open CLitLibrary(){
		MemZero(this, sizeof(*this));
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
		self.Size = 0;

		MemZero(self.Items, BuffSize);
	}
	open VOID UpdateSpotLight(LIGHT &Data, UINT Index = UINT4_MAX){
		if(Index == UINT4_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		XVECTOR Direction = Vec3Normalize(PackV3(Data.Direction));

		*Light = Data;
		Light->Direction = *(VECTOR3*)&Direction;
	}
	open VOID UpdatePointLight(LIGHT &Data, UINT Index = UINT4_MAX){
		if(Index == UINT4_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		Light->Intensity = Data.Intensity;
		Light->Position = Data.Position;
		Light->AttStart = Data.AttStart;
		Light->AttEnd = Data.AttEnd;
	}
	open VOID UpdateDirectionalLight(LIGHT &Data, UINT Index = UINT4_MAX){
		if(Index == UINT4_MAX)
			Index = self.Size++;

		LIGHT *Light = &self.Items[Index];
		XVECTOR Direction = Vec3Normalize(PackV3(Data.Direction));

		Light->Intensity = Data.Intensity;
		Light->Direction = *(VECTOR3*)&Direction;
		Light->AttEnd = REAL4_INFINITE;
	}
	open VOID TurnOnLight(UINT Index){
		LIGHT *pLight = &self.Items[Index];
		SPFP &Switch = pLight->AttEnd;
		if(Switch < 0.0f) Switch = -Switch;
	}
	open VOID TurnOffLight(UINT Index){
		LIGHT *pLight = &self.Items[Index];
		SPFP &Switch = pLight->AttEnd;
		if(Switch > 0.0f) Switch = -Switch;
	}
	open HGPUBUFF GetGPUAddress(){
		return self.lpBuffer->GetGPUVirtualAddress();
	}
};

//材质库
class CMtlLibrary{
	struct MTRLEX{
		UINT Ident;
		MATERIAL Base;
	};
	struct TEXTURE{
		UINT Ident;
		ID3DResource *lpBuffer;
	};

	typedef std::vector<MTRLEX> CMtlList;
	typedef std::vector<TEXTURE> CTexList;

	open UINT MtlCount;
	open UINT TexCount;
	open UINT *arrMtlID;
	open UINT *arrTexID;
	open ID3DViewHeap *lpSRVHeap;
	hide ID3DResource *lpMtlBuffer;
	hide ID3DResource **arrTexture;

	open ~CMtlLibrary(){
		self.Finalize();
	}
	open CMtlLibrary(){
		self.Initialize();
	}
	open VOID Finalize(){
		for(UINT i = 0; i < self.TexCount; ++i)
			self.arrTexture[i]->Release();

		DELETE_ARR(self.arrMtlID);
		DELETE_ARR(self.arrTexID);
		DELETE_ARR(self.arrTexture);

		RELEASE_REF(self.lpSRVHeap);
		RELEASE_REF(self.lpMtlBuffer);

		MemZero(this, sizeof(*this));
	}
	hide VOID Initialize(){
		MemZero(this, sizeof(*this));

		CMtlList *pMtlList = new CMtlList;
		CTexList *pTexList = new CTexList;

		self.arrMtlID = (UINT*)pMtlList;
		self.arrTexID = (UINT*)pTexList;
	}
	open VOID AddTexture(TCHR* Name, ID3DResource* lpTexture){
		TEXTURE Texture;
		CTexList *pTexList;

		Texture.Ident = StringHash4(Name);
		Texture.lpBuffer = lpTexture;

		pTexList = (CTexList*)self.arrTexID;
		pTexList->push_back(Texture);
	}
	open VOID AddTexturesFromDirectory(ID3DDevice* lpD3DDevice, TCHR* Directory, DXTKUploadBatch &ULBatch){
		UPTR DirLength;
		TCHR FilePath[256];

		BOOL bHasNext;
		HANDLE hSeeker;
		FILEINFO FileInfo;

		TEXTURE Texture;
		CTexList *pTexList;
		ID3DResource *lpTexture;

		StrCpy(FilePath, Directory);
		StrCat(FilePath, TXT("*.dds"));

		pTexList = (CTexList*)self.arrTexID;
		DirLength = StrLen(Directory);
		hSeeker = FindFirstFile(FilePath, &FileInfo);

		if(hSeeker == INVALID_HANDLE_VALUE)
			goto done;

		do{
			FilePath[DirLength] = TXT('\0');
			StrCat(FilePath, FileInfo.cFileName);

			dx::CreateDDSTextureFromFile(lpD3DDevice, ULBatch, FilePath, &lpTexture);
			Texture.Ident = StringHash4(FileInfo.cFileName);
			Texture.lpBuffer = lpTexture;
			pTexList->push_back(Texture);

			bHasNext = FindNextFile(hSeeker, &FileInfo);
		} while(bHasNext);
done:
		FindClose(hSeeker);
	}
	open VOID AddMaterialsFromFile(TCHR* FilePath){
		//定位数据

		HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		hFile = FindFileChunk(hFile, ASSET_MATERIAL);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		//读取数据

		UINT MtlCount;
		MTRLEX Material;
		CMtlList *pMtlList;

		pMtlList = (CMtlList*)self.arrMtlID;
		ReadFile(hFile, &MtlCount, sizeof(UINT), NULL, NULL);
		for(UINT i = 0; i < MtlCount; ++i){
			ReadFile(hFile, &Material, sizeof(MTRLEX), NULL, NULL);
			pMtlList->push_back(Material);
		}

		//关闭文件
done:
		CloseHandle(hFile);
	}
	hide VOID CreateTextureViews(ID3DDevice* lpD3DDevice){
		HD3DVIEW hTexView;
		D3DResDesc TexDesc;
		D3DSrvDesc ViewDesc;
		D3DViewHeapDesc HeapDesc;

		//创建视图堆

		HeapDesc.NodeMask = 0x0;
		HeapDesc.NumDescriptors = self.TexCount;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		lpD3DDevice->CreateDescriptorHeap(&HeapDesc, COM_ARGS(self.lpSRVHeap));

		//创建视图

		ViewDesc.Texture2D.PlaneSlice = 0;
		ViewDesc.Texture2D.MostDetailedMip = 0;
		ViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		ViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		hTexView = self.lpSRVHeap->GetCPUDescriptorHandleForHeapStart();
		for(UINT i = 0; i < self.TexCount; ++i){
			TexDesc = *(D3DResDesc*)&self.arrTexture[i]->GetDesc();
			ViewDesc.Format = TexDesc.Format;
			ViewDesc.Texture2D.MipLevels = TexDesc.MipLevels;
			lpD3DDevice->CreateShaderResourceView(self.arrTexture[i], &ViewDesc, hTexView);
			hTexView.Offset(1, ge::cbCSUView);
		}
	}
	open VOID SubmitResources(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch){
		CMtlList *pMtlList = (CMtlList*)self.arrMtlID;
		CTexList *pTexList = (CTexList*)self.arrTexID;

		//获取纹理和材质数量

		self.MtlCount = (UINT)pMtlList->size();
		self.TexCount = (UINT)pTexList->size();
		self.arrMtlID = new UINT[self.MtlCount];
		self.arrTexID = new UINT[self.TexCount];
		self.arrTexture = new ID3DResource*[self.TexCount];

		//保存纹理

		for(UINT i = 0; i < self.TexCount; ++i){
			self.arrTexID[i] = pTexList->at(i).Ident;
			self.arrTexture[i] = pTexList->at(i).lpBuffer;
		}

		//上传材质

		if(self.MtlCount > 0){
			MATERIAL *arrMtrl = new MATERIAL[self.MtlCount];

			for(UINT i = 0; i < self.MtlCount; ++i){
				self.arrMtlID[i] = pMtlList->at(i).Ident;
				arrMtrl[i] = pMtlList->at(i).Base;
			}

			dx::CreateStaticBuffer(lpD3DDevice, ULBatch, arrMtrl, sizeof(MATERIAL) * self.MtlCount,
				1, D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpMtlBuffer);
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
		hView.Offset(Index, ge::cbCSUView);
		return hView;
	}
	open HGPUBUFF GetMaterialGPUAddress(UINT Index){
		HGPUBUFF Handle = self.lpMtlBuffer->GetGPUVirtualAddress();
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
	open UINT cbVtxBuffer;
	open UINT cbIdxBuffer;
	open SUBMESH *arrSubset;
	open MESHMAP *arrMapping;
	open ID3DResource *lpVtxBuffer;
	open ID3DResource *lpIdxBuffer;
	open DXGIFormat IdxFormat;
	open VERTFORMAT VtxFormat;

	open ~CMesh(){
		self.Finalize();
	}
	open CMesh(){
		MemZero(this, sizeof(*this));
	}
	open CMesh(ID3DDevice* lpD3DDevice, HANDLE hFile, DXTKUploadBatch &ULBatch){
		self.Initialize(lpD3DDevice, hFile, ULBatch);
	}

	open VOID Initialize(ID3DDevice* lpD3DDevice, HANDLE hFile, DXTKUploadBatch &ULBatch){
		UINT StrSize, BuffSize;
		UINT MtlCount, MapCount;
		UINT VtxCount, IdxCount;
		DWRD VtxFormat, BuffAttr;
		BYTE *VtxBuffer, *IdxBuffer;

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
		self.IdxFormat = (VtxCount > UINT2_MAX) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;

		self.cbVtxBuffer = self.GetVertexByteCount() * VtxCount;
		self.cbIdxBuffer = self.GetIndexByteCount() * IdxCount;

		//读取顶点和索引

		VtxBuffer = new BYTE[self.cbVtxBuffer];
		IdxBuffer = new BYTE[self.cbIdxBuffer];

		ReadFile(hFile, VtxBuffer, self.cbVtxBuffer, NULL, NULL);
		ReadFile(hFile, IdxBuffer, self.cbIdxBuffer, NULL, NULL);

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

		//创建缓存

		BuffAttr = (VtxFormat & 0x80000000) ?
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

		dx::CreateStaticBuffer(lpD3DDevice, ULBatch, VtxBuffer, self.cbVtxBuffer, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpVtxBuffer, (D3DResFlag)BuffAttr);
		dx::CreateStaticBuffer(lpD3DDevice, ULBatch, IdxBuffer, self.cbIdxBuffer, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpIdxBuffer);
	}
	open VOID Finalize(){
		if(!self.lpVtxBuffer) return;

		DELETE_ARR(self.Name);
		DELETE_ARR(self.arrSubset);
		DELETE_ARR(self.arrMapping);

		RELEASE_REF(self.lpVtxBuffer);
		RELEASE_REF(self.lpIdxBuffer);

		MemZero(this, sizeof(*this));
	}
	open UINT GetIndexCount(){
		return self.cbIdxBuffer / self.GetIndexByteCount();
	}
	open UINT GetVertexCount(){
		return self.cbVtxBuffer / self.GetVertexByteCount();
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
		IBView.BufferLocation = self.lpIdxBuffer->GetGPUVirtualAddress();
		IBView.SizeInBytes = self.cbIdxBuffer;
		IBView.Format = self.IdxFormat;
		return IBView;
	}
	open D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuffer->GetGPUVirtualAddress();
		VBView.StrideInBytes = self.GetVertexByteCount();
		VBView.SizeInBytes = self.cbVtxBuffer;
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
		MemZero(this, sizeof(*this));
	}
	open CModel(ID3DDevice* lpD3DDevice, TCHR* FilePath, DXTKUploadBatch &ULBatch){
		self.Initialize(lpD3DDevice, FilePath, ULBatch);
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
	open VOID Initialize(ID3DDevice* lpD3DDevice, TCHR* FilePath, DXTKUploadBatch &ULBatch){
		//定位数据

		HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		hFile = FindFileChunk(hFile, ASSET_MESH);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		//读取数据

		ReadFile(hFile, &self.MeshCount, sizeof(UINT), NULL, NULL);
		self.FirstMesh = new CMesh(lpD3DDevice, hFile, ULBatch);

		CMesh *pMesh = self.FirstMesh;
		for(UINT i = 1; i < self.MeshCount; ++i){
			pMesh->Next = new CMesh(lpD3DDevice, hFile, ULBatch);
			pMesh = pMesh->Next;
		}

		//关闭文件
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
		UINT TexIdent = StringHash4A(TexName);
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
		MemZero(this, sizeof(*this));
	}
	open CSkeleton(ID3DDevice* lpD3DDevice, TCHR* FilePath){
		self.Initialize(lpD3DDevice, FilePath);
	}

	open VOID Initialize(ID3DDevice* lpD3DDevice, TCHR* FilePath){
		//定位数据

		HANDLE hFile = CreateFile(FilePath, GENERIC_READ, 0x0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		hFile = FindFileChunk(hFile, ASSET_BONE);
		if(hFile == INVALID_HANDLE_VALUE) goto done;

		//读取数据

		BONE *pBone;
		UINT StrSize;

		ReadFile(hFile, &self.numBone, sizeof(UINT), NULL, NULL);
		if(self.numBone == 0) return;

		self.arrBone = new BONE[self.numBone];
		self.lpTMBuffer = CreateUploadBuffer(lpD3DDevice, sizeof(MATRIX) * self.numBone, TRUE);
		self.lpTMBuffer->Map(0, NULL, (VOID**)&self.arrTrans);

		for(UINT i = 0; i < self.numBone; ++i){
			pBone = &self.arrBone[i];
			ReadFile(hFile, &StrSize, sizeof(UINT), NULL, NULL);
			ReadFile(hFile, pBone->Name, sizeof(CHAR) * StrSize, NULL, NULL);
			ReadFile(hFile, &pBone->nParent, sizeof(UINT), NULL, NULL);
			ReadFile(hFile, &pBone->matOffset, sizeof(MATRIX), NULL, NULL);
			ReadFile(hFile, &pBone->matInitial, sizeof(MATRIX), NULL, NULL);
			pBone->matTrans = pBone->matInitial;
			pBone->matCombine = MatIdentity();
			pBone->Name[StrSize] = '\0';
		}
done:
		CloseHandle(hFile);
	}
	open VOID Finalize(){
		RESET_ARR(self.arrBone);
		RESET_REF(self.lpTMBuffer);
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
		for(UINT i = 1; i < self.numBone; ++i)
			self.arrBone[i].matTrans = self.arrBone[i].matInitial;
	}
};

//----------------------------------------//


//--------------------变形器--------------------//

//变形管道状态对象
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
			lpSerialize->GetBufferSize(), COM_ARGS(Pso.lpRootSig));

		lpSerialize->Release();

		//创建PSO

		MemZero(&PSODesc, sizeof(D3DComputePipeStateDesc));
		PSODesc.pRootSignature = Pso.lpRootSig;
		PSODesc.CS = { lpShadeCode->GetBufferPointer(), lpShadeCode->GetBufferSize() };
		PSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		lpD3DDevice->CreateComputePipelineState(&PSODesc, COM_ARGS(Pso.lpPipeState));
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
		MemZero(this, sizeof(*this));
	}
	CMorpher(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, TCHR *FilePath){
		self.Initialize(lpD3DDevice, ULBatch, FilePath);
	}

	VOID Finalize(){
		RESET_REF(self.arrMesh);
	}
	VOID Initialize(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, TCHR *FilePath){
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

		dx::CreateStaticBuffer(lpD3DDevice, ULBatch, Buffer, cbBuffer, 1,
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
		HGPUBUFF hDestBuff = Dest->lpVtxBuffer->GetGPUVirtualAddress();

		SETTING Setting;
		Setting.MorphLerp = Lerp;
		Setting.bWithNormal = Dest->VtxFormat & VERT_NORMAL;
		Setting.cbVertex = Dest->GetVertexByteCount();
		Setting.VertCount = Dest->cbVtxBuffer / Setting.cbVertex;
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
struct SkyDome{
	UINT cbVtxBuffer;
	UINT cbIdxBuffer;
	ID3DRootSig *lpRootSig;
	ID3DPipeState *lpPipeState;
	ID3DViewHeap *lpSRVHeap;
	ID3DResource *lpTexture;
	ID3DResource *lpVtxBuffer;
	ID3DResource *lpIdxBuffer;

	~SkyDome(){
		self.Finalize();
	}
	SkyDome(){
		MemZero(this, sizeof(*this));
	}
	SkyDome(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, WCHR* TexPath, SPFP Diameter, D3DGfxPipeStateDesc &PSODesc){
		self.Initialize(lpD3DDevice, ULBatch, TexPath, Diameter, PSODesc);
	}

	VOID Initialize(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, WCHR* TexPath, SPFP Diameter, D3DGfxPipeStateDesc &PSODesc){
		self.CreateMesh(lpD3DDevice, ULBatch, Diameter);
		self.CreateTexture(lpD3DDevice, ULBatch, TexPath);
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
			lpSerialize->GetBufferSize(), COM_ARGS(self.lpRootSig));

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

		lpD3DDevice->CreateGraphicsPipelineState(pPSODesc, COM_ARGS(self.lpPipeState));

		lpVSCode->Release();
		lpPSCode->Release();
	}
	VOID CreateMesh(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, SPFP Diameter){
		USRT *arrIndex;
		VECTOR3 *arrVertex;
		UINT VtxCount, IdxCount;
		std::vector<USRT> IndexList;
		std::vector<DXTKGeomPrim::VertexType> VertList;

		DXTKGeomPrim::CreateSphere(VertList, IndexList, Diameter, 8, FALSE);

		VtxCount = (UINT)VertList.size();
		IdxCount = (UINT)IndexList.size();

		arrVertex = new VECTOR3[VtxCount];
		arrIndex = new USRT[IdxCount];

		for(UINT i = 0; i < VtxCount; ++i)
			arrVertex[i] = VertList[i].position;
		for(UINT i = 0; i < IdxCount; ++i)
			arrIndex[i] = IndexList[i];

		self.cbVtxBuffer = sizeof(VECTOR3) * VtxCount;
		self.cbIdxBuffer = sizeof(USRT) * VtxCount;

		dx::CreateStaticBuffer(lpD3DDevice, ULBatch, arrVertex, self.cbVtxBuffer, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpVtxBuffer);
		dx::CreateStaticBuffer(lpD3DDevice, ULBatch, arrIndex, self.cbIdxBuffer, 1,
			D3D12_RESOURCE_STATE_GENERIC_READ, &self.lpIdxBuffer);
	}
	VOID CreateTexture(ID3DDevice* lpD3DDevice, DXTKUploadBatch &ULBatch, WCHR* FilePath){
		HD3DVIEW hTexView;
		D3DResDesc tTexDesc;
		D3DSrvDesc tViewDesc;
		D3DViewHeapDesc tHeapDesc;

		//创建纹理

		dx::CreateDDSTextureFromFile(lpD3DDevice, ULBatch, FilePath, &self.lpTexture);

		//创建视图堆

		tHeapDesc.NodeMask = 0;
		tHeapDesc.NumDescriptors = 1;
		tHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		tHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		lpD3DDevice->CreateDescriptorHeap(&tHeapDesc, COM_ARGS(self.lpSRVHeap));

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
		if(!self.lpVtxBuffer) return;

		RELEASE_REF(self.lpRootSig);
		RELEASE_REF(self.lpPipeState);
		RELEASE_REF(self.lpSRVHeap);
		RELEASE_REF(self.lpTexture);
		RELEASE_REF(self.lpVtxBuffer);
		RELEASE_REF(self.lpIdxBuffer);

		MemZero(this, sizeof(*this));
	}
	HGPUVIEW GetTextureView(){
		return (HGPUVIEW)self.lpSRVHeap->GetGPUDescriptorHandleForHeapStart();
	}
	D3DIdxBuffView GetIndexBufferView(){
		D3DIdxBuffView IBView;
		IBView.BufferLocation = self.lpIdxBuffer->GetGPUVirtualAddress();
		IBView.SizeInBytes = self.cbIdxBuffer;
		IBView.Format = DXGI_FORMAT_R16_UINT;
		return IBView;
	}
	D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuffer->GetGPUVirtualAddress();
		VBView.StrideInBytes = sizeof(VECTOR3);
		VBView.SizeInBytes = self.cbVtxBuffer;
		return VBView;
	}
};

//----------------------------------------//


//--------------------粒子系统--------------------//

//粒子属性
enum PTCLATTR{
	PARTICLE_COORD = 0x0001,   //坐标
	PARTICLE_TEXTURE = 0x0002, //纹理
	PARTICLE_ARGB = 0x0004,    //颜色值
	PARTICLE_SIZE = 0x0008,    //尺寸
	PARTICLE_DSIZE = 0x0010,   //尺寸差
	PARTICLE_AGE = 0x0020,     //年龄
	PARTICLE_LIFE = 0x0040,    //寿命
	PARTICLE_TIMER = 0x0080,   //计时
	PARTICLE_VELOC = 0x0100,   //速度
	PARTICLE_ACCEL = 0x0200,   //加速度
	PARTICLE_DACCEL = 0x0400,  //加速度差
	PARTICLE_COLOR = 0x0800,   //颜色
	PARTICLE_DCOLOR = 0x1000,  //颜色差
};

//粒子视图
struct CPtcView{
	struct HomeBuff{
		DWRD PtcFormat;
		UINT PtcIndex;
		UINT PtcCount;
		UINT PtcMaxCnt;
		UINT PtcByteCnt;
		SIZE PtcSize;
		BYTE *PtcBuffer;
	};

	BYTE *Address;
	UINT ByteCount;
	UCHR hCoord;
	UCHR hSize;
	UCHR hColor32;
	UCHR hTexCoord;
	UCHR hAge;
	UCHR hLife;
	UCHR hTimer;
	UCHR hDifSize;
	UCHR hVeloc;
	UCHR hAccel;
	UCHR hDifAccel;
	UCHR hColor;
	UCHR hDifColor;
	UCHR hState;

	UINT Initialize(DWRD Format){
		MemZero(this, sizeof(*this));

		if(Format & PARTICLE_COORD){
			FORCE_CVT(self.ByteCount, self.hCoord);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_SIZE){
			FORCE_CVT(self.ByteCount, self.hSize);
			self.ByteCount += sizeof(VECTOR2);
		}
		if(Format & PARTICLE_ARGB){
			FORCE_CVT(self.ByteCount, self.hColor32);
			self.ByteCount += sizeof(ARGB);
		}
		if(Format & PARTICLE_TEXTURE){
			FORCE_CVT(self.ByteCount, self.hTexCoord);
			self.ByteCount += sizeof(VECTOR2);
		}
		if(Format & PARTICLE_AGE){
			FORCE_CVT(self.ByteCount, self.hAge);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_LIFE){
			FORCE_CVT(self.ByteCount, self.hLife);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_TIMER){
			FORCE_CVT(self.ByteCount, self.hTimer);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_DSIZE){
			FORCE_CVT(self.ByteCount, self.hDifSize);
			self.ByteCount += sizeof(SPFP);
		}
		if(Format & PARTICLE_VELOC){
			FORCE_CVT(self.ByteCount, self.hVeloc);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_ACCEL){
			FORCE_CVT(self.ByteCount, self.hAccel);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_DACCEL){
			FORCE_CVT(self.ByteCount, self.hDifAccel);
			self.ByteCount += sizeof(VECTOR3);
		}
		if(Format & PARTICLE_COLOR){
			FORCE_CVT(self.ByteCount, self.hColor);
			self.ByteCount += sizeof(COLOR128);
		}
		if(Format & PARTICLE_DCOLOR){
			FORCE_CVT(self.ByteCount, self.hDifColor);
			self.ByteCount += sizeof(COLOR128);
		}

		FORCE_CVT(self.ByteCount, self.hState);
		self.ByteCount += sizeof(bool);

		return self.ByteCount;
	}
	VOID SetPointer(VOID* Address){
		self.Address = (BYTE*)Address;
	}
	VOID OffsetPointer(INT4 Offset){
		self.Address += (INT4)self.ByteCount * Offset;
	}
	VOID Kill(){
		HomeBuff *pHomeBuff = (HomeBuff*)(this + 1);
		pHomeBuff->PtcCount--;
		self.Valid() = FALSE;
	}
	bool &Valid(){
		BYTE *pMember = self.Address + self.hState;
		return *((bool*)pMember);
	}
	SPFP &Age(){
		BYTE *pMember = self.Address + self.hAge;
		return *((SPFP*)pMember);
	}
	SPFP &Life(){
		BYTE *pMember = self.Address + self.hLife;
		return *((SPFP*)pMember);
	}
	SPFP &Timer(){
		BYTE *pMember = self.Address + self.hTimer;
		return *((SPFP*)pMember);
	}
	COLOR32 &Color32(){
		BYTE *pMember = self.Address + self.hColor32;
		return *((COLOR32*)pMember);
	}
	VECTOR3 &Coord(){
		BYTE *pMember = self.Address + self.hCoord;
		return *((VECTOR3*)pMember);
	}
	VECTOR3 &Veloc(){
		BYTE *pMember = self.Address + self.hVeloc;
		return *((VECTOR3*)pMember);
	}
	VECTOR3 &Accel(){
		BYTE *pMember = self.Address + self.hAccel;
		return *((VECTOR3*)pMember);
	}
	VECTOR3 &DifAccel(){
		BYTE *pMember = self.Address + self.hDifAccel;
		return *((VECTOR3*)pMember);
	}
	VECTOR2 &Size(){
		BYTE *pMember = self.Address + self.hSize;
		return *((VECTOR2*)pMember);
	}
	VECTOR2 &DifSize(){
		BYTE *pMember = self.Address + self.hDifSize;
		return *((VECTOR2*)pMember);
	}
	VECTOR2 &TexCoord(){
		BYTE *pMember = self.Address + self.hTexCoord;
		return *((VECTOR2*)pMember);
	}
	COLOR128 &Color(){
		BYTE *pMember = self.Address + self.hColor;
		return *((COLOR128*)pMember);
	}
	COLOR128 &DifColor(){
		BYTE *pMember = self.Address + self.hDifColor;
		return *((COLOR128*)pMember);
	}
};

//粒子事件
class CPtcEvent{
	open enum TYPE{
		EVENT_LIFE,
		EVENT_TIMER,
		EVENT_SIZE,
		EVENT_VELOC,
		EVENT_ACCEL,
		EVENT_COLOR,
		EVENT_TEXTURE,
		EVENT_SUBPROP,
		EVENT_SIZEX,
		EVENT_SIZEY,
		EVENT_VELOCX,
		EVENT_VELOCY,
		EVENT_VELOCZ,
		EVENT_ACCELX,
		EVENT_ACCELY,
		EVENT_ACCELZ,
		EVENT_COLORA,
		EVENT_COLORR,
		EVENT_COLORG,
		EVENT_COLORB,
		EVENT_TEXTUREU,
		EVENT_TEXTUREV,
	};

	open BOOL bFade; //渐变
	open TYPE Type; //类型
	open SPFP Time; //时间
	open SPFP Times[2]; //时间范围
	open CPtcEvent *Next; //下一个

	//解析文本(文本内容)
	open virtual VOID ParseContent(CHAR* Content) = 0;
	//更新粒子(粒子指针,间隔时间)
	open virtual VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan) = 0;

	//创建空事件(类型)[事件指针]
	static CPtcEvent* CreateEmpty(TYPE Type);
	//创建事件(类型,内容)[事件指针]
	static CPtcEvent* Create(CHAR *Action, CHAR *Time, CHAR* Type, CHAR* Content);
	//比较事件(元素0,元素1)[比较结果]
	static INT4 Compare(const VOID* pElem0, const VOID* pElem1){
		CPtcEvent *pEvent0 = *(CPtcEvent**)pElem0;
		CPtcEvent *pEvent1 = *(CPtcEvent**)pElem1;

		if(pEvent0->Type != pEvent1->Type)
			return((INT4)pEvent0->Type - (INT4)pEvent1->Type);

		if(pEvent0->Time < pEvent1->Time) return -1;
		else if(pEvent0->Time == pEvent1->Time) return 0;
		else return 1;
	}
	//解析数字(文本,@结果)
	static CHAR* ParseNumber(const CHAR* Content, SPFP Output[2]){
		CHAR szNumber[32];
		CHAR *pSrcChar, *pDestChar;

		pDestChar = szNumber;
		pSrcChar = (CHAR*)Content;

		if(!IsAlpha(*pSrcChar)){
			while((*pSrcChar == '-') || (*pSrcChar == '.') ||
				IsDigit(*pSrcChar)) *pDestChar++ = *pSrcChar++;
			*pDestChar = '\0';

			Output[0] = (SPFP)StoF(szNumber);
			Output[1] = Output[0];

			return(pSrcChar + 1);
		} else{
			pSrcChar += 5;
			pDestChar = szNumber;

			while((*pSrcChar == '-') || (*pSrcChar == '.') ||
				IsDigit(*pSrcChar)) *pDestChar++ = *pSrcChar++;
			*pDestChar = '\0';
			Output[0] = (SPFP)StoF(szNumber);

			pSrcChar++;
			pDestChar = szNumber;

			while((*pSrcChar == '-') || (*pSrcChar == '.') ||
				IsDigit(*pSrcChar)) *pDestChar++ = *pSrcChar++;
			*pDestChar = '\0';
			Output[1] = (SPFP)StoF(szNumber);

			return(pSrcChar + 2);
		}
	}
	//解析2D向量(文本,@结果)
	static VOID ParseVector2(const CHAR* Content, VECTOR2 Output[2]){
		SPFP x[2], y[2];

		Content = ParseNumber(Content, x);
		Content = ParseNumber(Content, y);

		Output[0] = { x[0], y[0] };
		Output[1] = { x[1], y[1] };
	}
	//解析3D向量(文本,@结果)
	static VOID ParseVector3(const CHAR* Content, VECTOR3 Output[2]){
		SPFP X[2], Y[2], Z[2];

		Content = ParseNumber(Content, X);
		Content = ParseNumber(Content, Y);
		Content = ParseNumber(Content, Z);

		Output[0] = { X[0], Y[0], Z[0] };
		Output[1] = { X[1], Y[1], Z[1] };
	}
	//解析颜色(文本,@结果)
	static VOID ParseColor(const CHAR* Content, COLOR128 Output[2]){
		SPFP X[2], Y[2], Z[2], w[2];

		Content = ParseNumber(Content, X);
		Content = ParseNumber(Content, Y);
		Content = ParseNumber(Content, Z);
		Content = ParseNumber(Content, w);

		Output[0] = { X[0], Y[0], Z[0], w[0] };
		Output[1] = { X[1], Y[1], Z[1], w[1] };
	}
	//解析混合模式(文本,@结果)
	static VOID ParseBlendMode(const CHAR* Content, DWRD &Output){
		if(MbsCmp(Content, "BLEND_ZERO") == 0)
			Output = D3D12_BLEND_ZERO;
		else if(MbsCmp(Content, "BLEND_ONE") == 0)
			Output = D3D12_BLEND_ONE;
		else if(MbsCmp(Content, "BLEND_SRCCOLOR") == 0)
			Output = D3D12_BLEND_SRC_COLOR;
		else if(MbsCmp(Content, "BLEND_INVSRCCOLOR") == 0)
			Output = D3D12_BLEND_INV_SRC_COLOR;
		else if(MbsCmp(Content, "BLEND_SRCALPHA") == 0)
			Output = D3D12_BLEND_SRC_ALPHA;
		else if(MbsCmp(Content, "BLEND_INVSRCALPHA") == 0)
			Output = D3D12_BLEND_INV_SRC_ALPHA;
		else if(MbsCmp(Content, "BLEND_DESTALPHA") == 0)
			Output = D3D12_BLEND_DEST_ALPHA;
		else if(MbsCmp(Content, "BLEND_INVDESTALPHA") == 0)
			Output = D3D12_BLEND_INV_DEST_ALPHA;
		else if(MbsCmp(Content, "BLEND_DESTCOLOR") == 0)
			Output = D3D12_BLEND_DEST_COLOR;
		else if(MbsCmp(Content, "BLEND_INVDESTCOLOR") == 0)
			Output = D3D12_BLEND_INV_DEST_COLOR;
		else if(MbsCmp(Content, "BLEND_SRCALPHASAT") == 0)
			Output = D3D12_BLEND_SRC_ALPHA_SAT;
		else if(MbsCmp(Content, "BLENDOP_ADD") == 0)
			Output = D3D12_BLEND_OP_ADD;
		else if(MbsCmp(Content, "BLENDOP_SUB") == 0)
			Output = D3D12_BLEND_OP_SUBTRACT;
		else if(MbsCmp(Content, "BLENDOP_REVSUB") == 0)
			Output = D3D12_BLEND_OP_REV_SUBTRACT;
		else if(MbsCmp(Content, "BLENDOP_MIN") == 0)
			Output = D3D12_BLEND_OP_MIN;
		else if(MbsCmp(Content, "BLENDOP_MAX") == 0)
			Output = D3D12_BLEND_OP_MAX;
		else if(MbsCmp(Content, "BLENDOP_OFF") == 0)
			Output = -1L;
	}
};

//粒子组
class CPtcGroup{
	open UINT Ident; //ID
	open UINT TexIdent; //纹理ID
	open UINT MtlIdent; //材质ID
	///发射器
	hide SPFP EmitWeight;   //发射计数
	open SPFP EmitRates[2]; //发射速率
	open VECTOR3 EmitRange; //发射范围
	///粒子
	open DWRD PtcFormat;    //粒子格式
	hide UINT PtcIndex;     //粒子索引
	open UINT PtcCount;     //粒子数量
	open UINT PtcMaxCnt;    //粒子最大数量
	open UINT PtcByteCnt;   //粒子字节数
	hide BYTE *PtcBuffer;   //粒子缓存
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
		MemZero(this, sizeof(*this));
	}

	open VOID Finalize(){
		if(!self.PtcBuffer) return;

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
		DELETE_ARR(self.PtcBuffer);

		MemZero(this, sizeof(*this));
	}
	open VOID Initialize(TiXmlElement* pXMLNode){
		//解析ID

		TiXmlAttribute *pNodeAttr = pXMLNode->FirstAttribute();

		if(MbsCmp(pNodeAttr->Name(), "name") == 0)
			self.Ident = StringHash4A((CHAR*)pNodeAttr->Value());

		//解析其它数据

		VECTOR2 Vec2Range[2];
		VECTOR3 Vec3Range[2];
		COLOR128 ColorRange[2];

		pXMLNode = pXMLNode->FirstChildElement();
		while(pXMLNode){
			if(MbsCmp(pXMLNode->Value(), "numparticles") == 0){
				self.PtcMaxCnt = StoI(pXMLNode->GetText());
			} else if(MbsCmp(pXMLNode->Value(), "texture") == 0){
				self.TexIdent = StringHash4A((CHAR*)pXMLNode->GetText());
			} else if(MbsCmp(pXMLNode->Value(), "events") == 0){
				self.ParseEvents(pXMLNode);
			} else if(MbsCmp(pXMLNode->Value(), "emitrate") == 0){
				CPtcEvent::ParseNumber(pXMLNode->GetText(), self.EmitRates);
			} else if(MbsCmp(pXMLNode->Value(), "emitrange") == 0){
				CPtcEvent::ParseVector3(pXMLNode->GetText(), Vec3Range);
				Vec3Sample(self.EmitRange, Vec3Range);
			} else if(MbsCmp(pXMLNode->Value(), "psize") == 0){
				CPtcEvent::ParseVector2(pXMLNode->GetText(), Vec2Range);
				Vec2Sample(self.PtcSize, Vec2Range);
			} else if(MbsCmp(pXMLNode->Value(), "color") == 0){
				CPtcEvent::ParseColor(pXMLNode->GetText(), ColorRange);
				ColorSample(self.PtcColor, ColorRange);
			}
			pXMLNode = pXMLNode->NextSiblingElement();
		}
	}
	hide VOID ParseEvents(TiXmlElement* pXMLNode){
		TiXmlAttribute *pNodeAttr;
		CPtcEvent **arrEvent;
		UINT numEvent, nEvent = 0;
		CHAR *Action, *Time, *Type, *Value;

		//解析事件数量

		pNodeAttr = pXMLNode->FirstAttribute();
		numEvent = StoI(pNodeAttr->Value());
		arrEvent = new CPtcEvent*[numEvent];

		//解析事件

		pXMLNode = pXMLNode->FirstChildElement();
		while(pXMLNode){
			pNodeAttr = pXMLNode->FirstAttribute();
			while(pNodeAttr){
				if(MbsCmp(pNodeAttr->Name(), "time") == 0)
					Time = (CHAR*)pNodeAttr->Value();
				else if(MbsCmp(pNodeAttr->Name(), "type") == 0)
					Type = (CHAR*)pNodeAttr->Value();
				pNodeAttr = pNodeAttr->Next();
			}

			Action = (CHAR*)pXMLNode->Value();
			Value = (CHAR*)pXMLNode->GetText();

			arrEvent[nEvent++] = CPtcEvent::Create(Action, Time, Type, Value);
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//优化事件列表

		self.CreateEventList(arrEvent, numEvent);
		DELETE_ARR(arrEvent);
	}
	hide VOID CreateEventList(CPtcEvent **arrEvent, UINT numEvent){
		DWRD LastType;
		UINT TypeCount;
		CPtcEvent *pEvent;

		//选取事件发生时间

		for(UINT i = 0; i < numEvent; ++i)
			arrEvent[i]->Time = RealSample(arrEvent[i]->Times);

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
			if(pEvent->Type > CPtcEvent::EVENT_SUBPROP){
				pEvent = CPtcEvent::CreateEmpty(self.arrTopEvent[i]->Type);
				pEvent->Next = self.arrTopEvent[i];
				self.arrTopEvent[i] = pEvent;
			}
		}

		//创建粒子缓存区

		self.CreateParticleBuffer();
	}
	hide VOID CreateParticleBuffer(){
		//根据事件类型给粒子分配属性

		self.PtcFormat = PARTICLE_COORD;
		self.PtcFormat |= PARTICLE_TIMER;

		CPtcEvent *pEvent;
		for(UINT i = 0; i < self.numTopEvent; ++i){
			pEvent = self.arrTopEvent[i];
			while(pEvent->Next){
				pEvent = pEvent->Next;
				if(pEvent->bFade) break;
			}

			switch(pEvent->Type){
				case CPtcEvent::EVENT_SIZE:{
					self.PtcFormat |= PARTICLE_SIZE;
					if(pEvent->bFade) self.PtcFormat |= PARTICLE_DSIZE;
				} break;
				case CPtcEvent::EVENT_LIFE:{
					self.PtcFormat |= PARTICLE_LIFE;
					self.PtcFormat |= PARTICLE_AGE;
				} break;
				case CPtcEvent::EVENT_TIMER:{
					self.PtcFormat |= PARTICLE_TIMER;
				} break;
				case CPtcEvent::EVENT_TEXTURE:
				case CPtcEvent::EVENT_TEXTUREU:
				case CPtcEvent::EVENT_TEXTUREV:{
					self.PtcFormat |= PARTICLE_TEXTURE;
				} break;
				case CPtcEvent::EVENT_COLOR:
				case CPtcEvent::EVENT_COLORR:
				case CPtcEvent::EVENT_COLORG:
				case CPtcEvent::EVENT_COLORB:
				case CPtcEvent::EVENT_COLORA:{
					self.PtcFormat |= PARTICLE_ARGB;
					self.PtcFormat |= PARTICLE_COLOR;
					if(pEvent->bFade) self.PtcFormat |= PARTICLE_DCOLOR;
				} break;
				case CPtcEvent::EVENT_VELOC:
				case CPtcEvent::EVENT_VELOCX:
				case CPtcEvent::EVENT_VELOCY:
				case CPtcEvent::EVENT_VELOCZ:{
					self.PtcFormat |= PARTICLE_VELOC;
					if(pEvent->bFade) self.PtcFormat |= PARTICLE_ACCEL;
				} break;
				case CPtcEvent::EVENT_ACCEL:
				case CPtcEvent::EVENT_ACCELX:
				case CPtcEvent::EVENT_ACCELY:
				case CPtcEvent::EVENT_ACCELZ:{
					self.PtcFormat |= PARTICLE_ACCEL;
					if(pEvent->bFade) self.PtcFormat |= PARTICLE_DACCEL;
				} break;
			}
		}

		//根据粒子属性分配内存空间

		self.PtcByteCnt = self.PtcView.Initialize(self.PtcFormat);
		self.PtcBuffer = new BYTE[self.PtcByteCnt * self.PtcMaxCnt]{ 0 };
		self.PtcView.SetPointer(self.PtcBuffer);
	}
	hide VOID GenParticle(){
		CPtcView *pPtcView = self.GetParticleView(self.PtcIndex);

		for(UINT i = 0; i < self.PtcMaxCnt; ++i){
			if(++self.PtcIndex >= self.PtcMaxCnt){
				self.PtcIndex = 0;
				pPtcView->SetPointer(self.PtcBuffer);
			} else pPtcView->OffsetPointer(1);

			if(!pPtcView->Valid()){
				self.PtcCount++;
				break;
			}
		}

		MemZero(pPtcView->Address, self.PtcByteCnt);
		RandVec3(pPtcView->Coord(), VECTOR3(0.0f, 0.0f, 0.0f), self.EmitRange);
		pPtcView->Valid() = TRUE;
	}
	open VOID Update(SPFP TimeSpan){
		SPFP Timer;
		SPFP EmitRate;
		UINT EmitWeight;
		CPtcView *pPtcView;
		CPtcEvent *pEvent, *NextEvent;

		pPtcView = self.GetParticleView(0);
		pPtcView->OffsetPointer(-1);

		for(UINT i = 0; i < self.PtcMaxCnt; ++i){
			pPtcView->OffsetPointer(1);
			if(!pPtcView->Valid()) continue;

			pPtcView->Timer() += TimeSpan;
			Timer = pPtcView->Timer();

			for(UINT j = 0; j < self.numTopEvent; ++j){
				pEvent = self.arrTopEvent[j];
				NextEvent = pEvent->Next;

				while(pEvent){
					if((Timer >= pEvent->Time) && (!NextEvent || (Timer < NextEvent->Time))){
						pEvent->UpdateParticle(*pPtcView, TimeSpan);
						break;
					} else{
						pEvent = NextEvent;
						if(pEvent) NextEvent = pEvent->Next;
					}
				}
			}
		}

		if(self.PtcCount < self.PtcMaxCnt){
			EmitRate = RealSample(self.EmitRates);
			self.EmitWeight += EmitRate * TimeSpan;

			if(self.EmitWeight >= 1.0f){
				EmitWeight = (UINT)self.EmitWeight;
				self.EmitWeight -= (SPFP)EmitWeight;

				if((EmitWeight + self.PtcCount) > self.PtcMaxCnt)
					EmitWeight = self.PtcMaxCnt - self.PtcCount;

				for(UINT i = 0; i < EmitWeight; ++i)
					self.GenParticle();
			}
		}
	}
	open open CPtcView* GetParticleView(UINT Index){
		self.PtcView.Address = self.PtcBuffer;
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
	open DWRD fDestBlend; //目标混合模式
	///粒子组
	open UINT numSubset;       //子集数量
	open CPtcGroup *arrSubset; //子集数组
	///顶点缓存区
	open DWRD VtxFormat;   //顶点格式
	open UINT cbVertex;    //顶点字节数
	open UINT cbVtxBuffer; //缓存区大小
	open ID3DResource *lpVtxBuffer; //顶点缓存区

	open ~CPtcSystem(){
		self.Finalize();
	}
	open CPtcSystem(){
		MemZero(this, sizeof(*this));
	}
	open CPtcSystem(ID3DDevice* lpD3DDevice, CHAR* ScriptPath){
		self.Initialize(lpD3DDevice, ScriptPath);
	}

	open VOID Finalize(){
		RESET_ARR(self.arrSubset);
		RESET_REF(self.lpVtxBuffer);
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, CHAR* ScriptPath){
		//设置默认值

		self.fBlendOP = D3D12_BLEND_OP_ADD;
		self.fSrcBlend = D3D12_BLEND_SRC_ALPHA;
		self.fDestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		//读取脚本

		TiXmlElement *pXMLNode;
		TiXmlDocument XMLScene(ScriptPath);

		XMLScene.LoadFile();
		pXMLNode = XMLScene.RootElement();
		if(MbsCmp(pXMLNode->Value(), "psystem") != 0) return;
		pXMLNode = pXMLNode->FirstChildElement();

		//解析基本数据

		while(pXMLNode){
			if(MbsCmp(pXMLNode->Value(), "numsubsets") == 0){
				self.numSubset = StoI(pXMLNode->GetText());
				self.arrSubset = new CPtcGroup[self.numSubset];
			} else if(MbsCmp(pXMLNode->Value(), "srcblend") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fSrcBlend);
			} else if(MbsCmp(pXMLNode->Value(), "dstblend") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fDestBlend);
			} else if(MbsCmp(pXMLNode->Value(), "blendop") == 0){
				CPtcEvent::ParseBlendMode(pXMLNode->GetText(), self.fBlendOP);
			} else if(MbsCmp(pXMLNode->Value(), "subset") == 0){
				break;
			}
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//解析粒子组

		for(UINT i = 0; i < self.numSubset; ++i){
			self.arrSubset[i].Initialize(pXMLNode);
			pXMLNode = pXMLNode->NextSiblingElement();
		}

		//创建顶点缓存和材质

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
		if(PtcFormat & PARTICLE_TEXTURE)
			self.VtxFormat |= VERT_TEXCOORD;

		//创建顶点缓存

		CVertView VertView(self.VtxFormat);

		self.cbVertex = VertView.ByteCount;
		self.cbVtxBuffer = self.cbVertex * self.GetParticleMaxCount();
		self.lpVtxBuffer = CreateUploadBuffer(lpD3DDevice, self.cbVtxBuffer, FALSE);
	}
	open VOID LinkTextures(UINT* TexSeq, UINT SeqSize){
		for(UINT i = 0; i < self.numSubset; ++i){
			for(UINT j = 0; j < SeqSize; ++j){
				if(self.arrSubset[i].TexIdent == TexSeq[j]){
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
		BYTE *VtxBuffer;
		CPtcView *pPtcView;
		CPtcGroup *pPtcGroup;

		self.lpVtxBuffer->Map(0, NULL, (VOID**)&VtxBuffer);

		for(UINT i = 0; i < self.numSubset; ++i){
			pPtcGroup = &self.arrSubset[i];
			if(pPtcGroup->PtcCount == 0) continue;

			pPtcView = pPtcGroup->GetParticleView(0);
			for(UINT j = 0; j < pPtcGroup->PtcMaxCnt; ++j){
				if(pPtcView->Valid()){
					MemCopy(VtxBuffer, pPtcView->Address, self.cbVertex);
					VtxBuffer += self.cbVertex;
				}
				pPtcView->OffsetPointer(1);
			}
		}

		self.lpVtxBuffer->Unmap(0, NULL);
	}
	open UINT GetParticleCount(){
		UINT PtcCount = 0;
		for(UINT i = 0; i < self.numSubset; ++i)
			PtcCount += self.arrSubset[i].PtcCount;
		return PtcCount;
	}
	open UINT GetParticleMaxCount(){
		UINT PtcCount = 0;
		for(UINT i = 0; i < self.numSubset; ++i)
			PtcCount += self.arrSubset[i].PtcMaxCnt;
		return PtcCount;
	}
	open D3DVertBuffView GetVertexBufferView(){
		D3DVertBuffView VBView;
		VBView.BufferLocation = self.lpVtxBuffer->GetGPUVirtualAddress();
		VBView.StrideInBytes = self.cbVertex;
		VBView.SizeInBytes = self.cbVtxBuffer;
		return VBView;
	}
	open D3DRenderTargetBlendDesc GetBlendDesc(){
		D3DRenderTargetBlendDesc BlendDesc = { FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };

		if(self.fBlendOP != -1){
			BlendDesc.BlendEnable = TRUE;
			BlendDesc.BlendOp = (D3DBlendOP)self.fBlendOP;
			BlendDesc.SrcBlend = (D3DBlendMode)self.fSrcBlend;
			BlendDesc.DestBlend = (D3DBlendMode)self.fDestBlend;
		}

		return BlendDesc;
	}
};

//----------------------------------------//


//--------------------粒子事件--------------------//

//标量型事件
struct PtcScalarEvent: public CPtcEvent{
	SPFP Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseNumber(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		UINT Offset = 0;
		SPFP *pDest, *pDestStep;
		SPFP TimeDiff, NextDest;
		BOOL bFadeOut = self.Next && self.Next->bFade;
		PtcScalarEvent *NextEvent = (PtcScalarEvent*)self.Next;

		switch(self.Type){
			case EVENT_SIZEY: Offset++;
			case EVENT_SIZEX:{
				pDest = (SPFP*)&iPtcView.Size() + Offset;;
				if(bFadeOut) pDestStep = (SPFP*)&iPtcView.DifSize() + Offset;;
			} break;
			case EVENT_TEXTUREV: Offset++;
			case EVENT_TEXTUREU:{
				pDest = (SPFP*)&iPtcView.TexCoord() + Offset;
			} break;
			case EVENT_VELOCZ: Offset++;
			case EVENT_VELOCY: Offset++;
			case EVENT_VELOCX:{
				pDest = (SPFP*)&iPtcView.Veloc() + Offset;
				if(bFadeOut) pDestStep = (SPFP*)&iPtcView.Accel() + Offset;
				*((SPFP*)&iPtcView.Coord() + Offset) += *pDest * TimeSpan;
			} break;
			case EVENT_ACCELZ: Offset++;
			case EVENT_ACCELY: Offset++;
			case EVENT_ACCELX:{
				pDest = (SPFP*)&iPtcView.Accel() + Offset;
				if(bFadeOut) pDestStep = (SPFP*)&iPtcView.DifAccel() + Offset;
			} break;
			case EVENT_COLORA: Offset++;
			case EVENT_COLORB: Offset++;
			case EVENT_COLORG: Offset++;
			case EVENT_COLORR:{
				pDest = (SPFP*)&iPtcView.Color() + Offset;
				if(bFadeOut) pDestStep = (SPFP*)&iPtcView.DifAccel() + Offset;
			} break;
		}

		if((iPtcView.Timer() - TimeSpan) <= self.Time){
			if(!self.bFade)
				*pDest = RealSample(self.Params);
			if(bFadeOut){
				if(self.Next->Time != REAL4_MAX)
					TimeDiff = self.Next->Time - self.Time;
				else TimeDiff = iPtcView.Life() - self.Time;

				NextDest = RealSample(NextEvent->Params);
				*pDestStep = (NextDest - *pDest) / TimeDiff;
			}
		} else if(bFadeOut) *pDest += *pDestStep * TimeSpan;
	}
};
//向量型事件
struct PtcVectorEvent: public CPtcEvent{
	VECTOR3 Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseVector3(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		using namespace dx;

		BOOL bFadeOut;
		SPFP TimeDiff;
		XVECTOR Vector0, Vector1, VecStep;
		VECTOR3 *pDest, *pDestStep, NextDest;
		PtcVectorEvent *NextEvent = (PtcVectorEvent*)self.Next;

		bFadeOut = self.Next && self.Next->bFade;

		if(self.Type == EVENT_VELOC){
			pDest = &iPtcView.Veloc();
			if(bFadeOut) pDestStep = &iPtcView.Accel();
			VecStep = PackV3(*pDest);
			Vector0 = PackV3(iPtcView.Coord());
			Vector0 += VecStep * TimeSpan;
			UnpackV3(iPtcView.Coord(), Vector0);
		} else if(self.Type == EVENT_ACCEL){
			pDest = &iPtcView.Accel();
			if(bFadeOut) pDestStep = &iPtcView.DifAccel();
		}

		if((iPtcView.Timer() - TimeSpan) <= self.Time){
			if(!self.bFade) Vec3Sample(*pDest, self.Params);
			if(bFadeOut){
				Vec3Sample(NextDest, NextEvent->Params);

				if(self.Next->Time != REAL4_MAX)
					TimeDiff = self.Next->Time - self.Time;
				else TimeDiff = iPtcView.Life() - self.Time;

				Vector0 = PackV3(*pDest);
				Vector1 = PackV3(NextDest);
				VecStep = (Vector1 - Vector0) / TimeDiff;
				UnpackV3(*pDestStep, VecStep);
			}
		} else if(bFadeOut){
			VecStep = PackV3(*pDestStep);
			Vector0 = PackV3(*pDest);
			Vector0 += VecStep * TimeSpan;
			UnpackV3(*pDest, Vector0);
		}
	}
};
//颜色型事件
struct PtcColorEvent: public CPtcEvent{
	COLOR128 Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseColor(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		using namespace dx;

		BOOL bFadeOut;
		SPFP TimeDiff;
		COLOR128 NextColor;
		XVECTOR Vector0, Vector1, VecStep;
		PtcColorEvent *NextEvent = (PtcColorEvent*)self.Next;

		bFadeOut = self.Next && self.Next->bFade;

		if((iPtcView.Timer() - TimeSpan) <= self.Time){
			if(!self.bFade){
				ColorSample(iPtcView.Color(), self.Params);
				ColorConvert32(iPtcView.Color32(), PackF4(iPtcView.Color()));
			}
			if(bFadeOut){
				ColorSample(NextColor, NextEvent->Params);

				if(self.Next->Time != REAL4_MAX)
					TimeDiff = self.Next->Time - self.Time;
				else TimeDiff = iPtcView.Life() - self.Time;

				Vector0 = PackF4(iPtcView.Color());
				Vector1 = PackF4(NextColor);
				VecStep = (Vector1 - Vector0) / TimeDiff;
				UnpackF4(iPtcView.DifColor(), VecStep);
			}
		} else if(bFadeOut){
			VecStep = PackF4(iPtcView.DifColor());
			Vector0 = PackF4(iPtcView.Color());
			Vector0 += VecStep * TimeSpan;

			UnpackF4(iPtcView.Color(), Vector0);
			ColorConvert32(iPtcView.Color32(), PackF4(iPtcView.Color()));
		}
	}
};
//尺寸型事件
struct PtcSizeEvent: public CPtcEvent{
	VECTOR2 Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseVector2(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		using namespace dx;

		BOOL bFadeOut;
		SPFP TimeDiff;
		VECTOR2 nxtSize;
		XVECTOR Vector0, Vector1, VecStep;
		PtcSizeEvent *NextEvent = (PtcSizeEvent*)self.Next;

		bFadeOut = self.Next && self.Next->bFade;

		if((iPtcView.Timer() - TimeSpan) <= self.Time){
			if(!self.bFade) Vec2Sample(iPtcView.Size(), self.Params);
			if(bFadeOut){
				Vec2Sample(nxtSize, NextEvent->Params);

				if(self.Next->Time != REAL4_MAX)
					TimeDiff = self.Next->Time - self.Time;
				else TimeDiff = iPtcView.Life() - self.Time;

				Vector0 = PackV2(iPtcView.Size());
				Vector1 = PackV2(nxtSize);
				VecStep = (Vector1 - Vector0) / TimeDiff;
				UnpackV2(iPtcView.DifSize(), VecStep);
			}
		} else if(bFadeOut){
			VecStep = PackV2(iPtcView.DifSize());
			Vector0 = PackV2(iPtcView.Size());
			Vector0 += VecStep * TimeSpan;
			UnpackV2(iPtcView.Size(), Vector0);
		}
	}
};
//计时型事件
struct PtcTimerEvent: public CPtcEvent{
	SPFP Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseNumber(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		if(self.Type == EVENT_LIFE){
			if((iPtcView.Timer() - TimeSpan) <= 0.0f){
				iPtcView.Age() = 0.0f;
				iPtcView.Life() = RealSample(self.Params);
			} else{
				iPtcView.Age() += TimeSpan;;
				if(iPtcView.Age() > iPtcView.Life()) iPtcView.Kill();
			}
		} else if(self.Type == EVENT_TIMER){
			if(iPtcView.Timer() > self.Time)
				iPtcView.Timer() = RealSample(self.Params);
		}
	}
};
//纹理型事件
struct PtcTexEvent: public CPtcEvent{
	VECTOR2 Params[2];

	VOID ParseContent(CHAR* Content){
		CPtcEvent::ParseVector2(Content, self.Params);
	}
	VOID UpdateParticle(CPtcView &iPtcView, SPFP TimeSpan){
		if((iPtcView.Timer() - TimeSpan) <= self.Time)
			Vec2Sample(iPtcView.TexCoord(), self.Params);
	}
};

//创建事件
CPtcEvent* CPtcEvent::Create(CHAR *Action, CHAR *Time, CHAR* Type, CHAR* Content){
	CPtcEvent *pEvent = NULL;

	if(MbsCmp(Type, "life") == 0){
		pEvent = new PtcTimerEvent;
		pEvent->Type = CPtcEvent::EVENT_LIFE;
	} else if(MbsCmp(Type, "timer") == 0){
		pEvent = new PtcTimerEvent;
		pEvent->Type = CPtcEvent::EVENT_TIMER;
	} else if(MbsCmp(Type, "size") == 0){
		pEvent = new PtcSizeEvent;
		pEvent->Type = CPtcEvent::EVENT_SIZE;
	} else if(MbsCmp(Type, "size-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_SIZEX;
	} else if(MbsCmp(Type, "size-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_SIZEY;
	} else if(MbsCmp(Type, "veloc") == 0){
		pEvent = new PtcVectorEvent;
		pEvent->Type = CPtcEvent::EVENT_VELOC;
	} else if(MbsCmp(Type, "veloc-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_VELOCX;
	} else if(MbsCmp(Type, "veloc-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_VELOCY;
	} else if(MbsCmp(Type, "veloc-z") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_VELOCZ;
	} else if(MbsCmp(Type, "accel") == 0){
		pEvent = new PtcVectorEvent;
		pEvent->Type = CPtcEvent::EVENT_ACCEL;
	} else if(MbsCmp(Type, "accel-x") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_ACCELX;
	} else if(MbsCmp(Type, "accel-y") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_ACCELY;
	} else if(MbsCmp(Type, "accel-z") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_ACCELZ;
	} else if(MbsCmp(Type, "color") == 0){
		pEvent = new PtcColorEvent;
		pEvent->Type = CPtcEvent::EVENT_COLOR;
	} else if(MbsCmp(Type, "color-a") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_COLORA;
	} else if(MbsCmp(Type, "color-r") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_COLORR;
	} else if(MbsCmp(Type, "color-g") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_COLORG;
	} else if(MbsCmp(Type, "color-b") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_COLORB;
	} else if(MbsCmp(Type, "texcoord") == 0){
		pEvent = new PtcTexEvent;
		pEvent->Type = CPtcEvent::EVENT_TEXTURE;
	} else if(MbsCmp(Type, "texcoord-u") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_TEXTUREU;
	} else if(MbsCmp(Type, "texcoord-v") == 0){
		pEvent = new PtcScalarEvent;
		pEvent->Type = CPtcEvent::EVENT_TEXTUREV;
	}

	if(pEvent){
		pEvent->ParseContent(Content);
		pEvent->bFade = FALSE;
		pEvent->Next = NULL;

		if(MbsCmp(Action, "set") == 0){
			CPtcEvent::ParseNumber(Time, pEvent->Times);
		} else if(MbsCmp(Action, "fade") == 0){
			CPtcEvent::ParseNumber(Time, pEvent->Times);
			pEvent->bFade = TRUE;
		} else if(MbsCmp(Action, "init") == 0){
			pEvent->Times[0] = 0.0f;
			pEvent->Times[1] = 0.0f;
		} else if(MbsCmp(Action, "final") == 0){
			pEvent->Times[0] = REAL4_MAX;
			pEvent->Times[1] = REAL4_MAX;
		}
	}

	return pEvent;
}
//创建空事件
CPtcEvent* CPtcEvent::CreateEmpty(TYPE Type){
	PtcScalarEvent *pEvent = new PtcScalarEvent;
	MemZero(pEvent, sizeof(PtcScalarEvent));
	pEvent->Type = Type;
	return pEvent;
}

//----------------------------------------//