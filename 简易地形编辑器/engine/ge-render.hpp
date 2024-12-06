//-------------------- 着色器数据 --------------------//

// Shader Data: Scene
struct SDSCENE{
	DWORD ShaderFlag;    //着色标记
	SPFPN FogColor[3];   //雾颜色
	USINT LightCnt;      //光源数量
	SPFPN AmbLight[3];   //环境光
	SPFPN FogStart;      //雾起点
	SPFPN FogDepth;      //雾深度
	SPFPN ScnWidth;      //屏幕宽度
	SPFPN ScnHeight;     //屏幕高度
	SPFPN rgKdInput[8];  //散列表Kd输入
	SPFPN rgKdOutput[8]; //散列表Kd输出
	SPFPN rgKsInput[8];  //散列表Ks输入
	SPFPN rgKsOutput[8]; //散列表Ks输出
};

// Shader Data: Frame
struct SDFRAME{
	MATRIX4 matViewProj; //观察投影矩阵
	VECTOR3 CamCoord;    //相机位置
	SPFPN TimeSpan;      //时间间隔
};

// Shader Data: Object
struct SDOBJECT{
	MMATRIX matWorld;     //世界矩阵
	MMATRIX matTexOrNorm; //纹理/法线矩阵
};

// Shader Data: Spline
struct SDSPLINE{
	SPFPN Pitch;
	SPFPN Width;
	SPFPN WidthB;
	SPFPN Height;
	SPFPN Roughness;
	SPFPN Metalness;
	SPFPN TexSplit;
	SPFPN BumpScale;
	IBOOL bShell;
	VECTOR3 UpDir;
	MATRIX4 matWorld;
};

// Shader Data: Terrain
struct SDTERRAIN{
	UTINY rgSlideId[8]; //幻灯片ID
	USINT SlideCnt;     //幻灯片数量
	USINT MaskCnt;      //遮罩数量
	ARGB8 UsedColor;    //指定颜色
	SPFPN BumpScale;    //凹凸缩放
	SPFPN AreaWidth;    //区域宽度
	SPFPN AreaDepth;    //区域深度
	SPFPN TileWidth;    //贴砖宽度
	SPFPN TileDepth;    //贴砖深度
	SPFPN TileLength;   //贴砖长度
	SPFPN TileHeight;   //贴砖高度
	MATRIX4 matWorld;   //世界矩阵
};

// Shader Data: Surface
struct SDSURFACE{
	SPFPN rgWavDirX[4];
	SPFPN rgWavDirZ[4];
	SPFPN rgWavPhase[4];
	SPFPN rgWavSlope[4];
	SPFPN rgWavAmplit[4];
	SPFPN rgWavLength[4];
	SPFPN TxlationX;
	SPFPN TxlationY;
	SPFPN TxlationZ;
	SPFPN Scaling;
	SPFPN TexScaleU;
	SPFPN TexScaleV;
	ARGB8 WireColor;
	ARGB8 BkgdColor;
};

// Shader Data: Extrusion
struct SDEXTRUSION{
	SPFPN Starts[3];
	SPFPN End;
	UTINY TexIdent;
	UTINY Metalness;
	UTINY Roughness;
	BYTET bHollow : 1;
	BYTET bBslash : 1;
};

//----------------------------------------//


//-------------------- 渲染管线 --------------------//

//着色器
class CShader{
	_open enum OUTPUTTYPE{
		OUT_TYPE_ColorBuff = (1L << 24),
		OUT_TYPE_IdBuff = (1L << 25),
	};

	_open DWORD VtxFlag;
	_open DWORD ResFlag;
	_secr ID3DBlob *lpVSCode;
	_secr ID3DBlob *lpHSCode;
	_secr ID3DBlob *lpDSCode;
	_secr ID3DBlob *lpGSCode;
	_secr ID3DBlob *lpPSCode;
	_secr ID3DRootSig *lpRootSig;
	_secr D3D1PRIMTOPO PrimTopo;
	_secr D3D12PRIMTYPE PrimType;

	_open ~CShader(){
		$m.Finalize();
	}
	_open CShader(){
		Var_Zero(this);
	}
	_open CShader(ID3DDevice *pD3dDevice, DWORD VtxFlag, DWORD ResFlag, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		$m.Initialize(pD3dDevice, VtxFlag, ResFlag, pSmplrDesc);
	}
	//////
	_open $VOID Finalize(){
		if($m.lpRootSig){
			SAFE_RELEASE($m.lpVSCode);
			SAFE_RELEASE($m.lpHSCode);
			SAFE_RELEASE($m.lpDSCode);
			SAFE_RELEASE($m.lpGSCode);
			SAFE_RELEASE($m.lpPSCode);
			SAFE_RELEASE($m.lpRootSig);
		}
	}
	_open $VOID Initialize(ID3DDevice *pD3dDevice, DWORD VtxFlag, DWORD ResFlag, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		USINT numMacro = 0;
		D3D1SHADERMACRO rgMacro[16] = {};

		//指针归零

		$m.lpVSCode = P_Null;
		$m.lpHSCode = P_Null;
		$m.lpDSCode = P_Null;
		$m.lpGSCode = P_Null;
		$m.lpPSCode = P_Null;

		//资源标记

		$m.VtxFlag = VtxFlag;
		$m.ResFlag = ResFlag;

		//图元类型

		if(ResFlag & ASSET_TYPE_Spline){
			$m.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
			$m.PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		} else if(ResFlag & ASSET_TYPE_Particle){
			$m.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			$m.PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		} else{
			$m.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			$m.PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}

		//宏定义

		if(VtxFlag & VFB_Color)
			rgMacro[numMacro++] = { "INPUT_COLOR", P_Null };
		if(VtxFlag & VFB_PSize)
			rgMacro[numMacro++] = { "INPUT_PSIZE", P_Null };
		if(VtxFlag & VFB_Normal)
			rgMacro[numMacro++] = { "INPUT_NORMAL", P_Null };
		if(VtxFlag & VFB_Tangent)
			rgMacro[numMacro++] = { "INPUT_TANGENT", P_Null };
		if(VtxFlag & VFB_BoneRig)
			rgMacro[numMacro++] = { "INPUT_BONERIG", P_Null };
		if(VtxFlag & VFB_TexCoord)
			rgMacro[numMacro++] = { "INPUT_TEXCOORD", P_Null };

		if(ResFlag & ASSET_TYPE_AlphaMap)
			rgMacro[numMacro++] = { "REQ_ALPHA_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_ColorMap)
			rgMacro[numMacro++] = { "REQ_COLOR_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_RoughMap)
			rgMacro[numMacro++] = { "REQ_ROUGH_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_NormMap)
			rgMacro[numMacro++] = { "REQ_NORMAL_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_HeightMap)
			rgMacro[numMacro++] = { "REQ_HEIGHT_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_EmissMap)
			rgMacro[numMacro++] = { "REQ_EMISSION_MAP", P_Null };
		if(ResFlag & ASSET_TYPE_OcclusMap)
			rgMacro[numMacro++] = { "REQ_OCCLUSION_MAP", P_Null };

		//字节码和根签名

		if(ResFlag & ASSET_TYPE_Spline){
			$m.InitAsSplineShader(pD3dDevice, pSmplrDesc);

			$m.lpHSCode = CompileShader(
				L"./engine/ge-spline.hlsl", "HullShading", "hs_5_1", rgMacro);
			$m.lpPSCode = CompileShader(
				L"./engine/ge-spline.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpDSCode = CompileShader(
				L"./engine/ge-spline.hlsl", "DomainShading", "ds_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-spline.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-spline.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & ASSET_TYPE_Particle){
			$m.InitAsParticleShader(pD3dDevice, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & ASSET_TYPE_Terrain){
			$m.InitAsTerrainShader(pD3dDevice, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & ASSET_TYPE_Surface){
			$m.InitAsSurfaceShader(pD3dDevice, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-surface.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-surface.hlsl", "VertexShading", "vs_5_1", rgMacro);
		} else if(ResFlag & OUT_TYPE_IdBuff){
			$m.InitAsModelShader(pD3dDevice, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-modelid.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-modelid.hlsl", "VertexShading", "vs_5_1", rgMacro);
		} else{
			$m.InitAsModelShader(pD3dDevice, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-model.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-model.hlsl", "VertexShading", "vs_5_1", rgMacro);
		}
	}
	_secr $VOID InitAsModelShader(ID3DDevice *pD3dDevice, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		const USINT numParam = 12;
		const USINT numSampler = 5;
		D3DX12VIEWRANGE rgViewRange[6];
		D3DX12ROOTPARAM rgParam[numParam];
		D3DX12SSAMPLERDESC rgSmplrDesc[numSampler];
		D3DX12ROOTSIGDESC SignDesc;
		ID3DBlob *lpSerialize;

		rgViewRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
		rgViewRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);
		rgViewRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);
		rgViewRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0);
		rgViewRange[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0);
		rgViewRange[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);   //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);   //帧属性
		rgParam[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL); //材质属性
		rgParam[2].InitAsConstants(32, 2, 0, D3D12_SHADER_VISIBILITY_VERTEX);     //对象属性

		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);  //光源列表
		rgParam[5].InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY_VERTEX); //骨骼列表

		rgParam[6].InitAsDescriptorTable(1, &rgViewRange[0], D3D12_SHADER_VISIBILITY_PIXEL);  //漫反射贴图
		rgParam[7].InitAsDescriptorTable(1, &rgViewRange[1], D3D12_SHADER_VISIBILITY_PIXEL);  //法线贴图
		rgParam[8].InitAsDescriptorTable(1, &rgViewRange[2], D3D12_SHADER_VISIBILITY_PIXEL);  //金属性-粗糙度贴图
		rgParam[9].InitAsDescriptorTable(1, &rgViewRange[3], D3D12_SHADER_VISIBILITY_PIXEL);  //透明度贴图
		rgParam[10].InitAsDescriptorTable(1, &rgViewRange[4], D3D12_SHADER_VISIBILITY_PIXEL); //自发光贴图
		rgParam[11].InitAsDescriptorTable(1, &rgViewRange[5], D3D12_SHADER_VISIBILITY_PIXEL); //环境遮蔽贴图

		rgSmplrDesc[0] = pSmplrDesc ?
			*(D3DX12SSAMPLERDESC*)pSmplrDesc : D3DX12SSAMPLERDESC(0);
		rgSmplrDesc[1].Init(1, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		rgSmplrDesc[2].Init(2, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		rgSmplrDesc[3].Init(3, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
		rgSmplrDesc[4].Init(4, D3D12_FILTER_ANISOTROPIC,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER);

		SignDesc.Init(numParam, rgParam, numSampler, rgSmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsSplineShader(ID3DDevice *pD3dDevice, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		D3DX12ROOTPARAM rgParam[8];
		D3DX12VIEWRANGE ViewRange;
		D3DX12ROOTSIGDESC SignDesc;
		D3D12SSAMPLERDESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : D3DX12SSAMPLERDESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //帧属性
		rgParam[2].InitAsConstants(DW_CNT(SDSPLINE), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[3].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);       //光源列表

		SignDesc.Init(5, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsSurfaceShader(ID3DDevice *pD3dDevice, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		D3DX12ROOTPARAM rgParam[8];
		D3DX12VIEWRANGE ViewRange;
		D3DX12ROOTSIGDESC SignDesc;
		D3D12SSAMPLERDESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : D3DX12SSAMPLERDESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);   //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);   //帧属性
		rgParam[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL); //材质属性
		rgParam[2].InitAsConstants(DW_CNT(SDSURFACE), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);       //光源列表
		rgParam[5].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图

		SignDesc.Init(6, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsTerrainShader(ID3DDevice *pD3dDevice, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		D3DX12ROOTPARAM rgParam[8];
		D3DX12VIEWRANGE ViewRange;
		D3DX12ROOTSIGDESC SignDesc;
		D3D12SSAMPLERDESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : D3DX12SSAMPLERDESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //帧属性
		rgParam[2].InitAsConstants(DW_CNT(SDTERRAIN), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[3].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);    //光源列表
		rgParam[5].InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY_GEOMETRY); //法线列表
		rgParam[6].InitAsShaderResourceView(2, 1, D3D12_SHADER_VISIBILITY_GEOMETRY); //特征列表
		rgParam[7].InitAsShaderResourceView(3, 1, D3D12_SHADER_VISIBILITY_PIXEL);    //投影列表

		SignDesc.Init(8, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDevice->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_AND_PPV($m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsParticleShader(ID3DDevice *pD3dDevice, _in D3D12SSAMPLERDESC *pSmplrDesc = P_Null){
		/*ToDo*/
	}
	_open $VOID GetInputDesc(USINT SlotId, _out STLVector<D3D12INPUTELEMDESC> &rResult){
		USINT ByteOffset = 0;
		D3D12INPUTELEMDESC Template = { P_Null, 0, DXGI_FORMAT_UNKNOWN,
			SlotId, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		if($m.VtxFlag & VFB_Coord){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Position";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VECTOR3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Normal){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Normal";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VECTOR3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Tangent){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Tangent";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VECTOR3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Color){
			Template.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			Template.SemanticName = "IP_Color";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(COLOR32);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_PSize){
			Template.Format = DXGI_FORMAT_R32G32_FLOAT;
			Template.SemanticName = "IP_PSize";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(SIZE2F);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_BoneRig){
			Template.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			Template.SemanticName = "IP_BoneIndices";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(UTINY[4]);
			rResult.push_back(Template);

			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_BoneWeights";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(SPFPN[3]);
			rResult.push_back(Template);
		}

		//纹理坐标布局

		USINT TexLayer = 0;
		CHAR8 *rgSemantic[] = {
			"IP_TexCoord", "IP_TexCoord1", "IP_TexCoord2",
			"IP_TexCoord3", "IP_TexCoord4", "IP_TexCoord5" };

		while($m.VtxFlag & (VFB_TexCoord << TexLayer)){
			Template.Format = DXGI_FORMAT_R32G32_FLOAT;
			Template.SemanticName = rgSemantic[TexLayer];
			Template.SemanticIndex = TexLayer++;
			Template.AlignedByteOffset = ByteOffset;

			ByteOffset += sizeof(VECTOR2);
			rResult.push_back(Template);
		}
	}
	_open D3D12SHADERBYTECODE GetGeometryShaderCode(){
		if(!$m.lpGSCode) return D3D12SHADERBYTECODE{};
		return D3DX12SHADERBYTECODE($m.lpGSCode);
	}
	_open D3D12SHADERBYTECODE GetDomainShaderCode(){
		if(!$m.lpDSCode) return D3D12SHADERBYTECODE{};
		return D3DX12SHADERBYTECODE($m.lpDSCode);
	}
	_open D3D12SHADERBYTECODE GetVertexShaderCode(){
		if(!$m.lpVSCode) return D3D12SHADERBYTECODE{};
		return D3DX12SHADERBYTECODE($m.lpVSCode);
	}
	_open D3D12SHADERBYTECODE GetPixelShaderCode(){
		if(!$m.lpPSCode) return D3D12SHADERBYTECODE{};
		return D3DX12SHADERBYTECODE($m.lpPSCode);
	}
	_open D3D12SHADERBYTECODE GetHullShaderCode(){
		if(!$m.lpHSCode) return D3D12SHADERBYTECODE{};
		return D3DX12SHADERBYTECODE($m.lpHSCode);
	}
	_open D3D12PRIMTYPE GetPrimitiveType(){
		return $m.PrimType;
	}
	_open D3D1PRIMTOPO GetPrimitiveTopology(){
		return $m.PrimTopo;
	}
	_open ID3DRootSig *GetRootSignature(){
		return $m.lpRootSig;
	}
};

//图形PSO
class CGfxPso{
	_secr CShader *pShader;
	_secr ID3DPipeState *lpPipeState;

	_open ~CGfxPso(){
		$m.Finalize();
	}
	_open CGfxPso(){
		Var_Zero(this);
	}
	_open CGfxPso(ID3DDevice *pD3dDevice, CShader &Shader, GEWnd &Window,
		_in D3D12BLENDDESC *pBlendDesc = P_Null,
		_in D3D12RASTERIZERDESC *pRstrzrDesc = P_Null,
		_in D3D12DEPTHSTENCILDESC *pDsbDesc = P_Null)
	{
		$m.Initialize(pD3dDevice, Shader, Window, pBlendDesc, pRstrzrDesc, pDsbDesc);
	}
	//////
	_open $VOID Initialize(ID3DDevice *pD3dDevice, CShader &Shader, GEWnd &Window,
		_in D3D12BLENDDESC *pBlendDesc = P_Null,
		_in D3D12RASTERIZERDESC *pRstrzrDesc = P_Null,
		_in D3D12DEPTHSTENCILDESC *pDsbDesc = P_Null)
	{
		USINT RenderCnt;
		IBOOL bUseIdBuff;
		DXGI1FORMAT IdBuffFmt;
		D3D12RESDESC RtbDesc;
		D3D12RESDESC DsbDesc;
		D3D12GRAPHICSPIPEDESC PsoDesc = {};
		STLVector<D3D12INPUTELEMDESC> InputDesc;

		Shader.GetInputDesc(0, InputDesc);

		bUseIdBuff = (Shader.ResFlag & CShader::OUT_TYPE_IdBuff);
		RenderCnt = bUseIdBuff ? 2 : 1;
		IdBuffFmt = bUseIdBuff ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN;

		RtbDesc = Window.lpRTBuffer->GetDesc();
		DsbDesc = Window.lpDSBuffer->GetDesc();

		PsoDesc.NodeMask = 0x0;
		PsoDesc.SampleMask = UINT_MAX;

		PsoDesc.NumRenderTargets = RenderCnt;
		PsoDesc.RTVFormats[0] = RtbDesc.Format;
		PsoDesc.RTVFormats[1] = IdBuffFmt;
		PsoDesc.DSVFormat = DsbDesc.Format;

		PsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		PsoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		PsoDesc.PrimitiveTopologyType = Shader.GetPrimitiveType();
		PsoDesc.pRootSignature = Shader.GetRootSignature();

		PsoDesc.CachedPSO = {};
		PsoDesc.StreamOutput = {};

		PsoDesc.HS = Shader.GetHullShaderCode();
		PsoDesc.PS = Shader.GetPixelShaderCode();
		PsoDesc.DS = Shader.GetDomainShaderCode();
		PsoDesc.VS = Shader.GetVertexShaderCode();
		PsoDesc.GS = Shader.GetGeometryShaderCode();

		PsoDesc.SampleDesc.Count = RtbDesc.SampleDesc.Count;
		PsoDesc.SampleDesc.Quality = RtbDesc.SampleDesc.Quality;
		PsoDesc.InputLayout.NumElements = (USINT)InputDesc.size();
		PsoDesc.InputLayout.pInputElementDescs = InputDesc.data();

		PsoDesc.BlendState = pBlendDesc ? *pBlendDesc : D3DX12BLENDDESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = pRstrzrDesc ? *pRstrzrDesc : D3DX12RASTERIZERDESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = pDsbDesc ? *pDsbDesc : D3DX12DEPTHSTENCILDESC(D3D12_DEFAULT);

		pD3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_AND_PPV($m.lpPipeState));
		$m.pShader = &Shader;
	}
	_open $VOID Finalize(){
		SAFE_RELEASE($m.lpPipeState);
	}
	_open CShader *GetShader(){
		return $m.pShader;
	}
	_open ID3DPipeState *GetBase(){
		return $m.lpPipeState;
	}
};

//渲染工具
class CRenderer{
	_secr using MATVIEW = const CMtlRepos::MATVIEW;
	_secr enum PARAMPORT{
		SCENE_PORT,
		FRAME_PORT,
		OBJECT_PORT,
		SUBOBJ_PORT,
		LIGHTS_PORT,
		BONES_PORT,
		ALBEDO_MAP_PORT,
		NORMAL_MAP_PORT,
		ROUGH_MAP_PORT,
		ALPHA_MAP_PORT,
		EMISSION_MAP_PORT,
		OCCLUSION_MAP_PORT,
	};

	_secr SDSCENE *pSceneAttr;
	_secr SDFRAME *pFrameAttr;
	_secr HD3D12GPURES hSceneSrv;
	_secr HD3D12GPURES hFrameSrv;
	_secr ID3DCmdList1 *pCmdList;
	_secr CUploadBuff ShaderConst;

	_open ~CRenderer(){
		$m.Finalize();
	}
	_open CRenderer(){
		Var_Zero(this);
	}
	_open CRenderer(ID3DDevice *pD3dDevice, ID3DCmdList1 *pCmdList, USINT MaxFrame){
		$m.Initialize(pD3dDevice, pCmdList, MaxFrame);
	}
	///预置&销毁
	_open $VOID Initialize(ID3DDevice *pD3dDevice, ID3DCmdList1 *pCmdList, USINT MaxFrame){
		USINT BufPitch = sizeof(SDFRAME);
		USINT BufSplit = CEIL_DIV(sizeof(SDSCENE), BufPitch) + MaxFrame;

		$m.ShaderConst.Initialize(pD3dDevice, BufSplit, BufPitch, B_True);
		$m.ShaderConst.EnableUpdate(B_True, B_True);

		$m.pSceneAttr = (SDSCENE*)$m.ShaderConst.GetMemory(MaxFrame);
		$m.pFrameAttr = (SDFRAME*)$m.ShaderConst.GetMemory(0);

		$m.hSceneSrv = $m.ShaderConst.GetVram(MaxFrame);
		$m.hFrameSrv = $m.ShaderConst.GetVram(0);

		$m.pCmdList = pCmdList;
	}
	_open $VOID Finalize(){
		$m.ShaderConst.Finalize();
	}
	///上传区设置
	_open $VOID SetFrame(USINT Frame){
		$m.hFrameSrv = $m.ShaderConst.GetVram(Frame);
		$m.pFrameAttr = (SDFRAME*)$m.ShaderConst.GetMemory(Frame);
	}
	_open $VOID SetCamera(CCamera &Camera){
		Camera.GetPosition($m.pFrameAttr->CamCoord);
		Camera.GetViewProjMat($m.pFrameAttr->matViewProj);
	}
	_open $VOID SetTimeSpan(SPFPN TimeSpan){
		$m.pFrameAttr->TimeSpan = TimeSpan;
	}
	_open $VOID SetAmbientLight(_in VECTOR3 &Light){
		$m.pSceneAttr->AmbLight[0] = Light.x;
		$m.pSceneAttr->AmbLight[1] = Light.y;
		$m.pSceneAttr->AmbLight[2] = Light.z;
	}
	_open $VOID SetKdMap(_in SPFPN prgKey[8], _in SPFPN prgValue[8]){
		for(USINT iElem = 0; iElem < 8; ++iElem){
			$m.pSceneAttr->rgKdInput[iElem] = prgKey[iElem];
			$m.pSceneAttr->rgKdOutput[iElem] = prgValue[iElem];
		}
	}
	_open $VOID SetKsMap(_in SPFPN prgKey[8], _in SPFPN prgValue[8]){
		for(USINT iElem = 0; iElem < 8; ++iElem){
			$m.pSceneAttr->rgKsInput[iElem] = prgKey[iElem];
			$m.pSceneAttr->rgKsOutput[iElem] = prgValue[iElem];
		}
	}
	_open $VOID SetFog(_in VECTOR3 &Color, SPFPN Start, SPFPN Depth){
		$m.pSceneAttr->FogStart = Start;
		$m.pSceneAttr->FogDepth = Depth;
		$m.pSceneAttr->FogColor[0] = Color.x;
		$m.pSceneAttr->FogColor[1] = Color.y;
		$m.pSceneAttr->FogColor[2] = Color.z;
	}
	///根参数设置
	_open $VOID SetBones(HD3D12GPURES hBones){
		$m.pCmdList->SetGraphicsRootShaderResourceView(BONES_PORT, hBones);
	}
	_open $VOID SetLights(HD3D12GPURES hLights, USINT Count){
		$m.pSceneAttr->LightCnt = Count;
		$m.pCmdList->SetGraphicsRootShaderResourceView(LIGHTS_PORT, hLights);
	}
	_open $VOID SetSplineAttrs(_in SDSPLINE *pAttrs){
		SDSPLINE Attrs = *pAttrs;

		if(Attrs.Height == 0.f) Attrs.TexSplit = 1.f;
		else if(Attrs.bShell) Attrs.TexSplit = 0.5f;
		else Attrs.TexSplit = 1.f / 3.f;

		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDSPLINE), &Attrs, 0);
	}
	_open $VOID SetSurfaceAttrs(_in SDSURFACE *pAttrs){
		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDSURFACE), pAttrs, 0);
	}
	_open $VOID SetTerrainAttrs(_in SDTERRAIN *pAttrs){
		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDTERRAIN), pAttrs, 0);
	}
	_open $VOID SetModelAttrs(_in MMATRIX &matWorld, _in MMATRIX *pTexMat = P_Null, USINT Ident = 0){
		SDOBJECT ObjAttrs;
		MMATRIX *pWorldMat = &ObjAttrs.matWorld;
		MMATRIX *pTexXform = &ObjAttrs.matTexOrNorm;
		MMATRIX *pNormWorld = &ObjAttrs.matTexOrNorm;

		if(pTexMat){
			*pWorldMat = Mat_Transpose(matWorld);
			*pTexXform = Mat_Transpose(*pTexMat);
		} else{
			*pWorldMat = Mat_Transpose(matWorld);
			*pNormWorld = Mat_Inverse(P_Null, matWorld);
			pNormWorld->r[3] = dx::g_XMZero;
		}

		if(Ident != 0)
			pNormWorld->r[3] = Vec_SetZ(pNormWorld->r[3], AS_SPFPN(Ident));

		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDOBJECT), &ObjAttrs, 0);
	}
	_open $VOID SetResourceViewHeap(ID3DViewHeap *pViewHeap){
		$m.pCmdList->SetDescriptorHeaps(1, &pViewHeap);
	}
	_open $VOID SetPipelineState(CGfxPso &GfxPso){
		$m.pCmdList->SetPipelineState(GfxPso.GetBase());
		$m.pCmdList->SetGraphicsRootSignature(GfxPso.GetShader()->GetRootSignature());
		$m.pCmdList->IASetPrimitiveTopology(GfxPso.GetShader()->GetPrimitiveTopology());

		$m.pCmdList->SetGraphicsRootConstantBufferView(SCENE_PORT, $m.hSceneSrv);
		$m.pCmdList->SetGraphicsRootConstantBufferView(FRAME_PORT, $m.hFrameSrv);
	}
	///物体绘制
	_open $VOID EndDraw(GEWnd &Window){
		$m.pCmdList->ResourceBarrier(1, &D3DX12RESBARRIER::Transition(Window.lpRTBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	_open $VOID BeginDraw(GEWnd &Window){
		USINT TargetCnt = Window.hIBView.ptr ? 2 : 1;
		D3DX12RESBARRIER ResBarrier = D3DX12RESBARRIER::Transition(Window.lpRTBuffer,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		$m.pCmdList->ResourceBarrier(1, &ResBarrier);
		$m.pCmdList->OMSetRenderTargets(TargetCnt, &Window.hCBView, B_True, &Window.hDSView);
		$m.pCmdList->RSSetScissorRects(1, &Window.rcScissor);
		$m.pCmdList->RSSetViewports(1, &Window.Viewport);
	}
	_open $VOID ClearWindow(GEWnd &Window, _in COLOR128 &Color){
		$m.pCmdList->ClearRenderTargetView(Window.hCBView, Color, 0, P_Null);

		$m.pCmdList->ClearDepthStencilView(Window.hDSView,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, P_Null);

		if(Window.hIBView.ptr)
			$m.pCmdList->ClearRenderTargetView(Window.hIBView, COLOR128(0, 0, 0, 0), 0, P_Null);
	}
	_open $VOID DrawModel(CMesh3D &Mesh, CMtlRepos *pMtlRepos = P_Null){
		$m.pCmdList->IASetIndexBuffer(&Mesh.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexsSrv());

		if(Mesh.numSubset == 0){
			$m.pCmdList->DrawIndexedInstanced(Mesh.GetIndexCount(), 1, 0, 0, 0);
		} else{
			for(USINT iElem = 0; iElem < Mesh.numSubset; ++iElem){
				SUBMESH *pSubMesh = &Mesh.lprgSubset[iElem];
				MATVIEW *pMtlView = pMtlRepos->GetMaterialById(pSubMesh->MtlIdent);

				if(pMtlView->iColorMap != UINT16_MAX){
					$m.pCmdList->SetGraphicsRootDescriptorTable(ALBEDO_MAP_PORT,
						pMtlRepos->GetTextureSrv(pMtlView->iColorMap));
				}
				if(pMtlView->iNormMap != UINT16_MAX){
					$m.pCmdList->SetGraphicsRootDescriptorTable(NORMAL_MAP_PORT,
						pMtlRepos->GetTextureSrv(pMtlView->iNormMap));
				}

				$m.pCmdList->SetGraphicsRootConstantBufferView(SUBOBJ_PORT,
					pMtlRepos->GetMaterialVram(pMtlView->iMtlBase));
				$m.pCmdList->DrawIndexedInstanced(
					pSubMesh->IdxCount, 1, pSubMesh->IdxStart, 0, 0);
			}
		}
	}
	_open $VOID DrawModels(CMesh3D *pFirst, CMtlRepos *pMtlRepos = P_Null){
		for(auto pMesh = pFirst; pMesh; pMesh = pMesh->pNext)
			$m.DrawModel(*pMesh, pMtlRepos);
	}
	_open $VOID DrawModel(CMesh3D &Mesh, CMtlRepos *pMtlRepos, HD3D12GPURES hCurMtrl){
		$m.pCmdList->IASetIndexBuffer(&Mesh.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexsSrv());

		for(USINT iElem = 0; iElem < Mesh.numSubset; ++iElem){
			SUBMESH *pSubMesh = &Mesh.lprgSubset[iElem];
			MATVIEW *pMtlView = pMtlRepos->GetMaterialById(pSubMesh->MtlIdent);

			if(pMtlView->iColorMap != UINT16_MAX){
				$m.pCmdList->SetGraphicsRootDescriptorTable(ALBEDO_MAP_PORT,
					pMtlRepos->GetTextureSrv(pMtlView->iColorMap));
			}
			if(pMtlView->iNormMap != UINT16_MAX){
				$m.pCmdList->SetGraphicsRootDescriptorTable(NORMAL_MAP_PORT,
					pMtlRepos->GetTextureSrv(pMtlView->iNormMap));
			}

			$m.pCmdList->SetGraphicsRootConstantBufferView(
				SUBOBJ_PORT, hCurMtrl);
			$m.pCmdList->DrawIndexedInstanced(
				pSubMesh->IdxCount, 1, pSubMesh->IdxStart, 0, 0);
		}
	}
	_open $VOID DrawModels(CMesh3D *pFirst, CMtlRepos *pMtlRepos, HD3D12GPURES hCurMtrl){
		for(auto pMesh = pFirst; pMesh; pMesh = pMesh->pNext)
			$m.DrawModel(*pMesh, pMtlRepos, hCurMtrl);
	}
	_open $VOID DrawSpline(CVtxBuff &Spline, D3D12GPUVIEWPTR hTexView){
		$m.pCmdList->SetGraphicsRootDescriptorTable(3, hTexView);
		$m.pCmdList->IASetVertexBuffers(0, 1, &Spline.GetSrv());
		$m.pCmdList->DrawInstanced(Spline.GetCount(), Spline.GetCount() / 4, 0, 0);
	}
	_open $VOID DrawTerrain(CGrid &Terrain, D3D12GPUVIEWPTR hTexViews, HD3D12GPURES hPrimFeats, HD3D12GPURES hPrjctrs){
		$m.pCmdList->SetGraphicsRootDescriptorTable(3, hTexViews);
		$m.pCmdList->SetGraphicsRootShaderResourceView(5, Terrain.GetNormalsVram());
		$m.pCmdList->SetGraphicsRootShaderResourceView(6, hPrimFeats);
		$m.pCmdList->SetGraphicsRootShaderResourceView(7, hPrjctrs);
		$m.pCmdList->IASetIndexBuffer(&Terrain.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Terrain.GetVertexsSrv());
		$m.pCmdList->DrawIndexedInstanced(Terrain.IdxCount, 1, 0, 0, 0);
	}
	_open $VOID DrawSurface(CGrid &Surface, D3D12GPUVIEWPTR hTexViews, HD3D12GPURES hMtlBase){
		$m.pCmdList->SetGraphicsRootConstantBufferView(3, hMtlBase);
		$m.pCmdList->SetGraphicsRootDescriptorTable(5, hTexViews);
		$m.pCmdList->IASetIndexBuffer(&Surface.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Surface.GetVertexsSrv());
		$m.pCmdList->DrawIndexedInstanced(Surface.IdxCount, 1, 0, 0, 0);
	}
	_open $VOID DrawSurface(_in D3D12VBVIEW &VboView, _in D3D12IBVIEW &IboView, USINT IdxCount, D3D12GPUVIEWPTR hTexViews, HD3D12GPURES hMtlBase){
		$m.pCmdList->SetGraphicsRootConstantBufferView(3, hMtlBase);
		$m.pCmdList->SetGraphicsRootDescriptorTable(5, hTexViews);
		$m.pCmdList->IASetIndexBuffer(&IboView);
		$m.pCmdList->IASetVertexBuffers(0, 1, &VboView);
		$m.pCmdList->DrawIndexedInstanced(IdxCount, 1, 0, 0, 0);
	}
	//_open $VOID DrawParticles(CPtcSystem *pPtcSystem){
	//	USINT TexIndex;
	//	USINT IdxOffset = 0;
	////
	//	$m.pCmdList->IASetVertexBuffers(0, 1, &PtcSystem.GetVertexBufferView());
	//
	//	for(USINT iExe = 0; iExe < PtcSystem.numSubset; ++iExe){
	//		CPtcGroup *pSubset = &PtcSystem.arrSubset[iExe];
	//		TexIndex = $m.pMtlRepos->GetTextureIndexById(pSubset->TexIdent);
	//
	//		$m.rSBObject->matWorld(0, 1) = pSubset->PtcSize.x;
	//		$m.rSBObject->matWorld(1, 1) = pSubset->PtcSize.y;
	//
	//		$m.SetDefaultMaterial($m.pCmdList, pSubset->PtcColor.c);
	//
	//		$m.pCmdList->SetGraphicsRootDescriptorTable(
	//			$m.ParamLead.iTexture, $m.pMtlRepos->GetTextureGpuView(TexIndex));
	//		$m.pCmdList->SetGraphicsRootConstantBufferView(
	//			CShader::OBJECT, $m.hVMObject);
	//		$m.pCmdList->DrawInstanced(
	//			pSubset->PtcCount, 1, IdxOffset, 0);
	//
	//		$m.rSBObject++;
	//		$m.hVMObject += sizeof(SDOBJECT);
	//		IdxOffset += pSubset->PtcCount;
	//	}
	//}
	//_open $VOID DrawSkyDome(CSkyDome *pSkyDome){
	//	/*$m.pCmdList->SetPipelineState(SkyDome.lpPipeState);
	//	$m.pCmdList->SetGraphicsRootSignature(SkyDome.lpRootSig);
	//
	//	$m.pCmdList->IASetIndexBuffer(&SkyDome.GetIndexBufferView());
	//	$m.pCmdList->IASetVertexBuffers(0, 1, &SkyDome.GetVertexBufferView());
	//	$m.pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//	$m.pCmdList->SetDescriptorHeaps(1, &SkyDome.pSrvHeap);
	//	$m.pCmdList->SetGraphicsRootConstantBufferView(0, $m.hFrameSrv);
	//	$m.pCmdList->SetGraphicsRootDescriptorTable(1, SkyDome.GetTextureGpuView());
	//
	//	$m.pCmdList->DrawIndexedInstanced(
	//		SkyDome.cbIdxBuff / sizeof(UHALF), 1, 0, 0, 0);*/
	//}
};

//----------------------------------------//


//-------------------- #动画纹理# --------------------//

////视频过滤器
//class IVideoFilter/*: public IMFReaderCallback*/{
//	//_open INT32 ImgPitch;
//	//_open USINT ImgWidth;
//	//_open USINT ImgHeight;
//	//_secr USINT RefCount;
//	//_secr IBOOL bLastFrame;
//	//_open BYTET *ImgBytes;
//	//_secr GUID ImgFormat;
//	//_secr HANDLE hFenceEvent;
//	//_secr CRITSECTION CritSection;
//
//	//_open ~IVideoFilter(){
//	//	$m.Finalize();
//	//}
//	//_open IVideoFilter(){
//	//	$m.hFenceEvent = P_Null;
//	//}
//	//_open $VOID Finalize(){
//	//	if($m.hFenceEvent){
//	//		DeleteCriticalSection(&$m.CritSection);
//	//		CloseHandle($m.hFenceEvent);
//	//		$m.hFenceEvent = P_Null;
//	//	}
//	//}
//	//_open $VOID WaitForRead(IBOOL &bLastFrame){
//	//	/*WaitForSingleObject($m.hFenceEvent, INFINITE);
//	//	bLastFrame = $m.bLastFrame;*/
//	//}
//	//_open $VOID Initialize(/*IMFMediaType* lpMediaType, GUID &ImgFormat*/){
//	//	/*$m.RefCount = 1;
//	//	$m.bLastFrame = B_False;
//	//	$m.ImgFormat = ImgFormat;
//	//	$m.hFenceEvent = CreateEvent(P_Null, B_False, B_False, P_Null);
//
//	//	InitializeCriticalSection(&$m.CritSection);
//	//	MFGetAttributeSize(lpMediaType, MF_MT_FRAME_SIZE, &$m.ImgWidth, &$m.ImgHeight);
//
//	//	lpMediaType->GetUINT32(MF_MT_DEFAULT_STRIDE, (USINT*)&$m.ImgPitch);
//	//	if(ImgFormat == MFVideoFormat_RGB24) $m.ImgPitch /= 3;
//	//	else $m.ImgPitch /= 4;*/
//	//}
//	//_open HRESULT stdcall OnFlush(ULONG){
//	//	return S_OK;
//	//}
//	//_open HRESULT stdcall OnEvent(/*ULONG, IMFMediaEvent**/){
//	//	return S_OK;
//	//}
//	//_open HRESULT stdcall OnReadSample(/*HRESULT, ULONG, DWORD StreamAttr, INT64, IMFSample* lpSample*/){
//	//	/*if(StreamAttr & MF_SOURCE_READERF_ENDOFSTREAM){
//	//		$m.bLastFrame = B_True;
//	//		SetEvent($m.hFenceEvent);
//	//		return S_OK;
//	//	} else $m.bLastFrame = B_False;
//
//	//	BYTET *SrcBytes;
//	//	IMFMediaBuff *lpSrcBuff;
//
//	//	EnterCriticalSection(&$m.CritSection);
//
//	//	lpSample->GetBufferByIndex(0, &lpSrcBuff);
//	//	lpSrcBuff->Lock(&SrcBytes, P_Null, P_Null);
//
//	//	if(($m.ImgFormat == MFVideoFormat_RGB) ||
//	//		($m.ImgFormat == MFVideoFormat_ARGB))
//	//	{
//	//		COLOR32 *Dest = (COLOR32*)$m.ImgBytes;
//	//		COLOR32 *Source = ((COLOR32*)SrcBytes);
//
//	//		if($m.ImgPitch < 0)
//	//			Source -= $m.ImgPitch * ((INT32)$m.ImgHeight - 1);
//
//	//		for(USINT n = 0; n < $m.ImgHeight; ++n){
//	//			for(USINT iExe = 0; iExe < $m.ImgWidth; ++iExe)
//	//				Dest[iExe] = Source[iExe];
//	//			Dest += $m.ImgWidth;
//	//			Source -= $m.ImgPitch;
//	//		}
//	//	} else if($m.ImgFormat == MFVideoFormat_RGB24){
//	//		COLOR32 *Dest = (COLOR32*)$m.ImgBytes;
//	//		COLOR24 *Source = ((COLOR24*)SrcBytes);
//
//	//		if($m.ImgPitch < 0)
//	//			Source -= $m.ImgPitch * ((INT32)$m.ImgHeight - 1);
//
//	//		for(USINT n = 0; n < $m.ImgHeight; ++n){
//	//			for(USINT iExe = 0; iExe < $m.ImgWidth; ++iExe){
//	//				Dest[iExe].c = *(ARGB8*)&Source[iExe];
//	//				Dest[iExe].a = 0xFF;
//	//			}
//	//			Dest += $m.ImgWidth;
//	//			Source += $m.ImgPitch;
//	//		}
//	//	}
//
//	//	lpSrcBuff->Unlock();
//	//	lpSrcBuff->Release();
//
//	//	LeaveCriticalSection(&$m.CritSection);
//	//	SetEvent($m.hFenceEvent);*/
//
//	//	return S_OK;
//	//}
//	//_open HRESULT stdcall QueryInterface(REFIID iid, $VOID** Output){
//	//	/*static const QITAB QITable[] = { QITABENT(IVideoFilter, IMFReaderCallback), {} };
//	//	return QISearch(this, QITable, iid, Output);*/
//	//}
//	//_open ULONG stdcall AddRef(){
//	//	return InterlockedIncrement(&$m.RefCount);
//	//}
//	//_open ULONG stdcall Release(){
//	//	ULONG RefCount = InterlockedDecrement(&$m.RefCount);
//	//	if(RefCount == 0) delete this;
//	//	return RefCount;
//	//}
//};
//
////视频纹理
//class VideoTexture{
//	_secr IBOOL bUpdated;
//	_open USINT TexPitch;
//	_open ULEXT Duration;
//	_secr ID3DResource *lpULHeap;
//	_open ID3DResource *lpTexture;
//	_secr IVideoFilter *lpFilter;
//	/*_secr IMFSrcReader *lpReader;*/
//
//	_open ~VideoTexture(){
//		$m.Finalize();
//	}
//	_open VideoTexture(){
//		Var_Zero(this);
//	}
//	_open $VOID Finalize(){
//		/*if(!$m.lpReader) return;
//
//		SAFE_RELEASE($m.lpReader);
//		SAFE_RELEASE($m.lpFilter);
//		SAFE_RELEASE($m.lpULHeap);
//		SAFE_RELEASE($m.lpTexture);
//
//		Var_Zero(this);*/
//	}
//	_open $VOID ReadSample(){
//		/*HRESULT Result = $m.lpULHeap->Map(
//			0, P_Null, ($VOID**)&$m.lpFilter->ImgBytes);
//
//		if(SUCCEEDED(Result)){
//			$m.bUpdated = B_True;
//			$m.lpReader->ReadSample(
//				MF_SOURCE_READER_FIRST_VIDEO_STREAM,
//				0L, P_Null, P_Null, P_Null, P_Null);
//		}*/
//	}
//	_open $VOID UploadResource(ID3DCmdList1 *lpCmdList){
//		////等待采样结束
//
//		//if(!$m.bUpdated) return;
//
//		//IBOOL bLastFrame;
//		//PROPVARIANT MediaPos;
//
//		//$m.bUpdated = B_False;
//		//$m.lpFilter->WaitForRead(bLastFrame);
//		//$m.lpULHeap->Unmap(0, P_Null);
//
//		//if(bLastFrame){
//		//	MediaPos.vt = VT_I8;
//		//	MediaPos.hVal.QuadPart = 0;
//		//	$m.lpReader->SetCurrentPosition(GUID_NULL, MediaPos);
//		//	$m.ReadSample();
//		//	return;
//		//}
//
//		////上传采样
//
//		//D3DPlacedRsrcLayout layout = { 0, { DXGI_FORMAT_B8G8R8A8_UNORM,
//		//	$m.lpFilter->ImgWidth, $m.lpFilter->ImgHeight, 1, $m.TexPitch } };
//
//		//D3DXTexCopyLocation Dest($m.lpTexture, 0);
//		//D3DXTexCopyLocation Source($m.lpULHeap, layout);
//
//		//lpCmdList->ResourceBarrier(1, &D3DX12RESBARRIER::Transition($m.lpTexture,
//		//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
//
//		//lpCmdList->CopyTextureRegion(&Dest, 0, 0, 0, &Source, P_Null);
//
//		//lpCmdList->ResourceBarrier(1, &D3DX12RESBARRIER::Transition($m.lpTexture,
//		//	D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
//	}
//	_open $VOID InitFilter(){
//		/*IMFMediaType *lpType;
//		GUID MajorType, SubType;
//
//		$m.lpReader->GetNativeMediaType(
//			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
//			MF_SOURCE_READER_CURRENT_TYPE_INDEX,
//			&lpType);
//
//		lpType->GetGUID(MF_MT_MAJOR_TYPE, &MajorType);
//		lpType->GetGUID(MF_MT_SUBTYPE, &SubType);
//
//		$m.lpFilter->Initialize(lpType, SubType);
//		SAFE_RELEASE(lpType);*/
//	}
//	_open $VOID InitReader(WCHAR* VideoPath){
//		/*PROPVARIANT ReaderProp;
//		IMFAttribute *lpReaderAttr;
//
//		$m.lpFilter = new IVideoFilter;
//
//		MFCreateAttributes(&lpReaderAttr, 1);
//		lpReaderAttr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, $m.lpFilter);
//		MFCreateSourceReaderFromURL(VideoPath, lpReaderAttr, &$m.lpReader);
//
//		$m.lpReader->GetPresentationAttribute(
//			MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &ReaderProp);
//
//		PropVariantToInt64(ReaderProp, (INT64*)&$m.Duration);
//		PropVariantClear(&ReaderProp);
//
//		SAFE_RELEASE(lpReaderAttr);*/
//	}
//	_open $VOID CreateTexture(ID3DDevice* pD3dDevice){
//		/*USINT Width, Height, Size;
//
//		Width = $m.lpFilter->ImgWidth;
//		Height = $m.lpFilter->ImgHeight;
//		Size = Width * Height * sizeof(ARGB8);
//
//		$m.TexPitch = Size / Height;
//
//		pD3dDevice->CreateCommittedResource(
//			&D3DXHeapProps(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
//			&D3DXRsrcDesc::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, Width, Height),
//			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, P_Null, IID_AND_PPV($m.lpTexture));
//
//		pD3dDevice->CreateCommittedResource(
//			&D3DXHeapProps(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
//			&D3DXRsrcDesc::Buffer(Size),
//			D3D12_RESOURCE_STATE_GENERIC_READ, P_Null, IID_AND_PPV($m.lpULHeap));*/
//	}
//	_open USINT Initialize(ID3DDevice* pD3dDevice, WCHAR* VideoPath){
//		$m.bUpdated = B_False;
//		$m.InitReader(VideoPath);
//		$m.InitFilter();
//		$m.CreateTexture(pD3dDevice);
//	}
//};

//----------------------------------------//


//-------------------- #投影特效# --------------------//

////镜面
//struct Mirror{
//	CGfxPso *pMirrorPSO;
//	CGfxPso *pMarkingPSO;
//	CRenderer *pRenderer;
//
//	~Mirror(){
//		$m.Finalize();
//	}
//	Mirror(){
//		Var_Zero(this);
//	}
//	$VOID Finalize(){
//		SAFE_DELETE($m.pMirrorPSO);
//		SAFE_DELETE($m.pMarkingPSO);
//	}
//	$VOID Initialize(ID3DDevice* pD3dDevice, D3D12GRAPHICSPIPEDESC &BasicDesc, CShader &Shader){
//		//ID3DPipeState *lpPipeState;
//		//D3DDepthStencilDesc *pDsbDesc;
//		//D3DBufBlendDesc *pRTDesc;
//		//D3DGfxPipeStateDesc PsoDesc;
//		//STLVector<D3D12INPUTELEMDESC> InputDesc(16);
//
//		//Shader.GetInputDesc(0, InputDesc);
//
//		//BasicDesc.VS = Shader.GetVertexShaderCode();
//		//BasicDesc.PS = Shader.GetPixelShaderCode();
//		//BasicDesc.pRootSignature = Shader.lpRootSig;
//		//BasicDesc.InputLayout.NumElements = (USINT)InputDesc.size();
//		//BasicDesc.InputLayout.pInputElementDescs = InputDesc.data();
//
//		////创建镜面PSO
//
//		//PsoDesc = BasicDesc;
//		//pRTDesc = &PsoDesc.BlendState.RenderTarget[0];
//
//		//pRTDesc->BlendEnable = B_True;
//		//pRTDesc->BlendOp = D3D12_BLEND_OP_ADD;
//		//pRTDesc->SrcBlend = D3D12_BLEND_SRC_ALPHA;
//		//pRTDesc->DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//		//pRTDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//		//pD3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_AND_PPV(lpPipeState));
//
//		//$m.pMirrorPSO->pShader = &Shader;
//		//$m.pMirrorPSO->lpPipeState = lpPipeState;
//
//		////创建标记PSO
//
//		//PsoDesc = BasicDesc;
//		//pDsbDesc = (D3DDepthStencilDesc*)&PsoDesc.DepthStencilState;
//
//		//pDsbDesc->StencilEnable = B_True;
//		//pDsbDesc->StencilReadMask = 0xFF;
//		//pDsbDesc->StencilWriteMask = 0xFF;
//		//pDsbDesc->DepthEnable = B_True;
//		//pDsbDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//		//pDsbDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
//
//		//pDsbDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
//		//pDsbDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
//		//pDsbDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
//		//pDsbDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
//
//		//PsoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0;
//
//		//pD3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_AND_PPV(lpPipeState));
//
//		//$m.pMirrorPSO->pShader = &Shader;
//		//$m.pMirrorPSO->lpPipeState = lpPipeState;
//	}
//	$VOID DrawMarking(ID3DCmdList1* lpCmdList, CModel *pMirror, MATRIX4 &matWorld, MATRIX4 &matTexture){
//		/*$m.pRenderer->SetPso(lpCmdList, $m.pMarkingPSO);
//		lpCmdList->OMSetStencilRef(1);
//		$m.pRenderer->SetTransformation(lpCmdList, matWorld, matTexture);
//		$m.pRenderer->DrawModels(lpCmdList, pMirror);*/
//	}
//	$VOID DrawMirror(ID3DCmdList1* lpCmdList, CModel *pMirror, MATRIX4 &matWorld, MATRIX4 &matTexture){
//		/*$m.pRenderer->SetPso(lpCmdList, $m.pMirrorPSO);
//		lpCmdList->OMSetStencilRef(0);
//		$m.pRenderer->SetTransformation(lpCmdList, matWorld, matTexture);
//		$m.pRenderer->DrawModels(lpCmdList, pMirror);*/
//	}
//	$VOID SetRenderer(CRenderer &Renderer){
//		$m.pRenderer = &Renderer;
//	}
//
//	static $VOID BuildReflectionMatrix(PLANE &Plane, MATRIX4 &matOut){
//		MMATRIX matReflect = Mat_Reflect(Pck_V4(&Plane));
//		Upk_Mat(&matOut, matReflect);
//	}
//	static $VOID BuildImagePSO(ID3DDevice* pD3dDevice, D3D12GRAPHICSPIPEDESC &BasicDesc, CShader &Shader, CGfxPso &Output){
//		/*ID3DPipeState *lpPipeState;
//		D3DGfxPipeStateDesc PsoDesc = BasicDesc;
//		D3DDepthStencilDesc *pDsbDesc = (D3DDepthStencilDesc*)&PsoDesc.DepthStencilState;
//		STLVector<D3D12INPUTELEMDESC> InputDesc(16);
//
//		Shader.GetInputDesc(0, InputDesc);
//
//		PsoDesc.VS = Shader.GetVertexShaderCode();
//		PsoDesc.PS = Shader.GetPixelShaderCode();
//		PsoDesc.pRootSignature = Shader.lpRootSig;
//		PsoDesc.InputLayout.NumElements = (USINT)InputDesc.size();
//		PsoDesc.InputLayout.pInputElementDescs = InputDesc.data();
//
//		PsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
//		PsoDesc.RasterizerState.FrontCounterClockwise = B_True;
//
//		pDsbDesc->StencilEnable = B_True;
//		pDsbDesc->StencilReadMask = 0xFF;
//		pDsbDesc->StencilWriteMask = 0xFF;
//		pDsbDesc->DepthEnable = B_True;
//		pDsbDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//		pDsbDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//
//		pDsbDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
//		pDsbDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
//		pDsbDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
//		pDsbDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
//
//		pD3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_AND_PPV(lpPipeState));
//	
//		Output.pShader = &Shader;
//		Output.lpPipeState = lpPipeState;*/
//	}
//};
//
////阴影
//struct Shadow{
//	static $VOID BuildShadowMatrix(PLANE &Plane, VECTOR4 &Light, MATRIX4 &matOut){
//		/*MMATRIX matShadow = Mat_Shadow(Pck_V4(Plane), Pck_V4(Light));
//		Upk_Mat(matOut, matShadow);*/
//	}
//	static $VOID PrepDrawShadow(ID3DCmdList1 *lpCmdList){
//		lpCmdList->OMSetStencilRef(0);
//	}
//	static $VOID GetShadowMaterial(SPFPN opacity, MATBASE &Output){
//		Output.Albedo = { 0.f, 0.f, 0.f };
//		Output.Fresnel = { 0.001f, 0.001f, 0.001f };
//		Output.Shininess = 0.f;
//		Output.Opacity = opacity;
//	}
//	static $VOID BuildShadowPSO(ID3DDevice* pD3dDevice, D3D12GRAPHICSPIPEDESC &BasicDesc, CShader &Shader, CGfxPso &Output){
//		/*ID3DPipeState *lpPipeState;
//		D3DDepthStencilDesc *pDsbDesc;
//		D3DGfxPipeStateDesc PsoDesc;
//		D3DBufBlendDesc *pRTDesc;
//		STLVector<D3D12INPUTELEMDESC> InputDesc(16);
//		
//		Shader.GetInputDesc(0, InputDesc);
//
//		PsoDesc = BasicDesc;
//		pDsbDesc = (D3DDepthStencilDesc*)&PsoDesc.DepthStencilState;
//		pRTDesc = &PsoDesc.BlendState.RenderTarget[0];
//
//		PsoDesc.VS = Shader.GetVertexShaderCode();
//		PsoDesc.PS = Shader.GetPixelShaderCode();
//		PsoDesc.pRootSignature = Shader.lpRootSig;
//		PsoDesc.InputLayout.NumElements = (USINT)InputDesc.size();
//		PsoDesc.InputLayout.pInputElementDescs = InputDesc.data();
//
//		pRTDesc->BlendEnable = B_True;
//		pRTDesc->BlendOp = D3D12_BLEND_OP_ADD;
//		pRTDesc->SrcBlend = D3D12_BLEND_SRC_ALPHA;
//		pRTDesc->DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//		pRTDesc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//		pDsbDesc->StencilEnable = B_True;
//		pDsbDesc->StencilReadMask = 0xFF;
//		pDsbDesc->StencilWriteMask = 0xFF;
//		pDsbDesc->DepthEnable = B_True;
//		pDsbDesc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//		pDsbDesc->DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//
//		pDsbDesc->FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
//		pDsbDesc->FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
//		pDsbDesc->FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR_SAT;
//		pDsbDesc->FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
//
//		pD3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_AND_PPV(lpPipeState));
//
//		Output.pShader = &Shader;
//		Output.lpPipeState = lpPipeState;*/
//	}
//};

//----------------------------------------//