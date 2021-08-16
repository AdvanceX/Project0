//--------------------渲染管线--------------------//

//上传缓存区
class CUploadBuff{
	hide UINT Stride; //缓存跨度
	open BYTE *MemStart; //内存地址
	open HGPUBUFF hVMStart; //显存地址
	hide ID3DResource *lpBuffer; //缓存对象

	open ~CUploadBuff(){
		self.Finalize();
	}
	open CUploadBuff(){
		ZeroMemory(this, sizeof(*this));
	}
	open CUploadBuff(ID3DDevice *lpD3DDevice, UINT Stride, UINT SegCount, BOOL bConst){
		self.Initialize(lpD3DDevice, Stride, SegCount, bConst);
	}

	open VOID Finalize(){
		if(self.lpBuffer){
			self.lpBuffer->Unmap(0, NULL);
			self.lpBuffer->Release();
			self.lpBuffer = NULL;
		}
	}
	open VOID Initialize(ID3DDevice *lpD3DDevice, UINT Stride, UINT SegCount, BOOL bConst){
		self.Stride = Stride;
		self.lpBuffer = CreateUploadBuffer(lpD3DDevice, Stride * SegCount, bConst);
		self.lpBuffer->Map(0, NULL, (VOID**)&self.MemStart);
		self.hVMStart = self.lpBuffer->GetGPUVirtualAddress();
	}
	open BYTE* GetCPUAddress(UINT SegIndex){
		UINT Offset = self.Stride * SegIndex;
		return self.MemStart + Offset;
	}
	open HGPUBUFF GetGPUAddress(UINT SegIndex){
		UINT Offset = self.Stride * SegIndex;
		return self.hVMStart + Offset;
	}
};

//着色器
class CShader{
	open enum PARAM{
		PARAM_SCENE,
		PARAM_FRAME,
		PARAM_OBJECT,
		PARAM_SUBOBJ,
	};
	open struct PARAMS{
		UCHR nBone;
		UCHR nLight;
		UCHR nTexture;
		UCHR nBumpMap;
	};

	open DWRD Style;
	open PARAMS Params;
	open D3DPrimTopo PrimTopo;
	open ID3DRootSig *lpRootSig;
	open ID3DBlob *lpVSCode;
	open ID3DBlob *lpPSCode;
	open ID3DBlob *lpGSCode;

	open ~CShader(){
		self.Finalize();
	}
	open CShader(){
		ZeroMemory(this, sizeof(*this));
	}
	open CShader(ID3DDevice* lpD3DDevice, DWRD Style){
		self.Initialize(lpD3DDevice, Style);
	}

	open VOID Finalize(){
		RELEASE_REF(self.lpVSCode);
		RELEASE_REF(self.lpPSCode);
		RELEASE_REF(self.lpGSCode);
		RELEASE_REF(self.lpRootSig);

		ZeroMemory(this, sizeof(*this));
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, DWRD Style, D3DStaticSamplerDesc *pMSAADesc = NULL){
		//创建宏定义

		UINT DefCount = 0;
		DWRD arrProp[] = { VERT_BONES, VERT_SIZE, VERT_COLOR,
			VERT_NORMAL, VERT_TANGENT, VERT_TEXCOORD, ASSET_MATERIAL };
		CHAR *arrDefine[] = { "INPUT_BONE", "INPUT_SIZE", "INPUT_COLOR",
			"INPUT_NORMAL", "INPUT_TANGENT", "INPUT_TEXTURE", "INPUT_MATERIAL" };
		D3DShaderMacro arrMacro[ARR_LEN(arrDefine)];

		ZeroMemory(this, sizeof(*this));
		ZeroMemory(arrMacro, sizeof(arrMacro));

		for(UINT i = 0; i < ARR_LEN(arrProp); ++i){
			if(Style & arrProp[i])
				arrMacro[DefCount++] = { arrDefine[i], "0" };
		}

		//创建字节码和根签名

		if(Style & ASSET_PARTICLE){
			self.lpVSCode = CompileShaderFromFile(
				L"./source/ge_particle.hlsl", arrMacro, "VertexShade", "vs_5_1");
			self.lpPSCode = CompileShaderFromFile(
				L"./source/ge_particle.hlsl", arrMacro, "PixelShade", "ps_5_1");
			self.lpGSCode = CompileShaderFromFile(
				L"./source/ge_particle.hlsl", arrMacro, "GeometryShade", "gs_5_1");

			self.Style = Style;
			self.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

			self.InitAsParticleShader(lpD3DDevice, arrMacro, pMSAADesc);
		} else{
			self.lpVSCode = CompileShaderFromFile(
				L"./source/ge_mesh.hlsl", arrMacro, "VertexShade", "vs_5_1");
			self.lpPSCode = CompileShaderFromFile(
				L"./source/ge_mesh.hlsl", arrMacro, "PixelShade", "ps_5_1");

			self.Style = Style;
			self.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			self.InitAsMeshShader(lpD3DDevice, arrMacro, pMSAADesc);
		}
	}
	hide VOID InitAsMeshShader(ID3DDevice* lpD3DDevice, D3DShaderMacro* arrMacro, D3DStaticSamplerDesc *pMSAADesc){
		UINT ParamCount = 0;
		UINT SamplerCount = 0;
		ID3DBlob *lpSerialize;
		D3DRootParam arrParam[8];
		D3DViewRange arrViewRange[2];
		D3DRootSigDesc RootSigDesc;
		D3DStaticSamplerDesc DefMSAADesc(0);

		arrViewRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); //纹理视图范围
		arrViewRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); //凹凸视图范围

		arrParam[0].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_ALL);    //场景参数
		arrParam[1].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_ALL);    //帧参数
		arrParam[2].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_VERTEX); //对象参数
		arrParam[3].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_PIXEL);  //子对象参数

		if(self.Style & VERT_NORMAL){ //光照参数
			arrParam[ParamCount].InitAsShaderResourceView(
				0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
			self.Params.nLight = ParamCount++;
		}
		if(self.Style & VERT_BONES){ //骨骼参数
			arrParam[ParamCount].InitAsShaderResourceView(
				1, 1, D3D12_SHADER_VISIBILITY_VERTEX);
			self.Params.nBone = ParamCount++;
		}
		if(self.Style & VERT_TEXCOORD){ //纹理参数
			arrParam[ParamCount].InitAsDescriptorTable(
				1, &arrViewRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
			self.Params.nTexture = ParamCount++;
		}
		if(self.Style & VERT_TANGENT){ //凹凸参数
			arrParam[ParamCount].InitAsDescriptorTable(
				1, &arrViewRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
			self.Params.nBumpMap = ParamCount++;
		}

		if(self.Style & VERT_TEXCOORD){ //采样器
			SamplerCount = 1;
			if(!pMSAADesc) pMSAADesc = &DefMSAADesc;
		}

		RootSigDesc.Init(ParamCount, arrParam, SamplerCount, pMSAADesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&RootSigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, NULL);

		lpD3DDevice->CreateRootSignature(0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), COM_ARGS(&self.lpRootSig));

		lpSerialize->Release();
	}
	hide VOID InitAsParticleShader(ID3DDevice* lpD3DDevice, D3DShaderMacro* arrMacro, D3DStaticSamplerDesc *pMSAADesc){
		UINT ParamCount = 0;
		ID3DBlob *lpSerialize;
		D3DViewRange ViewRange;
		D3DRootParam arrParam[6];
		D3DRootSigDesc SigDesc;
		D3DStaticSamplerDesc DefMSAADesc(0);

		arrParam[0].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_ALL);
		arrParam[1].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_ALL);
		arrParam[2].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_ALL);
		arrParam[3].InitAsConstantBufferView(ParamCount++, 0, D3D12_SHADER_VISIBILITY_PIXEL);

		if(self.Style & VERT_NORMAL){
			arrParam[ParamCount].InitAsShaderResourceView(
				0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
			self.Params.nLight = ParamCount++;
		}

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
		arrParam[ParamCount].InitAsDescriptorTable(
			1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL);
		self.Params.nTexture = ParamCount++;

		if(!pMSAADesc)
			pMSAADesc = &DefMSAADesc;

		SigDesc.Init(ParamCount, arrParam, 1, pMSAADesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SigDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &lpSerialize, NULL);

		lpD3DDevice->CreateRootSignature(0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), COM_ARGS(&self.lpRootSig));

		lpSerialize->Release();
	}
	open UINT CreateInputDesc(UINT Slot, STLSharedPtr<D3DInputElemDesc> &Output){
		D3DInputElemDesc Template = { NULL, 0, DXGI_FORMAT_UNKNOWN,
			Slot, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		D3DInputElemDesc *arrElem = new D3DInputElemDesc[16];
		Output.reset(arrElem);

		UINT ElemCount = 0;
		UINT ByteOffset = 0;

		if(self.Style & VERT_COORD){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "POSITION";
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(VECTOR3);
		}
		if(self.Style & VERT_NORMAL){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "NORMAL";
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(VECTOR3);
		}
		if(self.Style & VERT_TANGENT){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "TANGENT";
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(VECTOR3);
		}
		if(self.Style & VERT_SIZE){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "SIZE";
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(VECTOR2);
		}
		if(self.Style & VERT_BONES){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "WEIGHTS";
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(SPFP) * 3;

			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "BONEINDICES";
			arrElem[ElemCount].Format = DXGI_FORMAT_R8G8B8A8_UINT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(UCHR) * 4;
		}
		if(self.Style & VERT_COLOR){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "COLOR";
			arrElem[ElemCount].Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			ElemCount++;
			ByteOffset += sizeof(ARGB);
		}

		UINT ElemIndex = 0;
		DWRD fElement = VERT_TEXCOORD;
		while(self.Style & fElement){
			arrElem[ElemCount] = Template;
			arrElem[ElemCount].SemanticName = "TEXCOORD";
			arrElem[ElemCount].SemanticIndex = ElemIndex++;
			arrElem[ElemCount].Format = DXGI_FORMAT_R32G32_FLOAT;
			arrElem[ElemCount].AlignedByteOffset = ByteOffset;

			fElement <<= 1;
			ElemCount++;
			ByteOffset += sizeof(VECTOR2);
		}

		return ElemCount;
	}
	open D3DShaderBytecode GetVSBytecode(){
		D3DShaderBytecode Bytecode;

		if(!self.lpVSCode){
			ZeroMemory(&Bytecode, sizeof(D3DShaderBytecode));
		} else{
			Bytecode.BytecodeLength = self.lpVSCode->GetBufferSize();
			Bytecode.pShaderBytecode = self.lpVSCode->GetBufferPointer();
		}

		return Bytecode;
	}
	open D3DShaderBytecode GetPSBytecode(){
		D3DShaderBytecode Bytecode;

		if(!self.lpPSCode){
			ZeroMemory(&Bytecode, sizeof(D3DShaderBytecode));
		} else{
			Bytecode.BytecodeLength = self.lpPSCode->GetBufferSize();
			Bytecode.pShaderBytecode = self.lpPSCode->GetBufferPointer();
		}

		return Bytecode;
	}
	open D3DShaderBytecode GetGSBytecode(){
		D3DShaderBytecode Bytecode;

		if(!self.lpGSCode){
			ZeroMemory(&Bytecode, sizeof(D3DShaderBytecode));
		} else{
			Bytecode.BytecodeLength = self.lpGSCode->GetBufferSize();
			Bytecode.pShaderBytecode = self.lpGSCode->GetBufferPointer();
		}

		return Bytecode;
	}
};

//图形PSO
class CGfxPso{
	open CShader *pShader;
	open ID3DPipeState *lpPipeState;

	open ~CGfxPso(){
		self.Finalize();
	}
	open CGfxPso(){
		ZeroMemory(this, sizeof(*this));
	}
	open CGfxPso(ID3DDevice* lpD3DDevice, CShader &Shader, GEWnd &Window){
		self.Initialize(lpD3DDevice, Shader, Window);
	}
	open VOID Initialize(ID3DDevice* lpD3DDevice, CShader &Shader, GEWnd &Window){
		DXGISwapChainDesc SCDesc;
		D3DGfxPipeStateDesc PSODesc;
		STLSharedPtr<D3DInputElemDesc> InputDesc;
		UINT InputElemCount = Shader.CreateInputDesc(0, InputDesc);

		Window.lpSwapChain->GetDesc(&SCDesc);
		ZeroMemory(&PSODesc, sizeof(PSODesc));

		PSODesc.SampleMask = UINT_MAX;
		PSODesc.NumRenderTargets = 1;
		PSODesc.VS = Shader.GetVSBytecode();
		PSODesc.PS = Shader.GetPSBytecode();
		PSODesc.GS = Shader.GetGSBytecode();
		PSODesc.pRootSignature = Shader.lpRootSig;
		PSODesc.InputLayout.NumElements = InputElemCount;
		PSODesc.InputLayout.pInputElementDescs = InputDesc.get();
		PSODesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		PSODesc.RTVFormats[0] = SCDesc.BufferDesc.Format;
		PSODesc.SampleDesc.Count = SCDesc.SampleDesc.Count;
		PSODesc.SampleDesc.Quality = SCDesc.SampleDesc.Quality;
		PSODesc.BlendState = D3DBlendDesc(D3D12_DEFAULT);
		PSODesc.RasterizerState = D3DRasterizerDesc(D3D12_DEFAULT);
		PSODesc.DepthStencilState = D3DDepthStencilDesc(D3D12_DEFAULT);
		PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		lpD3DDevice->CreateGraphicsPipelineState(&PSODesc, COM_ARGS(&self.lpPipeState));
		self.pShader = &Shader;
	}
	open VOID Finalize(){
		RESET_REF(self.lpPipeState);
	}
};

//渲染工具
class CRenderer{
	hide UINT CurFrame;
	open UINT MaxFrame;
	hide CBSCENE *pScene;
	hide CBFRAME *pFrame;
	hide CBOBJECT *pObject;
	hide HGPUBUFF hVMScene;
	hide HGPUBUFF hVMFrame;
	hide HGPUBUFF hVMObject;
	hide CUploadBuff SceneBuff;
	hide CUploadBuff FrameBuff;
	hide CUploadBuff ObjectBuff;
	open CLitLibrary *pLitLibrary;
	open CMtlLibrary *pMtlLibrary;
	hide CShader::PARAMS Params;

	open CRenderer(){
		ZeroMemory(this, sizeof(*this));
	}
	open CRenderer(ID3DDevice *lpD3DDevice, UINT MaxFrame, UINT ObjCount){
		self.Initialize(lpD3DDevice, MaxFrame, ObjCount);
	}
	open VOID Initialize(ID3DDevice *lpD3DDevice, UINT MaxFrame, UINT ObjCount){
		ZeroMemory(this, sizeof(*this));
		self.MaxFrame = MaxFrame;

		self.SceneBuff.Initialize(lpD3DDevice, sizeof(CBSCENE), 1, TRUE);
		self.FrameBuff.Initialize(lpD3DDevice, sizeof(CBFRAME), MaxFrame, TRUE);
		self.ObjectBuff.Initialize(lpD3DDevice, sizeof(CBOBJECT) * ObjCount, MaxFrame, TRUE);

		self.pScene = (CBSCENE*)self.SceneBuff.MemStart;
		self.pFrame = (CBFRAME*)self.FrameBuff.MemStart;
		self.pObject = (CBOBJECT*)self.ObjectBuff.MemStart;

		self.hVMScene = self.SceneBuff.hVMStart;
		self.hVMFrame = self.FrameBuff.hVMStart;
		self.hVMObject = self.ObjectBuff.hVMStart;

		ZeroMemory(self.pScene, sizeof(CBSCENE));
		ZeroMemory(self.pFrame, sizeof(CBFRAME) * MaxFrame);
	}
	open VOID Finalize(){
		self.ObjectBuff.Finalize();
		self.SceneBuff.Finalize();
		self.FrameBuff.Finalize();

		ZeroMemory(this, sizeof(*this));
	}
	///命令无关设置
	open VOID SetFrame(UINT Frame){
		self.CurFrame = Frame;

		self.pFrame = (CBFRAME*)self.FrameBuff.GetCPUAddress(self.CurFrame);
		self.pObject = (CBOBJECT*)self.ObjectBuff.GetCPUAddress(self.CurFrame);

		self.hVMFrame = self.FrameBuff.GetGPUAddress(self.CurFrame);
		self.hVMObject = self.ObjectBuff.GetGPUAddress(self.CurFrame);
	}
	open VOID SetCamera(CCamera &Camera){
		XMATRIX matViewProj = MatTranspose(Camera.matView * Camera.matProj);
		UnpackMat(self.pFrame->matViewProj, matViewProj);
		UnpackV3(self.pFrame->CameraPos, Camera.Position);
	}
	open VOID SetCartoonShade(BOOL bEnable){
		self.pScene->bCartoonShade = bEnable;
	}
	open VOID SetAmbientLight(VECTOR3 &Intensity){
		self.pScene->AmbientLight = Intensity;
	}
	open VOID SetLightLibrary(CLitLibrary &LitLibrary){
		self.pLitLibrary = &LitLibrary;
	}
	open VOID SetMaterialLibrary(CMtlLibrary &MtlLibrary){
		self.pMtlLibrary = &MtlLibrary;
	}
	open VOID SetTransformation(MATRIX &matWorld, MATRIX &matTexAnim){
		//设置世界变换和纹理动画变换

		self.pObject->matWorld = matWorld;
		self.pObject->matTexAnim = matTexAnim;

		//计算法线变换

		XMATRIX matInvWorld = PackMat(matWorld);
		matInvWorld.r[3] = VecSet(0.0f, 0.0f, 0.0f, 1.0f);
		matInvWorld = MatTranspose(MatInverse(matInvWorld, NULL));
		UnpackMat(self.pObject->matInvWorld, matInvWorld);
	}
	open VOID SetFog(VECTOR3 &Color, SPFP Start, SPFP Range){
		self.pScene->FogColor = Color;
		self.pScene->FogStart = Start;
		self.pScene->FogRange = Range;
	}
	///命令相关设置
	open VOID SetPSO(ID3DGfxCmdList* lpCmdList, CGfxPso &GfxPso){
		self.Params = GfxPso.pShader->Params;
		self.pScene->LightCount = self.pLitLibrary ? self.pLitLibrary->Size : 0;

		lpCmdList->SetPipelineState(GfxPso.lpPipeState);
		lpCmdList->SetGraphicsRootSignature(GfxPso.pShader->lpRootSig);
		lpCmdList->IASetPrimitiveTopology(GfxPso.pShader->PrimTopo);

		lpCmdList->SetGraphicsRootConstantBufferView(
			CShader::PARAM_SCENE, self.hVMScene);
		lpCmdList->SetGraphicsRootConstantBufferView(
			CShader::PARAM_FRAME, self.hVMFrame);

		if(self.Params.nTexture != 0)
			lpCmdList->SetDescriptorHeaps(1, &self.pMtlLibrary->lpSRVHeap);
		if(self.Params.nLight != 0){
			lpCmdList->SetGraphicsRootShaderResourceView(
				self.Params.nLight, self.pLitLibrary->GetGPUAddress());
		}
	}
	open VOID SetSkeleton(ID3DGfxCmdList* lpCmdList, ID3DResource* lpSkeleton){
		if(self.Params.nBone != 0){
			lpCmdList->SetGraphicsRootShaderResourceView(
				self.Params.nBone, lpSkeleton->GetGPUVirtualAddress());
		}
	}
	open VOID ClearWindow(ID3DGfxCmdList* lpCmdList, GEWnd &Window, SPFP Color[4]){
		lpCmdList->ClearRenderTargetView(Window.hRTView, Color, 0, NULL);
		lpCmdList->ClearDepthStencilView(Window.hDSView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	}
	open VOID BeginDraw(ID3DGfxCmdList* lpCmdList, GEWnd &Window){
		lpCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Window.CurBackBuff,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		lpCmdList->OMSetRenderTargets(1, &Window.hRTView, TRUE, &Window.hDSView);
		lpCmdList->RSSetScissorRects(1, &Window.rcScissor);
		lpCmdList->RSSetViewports(1, &Window.Viewport);
	}
	open VOID EndDraw(ID3DGfxCmdList* lpCmdList, GEWnd &Window){
		lpCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Window.CurBackBuff,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	open VOID DrawMeshs(ID3DGfxCmdList* lpCmdList, CMesh* FirstMesh, UINT MtlIndex = UINT_MAX){
		lpCmdList->SetGraphicsRootConstantBufferView(
			CShader::PARAM_OBJECT, self.hVMObject);

		CMesh *pMesh = FirstMesh;
		if(MtlIndex == UINT_MAX){
			while(pMesh){
				self.DrawMesh(lpCmdList, *pMesh);
				pMesh = pMesh->Next;
			}
		} else {
			while(pMesh){
				self.DrawFixMaterialMesh(lpCmdList, *pMesh, MtlIndex);
				pMesh = pMesh->Next;
			}
		}

		self.pObject++;
		self.hVMObject += sizeof(CBOBJECT);
	}
	open VOID DrawMesh(ID3DGfxCmdList* lpCmdList, CMesh &Mesh){
		lpCmdList->IASetIndexBuffer(&Mesh.GetIndexBufferView());
		lpCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexBufferView());

		if(Mesh.MtlCount == 0){
			UPTR IdxCount = (UPTR)Mesh.arrSubset;
			lpCmdList->DrawIndexedInstanced((UINT)IdxCount, 1, 0, 0, 0);
		} else{
			HGPUBUFF hMtlBuff = self.pMtlLibrary->GetMaterialGPUAddress(0);

			for(UINT i = 0; i < Mesh.MtlCount; ++i){
				if(Mesh.VtxFormat & VERT_TEXCOORD){
					UINT TexIndex = Mesh.arrMapping[i].nDiffuse;
					lpCmdList->SetGraphicsRootDescriptorTable(
						self.Params.nTexture, self.pMtlLibrary->GetTextureView(TexIndex));
				}
				if(Mesh.VtxFormat & VERT_TANGENT){
					UINT TexIndex = Mesh.arrMapping[i].nBump;
					lpCmdList->SetGraphicsRootDescriptorTable(
						self.Params.nBumpMap, self.pMtlLibrary->GetTextureView(TexIndex));
				}

				SUBMESH *pSubmesh = &Mesh.arrSubset[i];

				lpCmdList->SetGraphicsRootConstantBufferView(
					CShader::PARAM_SUBOBJ, hMtlBuff + pSubmesh->MtlIdent);
				lpCmdList->DrawIndexedInstanced(
					pSubmesh->IdxCount, 1, pSubmesh->IdxStart, 0, 0);
			}
		}
	}
	open VOID DrawFixMaterialMesh(ID3DGfxCmdList* lpCmdList, CMesh &Mesh, UINT MtlIndex){
		lpCmdList->IASetIndexBuffer(&Mesh.GetIndexBufferView());
		lpCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexBufferView());

		if(Mesh.MtlCount == 0){
			UPTR IdxCount = (UPTR)Mesh.arrSubset;
			lpCmdList->DrawIndexedInstanced((UINT)IdxCount, 1, 0, 0, 0);
		} else{
			HGPUBUFF hMtlBuff = self.pMtlLibrary->GetMaterialGPUAddress(MtlIndex);
			lpCmdList->SetGraphicsRootConstantBufferView(CShader::PARAM_SUBOBJ, hMtlBuff);

			for(UINT i = 0; i < Mesh.MtlCount; ++i){
				if(Mesh.VtxFormat & VERT_TEXCOORD){
					UINT nDiffuse = Mesh.arrMapping[i].nDiffuse;
					lpCmdList->SetGraphicsRootDescriptorTable(
						self.Params.nTexture, self.pMtlLibrary->GetTextureView(nDiffuse));
				}
				if(Mesh.VtxFormat & VERT_TANGENT){
					UINT nBump = Mesh.arrMapping[i].nBump;
					lpCmdList->SetGraphicsRootDescriptorTable(
						self.Params.nBumpMap, self.pMtlLibrary->GetTextureView(nBump));
				}

				SUBMESH *pSubmesh = &Mesh.arrSubset[i];
				lpCmdList->DrawIndexedInstanced(pSubmesh->IdxCount, 1, pSubmesh->IdxStart, 0, 0);
			}
		}
	}
	open VOID DrawParticles(ID3DGfxCmdList* lpCmdList, CPtcSystem &PtcSystem){
		UINT IdxOffset = 0;
		MATERIAL Material;
		Material.Shininess = 0.1f;
		Material.Fresnel = { 0.1f, 0.1f, 0.1f };

		lpCmdList->IASetVertexBuffers(0, 1, &PtcSystem.GetVertexBufferView());

		for(UINT i = 0; i < PtcSystem.numSubset; ++i){
			CPtcGroup *pPtcGroup = &PtcSystem.arrSubset[i];
			UINT TexIndex = pPtcGroup->TexIdent;

			self.pObject->matWorld(0, 1) = pPtcGroup->PtcSize.x;
			self.pObject->matWorld(1, 1) = pPtcGroup->PtcSize.y;

			Material.Albedo.x = pPtcGroup->PtcColor.r;
			Material.Albedo.y = pPtcGroup->PtcColor.g;
			Material.Albedo.z = pPtcGroup->PtcColor.b;
			Material.Opacity = pPtcGroup->PtcColor.a;

			lpCmdList->SetGraphicsRootDescriptorTable(
				self.Params.nTexture, self.pMtlLibrary->GetTextureView(TexIndex));
			lpCmdList->SetComputeRoot32BitConstants(
				CShader::PARAM_SUBOBJ, sizeof(MATERIAL) / 4, &Material, 0);
			lpCmdList->SetGraphicsRootConstantBufferView(
				CShader::PARAM_OBJECT, self.hVMObject);
			lpCmdList->DrawInstanced(
				pPtcGroup->PtcCount, 1, IdxOffset, 0);

			self.pObject++;
			self.hVMObject += sizeof(CBOBJECT);
			IdxOffset += pPtcGroup->PtcCount;
		}
	}
	open VOID DrawSkyDome(ID3DGfxCmdList* lpCmdList, CSkyDome &SkyDome){
		lpCmdList->SetPipelineState(SkyDome.lpPipeState);
		lpCmdList->SetGraphicsRootSignature(SkyDome.lpRootSig);

		lpCmdList->IASetIndexBuffer(&SkyDome.GetIndexBufferView());
		lpCmdList->IASetVertexBuffers(0, 1, &SkyDome.GetVertexBufferView());
		lpCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		lpCmdList->SetDescriptorHeaps(1, &SkyDome.lpSRVHeap);
		lpCmdList->SetGraphicsRootConstantBufferView(0, self.hVMFrame);
		lpCmdList->SetGraphicsRootDescriptorTable(1, SkyDome.GetTextureView());

		lpCmdList->DrawIndexedInstanced(
			SkyDome.cbIdxBuff / sizeof(USRT), 1, 0, 0, 0);
	}
};

//----------------------------------------//


//--------------------动画纹理--------------------//

//视频过滤器
class IVideoFilter: public IMFReaderCallback{
	open INT4 ImgPitch;
	open UINT ImgWidth;
	open UINT ImgHeight;
	hide UINT RefCount;
	hide BOOL bLastFrame;
	open BYTE *ImgBytes;
	hide GUID ImgFormat;
	hide HANDLE hFenceEvent;
	hide CRITSECTION CritSection;

	open ~IVideoFilter(){
		self.Finalize();
	}
	open IVideoFilter(){
		self.hFenceEvent = NULL;
	}
	open VOID Finalize(){
		if(self.hFenceEvent){
			DeleteCriticalSection(&self.CritSection);
			CloseHandle(self.hFenceEvent);
			self.hFenceEvent = NULL;
		}
	}
	open VOID WaitForRead(BOOL &bLastFrame){
		WaitForSingleObject(self.hFenceEvent, INFINITE);
		bLastFrame = self.bLastFrame;
	}
	open VOID Initialize(IMFMediaType* lpMediaType, GUID &ImgFormat){
		self.RefCount = 1;
		self.bLastFrame = FALSE;
		self.ImgFormat = ImgFormat;
		self.hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		InitializeCriticalSection(&self.CritSection);
		MFGetAttributeSize(lpMediaType, MF_MT_FRAME_SIZE, &self.ImgWidth, &self.ImgHeight);

		lpMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT*)&self.ImgPitch);
		if(ImgFormat == MFVideoFormat_RGB24) self.ImgPitch /= 3;
		else self.ImgPitch /= 4;
	}
	open HRESULT stdcall OnFlush(ULONG){
		return SUCC;
	}
	open HRESULT stdcall OnEvent(ULONG, IMFMediaEvent*){
		return SUCC;
	}
	open HRESULT stdcall OnReadSample(HRESULT, ULONG, DWRD StreamAttr, LINT, IMFMediaSample* lpSample){
		if(StreamAttr & MF_SOURCE_READERF_ENDOFSTREAM){
			self.bLastFrame = TRUE;
			SetEvent(self.hFenceEvent);
			return SUCC;
		} else self.bLastFrame = FALSE;

		BYTE *SrcBytes;
		IMFMediaBuff *lpSrcBuff;

		EnterCriticalSection(&self.CritSection);

		lpSample->GetBufferByIndex(0, &lpSrcBuff);
		lpSrcBuff->Lock(&SrcBytes, NULL, NULL);

		if((self.ImgFormat == MFVideoFormat_RGB32) ||
			(self.ImgFormat == MFVideoFormat_ARGB32))
		{
			COLOR32 *Dest = (COLOR32*)self.ImgBytes;
			COLOR32 *Source = ((COLOR32*)SrcBytes);

			if(self.ImgPitch < 0)
				Source -= self.ImgPitch * ((INT4)self.ImgHeight - 1);

			for(UINT n = 0; n < self.ImgHeight; ++n){
				for(UINT i = 0; i < self.ImgWidth; ++i)
					Dest[i] = Source[i];
				Dest += self.ImgWidth;
				Source -= self.ImgPitch;
			}
		} else if(self.ImgFormat == MFVideoFormat_RGB24){
			COLOR32 *Dest = (COLOR32*)self.ImgBytes;
			COLOR24 *Source = ((COLOR24*)SrcBytes);

			if(self.ImgPitch < 0)
				Source -= self.ImgPitch * ((INT4)self.ImgHeight - 1);

			for(UINT n = 0; n < self.ImgHeight; ++n){
				for(UINT i = 0; i < self.ImgWidth; ++i){
					Dest[i].c = *(ARGB*)&Source[i];
					Dest[i].a = 0xff;
				}
				Dest += self.ImgWidth;
				Source += self.ImgPitch;
			}
		}

		lpSrcBuff->Unlock();
		lpSrcBuff->Release();

		LeaveCriticalSection(&self.CritSection);
		SetEvent(self.hFenceEvent);

		return SUCC;
	}
	open HRESULT stdcall QueryInterface(REFIID iid, VOID** Output){
		static const QITAB QITable[] = { QITABENT(IVideoFilter, IMFReaderCallback), { 0 } };
		return QISearch(this, QITable, iid, Output);
	}
	open ULONG stdcall AddRef(){
		return InterlockedIncrement(&self.RefCount);
	}
	open ULONG stdcall Release(){
		ULONG RefCount = InterlockedDecrement(&self.RefCount);
		if(RefCount == 0) delete this;
		return RefCount;
	}
};

//视频纹理
class VideoTexture{
	hide BOOL bUpdated;
	open UINT TexPitch;
	open ULNG Duration;
	hide ID3DResource *lpULHeap;
	open ID3DResource *lpTexture;
	hide IVideoFilter *lpFilter;
	hide IMFSourceReader *lpReader;

	open ~VideoTexture(){
		self.Finalize();
	}
	open VideoTexture(){
		ZeroMemory(this, sizeof(*this));
	}
	open VOID Finalize(){
		if(!self.lpReader) return;

		RELEASE_REF(self.lpReader);
		RELEASE_REF(self.lpFilter);
		RELEASE_REF(self.lpULHeap);
		RELEASE_REF(self.lpTexture);

		ZeroMemory(this, sizeof(*this));
	}
	open VOID ReadSample(){
		HRESULT Result = self.lpULHeap->Map(
			0, NULL, (VOID**)&self.lpFilter->ImgBytes);

		if(SUCCEEDED(Result)){
			self.bUpdated = TRUE;
			self.lpReader->ReadSample(
				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0x0, NULL, NULL, NULL, NULL);
		}
	}
	open VOID UploadResource(ID3DGfxCmdList *lpCmdList){
		//等待采样结束

		if(!self.bUpdated) return;

		BOOL bLastFrame;
		PROPVARIANT MediaPos;

		self.bUpdated = FALSE;
		self.lpFilter->WaitForRead(bLastFrame);
		self.lpULHeap->Unmap(0, NULL);

		if(bLastFrame){
			MediaPos.vt = VT_I8;
			MediaPos.hVal.QuadPart = 0;
			self.lpReader->SetCurrentPosition(GUID_NULL, MediaPos);
			self.ReadSample();
			return;
		}

		//上传采样

		D3DPlacedResLayout layout = { 0, { DXGI_FORMAT_B8G8R8A8_UNORM,
			self.lpFilter->ImgWidth, self.lpFilter->ImgHeight, 1, self.TexPitch } };

		D3DTexCopyLocation Dest(self.lpTexture, 0);
		D3DTexCopyLocation Source(self.lpULHeap, layout);

		lpCmdList->ResourceBarrier(1, &D3DResBarrier::Transition(self.lpTexture,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

		lpCmdList->CopyTextureRegion(&Dest, 0, 0, 0, &Source, NULL);

		lpCmdList->ResourceBarrier(1, &D3DResBarrier::Transition(self.lpTexture,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
	open VOID InitFilter(){
		IMFMediaType *lpType;
		GUID MajorType, SubType;

		self.lpReader->GetNativeMediaType(
			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			MF_SOURCE_READER_CURRENT_TYPE_INDEX,
			&lpType);

		lpType->GetGUID(MF_MT_MAJOR_TYPE, &MajorType);
		lpType->GetGUID(MF_MT_SUBTYPE, &SubType);

		self.lpFilter->Initialize(lpType, SubType);
		RELEASE_REF(lpType);
	}
	open VOID InitReader(WCHR* VideoPath){
		PROPVARIANT ReaderProp;
		IMFMediaAttr *lpReaderAttr;

		self.lpFilter = new IVideoFilter;

		MFCreateAttributes(&lpReaderAttr, 1);
		lpReaderAttr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, self.lpFilter);
		MFCreateSourceReaderFromURL(VideoPath, lpReaderAttr, &self.lpReader);

		self.lpReader->GetPresentationAttribute(
			MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &ReaderProp);

		PropVariantToInt64(ReaderProp, (LINT*)&self.Duration);
		PropVariantClear(&ReaderProp);

		RELEASE_REF(lpReaderAttr);
	}
	open VOID CreateTexture(ID3DDevice* lpD3DDevice){
		UINT Width, Height, Size;

		Width = self.lpFilter->ImgWidth;
		Height = self.lpFilter->ImgHeight;
		Size = Width * Height * sizeof(ARGB);

		self.TexPitch = Size / Height;

		lpD3DDevice->CreateCommittedResource(
			&D3DHeapProps(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&D3DResDesc::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, Width, Height),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, NULL, COM_ARGS(&self.lpTexture));

		lpD3DDevice->CreateCommittedResource(
			&D3DHeapProps(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&D3DResDesc::Buffer(Size),
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL, COM_ARGS(&self.lpULHeap));
	}
	open UINT Initialize(ID3DDevice* lpD3DDevice, WCHR* VideoPath){
		self.bUpdated = FALSE;
		self.InitReader(VideoPath);
		self.InitFilter();
		self.CreateTexture(lpD3DDevice);
	}
};

//----------------------------------------//


//--------------------投影特效--------------------//

//镜面
struct Mirror{
	CGfxPso *pMirrorPSO;
	CGfxPso *pMarkingPSO;
	CRenderer *pRenderer;

	~Mirror(){
		self.Finalize();
	}
	Mirror(){
		ZeroMemory(this, sizeof(*this));
	}
	VOID Finalize(){
		RESET_PTR(self.pMirrorPSO);
		RESET_PTR(self.pMarkingPSO);
	}
	VOID Initialize(ID3DDevice* lpD3DDevice, D3DGfxPipeStateDesc &BasicDesc, CShader &Shader){
		ID3DPipeState *lpPipeState;
		D3DDepthStencilDesc *pDSBDesc;
		D3DBuffBlendDesc *pRTDesc;
		D3DGfxPipeStateDesc PSODesc;
		STLSharedPtr<D3DInputElemDesc> InputDesc;
		UINT InputElemCount = Shader.CreateInputDesc(0, InputDesc);

		BasicDesc.VS = Shader.GetVSBytecode();
		BasicDesc.PS = Shader.GetPSBytecode();
		BasicDesc.pRootSignature = Shader.lpRootSig;
		BasicDesc.InputLayout.NumElements = InputElemCount;
		BasicDesc.InputLayout.pInputElementDescs = InputDesc.get();

		//创建镜面PSO

		PSODesc = BasicDesc;
		pRTDesc = &PSODesc.BlendState.RenderTarget[0];

		pRTDesc->BlendEnable = TRUE;
		pRTDesc->BlendOp = D3D12_BLEND_OP_ADD;
		pRTDesc->SrcBlend = D3D12_BLEND_SRC_ALPHA;
		pRTDesc->DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		pRTDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		lpD3DDevice->CreateGraphicsPipelineState(&PSODesc, COM_ARGS(&lpPipeState));

		self.pMirrorPSO->pShader = &Shader;
		self.pMirrorPSO->lpPipeState = lpPipeState;

		//创建标记PSO

		PSODesc = BasicDesc;
		pDSBDesc = (D3DDepthStencilDesc*)&PSODesc.DepthStencilState;

		pDSBDesc->StencilEnable = TRUE;
		pDSBDesc->StencilReadMask = 0xff;
		pDSBDesc->StencilWriteMask = 0xff;
		pDSBDesc->DepthEnable = TRUE;
		pDSBDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pDSBDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		pDSBDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		pDSBDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		PSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0;

		lpD3DDevice->CreateGraphicsPipelineState(&PSODesc, COM_ARGS(&lpPipeState));

		self.pMirrorPSO->pShader = &Shader;
		self.pMirrorPSO->lpPipeState = lpPipeState;
	}
	VOID DrawMarking(ID3DGfxCmdList* lpCmdList, CMesh *pMirror, MATRIX &matWorld, MATRIX &matTexAnim){
		self.pRenderer->SetPSO(lpCmdList, *self.pMarkingPSO);
		lpCmdList->OMSetStencilRef(1);
		self.pRenderer->SetTransformation(matWorld, matTexAnim);
		self.pRenderer->DrawMeshs(lpCmdList, pMirror);
	}
	VOID DrawMirror(ID3DGfxCmdList* lpCmdList, CMesh *pMirror, MATRIX &matWorld, MATRIX &matTexAnim){
		self.pRenderer->SetPSO(lpCmdList, *self.pMirrorPSO);
		lpCmdList->OMSetStencilRef(0);
		self.pRenderer->SetTransformation(matWorld, matTexAnim);
		self.pRenderer->DrawMeshs(lpCmdList, pMirror);
	}
	VOID SetRenderer(CRenderer &Renderer){
		self.pRenderer = &Renderer;
	}

	static VOID BuildReflectionMatrix(PLANE &Plane, MATRIX &matOut){
		XMATRIX matReflect = MatReflect(PackF4(Plane));
		UnpackMat(matOut, matReflect);
	}
	static VOID BuildImagePSO(ID3DDevice* lpD3DDevice, D3DGfxPipeStateDesc &BasicDesc, CShader &Shader, CGfxPso &Output){
		ID3DPipeState *lpPipeState;
		D3DGfxPipeStateDesc PSODesc = BasicDesc;
		D3DDepthStencilDesc *pDSBDesc = (D3DDepthStencilDesc*)&PSODesc.DepthStencilState;
		STLSharedPtr<D3DInputElemDesc> InputDesc;
		UINT InputElemCount = Shader.CreateInputDesc(0, InputDesc);

		PSODesc.VS = Shader.GetVSBytecode();
		PSODesc.PS = Shader.GetPSBytecode();
		PSODesc.pRootSignature = Shader.lpRootSig;
		PSODesc.InputLayout.NumElements = InputElemCount;
		PSODesc.InputLayout.pInputElementDescs = InputDesc.get();

		PSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		PSODesc.RasterizerState.FrontCounterClockwise = TRUE;

		pDSBDesc->StencilEnable = TRUE;
		pDSBDesc->StencilReadMask = 0xff;
		pDSBDesc->StencilWriteMask = 0xff;
		pDSBDesc->DepthEnable = TRUE;
		pDSBDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pDSBDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;

		pDSBDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		lpD3DDevice->CreateGraphicsPipelineState(&PSODesc, COM_ARGS(&lpPipeState));
	
		Output.pShader = &Shader;
		Output.lpPipeState = lpPipeState;
	}
};

//阴影
struct Shadow{
	static VOID BuildShadowMatrix(PLANE &Plane, VECTOR4 &Light, MATRIX &matOut){
		XMATRIX matShadow = MatShadow(PackF4(Plane), PackV4(Light));
		UnpackMat(matOut, matShadow);
	}
	static VOID PrepDrawShadow(ID3DGfxCmdList *lpCmdList){
		lpCmdList->OMSetStencilRef(0);
	}
	static VOID GetShadowMaterial(SPFP opacity, MATERIAL &Output){
		Output.Albedo = { 0.0f, 0.0f, 0.0f };
		Output.Fresnel = { 0.001f, 0.001f, 0.001f };
		Output.Shininess = 0.0f;
		Output.Opacity = opacity;
	}
	static VOID BuildShadowPSO(ID3DDevice* lpD3DDevice, D3DGfxPipeStateDesc &BasicDesc, CShader &Shader, CGfxPso &Output){
		ID3DPipeState *lpPipeState;
		D3DDepthStencilDesc *pDSBDesc;
		D3DGfxPipeStateDesc PSODesc;
		D3DBuffBlendDesc *pRTDesc;
		STLSharedPtr<D3DInputElemDesc> InputDesc;
		UINT InputElemCount = Shader.CreateInputDesc(0, InputDesc);

		PSODesc = BasicDesc;
		pDSBDesc = (D3DDepthStencilDesc*)&PSODesc.DepthStencilState;
		pRTDesc = &PSODesc.BlendState.RenderTarget[0];

		PSODesc.VS = Shader.GetVSBytecode();
		PSODesc.PS = Shader.GetPSBytecode();
		PSODesc.pRootSignature = Shader.lpRootSig;
		PSODesc.InputLayout.NumElements = InputElemCount;
		PSODesc.InputLayout.pInputElementDescs = InputDesc.get();

		pRTDesc->BlendEnable = TRUE;
		pRTDesc->BlendOp = D3D12_BLEND_OP_ADD;
		pRTDesc->SrcBlend = D3D12_BLEND_SRC_ALPHA;
		pRTDesc->DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		pRTDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		pDSBDesc->StencilEnable = TRUE;
		pDSBDesc->StencilReadMask = 0xff;
		pDSBDesc->StencilWriteMask = 0xff;
		pDSBDesc->DepthEnable = TRUE;
		pDSBDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pDSBDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;

		pDSBDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pDSBDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR_SAT;
		pDSBDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

		lpD3DDevice->CreateGraphicsPipelineState(&PSODesc, COM_ARGS(&lpPipeState));

		Output.pShader = &Shader;
		Output.lpPipeState = lpPipeState;
	}
};

//----------------------------------------//