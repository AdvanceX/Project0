//-------------------- 着色器数据 --------------------//

// Shader Data: Scene
struct SDScene{
	DWORD ExtraFlag;    //着色标记
	SPFPN FogColor[3];   //雾颜色
	UNS32 LightCnt;      //光源数量
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
struct SDFrame{
	MFlt4x4 matViewProj; //观察投影矩阵
	VFloat3 EyeCoord;    //相机位置
	SPFPN TimeSpan;       //时间间隔
};

// Shader Data: Object
struct SDObject{
	MXMATRIX matWorld;     //世界矩阵
	MXMATRIX matTexOrNorm; //纹理/法线矩阵
};

// Shader Data: Spline
struct SDSpline{
	SPFPN Pitch;
	SPFPN Width;
	SPFPN WidthB;
	SPFPN Height;
	SPFPN Roughness;
	SPFPN Metalness;
	SPFPN TexSplit;
	SPFPN BumpScale;
	IBOOL bShell;
	VFloat3 UpDir;
	MFlt4x4 matWorld;
};

// Shader Data: Surface
struct SDSurface{
	SPFPN rgWavDirX[4];
	SPFPN rgWavDirZ[4];
	SPFPN rgWavPhase[4];
	SPFPN rgWavSlope[4];
	SPFPN rgWavAmplit[4];
	SPFPN rgWavLength[4];
	SPFPN LowerLeftX;
	SPFPN LowerLeftY;
	SPFPN LowerLeftZ;
	SPFPN LowerLeftW;
	SPFPN TexScaleU;
	SPFPN TexScaleV;
	SPFPN CellWidth;
	SPFPN CellDepth;
	ARGB8 WireColor;
	ARGB8 BkgdColor;
};

// Shader Data: Terrain
struct SDTerrain{
	ARGB8 ResAttr;    //资源属性
	ARGB8 CurColor;   //指定颜色
	ARGB8 SlideIds0;  //幻灯片ID
	ARGB8 SlideIds1;  //幻灯片ID
	UNS32 ColCount;   //网格行数
	UNS32 RowCount;   //网格列数
	SPFPN AreaWidth;  //区域宽度
	SPFPN AreaDepth;  //区域深度
	SPFPN CellWidth;  //单元格宽度
	SPFPN CellDepth;  //单元格深度
	SPFPN TileWidth;  //贴砖宽度
	SPFPN TileDepth;  //贴砖深度
	SPFPN LowerLeftX; //左下角坐标X
	SPFPN LowerLeftY; //左下角坐标Y
	SPFPN LowerLeftZ; //左下角坐标Z
	SPFPN LayerSize;  //层高度
};

// Shader Data: Grid Location
struct SDGridLoc{
	UNS32 ColId;  //行ID
	UNS32 RowId;  //列ID
	DWORD DirTag; //方向
};

//----------------------------------------//


//-------------------- 渲染管线 --------------------//

// Shader
class CShader{
	_open enum OUTPUTTYPE{
		OUT_TYPE_ColorBuff = (1L << 24),
		OUT_TYPE_IdBuff = (1L << 25),
		OUT_TYPE_ModelArr = (1L << 26),
	};

	_open DWORD VtxFlag;
	_open DWORD ResFlag;
	_secr ID3DBlob *lpVSCode;
	_secr ID3DBlob *lpHSCode;
	_secr ID3DBlob *lpDSCode;
	_secr ID3DBlob *lpGSCode;
	_secr ID3DBlob *lpPSCode;
	_secr ID3D12RootSignature *lpRootSig;
	_secr D3D_PRIMITIVE_TOPOLOGY PrimTopo;
	_secr D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimType;

	_open ~CShader(){
		$m.Release();
	}
	_open CShader(){
		Var_Zero(this);
	}
	_open CShader(ID3D12Device4 *pD3dDev, DWORD VtxFlag, DWORD ResFlag, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		$m.Initialize(pD3dDev, VtxFlag, ResFlag, pSmplrDesc);
	}
	//////
	_open $VOID Release(){
		if($m.lpRootSig){
			SAFE_RELEASE($m.lpVSCode);
			SAFE_RELEASE($m.lpHSCode);
			SAFE_RELEASE($m.lpDSCode);
			SAFE_RELEASE($m.lpGSCode);
			SAFE_RELEASE($m.lpPSCode);
			SAFE_RELEASE($m.lpRootSig);
		}
	}
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, DWORD VtxFlag, DWORD ResFlag, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		UNS32 numMacro = 0;
		D3D_SHADER_MACRO rgMacro[16] = {};

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
		} else if(ResFlag & ASSET_TYPE_Surface){
			$m.PrimTopo = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			$m.PrimType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		} else if(ResFlag & ASSET_TYPE_Terrain){
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

		if(ResFlag & TEX_TYPE_Opacity)
			rgMacro[numMacro++] = { "REQ_ALPHA_MAP", P_Null };
		if(ResFlag & TEX_TYPE_Albedo)
			rgMacro[numMacro++] = { "REQ_COLOR_MAP", P_Null };
		if(ResFlag & TEX_TYPE_MetalRg)
			rgMacro[numMacro++] = { "REQ_ROUGH_MAP", P_Null };
		if(ResFlag & TEX_TYPE_Normal)
			rgMacro[numMacro++] = { "REQ_NORMAL_MAP", P_Null };
		if(ResFlag & TEX_TYPE_Height)
			rgMacro[numMacro++] = { "REQ_HEIGHT_MAP", P_Null };
		if(ResFlag & TEX_TYPE_Emission)
			rgMacro[numMacro++] = { "REQ_EMISSION_MAP", P_Null };
		if(ResFlag & TEX_TYPE_Occlusion)
			rgMacro[numMacro++] = { "REQ_OCCLUSION_MAP", P_Null };

		//字节码和根签名

		if(ResFlag & ASSET_TYPE_Spline){
			$m.InitAsSplineShader(pD3dDev, pSmplrDesc);

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
			$m.InitAsParticleShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-particle.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & ASSET_TYPE_Terrain){
			$m.InitAsTerrainShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-terrain.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & ASSET_TYPE_Surface){
			$m.InitAsSurfaceShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-surface.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-surface.hlsl", "VertexShading", "vs_5_1", rgMacro);
			$m.lpGSCode = CompileShader(
				L"./engine/ge-surface.hlsl", "GeometryShading", "gs_5_1", rgMacro);
		} else if(ResFlag & OUT_TYPE_IdBuff){
			$m.InitAsModelShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-modelid.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-modelid.hlsl", "VertexShading", "vs_5_1", rgMacro);
		} else if(ResFlag & OUT_TYPE_ModelArr){
			$m.InitAsModelArrayShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-modelarr.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-modelarr.hlsl", "VertexShading", "vs_5_1", rgMacro);
		} else{
			$m.InitAsModelShader(pD3dDev, pSmplrDesc);

			$m.lpPSCode = CompileShader(
				L"./engine/ge-model.hlsl", "PixelShading", "ps_5_1", rgMacro);
			$m.lpVSCode = CompileShader(
				L"./engine/ge-model.hlsl", "VertexShading", "vs_5_1", rgMacro);
		}
	}
	_secr $VOID InitAsModelShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		const UNS32 numParam = 12;
		const UNS32 numSampler = 5;
		CD3DX12_DESCRIPTOR_RANGE rgViewRange[6];
		CD3DX12_ROOT_PARAMETER rgParam[numParam];
		CD3DX12_STATIC_SAMPLER_DESC rgSmplrDesc[numSampler];
		CD3DX12_ROOT_SIGNATURE_DESC SignDesc;
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
			*(CD3DX12_STATIC_SAMPLER_DESC*)pSmplrDesc : CD3DX12_STATIC_SAMPLER_DESC(0);
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

		pD3dDev->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_PPV_ARGS(&$m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsSplineShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		CD3DX12_ROOT_PARAMETER rgParam[8];
		CD3DX12_DESCRIPTOR_RANGE ViewRange;
		CD3DX12_ROOT_SIGNATURE_DESC SignDesc;
		D3D12_STATIC_SAMPLER_DESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : CD3DX12_STATIC_SAMPLER_DESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //帧属性
		rgParam[2].InitAsConstants(DW_CNT(SDSpline), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[3].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);       //光源列表

		SignDesc.Init(5, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDev->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_PPV_ARGS(&$m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsSurfaceShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		CD3DX12_ROOT_PARAMETER rgParam[6];
		CD3DX12_DESCRIPTOR_RANGE ViewRange;
		CD3DX12_ROOT_SIGNATURE_DESC SignDesc;
		D3D12_STATIC_SAMPLER_DESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : CD3DX12_STATIC_SAMPLER_DESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);   //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);   //帧属性
		rgParam[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_PIXEL); //材质属性
		rgParam[2].InitAsConstants(DW_CNT(SDSurface), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);       //光源列表
		//rgParam[5].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图

		SignDesc.Init(5, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDev->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_PPV_ARGS(&$m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsTerrainShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		ID3DBlob *lpSerialize;
		CD3DX12_ROOT_PARAMETER rgParam[10];
		CD3DX12_DESCRIPTOR_RANGE ViewRange;
		CD3DX12_ROOT_SIGNATURE_DESC SignDesc;
		D3D12_STATIC_SAMPLER_DESC SmplrDesc;

		SmplrDesc = pSmplrDesc ? *pSmplrDesc : CD3DX12_STATIC_SAMPLER_DESC(0);

		ViewRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //帧属性
		rgParam[2].InitAsConstants(DW_CNT(SDTerrain), 2, 0, D3D12_SHADER_VISIBILITY_ALL); //对象属性
		rgParam[3].InitAsDescriptorTable(1, &ViewRange, D3D12_SHADER_VISIBILITY_PIXEL); //纹理贴图
		rgParam[4].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);    //光源列表
		rgParam[5].InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY_GEOMETRY); //图元特征
		rgParam[6].InitAsShaderResourceView(2, 1, D3D12_SHADER_VISIBILITY_GEOMETRY); //高度场
		rgParam[7].InitAsShaderResourceView(3, 1, D3D12_SHADER_VISIBILITY_PIXEL);    //纹理投影

		SignDesc.Init(8, rgParam, 1, &SmplrDesc,
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);

		D3D12SerializeRootSignature(&SignDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0, &lpSerialize, P_Null);

		pD3dDev->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_PPV_ARGS(&$m.lpRootSig));

		lpSerialize->Release();
	}
	_secr $VOID InitAsParticleShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		/*ToDo*/
	}
	_secr $VOID InitAsModelArrayShader(ID3D12Device4 *pD3dDev, _in D3D12_STATIC_SAMPLER_DESC *pSmplrDesc = P_Null){
		const UNS32 numParam = 11;
		const UNS32 numSampler = 5;
		CD3DX12_DESCRIPTOR_RANGE rgViewRange[6];
		CD3DX12_ROOT_PARAMETER rgParam[numParam];
		CD3DX12_STATIC_SAMPLER_DESC rgSmplrDesc[numSampler];
		CD3DX12_ROOT_SIGNATURE_DESC SignDesc;
		ID3DBlob *lpSerialize;

		rgViewRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
		rgViewRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);
		rgViewRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);
		rgViewRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0);
		rgViewRange[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0);
		rgViewRange[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 0);

		rgParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); //场景属性
		rgParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); //帧属性
		rgParam[2].InitAsConstants(32, 2, 0, D3D12_SHADER_VISIBILITY_VERTEX);   //对象组属性
		rgParam[3].InitAsShaderResourceView(0, 1, D3D12_SHADER_VISIBILITY_VERTEX); //实例数据
		rgParam[4].InitAsShaderResourceView(1, 1, D3D12_SHADER_VISIBILITY_PIXEL);  //光源列表

		rgParam[5].InitAsDescriptorTable(1, &rgViewRange[0], D3D12_SHADER_VISIBILITY_PIXEL);  //漫反射贴图
		rgParam[6].InitAsDescriptorTable(1, &rgViewRange[1], D3D12_SHADER_VISIBILITY_PIXEL);  //法线贴图
		rgParam[7].InitAsDescriptorTable(1, &rgViewRange[2], D3D12_SHADER_VISIBILITY_PIXEL);  //金属性-粗糙度贴图
		rgParam[8].InitAsDescriptorTable(1, &rgViewRange[3], D3D12_SHADER_VISIBILITY_PIXEL);  //透明度贴图
		rgParam[9].InitAsDescriptorTable(1, &rgViewRange[4], D3D12_SHADER_VISIBILITY_PIXEL);  //自发光贴图
		rgParam[10].InitAsDescriptorTable(1, &rgViewRange[5], D3D12_SHADER_VISIBILITY_PIXEL); //环境遮蔽贴图

		rgSmplrDesc[0] = pSmplrDesc ?
			*(CD3DX12_STATIC_SAMPLER_DESC*)pSmplrDesc : CD3DX12_STATIC_SAMPLER_DESC(0);
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

		pD3dDev->CreateRootSignature(0x0, lpSerialize->GetBufferPointer(),
			lpSerialize->GetBufferSize(), IID_PPV_ARGS(&$m.lpRootSig));

		lpSerialize->Release();
	}
	_open $VOID GetInputDesc(UNS32 SlotId, _out TSeqList<D3D12_INPUT_ELEMENT_DESC> &rResult){
		UNS32 ByteOffset = 0;
		D3D12_INPUT_ELEMENT_DESC Template = { P_Null, 0, DXGI_FORMAT_UNKNOWN,
			SlotId, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		if($m.VtxFlag & VFB_RowCol){
			Template.Format = DXGI_FORMAT_R32G32_UINT;
			Template.SemanticName = "IP_RowCol";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(UNS32[2]);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Coord){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Position";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VFloat3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Normal){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Normal";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VFloat3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Tangent){
			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_Tangent";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VFloat3);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_Color){
			Template.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			Template.SemanticName = "IP_Color";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(PVColor);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_PSize){
			Template.Format = DXGI_FORMAT_R32G32_FLOAT;
			Template.SemanticName = "IP_PSize";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(VFloat2);
			rResult.push_back(Template);
		}
		if($m.VtxFlag & VFB_BoneRig){
			Template.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			Template.SemanticName = "IP_BoneIndices";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(UNS08[4]);
			rResult.push_back(Template);

			Template.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			Template.SemanticName = "IP_BoneWeights";
			Template.AlignedByteOffset = ByteOffset;
			ByteOffset += sizeof(SPFPN[3]);
			rResult.push_back(Template);
		}

		//纹理坐标布局

		UNS32 TexLayer = 0;
		CHAR8 *rgSemantic[] = {
			"IP_TexCoord", "IP_TexCoord1", "IP_TexCoord2",
			"IP_TexCoord3", "IP_TexCoord4", "IP_TexCoord5" };

		while($m.VtxFlag & (VFB_TexCoord << TexLayer)){
			Template.Format = DXGI_FORMAT_R32G32_FLOAT;
			Template.SemanticName = rgSemantic[TexLayer];
			Template.SemanticIndex = TexLayer++;
			Template.AlignedByteOffset = ByteOffset;

			ByteOffset += sizeof(VFloat2);
			rResult.push_back(Template);
		}
	}
	_open D3D12_SHADER_BYTECODE GetGeometryShaderCode(){
		if(!$m.lpGSCode) return D3D12_SHADER_BYTECODE{};
		return CD3DX12_SHADER_BYTECODE($m.lpGSCode);
	}
	_open D3D12_SHADER_BYTECODE GetDomainShaderCode(){
		if(!$m.lpDSCode) return D3D12_SHADER_BYTECODE{};
		return CD3DX12_SHADER_BYTECODE($m.lpDSCode);
	}
	_open D3D12_SHADER_BYTECODE GetVertexShaderCode(){
		if(!$m.lpVSCode) return D3D12_SHADER_BYTECODE{};
		return CD3DX12_SHADER_BYTECODE($m.lpVSCode);
	}
	_open D3D12_SHADER_BYTECODE GetPixelShaderCode(){
		if(!$m.lpPSCode) return D3D12_SHADER_BYTECODE{};
		return CD3DX12_SHADER_BYTECODE($m.lpPSCode);
	}
	_open D3D12_SHADER_BYTECODE GetHullShaderCode(){
		if(!$m.lpHSCode) return D3D12_SHADER_BYTECODE{};
		return CD3DX12_SHADER_BYTECODE($m.lpHSCode);
	}
	_open D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(){
		return $m.PrimType;
	}
	_open D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(){
		return $m.PrimTopo;
	}
	_open ID3D12RootSignature *GetRootSignature(){
		return $m.lpRootSig;
	}
};

// Graphics PSO
class CGfxPso{
	_secr CShader *pShader;
	_secr ID3D12PipelineState *lpPipeState;

	_open ~CGfxPso(){
		$m.Release();
	}
	_open CGfxPso(){
		Var_Zero(this);
	}
	_open CGfxPso(ID3D12Device4 *pD3dDev, CShader &Shader, GEWnd &Window,
		_in D3D12_BLEND_DESC *pBlendDesc = P_Null,
		_in D3D12_RASTERIZER_DESC *pRstrzrDesc = P_Null,
		_in D3D12_DEPTH_STENCIL_DESC *pDsbDesc = P_Null)
	{
		$m.Initialize(pD3dDev, Shader, Window, pBlendDesc, pRstrzrDesc, pDsbDesc);
	}
	//////
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, CShader &Shader, GEWnd &Window,
		_in D3D12_BLEND_DESC *pBlendDesc = P_Null,
		_in D3D12_RASTERIZER_DESC *pRstrzrDesc = P_Null,
		_in D3D12_DEPTH_STENCIL_DESC *pDsbDesc = P_Null)
	{
		UNS32 TargetCnt;
		IBOOL bUseIdBuff;
		DXGI_FORMAT IdFormat;
		D3D12_RESOURCE_DESC RtbDesc;
		D3D12_RESOURCE_DESC DsbDesc;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		TSeqList<D3D12_INPUT_ELEMENT_DESC> InputDesc;

		Shader.GetInputDesc(0, InputDesc);

		bUseIdBuff = (Shader.ResFlag & CShader::OUT_TYPE_IdBuff);
		IdFormat = bUseIdBuff ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN;
		TargetCnt = bUseIdBuff ? 2 : 1;

		RtbDesc = Window.lpRTBuffer->GetDesc();
		DsbDesc = Window.lpDSBuffer->GetDesc();

		PsoDesc.NodeMask = 0x0;
		PsoDesc.SampleMask = UINT_MAX;

		PsoDesc.NumRenderTargets = TargetCnt;
		PsoDesc.RTVFormats[0] = RtbDesc.Format;
		PsoDesc.RTVFormats[1] = IdFormat;
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
		PsoDesc.InputLayout.NumElements = (UNS32)InputDesc.size();
		PsoDesc.InputLayout.pInputElementDescs = InputDesc.data();

		PsoDesc.BlendState = pBlendDesc ? *pBlendDesc : CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = pRstrzrDesc ? *pRstrzrDesc : CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = pDsbDesc ? *pDsbDesc : CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

		pD3dDev->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&$m.lpPipeState));
		$m.pShader = &Shader;
	}
	_open $VOID Release(){
		SAFE_RELEASE($m.lpPipeState);
	}
	_open CShader *GetShader(){
		return $m.pShader;
	}
	_open ID3D12PipelineState *GetBase(){
		return $m.lpPipeState;
	}
};

// Renderer
class CRenderer{
	_secr using NMtlView = const CMtlRepos::NMtlView;
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

	_secr SDScene *pSceneAttr;
	_secr SDFrame *pFrameAttr;
	_secr D3D12_GPU_VIRTUAL_ADDRESS hSceneSrv;
	_secr D3D12_GPU_VIRTUAL_ADDRESS hFrameSrv;
	_secr ID3D12GraphicsCommandList *pCmdList;
	_secr CUploadBuff ShaderConst;

	_open ~CRenderer(){
		$m.Release();
	}
	_open CRenderer(){
		Var_Zero(this);
	}
	_open CRenderer(ID3D12Device4 *pD3dDev, ID3D12GraphicsCommandList *pCmdList, UNS32 MaxFrame){
		$m.Initialize(pD3dDev, pCmdList, MaxFrame);
	}
	///预置&销毁
	_open $VOID Initialize(ID3D12Device4 *pD3dDev, ID3D12GraphicsCommandList *pCmdList, UNS32 MaxFrame){
		UNS32 BufPitch = sizeof(SDFrame);
		UNS32 BufSplit = CEIL_DIV(sizeof(SDScene), BufPitch) + MaxFrame;

		$m.ShaderConst.Initialize(pD3dDev, BufSplit, BufPitch, B_True);
		$m.ShaderConst.EnableUpdate(B_True, B_True);

		$m.pSceneAttr = (SDScene*)$m.ShaderConst.GetMemory(MaxFrame);
		$m.pFrameAttr = (SDFrame*)$m.ShaderConst.GetMemory(0);

		$m.hSceneSrv = $m.ShaderConst.GetVram(MaxFrame);
		$m.hFrameSrv = $m.ShaderConst.GetVram(0);

		$m.pCmdList = pCmdList;
	}
	_open $VOID Release(){
		$m.ShaderConst.Release();
	}
	///上传区设置
	_open $VOID SetFrame(UNS32 Frame){
		$m.hFrameSrv = $m.ShaderConst.GetVram(Frame);
		$m.pFrameAttr = (SDFrame*)$m.ShaderConst.GetMemory(Frame);
	}
	_open $VOID SetTimeSpan(SPFPN TimeSpan){
		$m.pFrameAttr->TimeSpan = TimeSpan;
	}
	_open $VOID SetCamera(_in MXCamera &Camera){
		Camera.GetPosition($m.pFrameAttr->EyeCoord);
		Camera.GetViewProjMat($m.pFrameAttr->matViewProj);
	}
	_open $VOID SetAmbientLight(_in VFloat3 &Light){
		$m.pSceneAttr->AmbLight[0] = Light.x;
		$m.pSceneAttr->AmbLight[1] = Light.y;
		$m.pSceneAttr->AmbLight[2] = Light.z;
	}
	_open $VOID SetKdMap(_in SPFPN prgKey[8], _in SPFPN prgValue[8]){
		for(UNS32 iElem = 0; iElem < 8; ++iElem){
			$m.pSceneAttr->rgKdInput[iElem] = prgKey[iElem];
			$m.pSceneAttr->rgKdOutput[iElem] = prgValue[iElem];
		}
	}
	_open $VOID SetKsMap(_in SPFPN prgKey[8], _in SPFPN prgValue[8]){
		for(UNS32 iElem = 0; iElem < 8; ++iElem){
			$m.pSceneAttr->rgKsInput[iElem] = prgKey[iElem];
			$m.pSceneAttr->rgKsOutput[iElem] = prgValue[iElem];
		}
	}
	_open $VOID SetFog(_in VFloat3 &Color, SPFPN Start, SPFPN Depth){
		$m.pSceneAttr->FogStart = Start;
		$m.pSceneAttr->FogDepth = Depth;
		$m.pSceneAttr->FogColor[0] = Color.x;
		$m.pSceneAttr->FogColor[1] = Color.y;
		$m.pSceneAttr->FogColor[2] = Color.z;
	}
	///根参数设置
	_open $VOID SetBones(D3D12_GPU_VIRTUAL_ADDRESS hBones){
		$m.pCmdList->SetGraphicsRootShaderResourceView(BONES_PORT, hBones);
	}
	_open $VOID SetLights(D3D12_GPU_VIRTUAL_ADDRESS hLights, UNS32 Count){
		$m.pSceneAttr->LightCnt = Count;
		$m.pCmdList->SetGraphicsRootShaderResourceView(LIGHTS_PORT, hLights);
	}
	_open $VOID SetSplineAttrs(_in SDSpline *pAttrs){
		SDSpline Attrs = *pAttrs;

		if(Attrs.Height == 0.f) Attrs.TexSplit = 1.f;
		else if(Attrs.bShell) Attrs.TexSplit = 0.5f;
		else Attrs.TexSplit = 1.f / 3.f;

		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDSpline), &Attrs, 0);
	}
	_open $VOID SetSurfaceAttrs(_in SDSurface *pAttrs){
		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDSurface), pAttrs, 0);
	}
	_open $VOID SetTerrainAttrs(_in SDTerrain *pAttrs){
		SDTerrain Attrs = *pAttrs;
		Attrs.CellWidth = Attrs.AreaWidth / Attrs.ColCount;
		Attrs.CellDepth = Attrs.AreaDepth / Attrs.RowCount;

		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDTerrain), &Attrs, 0);
	}
	_open $VOID SetModelAttrs(_in MXMATRIX &matWorld, _in MXMATRIX *pTexMat = P_Null, UNS32 Ident = 0){
		SDObject ObjAttrs;
		MXMATRIX *pWorldMat = &ObjAttrs.matWorld;
		MXMATRIX *pTexXform = &ObjAttrs.matTexOrNorm;
		MXMATRIX *pNormWorld = &ObjAttrs.matTexOrNorm;

		if(pTexMat){
			*pWorldMat = dx::XMMatrixTranspose(matWorld);
			*pTexXform = dx::XMMatrixTranspose(*pTexMat);
		} else{
			*pWorldMat = dx::XMMatrixTranspose(matWorld);
			*pNormWorld = dx::XMMatrixInverse(P_Null, matWorld);
			pNormWorld->r[3] = dx::g_XMZero;
		}

		if(Ident != 0)
			pNormWorld->r[3] = dx::XMVectorSetZ(pNormWorld->r[3], AS_SPFPN(Ident));

		$m.pCmdList->SetGraphicsRoot32BitConstants(
			OBJECT_PORT, DW_CNT(SDObject), &ObjAttrs, 0);
	}
	_open $VOID SetResourceViewHeap(ID3D12DescriptorHeap *pViewHeap){
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
		$m.pCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Window.lpRTBuffer,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
	_open $VOID BeginDraw(GEWnd &Window){
		UNS32 TargetCnt = Window.hIBView.ptr ? 2 : 1;
		CD3DX12_RESOURCE_BARRIER ResBarrier = CD3DX12_RESOURCE_BARRIER::Transition(Window.lpRTBuffer,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		$m.pCmdList->ResourceBarrier(1, &ResBarrier);
		$m.pCmdList->OMSetRenderTargets(TargetCnt, &Window.hCBView, B_True, &Window.hDSView);
		$m.pCmdList->RSSetScissorRects(1, &Window.rcScissor);
		$m.pCmdList->RSSetViewports(1, &Window.Viewport);
	}
	_open $VOID ClearWindow(GEWnd &Window, _in FColor4 &Color){
		$m.pCmdList->ClearRenderTargetView(Window.hCBView, Color, 0, P_Null);

		$m.pCmdList->ClearDepthStencilView(Window.hDSView,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, P_Null);

		if(Window.hIBView.ptr)
			$m.pCmdList->ClearRenderTargetView(Window.hIBView, FColor4(0, 0, 0, 0), 0, P_Null);
	}
	_open $VOID DrawModel(CMesh3D &Mesh, CMtlRepos *pMtlRepos = P_Null){
		$m.pCmdList->IASetIndexBuffer(&Mesh.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexsSrv());

		if(Mesh.numSubset == 0){
			$m.pCmdList->DrawIndexedInstanced(Mesh.GetIndexCount(), 1, 0, 0, 0);
		} else{
			for(UNS32 iElem = 0; iElem < Mesh.numSubset; ++iElem){
				GSubMesh *pSubMesh = &Mesh.lprgSubset[iElem];
				NMtlView *pMtlView = pMtlRepos->GetMaterialById(pSubMesh->MtlIdent);

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
	_open $VOID DrawModel(CMesh3D &Mesh, CMtlRepos *pMtlRepos, D3D12_GPU_VIRTUAL_ADDRESS hCurMtrl){
		$m.pCmdList->IASetIndexBuffer(&Mesh.GetIndicesSrv());
		$m.pCmdList->IASetVertexBuffers(0, 1, &Mesh.GetVertexsSrv());

		for(UNS32 iElem = 0; iElem < Mesh.numSubset; ++iElem){
			GSubMesh *pSubMesh = &Mesh.lprgSubset[iElem];
			NMtlView *pMtlView = pMtlRepos->GetMaterialById(pSubMesh->MtlIdent);

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
	_open $VOID DrawModels(CMesh3D *pFirst, CMtlRepos *pMtlRepos, D3D12_GPU_VIRTUAL_ADDRESS hCurMtrl){
		for(auto pMesh = pFirst; pMesh; pMesh = pMesh->pNext)
			$m.DrawModel(*pMesh, pMtlRepos, hCurMtrl);
	}
	_open $VOID DrawSpline(CVtxBuff &Spline, D3D12_GPU_DESCRIPTOR_HANDLE hTexView){
		$m.pCmdList->SetGraphicsRootDescriptorTable(3, hTexView);
		$m.pCmdList->IASetVertexBuffers(0, 1, &Spline.GetSrv());
		$m.pCmdList->DrawInstanced(Spline.GetCount(), Spline.GetCount() / 4, 0, 0);
	}
	_open $VOID DrawTerrain(CHeightField &Terrain, D3D12_GPU_DESCRIPTOR_HANDLE hTexViews, D3D12_GPU_VIRTUAL_ADDRESS hSlideProjs){
		$m.pCmdList->SetGraphicsRootDescriptorTable(3, hTexViews);
		$m.pCmdList->SetGraphicsRootShaderResourceView(5, Terrain.GetFeaturesVram());
		$m.pCmdList->SetGraphicsRootShaderResourceView(6, Terrain.GetVertexsVram());
		$m.pCmdList->SetGraphicsRootShaderResourceView(7, hSlideProjs);
		$m.pCmdList->DrawInstanced(Terrain.GetCellCount(), 1, 0, 0);
	}
	_open $VOID DrawSurface(_in D3D12_VERTEX_BUFFER_VIEW &VboView, D3D12_GPU_DESCRIPTOR_HANDLE hTexViews, D3D12_GPU_VIRTUAL_ADDRESS hMtlBase){
		$m.pCmdList->SetGraphicsRootConstantBufferView(3, hMtlBase);
		//$m.pCmdList->SetGraphicsRootDescriptorTable(5, hTexViews);
		$m.pCmdList->IASetVertexBuffers(0, 1, &VboView);
		$m.pCmdList->DrawInstanced(VboView.SizeInBytes / VboView.StrideInBytes, 1, 0, 0);
	}
	//_open $VOID DrawParticles(CPtcSystem *pPtcSystem){
	//	UNS32 TexIndex;
	//	UNS32 IdxOffset = 0;
	////
	//	$m.pCmdList->IASetVertexBuffers(0, 1, &PtcSystem.GetVertexBufferView());
	//
	//	for(UNS32 iExe = 0; iExe < PtcSystem.numSubset; ++iExe){
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
	//		$m.hVMObject += sizeof(SDObject);
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
	//		SkyDome.cbIdxBuff / sizeof(UNS16), 1, 0, 0, 0);*/
	//}
};

//----------------------------------------//