class CMGEditor: public IAppBase{
#define EYE_POS_Y 20.f
#define EYE_SLOPE 60.f
//////
#define TILE_SIZE  8.f
#define CELL_SIZEH 0.5f
#define CELL_SIZEV 1.5f
#define CELL_SPLIT 2
//////
#define MAX_BRUSH   32
#define MAX_PATH_PT 128
//////
#define MIN_UNIT_ID   0x8
#define MIN_STRUCT_ID 0x80000000

	_secr using NLandVert = CHeightField::NVertex;
	//////
	_open enum DIRTAG2{
		DIR_LeftUp = (1L << 6),
		DIR_RightUp = (1L << 7),
		DIR_LeftDown = (1L << 8),
		DIR_RightDown = (1L << 9),
	};
	_open enum FUNCMODE{
		MODE_None,
		FIRST_TERR_MODE,
		MODE_Uplift = FIRST_TERR_MODE,
		MODE_Wiring,
		MODE_Steepen,
		MODE_Uniform,
		MODE_Sharpen,
		MODE_MakeAbyss,
		MODE_MakeCliff,
		MODE_MakeWater,
		MODE_MakeClosed,
		MODE_EditColor,
		MODE_EditPattern,
		MODE_EditMetal,
		MODE_EditRough,
		LAST_TERR_MODE = MODE_EditRough,
		//////
		FIRST_DECOR_MODE,
		MODE_MakeRoad = FIRST_DECOR_MODE,
		MODE_MakeSlope,
		MODE_MakeBridge,
		MODE_MakeDecor,
		LAST_DECOR_MODE = MODE_MakeDecor,
		//////
		FIRST_ACTOR_MODE,
		MODE_AddUnit = FIRST_ACTOR_MODE,
		MODE_AddStruct,
		MODE_AdjustActor,
		LAST_ACTOR_MODE = MODE_AdjustActor,
		//////
		MODE_COUNT
	};
	_open enum BRUSHTYPE{
		BR_TYPE_Rect,
		BR_TYPE_Circle,
		BR_TYPE_Rhombus,
	};
	//////
	_secr struct NMarquee{
		union{
			URect RectU;
			struct{
				UNS32 L;
				UNS32 T;
				UNS32 R;
				UNS32 B;
			};
		};
		FRect Rect;
		VFloat2 Cen;
		SPFPN Rad;
		SPFPN RadSq;
		IBOOL bRhom : 1;
		IBOOL bRect : 1;
		IBOOL bCir : 1;
	};
	_secr struct NPreView{
		SIZE Scale;
		UNS32 PsoId;
		UNS32 MdlId;
		VFloat3 Coord;
		CMeshAssy *pModel;
	};
	_secr struct NWavView{
		SPFPN Level;
		UNS32 ClassId;
		UNS32 NumCell;
		UNS32 CellBase;

		NWavView(){}
		NWavView(SPFPN Level, UNS32 ClassId, UNS32 NumCell, UNS32 CellBase){
			$m.Level = Level;
			$m.ClassId = ClassId;
			$m.NumCell = NumCell;
			$m.CellBase = CellBase;
		}
	};
	_secr struct NWavClass{
		SPFPN rgWavDirX[4];
		SPFPN rgWavDirZ[4];
		SPFPN rgWavPhase[4];
		SPFPN rgWavSlope[4];
		SPFPN rgWavAmplit[4];
		SPFPN rgWavLength[4];
	};
	_secr struct NCellInfo{
		UNS32 iHolder = 0;
		UNS32 iWavView = 0;
	};
	_secr struct NFaceFeat{
		INT16 Extrusion;
		WORDT Padding: 12;
		WORDT bDecline: 1;
		WORDT bDiscard: 1;
		WORDT bSmooth: 1;
		WORDT bBslash: 1;
	};

	_secr DWORD vHeader;
	///编辑状态
	_open BYTET FuncMode;
	_secr BYTET LayupDir;
	_secr BOOL8 bFirstOp: 4;
	_secr BOOL8 bPicking: 4;
	_secr BOOL8 bConflict: 2;
	_secr BOOL8 bHideGrid: 2;
	_secr BOOL8 bPrepIdImg: 2;
	_secr BOOL8 bPickObjId: 2;
	///场景信息
	_secr SPFPN AreaWidth;
	_secr SPFPN AreaDepth;
	_secr UNS16 NumMapCol;
	_secr UNS16 NumMapRow;
	_secr UNS16 NumMeridian; //经线
	_secr UNS16 NumParallel; //纬线
	///视域信息
	_secr SPFPN TanAlpha;
	_secr SPFPN TanGamma;
	_secr SPFPN TanTheta;
	_secr SPFPN FovWidth;
	_secr SPFPN FovDepth;
	_secr SPFPN EyeScopeX0;
	_secr SPFPN EyeScopeX1;
	_secr SPFPN EyeScopeY0;
	_secr SPFPN EyeScopeY1;
	_secr SPFPN EyeScopeZ0;
	_secr SPFPN EyeScopeZ1;
	///笔刷属性
	_secr WORDT BrushShape;
	_open UNS16 BrushMapId;
	_open ARGB8 BrushColor;
	_secr SPFPN BrushPower;
	_secr SPFPN BrushRad1;
	_secr SPFPN BrushRad0;
	///水域信息
	_secr SPFPN WavLevel;
	_secr UNS16 iWavView;
	_secr UNS16 iWavClass;
	_secr NWavClass rgWavClass[8];
	///选区记录
	_secr D3D12_BOX TexFrame;
	_secr D2D1_RECT_F MapFrame;
	_secr D2D1_RECT_U CpyFrame;
	_secr D2D1_RECT_U MenuFrame;
	///杂项
	_secr VFloat3 SelPoint;
	_secr NPreView Preview;
	_secr NCellInfo *lprgCellInfo;
	//资源句柄
	_secr PVColor *psCanvPx;
	_secr MFlt4x4 *prgMarkProj;
	_secr GMtlLite *prgWavMatl;
	_secr GMtlBase *prgToneMatl;
	_secr D3D12_GPU_VIRTUAL_ADDRESS hWavMatls;
	_secr D3D12_GPU_VIRTUAL_ADDRESS hToneMatls;
	_secr D3D12_GPU_VIRTUAL_ADDRESS hMarkProjs;
	///摄影机
	_secr MXCamera Camera;
	_secr MXCamera Prjctr;
	///路径&水域
	_secr TSeqList<VFloat3> Paths;
	_secr TSeqList<NWavView> WavViews;
	///渲染管线
	_secr CGfxPso WavPso;
	_secr CGfxPso LandPso;
	_secr CGfxPso RoadPso;
	_secr CGfxPso WirePso0;
	_secr CGfxPso WirePso1;
	_secr CShader WavShader;
	_secr CShader LandShader;
	_secr CShader RoadShader;
	///地形模型
	_secr CVtxBuff Roads;
	_secr CVtxBuff Water;
	_secr CUploadBuff ResMisc;
	_secr CHeightField Ground;
	///地形纹理
	_secr ID3D12Resource *rgMapTex[5];
	_secr ID3D12Resource *lpMapCanvas;
	_secr ID3D12DescriptorHeap *lpTexViews;
	//////
	_secr DWORD vFooter;

	_open ~CMGEditor(){
		$m.Finalize();
	}
	_open CMGEditor(){
		$m.Initialize();
	}
	//////
	_secr $VOID Initialize(){
		//归零
		{
			Mem_Zero(&$m.vHeader, (BYTET*)&$m.vFooter - (BYTET*)&$m.vHeader);
		}

		//视域信息
		{
			SPFPN Alpha = EYE_SLOPE - (gMyFovAngY * 0.5f);
			SPFPN Gamma = 180.f - gMyFovAngY - Alpha;
			SPFPN Theta = 90.f - (gMyFovAngX * 0.5f);

			$m.TanAlpha = tan(TO_RAD(Alpha));
			$m.TanGamma = tan(TO_RAD(Gamma));
			$m.TanTheta = tan(TO_RAD(Theta)) / $m.TanAlpha;
		}

		//地形模型
		{
			$m.Roads.Initialize(GE3d::lpD3dDevice, MAX_PATH_PT, sizeof(VFloat3));
			$m.CreateTerrain(128, 128, 0xFFE4A866, 0x800000);
		}

		//地形着色器
		{
			$m.RoadShader.Initialize(GE3d::lpD3dDevice, VFB_Coord, ASSET_TYPE_Spline);
			$m.LandShader.Initialize(GE3d::lpD3dDevice, 0x0, ASSET_TYPE_Terrain | TEX_TYPE_Albedo | TEX_TYPE_Normal);
			$m.WavShader.Initialize(GE3d::lpD3dDevice, VFB_RowCol, ASSET_TYPE_Surface);

			$m.WavPso.Initialize(GE3d::lpD3dDevice, $m.WavShader, gMyWndEx);
			$m.LandPso.Initialize(GE3d::lpD3dDevice, $m.LandShader, gMyWndEx);

			$m.RoadPso.Initialize(GE3d::lpD3dDevice, $m.RoadShader, gMyWndEx,
				P_Null, P_Null, &DXTKCommStates::DepthDefault);
			$m.WirePso0.Initialize(GE3d::lpD3dDevice, $m.LandShader, gMyWndEx,
				P_Null, &DXTKCommStates::Wireframe, &DXTKCommStates::DepthDefault);
			$m.WirePso1.Initialize(GE3d::lpD3dDevice, $m.WavShader, gMyWndEx,
				P_Null, &DXTKCommStates::Wireframe, &DXTKCommStates::DepthNone);
		}

		//资源杂项
		{
			const UNS32 NumToneMatl = 4;
			const UNS32 NumWavMatl = 8;

			UNS32 cbToneMatls = sizeof(GMtlBase) * NumToneMatl;
			UNS32 cbWavMatls = sizeof(GMtlLite) * NumWavMatl;
			UNS32 cbMarkProjs = sizeof(MFlt4x4) * gMyGfxCmd.MaxFrame;
			UNS32 cbBuffer = cbToneMatls + cbWavMatls + cbMarkProjs;

			$m.ResMisc.Initialize(GE3d::lpD3dDevice, cbBuffer, 1, B_True);
			$m.ResMisc.EnableUpdate(B_True, B_True);

			$m.hToneMatls = $m.ResMisc.GetVram(0);
			$m.hWavMatls = $m.hToneMatls + cbToneMatls;
			$m.hMarkProjs = $m.hWavMatls + cbWavMatls;

			$m.prgToneMatl = (GMtlBase*)$m.ResMisc.GetMemory(0);
			$m.prgWavMatl = (GMtlLite*)($m.prgToneMatl + NumToneMatl);
			$m.prgMarkProj = (MFlt4x4*)($m.prgWavMatl + NumWavMatl);

			$m.prgToneMatl[0].Opacity = 1.f;
			$m.prgToneMatl[1].Opacity = 1.f;
			$m.prgToneMatl[2].Opacity = 1.f;
			$m.prgToneMatl[3].Opacity = 1.f;

			$m.prgToneMatl[0].TexAttrs[0] = TEX_TYPE_Albedo;
			$m.prgToneMatl[1].TexAttrs[0] = TEX_TYPE_Albedo;
			$m.prgToneMatl[2].TexAttrs[0] = TEX_TYPE_Albedo;
			$m.prgToneMatl[3].TexAttrs[0] = TEX_TYPE_Albedo;

			$m.prgToneMatl[0].Emission = VFloat3(0.4f, 0.f, 0.f);   //加红
			$m.prgToneMatl[1].Emission = VFloat3(0.f, 0.4f, 0.f);   //加绿
			$m.prgToneMatl[3].Emission = VFloat3(0.4f, 0.4f, 0.4f); //加白

			$m.prgToneMatl[0].Albedo = VFloat3(1.f, 0.5f, 0.5f);  //加红
			$m.prgToneMatl[1].Albedo = VFloat3(0.5f, 1.f, 0.5f);  //加绿
			$m.prgToneMatl[2].Albedo = VFloat3(0.5f, 0.5f, 0.5f); //加黑
			$m.prgToneMatl[3].Albedo = VFloat3(1.f, 1.f, 1.f);    //加白

			$m.prgWavMatl[0].Albedo = VFloat4(0.40f, 0.60f, 0.80f, 1.f);
			$m.prgWavMatl[1].Albedo = VFloat4(0.32f, 0.62f, 0.52f, 1.f);
			$m.prgWavMatl[2].Albedo = VFloat4(0.43f, 0.83f, 0.83f, 1.f);
			$m.prgWavMatl[3].Albedo = VFloat4(0.44f, 0.64f, 0.24f, 1.f);
			$m.prgWavMatl[4].Albedo = VFloat4(0.45f, 0.35f, 0.75f, 1.f);
			$m.prgWavMatl[5].Albedo = VFloat4(0.16f, 0.66f, 0.56f, 1.f);
			$m.prgWavMatl[6].Albedo = VFloat4(0.47f, 0.17f, 0.87f, 1.f);
			$m.prgWavMatl[7].Albedo = VFloat4(0.88f, 0.68f, 0.88f, 1.f);
		}

		//水波参数
		{
			FVector2 WavDir;
			NWavClass *prgClass = $m.rgWavClass;

			for(UNS32 iExe = 0; iExe < 4; ++iExe){
				WavDir.x = RandReal(-1.f, 1.f);
				WavDir.y = RandReal(-1.f, 1.f);
				WavDir.Normalize();

				prgClass[0].rgWavDirX[iExe] = WavDir.x;
				prgClass[0].rgWavDirZ[iExe] = WavDir.y;
				prgClass[0].rgWavPhase[iExe] = RandReal(0.f, 1.f) * SIMD_PI * 2.f;
				prgClass[0].rgWavSlope[iExe] = RandReal(0.1f, 0.4f);
				prgClass[0].rgWavAmplit[iExe] = RandReal(0.05f, 0.2f);
				prgClass[0].rgWavLength[iExe] = RandReal(4.0f, 8.f);
			}

			prgClass[1] = prgClass[0];
			prgClass[2] = prgClass[0];
			prgClass[3] = prgClass[0];
			prgClass[4] = prgClass[0];
			prgClass[5] = prgClass[0];
			prgClass[6] = prgClass[0];
			prgClass[7] = prgClass[0];
		}

		//笔刷
		{
			CWidget *pGuiCtrl = mgGetWidget(ID_LBL_BrushSize);
			$m.SetBrushSize(_wtoi(pGuiCtrl->GetValue()));

			pGuiCtrl = mgGetWidget(ID_LBL_BrushPower);
			$m.SetBrushPower((SPFPN)_wtof(pGuiCtrl->GetValue()));

			pGuiCtrl = mgGetWidget(ID_RDO_RectBrush);
			$m.SetBrushShape(pGuiCtrl->IsChecked() ? BR_TYPE_Rect : BR_TYPE_Circle);

			pGuiCtrl = mgGetWidget(ID_GRP_SculptScape);
			$m.SetFunctionMode(((CBtnGroup*)pGuiCtrl)->GetChoice());

			pGuiCtrl = mgGetWidget(ID_LST_BrushColor);
			$m.SetBrushColor(((CListBox*)pGuiCtrl)->GetItem(1)->Color);
		}

		//等待GPU

		gMyGfxCmd.WaitForGpu();
		gMyWndEx.rcScissor.left = (UNS32)gMyPanelW;
	}
	_secr $VOID Finalize(){
		$m.CleanTexture();
		SAFE_DELETEA($m.lprgCellInfo);
	}
	///每帧绘制
	_open $VOID Paint(){
		gMyGfxCmd.BeginFrame();
		$m.SyncTexture();
		gMyRender.SetFrame(gMyGfxCmd.CurFrame);
		gMyRender.SetCamera($m.Camera);
		gMyRender.BeginDraw(gMyWndEx);
		gMyRender.ClearWindow(gMyWndEx, FColor4(0.f, 0.f, 0.f, 1.0));
		$m.PaintGround();
		$m.PaintRoads();
		$m.PaintWater();
		$m.PaintObjects();
		gMyRender.EndDraw(gMyWndEx);
		$m.SyncIdBuffer();
		gMyGfxCmd.EndFrame();

		GE2d::BeginDraw(gMyWndEx.pD3d11Buff, gMyWndEx.pD2dTarget);
		$m.PaintPaths();
		gMyGuiRoot->OnMessage(WM_PAINT, 0, 0);
		GEGui::BeginDraw();
		$m.PaintWaterPanel();
		GEGui::EndDraw(gMyWndEx.pD3d11Rtv);
		GE2d::EndDraw(gMyWndEx.pD3d11Buff);

		gMyWndEx.PresentBuffer();
	}
	_secr $VOID PaintPaths(){
		if($m.Paths.size() == 0) return;

		GE2d::lpPureBrush->SetColor(D2DXColor(0.f, 1.f, 0.f));

		MXMATRIX matViewProj = $m.Camera.GetViewProjMat();
		MXMATRIX matViwprt = dx::XMMatrixFromViewport(*(FViewport*)&gMyWndEx.Viewport);
		MXMATRIX matToScrn = matViewProj * matViwprt;

		FVector3 *pPoint = (FVector3*)$m.Paths.data();
		FVector3 *pEndPoint = pPoint + $m.Paths.size();
		VFloat3 Point0(FLT_NAN, 0.f, 0.f);

		for(; pPoint != pEndPoint; ++pPoint){
			if(isnan(pPoint->x)){
				Point0 = *pPoint;
				continue;
			}

			FVector3 Point1 = dx::XMVector3TransformCoord(Ld_V3(pPoint), matToScrn);
			FRect rcPoint = MAKE_SQUARE(Point1, 5.f, 5.f);
			GE2d::lpD2dDevCtx->FillRectangle(rcPoint, GE2d::lpPureBrush);

			if(!isnan(Point0.x)){
				D2D_POINT_2F P0 = { Point0.x, Point0.y };
				D2D_POINT_2F P1 = { Point1.x, Point1.y };
				GE2d::lpD2dDevCtx->DrawLine(P0, P1, GE2d::lpPureBrush, 2);
			}

			Point0 = Point1;
		}
	}
	_secr $VOID PaintRoads(){
		if($m.Roads.GetCount() > 0){
			SDSpline RoadDesc = {};
			RoadDesc.Pitch = 2.f;
			RoadDesc.Width = 2.f;
			RoadDesc.WidthB = 0.3f;
			RoadDesc.Height = 0.3f;
			RoadDesc.bShell = B_True;
			RoadDesc.UpDir = FVector3::Up;

			gMyRender.SetPipelineState($m.RoadPso);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
			gMyRender.SetSplineAttrs(&RoadDesc);
			gMyRender.DrawSpline($m.Roads, gMyMtlLib.GetTextureSrvById(StrHash32("Highway")));
		}
	}
	_secr $VOID PaintWater(){
		if($m.Water.GetCount() == 0) return;

		SDSurface Param = {};
		NWavClass *pClass;
		D3D12_VERTEX_BUFFER_VIEW VtxView;
		D3D12_GPU_DESCRIPTOR_HANDLE hTexViews = {};
		D3D12_GPU_VIRTUAL_ADDRESS hMtlBase;

		NWavView *pView = $m.WavViews.data() + 1;
		NWavView *pEndView = pView + $m.WavViews.size() - 1;

		gMyRender.SetPipelineState($m.WavPso);
		gMyRender.SetResourceViewHeap($m.lpTexViews);
		gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

		Param.CellWidth = CELL_SIZEH;
		Param.CellDepth = CELL_SIZEH;

		VtxView = $m.Water.GetSrv();

		for(; pView != pEndView; ++pView){
			if(pView->NumCell == 0) continue;

			pClass = $m.rgWavClass + pView->ClassId;
			hMtlBase = $m.GetWaterMaterial(pView->ClassId);

			Param.LowerLeftY = pView->Level;
			VtxView.SizeInBytes = pView->NumCell * sizeof(VUint2);

			Mem_Move(&Param, pClass, sizeof(NWavClass));

			gMyRender.SetSurfaceAttrs(&Param);
			gMyRender.DrawSurface(VtxView, hTexViews, hMtlBase);

			VtxView.BufferLocation += VtxView.SizeInBytes;
		}

		if(!$m.bHideGrid && ($m.FuncMode == MODE_MakeWater)){
			pView = $m.WavViews.data() + $m.iWavView;
			if(pView->NumCell == 0) return;

			VtxView = $m.Water.GetSrv();
			VtxView.SizeInBytes = pView->NumCell * sizeof(VUint2);
			VtxView.BufferLocation += pView->CellBase * sizeof(VUint2);

			Param.WireColor = 0xFF00FF00;
			Param.LowerLeftY = pView->Level;

			pClass = $m.rgWavClass + pView->ClassId;
			Mem_Move(&Param, pClass, sizeof(NWavClass));

			gMyRender.SetPipelineState($m.WirePso1);
			gMyRender.SetResourceViewHeap($m.lpTexViews);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
			gMyRender.SetSurfaceAttrs(&Param);
			gMyRender.DrawSurface(VtxView, hTexViews, 0);
		}
	}
	_secr $VOID PaintGround(){
		D3D12_GPU_VIRTUAL_ADDRESS hMarkProj = $m.GetLandmarkProjection(gMyGfxCmd.CurFrame);
		D3D12_GPU_DESCRIPTOR_HANDLE hTexView = $m.lpTexViews->GetGPUDescriptorHandleForHeapStart();

		IBOOL bTerrEdit = $m.InLandEditMode();
		UNS08 NumSlide = ($m.bPicking && bTerrEdit) ? 1 : 0;

		SDTerrain LandDesc = {};
		PVUbyte4(0, NumSlide, 2, 0);
		LandDesc.ResAttr = PVUbyte4(0, NumSlide, 2, 0).v;
		LandDesc.SlideIds0 = PVUbyte4((UNS08)$m.BrushShape, 0, 0, 0).v;
		LandDesc.LayerSize = CELL_SIZEV;
		LandDesc.LowerLeftX = $m.Ground.LowerLeftX;
		LandDesc.LowerLeftZ = $m.Ground.LowerLeftZ;
		LandDesc.RowCount = $m.Ground.RowCount;
		LandDesc.ColCount = $m.Ground.ColCount;
		LandDesc.AreaWidth = $m.AreaWidth;
		LandDesc.AreaDepth = $m.AreaDepth;
		LandDesc.TileWidth = TILE_SIZE;
		LandDesc.TileDepth = TILE_SIZE;

		gMyRender.SetPipelineState($m.LandPso);
		gMyRender.SetResourceViewHeap($m.lpTexViews);
		gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
		gMyRender.SetTerrainAttrs(&LandDesc);
		gMyRender.DrawTerrain($m.Ground, hTexView, hMarkProj);

		if(!$m.bHideGrid){
			LandDesc.CurColor = 0xFFFFFF;
			gMyRender.SetPipelineState($m.WirePso0);
			gMyRender.SetResourceViewHeap($m.lpTexViews);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
			gMyRender.SetTerrainAttrs(&LandDesc);
			gMyRender.DrawTerrain($m.Ground, hTexView, hMarkProj);
		}
	}
	_secr $VOID PaintObjects(){
		UIPTR NumAnimal = gMyAnimals.size();
		UIPTR NumStruct = gMyStructs.size();

		CMGAnimal *prgAnimal = gMyAnimals.data();
		CMGStructure *prgStruct = gMyStructs.data();

		if(NumAnimal != 0){
			gMyRender.SetPipelineState(gMyPsoArr[ID_PSO_Animal]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			for(UNS32 iObj = 0; iObj < NumAnimal; ++iObj){
				CMGAnimal *pObject = prgAnimal + iObj;

				UNS32 iModel = gMyAnimalClsArr[pObject->iOrgProps].iModel;
				CMeshAssy *pModel = mgGetModel(0, iModel);

				MXVECTOR Coord = Ld_V3(&pObject->Position);
				MXMATRIX matWorld = dx::XMMatrixTranslationFromVector(Coord);

				gMyRender.SetModelAttrs(matWorld, P_Null, MIN_UNIT_ID + iObj);
				gMyRender.DrawModels(pModel->lpFirst, &gMyMtlLib);
			}
		}

		if(NumStruct != 0){
			gMyRender.SetPipelineState(gMyPsoArr[ID_PSO_Struct]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			for(UNS32 iObj = 0; iObj < NumStruct; ++iObj){
				CMGStructure *pObject = prgStruct + iObj;

				UNS32 iModel = gMyStructClsArr[pObject->iOrgProps].iModel;
				CMeshAssy *pModel = mgGetModel(0, iModel);

				MXVECTOR Coord = Ld_V3(&pObject->Position);
				MXMATRIX matWorld = dx::XMMatrixTranslationFromVector(Coord);

				gMyRender.SetModelAttrs(matWorld, P_Null, MIN_STRUCT_ID + iObj);
				gMyRender.DrawModels(pModel->lpFirst, &gMyMtlLib);
			}
		}

		if($m.bPicking && $m.Preview.pModel){
			gMyRender.SetPipelineState(gMyPsoArr[$m.Preview.PsoId]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			MXVECTOR Coord = Ld_V3(&$m.Preview.Coord);
			MXMATRIX matWorld = dx::XMMatrixTranslationFromVector(Coord);
			CMesh3D *pModel = $m.Preview.pModel->lpFirst;
			D3D12_GPU_VIRTUAL_ADDRESS hMatl = $m.GetToneMaterial($m.bConflict ? 0 : 1);

			gMyRender.SetModelAttrs(matWorld);
			gMyRender.DrawModels(pModel, &gMyMtlLib, hMatl);
		}
	}
	_secr $VOID PaintWaterPanel(){
		if($m.FuncMode != MODE_MakeWater) return;

		IBOOL bPressed;
		ImVec2 BtnSizeL(34.f, 34.f);
		ImVec2 BtnSizeS(30.f, 22.f);
		ImVec2 WndSize(164.f, 206.f);
		ImVec2 WndPos(gMyWndW - WndSize.x, 0.f);
		GMtlLite *prgMatl = $m.prgWavMatl;
		NWavView *pWavView = &$m.WavViews[$m.iWavView];

		imgui::SetNextWindowPos(WndPos);
		imgui::SetNextWindowSize(WndSize);
		imgui::Begin(u8"Water");

		imgui::Text(u8"Height:");
		imgui::InputFloat("", &pWavView->Level, 0.1f, 0.1f, "%.1f");

		imgui::Text(u8"Style:");
		bPressed = imgui::ColorButton("Style0", *(ImVec4*)&prgMatl[0].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 0;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style1", *(ImVec4*)&prgMatl[1].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 1;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style2", *(ImVec4*)&prgMatl[2].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 2;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style3", *(ImVec4*)&prgMatl[3].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 3;
		bPressed = imgui::ColorButton("Style4", *(ImVec4*)&prgMatl[4].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 4;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style5", *(ImVec4*)&prgMatl[5].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 5;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style6", *(ImVec4*)&prgMatl[6].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 6;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style7", *(ImVec4*)&prgMatl[7].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 7;

		imgui::Spacing();
		imgui::Separator();
		imgui::Spacing();
		imgui::Indent((WndSize.x - BtnSizeS.x - 8.f) * 0.5f);
		bPressed = imgui::Button(u8"OK", BtnSizeS);

		WndPos = imgui::GetWindowPos();
		WndSize = imgui::GetWindowSize();

		imgui::End();

		$m.MenuFrame.left = (UNS32)WndPos.x;
		$m.MenuFrame.top = (UNS32)WndPos.y;
		$m.MenuFrame.right = UNS32(WndPos.x + WndSize.x);
		$m.MenuFrame.bottom = UNS32(WndPos.y + WndSize.y);

		$m.WavLevel = pWavView->Level;
		$m.iWavClass = pWavView->ClassId;

		if(bPressed) $m.NewWaterView();
	}
	///每帧更新
	_open $VOID Update(){
		$m.UpdateView();
		$m.UpdatePicking();

		if($m.bPickObjId)
			$m.UpdateBodyPicking();
		if($m.Preview.pModel)
			$m.UpdatePreview();
		if($m.Water.GetCount() > 0)
			$m.UpdateWave();
		if($m.InLandEditMode()){
			$m.UpdateBrush();
			$m.UpdateLandPicking();
		}
	}
	_secr $VOID UpdateView(){
		IBOOL bUpdated = B_False;
		VFloat3 *pEyePos = (VFloat3*)&$m.Camera.Position;

		if(!gKeysTest[DIK_LALT] && !gKeysTest[DIK_LCONTROL]){
			if(GEInp::MouseState.lZ > 0){
				if(pEyePos->y > $m.EyeScopeY0){
					$m.Camera.Walk(5.f);
					$m.ResetFov();
					$m.RestrictView();
					bUpdated = B_True;
				}
			} else if(GEInp::MouseState.lZ < 0){
				if(pEyePos->y < $m.EyeScopeY1){
					$m.Camera.Walk(-5.f);
					$m.ResetFov();
					$m.RestrictView();
					bUpdated = B_True;
				}
			}
		}

		if(gKeysTest[DIK_A] && (pEyePos->x > $m.EyeScopeX0)){
			$m.Camera.Strafe(-0.1f);
			bUpdated = B_True;
		} else if(gKeysTest[DIK_D] && (pEyePos->x < $m.EyeScopeX1)){
			$m.Camera.Strafe(0.1f);
			bUpdated = B_True;
		}
		if(gKeysTest[DIK_W] && (pEyePos->z < $m.EyeScopeZ1)){
			$m.Camera.HorizontalWalk(0.1f);
			bUpdated = B_True;
		} else if(gKeysTest[DIK_S] && (pEyePos->z > $m.EyeScopeZ0)){
			$m.Camera.HorizontalWalk(-0.1f);
			bUpdated = B_True;
		}

		if(bUpdated) $m.Camera.UpdateView(B_False);
	}
	_secr $VOID UpdateWave(){
		///相位计算

		SPFPN *pPhase = $m.rgWavClass[0].rgWavPhase;
		SPFPN *pPhaseE = pPhase + 4;

		for(; pPhase != pPhaseE; ++pPhase){
			*pPhase += 0.008f;
			*pPhase = fmod(*pPhase, 2.f * SIMD_PI);
		}

		///数据同步

		NWavClass *pSurf0 = &$m.rgWavClass[0];
		NWavClass *pSurf = pSurf0 + 1;

		for(UNS32 iWav = 1; iWav < 8; ++iWav){
			pSurf->rgWavPhase[0] = pSurf0->rgWavPhase[0];
			pSurf->rgWavPhase[1] = pSurf0->rgWavPhase[1];
			pSurf->rgWavPhase[2] = pSurf0->rgWavPhase[2];
			pSurf->rgWavPhase[3] = pSurf0->rgWavPhase[3];
			pSurf++;
		}
	}
	_secr $VOID UpdateBrush(){
		///尺寸更新
		if(gKeysTest[DIK_LALT]){
			INT32 Delta = 0;

			if(GEInp::MouseState.lZ < 0)
				Delta = -1;
			else if(GEInp::MouseState.lZ > 0)
				Delta = 1;

			if(Delta != 0){
				CLabel *pCtrl = mgGetLabel(ID_LBL_BrushSize);
				INT32 Value = _wtoi(pCtrl->lpsText) + Delta;
				Value = CLAMP_(Value, 1, MAX_BRUSH);
				_snwprintf(pCtrl->lpsText, 4, L"%d", Value);
				$m.SetBrushSize(Value);
			}
		}

		///力度更新
		if(gKeysTest[DIK_LCONTROL]){
			SPFPN Delta = 0.f;

			if(GEInp::MouseState.lZ < 0)
				Delta = -0.1f;
			else if(GEInp::MouseState.lZ > 0)
				Delta = 0.1f;

			if(Delta != 0.f){
				CLabel *pCtrl = mgGetLabel(ID_LBL_BrushPower);
				SPFPN Value = (SPFPN)_wtof(pCtrl->lpsText) + Delta;
				Value = CLAMP_(Value, 0.f, 1.f);
				_snwprintf(pCtrl->lpsText, 8, L"%g", Value);
				$m.SetBrushPower(Value);
			}
		}

		///方向更新
		if(GEInp::JustPressed(DIK_TAB)){
			switch($m.LayupDir){
				case DIR_Back:{ $m.LayupDir = DIR_Right; break; };
				case DIR_Front:{ $m.LayupDir = DIR_Left; break; };
				case DIR_Left:{ $m.LayupDir = DIR_Back; break; };
				case DIR_Right:{ $m.LayupDir = DIR_Front; break; };
			}
		}
	}
	_secr $VOID UpdatePreview(){
		$m.bConflict = B_False;

		SIZE *pScale = &$m.Preview.Scale;
		VFloat3 *pCoord = &$m.Preview.Coord;

		if($m.Preview.PsoId == ID_PSO_Struct){
			*pCoord = $m.GetSnapedLocation(*pScale);

			RECT rcScope = $m.GetOccupiedRegion(*pCoord, *pScale);
			INT32 MaxCol = $m.Ground.ColCount - 1;
			INT32 MaxRow = $m.Ground.RowCount - 1;

			if((rcScope.left < 0) || (rcScope.right > MaxCol) ||
				(rcScope.bottom < 0) || ((rcScope.top > MaxRow)))
			{
				$m.bConflict = B_True;
				return;
			}

			for(INT32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
				INT32 iLine = $m.Ground.ColCount * iRow;
				for(INT32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol){
					INT32 iCell = iLine + iCol;

					if($m.lprgCellInfo[iCell].iHolder != 0){
						$m.bConflict = B_True;
						return;
					}
				}
			}
		} else {
			*pCoord = $m.SelPoint;
		}
	}
	_secr $VOID UpdatePicking(){
		$m.bPicking = B_False;
		if($m.InAdjustmentMode()) return;

		SPFPN CursorX = (SPFPN)gCursor->x;
		SPFPN CursorY = (SPFPN)gCursor->y;
		if(CursorX <= gMyPanelW) return;

		VFloat2 Cursor(CursorX, CursorY);
		if(PointInRect(Cursor, ToRectF(MenuFrame))) return;

		SPFPN RayDist;
		VFloat2 WndSize = { gMyWndW, gMyWndH };
		MXVECTOR RayOrg, RayDir;
		dx::XMRayFromScreen(RayOrg, RayDir, Cursor, WndSize, $m.Camera.matProj, $m.Camera.matView);
		FRay3 ScrRay(RayOrg, RayDir);

		$m.bPicking = $m.Ground.IntersectRay(ScrRay, &RayDist);
		if($m.bPicking)	$m.SelPoint = ScrRay.position + (ScrRay.direction * RayDist);
	}
	_secr $VOID UpdateLandPicking(){
		if(!$m.bPicking) return;

		SPFPN BrushRad = $m.BrushRad0;
		SPFPN CellSize = CELL_SIZEH;
		SPFPN AlignF = fmod(BrushRad, CellSize);

		if($m.InLandPaintMode()){
			BrushRad /= CELL_SPLIT;
			CellSize /= CELL_SPLIT;
		}

		$m.SelPoint.x -= fmod($m.SelPoint.x, CellSize);
		$m.SelPoint.z -= fmod($m.SelPoint.z, CellSize);
		$m.SelPoint.y += 100.f;

		if(AlignF != 0.f){
			AlignF = CellSize * 0.5f;
			$m.SelPoint.x += AlignF;
			$m.SelPoint.z += AlignF;
		}

		$m.MapFrame.top = $m.SelPoint.z + BrushRad;
		$m.MapFrame.left = $m.SelPoint.x - BrushRad;
		$m.MapFrame.right = $m.SelPoint.x + BrushRad;
		$m.MapFrame.bottom = $m.SelPoint.z - BrushRad;

		$m.Prjctr.Orthographic(BrushRad * 2.f, BrushRad * 2.f, 0.f, 1000.f);
		$m.Prjctr.Locate(Ld_V3(&$m.SelPoint));
		$m.Prjctr.UpdateView(B_False);
		$m.Prjctr.GetViewProjTexMat($m.prgMarkProj[gMyGfxCmd.CurFrame]);
	}
	_secr $VOID UpdateBodyPicking(){
		UNS32 iPickedPx = (gMyWndEx.Width * gCursor->y) + gCursor->x;
		UNS32 iPickedObj = gMyWndEx.prgIdPixel[iPickedPx];
	}
	///辅助功能
	_secr $VOID ResetFov(){
		SPFPN EyePosY = $m.Camera.GetPositionY();
		SPFPN ViewRngF = EyePosY / $m.TanAlpha;
		SPFPN ViewRngB = EyePosY / $m.TanGamma;
		SPFPN ViewRngS = EyePosY / $m.TanTheta;
		SPFPN FadeArea = gMyPanelW / gMyWndW * ViewRngS * 2.f;

		$m.EyeScopeZ0 = ViewRngB - CELL_SIZEH;
		$m.EyeScopeX0 = ViewRngS - CELL_SIZEH - FadeArea;
		$m.EyeScopeZ1 = $m.AreaDepth - ViewRngF + CELL_SIZEH;
		$m.EyeScopeX1 = $m.AreaWidth - ViewRngS + CELL_SIZEH;

		$m.FovDepth = ViewRngF + ViewRngB;
		$m.FovWidth = ViewRngS * 2.f;
	}
	_secr $VOID ResetWater(){
		//////

		UNS32 NumCell = 0;
		UIPTR NumView = $m.WavViews.size();

		NWavView *pView = &$m.WavViews[1];
		NWavView *pEndView = pView + NumView - 1;

		for(; pView != pEndView; ++pView){
			if(pView->NumCell != 0){
				pView->CellBase = NumCell;
				NumCell += pView->NumCell;
				pView->NumCell = 0;
			}
		}

		//////

		UNS32 MaxCol = $m.Ground.ColCount;
		UNS32 MaxRow = $m.Ground.RowCount;

		VUint2 *pVert;
		VUint2 *prgVert = (VUint2*)$m.Water.GetMemory(0);
		NWavView *prgView = $m.WavViews.data();
		NCellInfo *prgCell = $m.lprgCellInfo;

		for(UNS32 iRow = 0; iRow < MaxRow; ++iRow){
			for(UNS32 iCol = 0; iCol < MaxCol; ++iCol){
				if(prgCell[iCol].iWavView == 0) continue;
				pView = &prgView[prgCell[iCol].iWavView];
				pVert = &prgVert[pView->CellBase + pView->NumCell];
				pVert->x = iCol;
				pVert->y = iRow;
				pView->NumCell += 1;
			}
			prgCell += MaxCol;
		}

		$m.Water.SetCount(NumCell);
	}
	_secr $VOID ResetChunks(){
		/*UNS32 MaxCol = $m.Ground.ColCount;
		UNS32 MaxRow = $m.Ground.RowCount;
		NMarquee Marquee = $m.GetPickedRegion();
		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLine = iRow * MaxCol;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				NFaceFeat *pFeat0 = prgFeat + (iLine + iCol)*2;
				NFaceFeat *pFeat1 = pFeat0 + 1;

				NFaceFeat *pFeatL = pFeat0 - 1;
				NFaceFeat *pFeatR = pFeat1 + 1;
				NFaceFeat *pFeatF = pFeat0 - (MaxCol * 2);
				NFaceFeat *pFeatB = pFeat0 + (MaxCol * 2);

				SPFPN *pHeightL = &pFeatL->Starts[0];
				SPFPN *pHeightR = &pFeatR->Starts[0];
				SPFPN *pHeightF = &pFeatF->Starts[1];
				SPFPN *pHeightB = &pFeatB->Starts[0];

				if((iCol > 0) && pFeatL->bBslash)
					pHeightL++;
				if((iCol < (MaxCol - 1)) && pFeatR->bBslash)
					pHeightR++;
				if((iRow > 0) && pFeatF->bBslash){
					pFeatF++;
					pHeightF = &pFeatF->Starts[0];
				}
				if((iRow < (MaxRow - 1)) && !pFeatB->bBslash){
					pFeatB++;
					pHeightB = &pFeatB->Starts[1];
				}

				pFeat0->Starts[0] = pFeat0->End;
				pFeat0->Starts[1] = pFeat0->End;
				pFeat0->Starts[2] = pFeat1->End;
				pFeat1->Starts[0] = pFeat1->End;
				pFeat1->Starts[1] = pFeat1->End;
				pFeat1->Starts[2] = pFeat0->End;

				if(!pFeat0->bBslash){
					if(iCol > 0){
						pFeat0->Starts[0] = pFeatL->End;
						*pHeightL = pFeat0->End;
					}
					if(iCol < (MaxCol - 1)){
						pFeat1->Starts[0] = pFeatR->End;
						*pHeightR = pFeat1->End;
					}
					if(iRow > 0){
						pFeat1->Starts[1] = pFeatF->End;
						*pHeightF = pFeat1->End;
					}
					if(iRow < (MaxRow - 1)){
						pFeat0->Starts[1] = pFeatB->End;
						*pHeightB = pFeat0->End;
					}
				} else{
					if(iCol > 0){
						pFeat0->Starts[1] = pFeatL->End;
						*pHeightL = pFeat0->End;
					}
					if(iCol < (MaxCol - 1)){
						pFeat1->Starts[1] = pFeatR->End;
						*pHeightR = pFeat1->End;
					}
					if(iRow > 0){
						pFeat0->Starts[0] = pFeatF->End;
						*pHeightF = pFeat0->End;
					}
					if(iRow < (MaxRow - 1)){
						pFeat1->Starts[0] = pFeatB->End;
						*pHeightB = pFeat1->End;
					}
				}
			}
		}*/
	}
	_secr $VOID RestrictView(){
		VFloat3 *pEyePos = (VFloat3*)&$m.Camera.Position;

		if(pEyePos->y > $m.EyeScopeY1)
			pEyePos->y = $m.EyeScopeY1;
		else if(pEyePos->y < $m.EyeScopeY0)
			pEyePos->y = $m.EyeScopeY0;

		if($m.FovWidth < $m.AreaWidth){
			if(pEyePos->x > $m.EyeScopeX1)
				pEyePos->x = $m.EyeScopeX1;
			else if(pEyePos->x < $m.EyeScopeX0)
				pEyePos->x = $m.EyeScopeX0;
		} else{
			pEyePos->x = $m.AreaWidth * 0.5f;
			pEyePos->x += (gMyPanelW / gMyWndW) * (pEyePos->y / $m.TanTheta);
		}

		if($m.FovDepth < $m.AreaDepth){
			if(pEyePos->z > $m.EyeScopeZ1)
				pEyePos->z = $m.EyeScopeZ1;
			else if(pEyePos->z < $m.EyeScopeZ0)
				pEyePos->z = $m.EyeScopeZ0;
		} else{
			SPFPN ViewRngB = pEyePos->y / $m.TanGamma;
			SPFPN ViewRngF = pEyePos->y / $m.TanAlpha;

			if(ViewRngB < 0.f){
				pEyePos->z = $m.AreaDepth * 0.5f;
				pEyePos->z -= ($m.FovDepth * 0.5f) - ViewRngB;
			} else{
				pEyePos->z = $m.AreaDepth * 0.5f;
				pEyePos->z -= (ViewRngF - ViewRngB) * 0.5f;
			}
		}
	}
	_secr $VOID SyncTexture(){
		if($m.TexFrame.back != 0){
			UNS32 DstSize, DstLeft, DstTop;
			D3D12_TEXTURE_COPY_LOCATION Dest, Source;

			//计算布局

			DstTop = $m.TexFrame.top;
			DstLeft = $m.TexFrame.left;
			DstSize = $m.NumMapCol * $m.NumMapRow * sizeof(ARGB8);

			Dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			Dest.pResource = $m.rgMapTex[0];

			Source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Source.pResource = $m.lpMapCanvas;
			Source.PlacedFootprint.Footprint.Depth = 1;
			Source.PlacedFootprint.Footprint.Width = $m.NumMapCol;
			Source.PlacedFootprint.Footprint.Height = $m.NumMapRow;
			Source.PlacedFootprint.Footprint.RowPitch = $m.NumMapCol * sizeof(ARGB8);
			Source.PlacedFootprint.Footprint.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

			//复制像素

			if($m.FuncMode <= MODE_EditPattern){
				Dest.SubresourceIndex = 0;
				Source.PlacedFootprint.Offset = 0;
				gMyGfxCmd.lpCmdList->CopyTextureRegion(&Dest, DstLeft, DstTop, 0, &Source, &$m.TexFrame);

				Dest.SubresourceIndex = 2;
				Source.PlacedFootprint.Offset = DstSize * 2;
				gMyGfxCmd.lpCmdList->CopyTextureRegion(&Dest, DstLeft, DstTop, 0, &Source, &$m.TexFrame);

				Dest.SubresourceIndex = 3;
				Source.PlacedFootprint.Offset = DstSize * 3;
				gMyGfxCmd.lpCmdList->CopyTextureRegion(&Dest, DstLeft, DstTop, 0, &Source, &$m.TexFrame);
			} else{
				Dest.SubresourceIndex = 1;
				Source.PlacedFootprint.Offset = DstSize;
				gMyGfxCmd.lpCmdList->CopyTextureRegion(&Dest, DstLeft, DstTop, 0, &Source, &$m.TexFrame);
			}

			$m.TexFrame.back = 0;
		}
	}
	_secr $VOID SyncIdBuffer(){
		if($m.bPrepIdImg){
			$m.bPickObjId = B_True;
			$m.bPrepIdImg = B_False;
			gMyWndEx.ReadbackIdBuffer(gMyGfxCmd.lpCmdList);
		}
	}
	_secr $VOID CleanTexture(){
		if($m.lpTexViews){
			for(UNS32 iTex = 0; iTex < ARR_LEN($m.rgMapTex); ++iTex){
				$m.rgMapTex[iTex]->Release();
				$m.rgMapTex[iTex] = P_Null;
			}

			$m.lpMapCanvas->Unmap(0, P_Null);
			$m.lpMapCanvas->Release();
			$m.lpMapCanvas = P_Null;

			$m.lpTexViews->Release();
			$m.lpTexViews = P_Null;
		}
	}
	_secr $VOID NewWaterView(){
		UIPTR NumView = $m.WavViews.size();
		NWavView *pView = &$m.WavViews[1];

		for($m.iWavView = 1; $m.iWavView < NumView; ++$m.iWavView){
			if(pView->NumCell == 0) break;
			pView++;
		}

		if($m.iWavView == NumView){
			$m.WavViews.emplace_back($m.WavLevel, $m.iWavClass, 0, 0);
		} else{
			pView->NumCell = 0;
			pView->CellBase = 0;
			pView->Level = $m.WavLevel;
			pView->ClassId = $m.iWavClass;
		}
	}
	_secr $VOID FitLandWiring(){
		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);
		NMarquee Marquee = $m.GetPickedRegion();
		VUint2 CenterU = RECT_CENTER(Marquee.RectU);
		FCircle2 Circle(RECT_CENTER($m.MapFrame), $m.BrushRad0);
		UNS32 RowPitch = $m.Ground.ColCount;

		if(Marquee.bRect){
			if(Marquee.L == Marquee.R) return;

			UNS32 *prgBound = &Marquee.L;
			IBOOL rgFlag[4] = { B_True, B_False, B_False, B_True };

			for(UNS32 K = 0; K < 2; ++K){
				for(UNS32 L = 0; L < 2; ++L){
					UNS32 iRow = prgBound[K*2 + 1];
					UNS32 iCol = prgBound[L * 2];
					UNS32 iFace = (RowPitch*iRow + iCol) * 2;
					IBOOL bBslash = rgFlag[K*2 + L];

					if(bBslash){
						prgFeat[iFace].bBslash = B_True;
					} else{
						prgFeat[iFace].bBslash = B_False;
					}
				}
			}
		} else{
			for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
				for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
					UNS32 iFace = (RowPitch*iRow + iCol) * 2;
					UNS32 Count = 0;
					VFloat2 rgVert[4];

					rgVert[0] = { iCol * CELL_SIZEH, iRow * CELL_SIZEH };
					rgVert[1] = { rgVert[0].x + CELL_SIZEH, rgVert[0].y };
					rgVert[2] = { rgVert[0].x, rgVert[0].y + CELL_SIZEH };
					rgVert[3] = { rgVert[1].x, rgVert[2].y };

					if(Marquee.bCir){
						for(UNS32 iVtx = 0; iVtx < 4; ++iVtx){
							if(PointInCircle(rgVert[iVtx], Circle))
								Count++;
						}
					} else if(Marquee.bRhom){
						for(UNS32 iVtx = 0; iVtx < 4; ++iVtx){
							if(PointInRhombus(rgVert[iVtx], $m.MapFrame, $m.BrushRad0))
								Count++;
						}
					}

					if((Count == 0) || (Count == 4))
						continue;

					if(iCol <= CenterU.x){
						if(iRow <= CenterU.y){
							prgFeat[iFace].bBslash = B_True;
						} else{
							prgFeat[iFace].bBslash = B_False;
						}
					} else{
						if(iRow <= CenterU.y){
							prgFeat[iFace].bBslash = B_False;
						} else{
							prgFeat[iFace].bBslash = B_True;
						}
					}
				}
			}
		}

		$m.Ground.UpdateNormals($m.MapFrame);
	}
	///装饰、角色添加
	_secr $VOID AddActor(){
		if(!$m.bConflict){
			if($m.Preview.PsoId == ID_PSO_Animal){
				MGAnimalClass *pClass = mgGetAnimalClass($m.Preview.MdlId);
				UNS32 ClassId = UNS32(pClass - gMyAnimalClsArr);
				gMyAnimals.emplace_back(ClassId, *pClass, $m.Preview.Coord);
				// end if
			} else if($m.Preview.PsoId == ID_PSO_Struct){
				MGStructClass *pClass = mgGetStructClass($m.Preview.MdlId);
				UNS32 ClassId = UNS32(pClass - gMyStructClsArr);
				RECT rcScope = $m.GetOccupiedRegion($m.Preview.Coord, $m.Preview.Scale);
				gMyStructs.emplace_back(ClassId, *pClass, $m.Preview.Coord);
				$m.ModifyRegionMask(rcScope, 1);
			}
		}
	}
	_secr $VOID AddDecoration(){

	}
	///特殊地形生成
	_secr $VOID MakeAbyss(){
		IBOOL bOpShift = gKeysTest[DIK_LSHIFT];
		NMarquee Marquee = $m.GetPickedRegion();
		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);

		$m.FitLandWiring();

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLine = iRow * $m.Ground.ColCount;
			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				UNS32 iFace = (iLine + iCol) * 2;
				for(UNS32 iPart = 0; iPart < 2; ++iPart, ++iFace){
					NFaceFeat *pFeat = prgFeat + iFace;

					if(Marquee.bCir && !$m.TerrainInCircle(iRow, iCol, iFace))
						continue;
					if(Marquee.bRhom && !$m.TerrainInRhombus(iRow, iCol, iFace))
						continue;

					if(!bOpShift){
						pFeat->Extrusion = -4;
						pFeat->bDiscard = B_True;
					} else if(pFeat->bDiscard){
						pFeat->Extrusion = 0;
						pFeat->bDiscard = B_False;
					}
				}
			}
		}

		$m.ResetChunks();
	}
	_secr $VOID MakeCliff(){
		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);
		NMarquee Marquee = $m.GetPickedRegion();

		IBOOL bUniform = gKeysTest[DIK_LALT];
		INT16 Height = bUniform ? $m.CalcUniformCliff(Marquee.RectU) : 0;
		INT16 Delta = gKeysTest[DIK_LSHIFT] ? -1 : 1;

		$m.FitLandWiring();

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftCell = iRow * $m.Ground.ColCount;
			UNS32 iFace = (iLeftCell + Marquee.L) * 2;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				for(UNS32 iPart = 0; iPart < 2; ++iPart, ++iFace){
					NFaceFeat *pFeat = prgFeat + iFace;

					if(pFeat->bDiscard)
						continue;
					if(Marquee.bCir && !$m.TerrainInCircle(iRow, iCol, iFace))
						continue;
					if(Marquee.bRhom && !$m.TerrainInRhombus(iRow, iCol, iFace))
						continue;

					if(bUniform) pFeat->Extrusion = Height;
					else pFeat->Extrusion += Delta;
				}
			}
		}

		$m.ResetChunks();
	}
	_secr $VOID MakeWater(){
		IBOOL bOpShift = gKeysTest[DIK_LSHIFT];
		NMarquee Marquee = $m.GetPickedRegion();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);
		NWavView *prgView = $m.WavViews.data();
		NWavView *pCurView = prgView + $m.iWavView;

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftCell = iRow * $m.Ground.ColCount;
			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				UNS32 &rViewId = $m.lprgCellInfo[iLeftCell + iCol].iWavView;

				if(Marquee.bCir && !PointInCircle(ToPoint(iCol, iRow), Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(ToPoint(iCol, iRow), Marquee.Rect, Marquee.Rad))
					continue;

				if(bOpShift){
					if(rViewId != 0){
						prgView[rViewId].NumCell -= 1;
						rViewId = 0;
					}
				} else{
					if(rViewId != $m.iWavView){
						if(rViewId != 0) prgView[rViewId].NumCell -= 1;
						pCurView->NumCell += 1;
						rViewId = $m.iWavView;
					}
				}
			}
		}

		$m.ResetWater();
	}
	_secr $VOID MakeClosed(){
		
	}
	_secr $VOID MakeRoad(IBOOL bBreak){
		static UNS32 tNumCtrl = 0;

		if(bBreak){
			UNS32 PopCnt = tNumCtrl % 4;
			for(; PopCnt != 0; --PopCnt)
				$m.Paths.pop_back();

			if($m.Paths.size() > 0){
				if(!isnan($m.Paths.back().x))
					$m.Paths.emplace_back(NAN, 0.f, 0.f);
			}
			tNumCtrl = 0;
			return;
		}

		VFloat3 NewPoint = $m.SelPoint;
		UNS32 DstStart = $m.Roads.GetCount();

		tNumCtrl += 1;
		NewPoint.y += 0.5f;
		$m.Paths.push_back(NewPoint);

		if((tNumCtrl > 4) && ((tNumCtrl % 4) == 2)){
			FVector3 *prgDest = (FVector3*)$m.Roads.GetMemory(DstStart);
			UIPTR OldSize = $m.Paths.size();
			$m.Paths.resize(OldSize + 2);
			FVector3 *prgSource = (FVector3*)&$m.Paths.back() - 4;
			FVector3 *pPrev1 = prgDest - 1;
			FVector3 *pPrev2 = pPrev1 - 1;

			prgSource[3] = prgSource[1];
			prgSource[2] = prgSource[0];
			prgSource[1] = *pPrev1;
			prgSource[0] = (*pPrev1 + *pPrev2) * 0.5f;
			*pPrev1 = prgSource[0];

			prgDest[0] = prgSource[0];
			prgDest[1] = prgSource[1];
			prgDest[2] = prgSource[2];
			prgDest[3] = prgSource[3];

			tNumCtrl += 2;
			$m.Roads.SetCount(DstStart + 4);

			// end branch
		} else if(tNumCtrl == 4){
			VFloat3 *prgDest = (VFloat3*)$m.Roads.GetMemory(DstStart);
			VFloat3 *prgSource = &$m.Paths.back() - 3;

			prgDest[0] = prgSource[0];
			prgDest[1] = prgSource[1];
			prgDest[2] = prgSource[2];
			prgDest[3] = prgSource[3];

			$m.Roads.SetCount(DstStart + 4);
		}
	}
	///地形编辑
	_secr $VOID EditLandEdge(){
		BOOL8 bOpShift = gKeysTest[DIK_LSHIFT];
		UNS32 RowPitch = $m.Ground.ColCount;

		NMarquee Marquee = $m.GetPickedRegion();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftCell = RowPitch * iRow;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				VFloat2 Point = ToPoint(iCol, iRow);

				if(Marquee.bCir && !PointInCircle(Point, Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Point, Marquee.Rect, Marquee.Rad))
					continue;

				UNS32 iFace = (iLeftCell + iCol) * 2;

				if(bOpShift){
					prgFeat[iFace].bBslash = B_False;
				} else{
					prgFeat[iFace].bBslash = B_True;
				}
			}
		}

		$m.Ground.UpdateNormals($m.MapFrame);
	}
	_secr $VOID EditLandHeight(){
		SPFPN Power, Height, DistSq;

		BOOL8 bOpShift = gKeysTest[DIK_LSHIFT];
		BOOL8 bUniform = ($m.FuncMode == MODE_Uniform);
		BOOL8 bSteepen = ($m.FuncMode == MODE_Steepen);
		BOOL8 bFlatten = (bSteepen && bOpShift);

		NMarquee Marquee = $m.GetPickedVertices();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		///计算高度

		if(bUniform){
			Height = $m.CalcUniformAltitude(Marquee.RectU);
		} else if(bFlatten){
			Height = $m.CalcAverageAltitude(Marquee.RectU);
			Power = $m.BrushPower * 0.5f;
		} else if(bOpShift){
			Power = -$m.BrushPower;
		} else{
			Power = $m.BrushPower;
		}

		///更新顶点

		NLandVert *prgVert = $m.Ground.prgVertex;
		prgVert += Marquee.B * $m.NumMeridian;

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				SPFPN &rCoordY = prgVert[iCol].CoordY;
				VFloat2 Coord = $m.GetTerrainVertex(iRow, iCol);

				if(Marquee.bCir && !PointInCircle(Coord, Circle, &DistSq))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Coord, Marquee.Rect, Marquee.Rad - CELL_SIZEH))
					continue;

				if(bFlatten) rCoordY = LERP_(rCoordY, Height, Power);
				else if(bUniform) rCoordY = Height;
				else if(bSteepen) rCoordY += $m.CalcTerrainNoise(Coord);
				else if(!Marquee.bCir) rCoordY += Power;
				else rCoordY += Power * (1.f - DistSq/Marquee.RadSq);
			}
			prgVert += $m.NumMeridian;
		}

		///更新布线

		if($m.FuncMode != MODE_Uplift){
			$m.Ground.UpdateNormals($m.MapFrame);
		} else{
			$m.FitLandWiring();
		}
	}
	_secr $VOID EditLandShading(){
		IBOOL bOpShift = gKeysTest[DIK_LSHIFT];
		UNS32 RowPitch = $m.Ground.ColCount;

		NMarquee Marquee = $m.GetPickedRegion();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		NFaceFeat *prgFeat = $m.GetTerrainFeature(0);

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftCell = RowPitch * iRow;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				VFloat2 Point = ToPoint(iCol, iRow);

				if(Marquee.bCir && !PointInCircle(Point, Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Point, Marquee.Rect, Marquee.Rad))
					continue;

				UNS32 iFace0 = (iLeftCell + iCol) * 2;
				UNS32 iFace1 = iFace0 + 1;

				if(bOpShift){
					prgFeat[iFace0].bSmooth = B_True;
					prgFeat[iFace1].bSmooth = B_True;
				} else{
					prgFeat[iFace0].bSmooth = B_False;
					prgFeat[iFace1].bSmooth = B_False;
				}
			}
		}
	}
	_secr $VOID EditLandColor(){
		SPFPN Falloff;
		UNS32 Weight0, Weight1;
		UNS32 MapSize = $m.NumMapCol * $m.NumMapRow;

		PVColor *psMap0 = $m.psCanvPx;
		PVColor *psMap1 = psMap0 + (MapSize * 2);
		PVColor *psMap2 = psMap1 + MapSize;

		NMarquee Marquee = $m.GetPickedCanvas();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		//更新像素

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftPx = iRow * $m.NumMapCol;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				VFloat2 Point = ToPoint(iCol, iRow);

				if(Marquee.bCir && !PointInCircle(Point, Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Point, Marquee.Rect, Marquee.Rad))
					continue;

				UNS32 iPixel = iLeftPx + iCol;
				PVColor *pPixel1 = psMap1 + iPixel;
				PVColor *pPixel2 = psMap2 + iPixel;

				Weight1 =
					(UNS32)pPixel1->r + (UNS32)pPixel1->g + (UNS32)pPixel1->b + (UNS32)pPixel1->a +
					(UNS32)pPixel2->r + (UNS32)pPixel2->g +	(UNS32)pPixel2->b + (UNS32)pPixel2->a;

				Weight0 = UNS32($m.BrushPower * 255.f);
				Weight0 = MIN_(Weight0, 255);

				if((Weight0 + Weight1) > 255){
					Falloff = SPFPN(255 - Weight0) / 255.f;
					pPixel1->r = UNS08(pPixel1->r * Falloff);
					pPixel1->g = UNS08(pPixel1->g * Falloff);
					pPixel1->b = UNS08(pPixel1->b * Falloff);
					pPixel1->a = UNS08(pPixel1->a * Falloff);
					pPixel2->r = UNS08(pPixel2->r * Falloff);
					pPixel2->g = UNS08(pPixel2->g * Falloff);
					pPixel2->b = UNS08(pPixel2->b * Falloff);
					pPixel2->a = UNS08(pPixel2->a * Falloff);
				}

				psMap0[iPixel] = $m.BrushColor;
			}
		}

		//更新变动范围

		$m.TexFrame.top = Marquee.B;
		$m.TexFrame.left = Marquee.L;
		$m.TexFrame.right = Marquee.R + 1;
		$m.TexFrame.bottom = Marquee.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	_secr $VOID EditLandTexture(){
		UNS32 TexId = $m.BrushMapId % 4;
		UNS32 ChanId = 3 - ((TexId + 1) % 4); //BGRA转RGBA
		UNS32 CanvId = ($m.BrushMapId < 4) ? 2 : 3;
		UNS32 MapSize = $m.NumMapCol * $m.NumMapRow;

		PVColor *psMap0 = $m.psCanvPx + (MapSize * CanvId);
		PVColor *psMap1 = $m.psCanvPx + (MapSize * (5 - CanvId));

		NMarquee Marquee = $m.GetPickedCanvas();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		//更新像素

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftPx = iRow * $m.NumMapCol;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				VFloat2 Point = ToPoint(iCol, iRow);

				if(Marquee.bCir && !PointInCircle(Point, Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Point, Marquee.Rect, Marquee.Rad))
					continue;

				PVColor *pPixel0 = psMap0 + iLeftPx + iCol;
				PVColor *pPixel1 = psMap1 + iLeftPx + iCol;
				UNS08 *pChan = &pPixel0->b + ChanId;
				UNS32 Weight = UNS32(*pChan) + UNS32($m.BrushPower * 255.f);

				Weight = CLAMP_(Weight, 0, 255);
				*pChan = (UNS08)Weight;

				UNS32 WeightSum =
					pPixel0->r + pPixel0->g + pPixel0->b + pPixel0->a +
					pPixel1->r + pPixel1->g + pPixel1->b + pPixel1->a;

				if(WeightSum > 255){
					SPFPN Falloff = SPFPN(255 - Weight) / 255.f;
					pPixel0->r = UNS08(pPixel0->r * Falloff);
					pPixel0->g = UNS08(pPixel0->g * Falloff);
					pPixel0->b = UNS08(pPixel0->b * Falloff);
					pPixel0->a = UNS08(pPixel0->a * Falloff);
					pPixel1->r = UNS08(pPixel1->r * Falloff);
					pPixel1->g = UNS08(pPixel1->g * Falloff);
					pPixel1->b = UNS08(pPixel1->b * Falloff);
					pPixel1->a = UNS08(pPixel1->a * Falloff);
				}

				*pChan = (UNS08)Weight;
			}
		}

		//更新变动范围

		$m.TexFrame.top = Marquee.B;
		$m.TexFrame.left = Marquee.L;
		$m.TexFrame.right = Marquee.R + 1;
		$m.TexFrame.bottom = Marquee.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	_secr $VOID EditLandMaterial(){
		UNS32 ChanId = ($m.FuncMode == MODE_EditMetal) ? 1 : 2;
		UNS32 MapSize = $m.NumMapCol * $m.NumMapRow;
		PVColor *psMap = $m.psCanvPx + MapSize;
		NMarquee Marquee = $m.GetPickedCanvas();
		FCircle2 Circle(Marquee.Cen, Marquee.Rad);

		//更新像素

		for(UNS32 iRow = Marquee.B; iRow <= Marquee.T; ++iRow){
			UNS32 iLeftPx = iRow * $m.NumMapCol;

			for(UNS32 iCol = Marquee.L; iCol <= Marquee.R; ++iCol){
				VFloat2 Point = ToPoint(iCol, iRow);

				if(Marquee.bCir && !PointInCircle(Point, Circle))
					continue;
				if(Marquee.bRhom && !PointInRhombus(Point, Marquee.Rect, Marquee.Rad))
					continue;

				PVColor *pPixel = psMap + iLeftPx + iCol;
				UNS08 *pChan = &pPixel->b + ChanId;

				*pChan = UNS08($m.BrushPower * 255.f);
				*pChan = MIN_(255, *pChan);
			}
		}

		//更新变动范围

		$m.TexFrame.top = Marquee.B;
		$m.TexFrame.left = Marquee.L;
		$m.TexFrame.right = Marquee.R + 1;
		$m.TexFrame.bottom = Marquee.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	///编辑器设置
	_open $VOID SetBrushSize(UNS32 Scale){
		if(($m.BrushShape == BR_TYPE_Rhombus) && ((Scale % 2) != 0)){
			$m.BrushRad1 = CELL_SIZEH * Scale * 0.5f;
			$m.BrushRad0 = CELL_SIZEH * (Scale + 1) * 0.5f;
		} else{
			$m.BrushRad1 = CELL_SIZEH * Scale * 0.5f;
			$m.BrushRad0 = $m.BrushRad1;
		}
	}
	_open $VOID SetBrushPower(SPFPN Power){
		$m.BrushPower = Power;
	}
	_open $VOID SetBrushColor(ARGB8 Color){
		$m.BrushColor = Color;
	}
	_open $VOID SetBrushShape(BRUSHTYPE Type){
		$m.BrushShape = Type;
		$m.BrushRad0 = $m.BrushRad1;

		if(Type == BR_TYPE_Rhombus){
			if(fmod($m.BrushRad1, CELL_SIZEH) != 0)
				$m.BrushRad0 += CELL_SIZEH * 0.5f;
		}
	}
	_open $VOID SetBrushMapping(UNS32 Ident){
		$m.BrushMapId = (UNS16)Ident;
	}
	_open $VOID SetFunctionMode(DWORD Mode){
		if(Mode < MODE_COUNT){
			$m.FuncMode = (BYTET)Mode;
		} else{
			switch(Mode){
				case ID_RDO_Uplift:{
					$m.FuncMode = MODE_Uplift;
				} break;
				case ID_RDO_Steepen:{
					$m.FuncMode = MODE_Steepen;
				} break;
				case ID_RDO_Flatten:{
					$m.FuncMode = MODE_Uniform;
				} break;
				case ID_RDO_Sharpen:{
					$m.FuncMode = MODE_Sharpen;
				} break;
				case ID_LST_BrushColor:{
					$m.FuncMode = MODE_EditColor;
				} break;
				case ID_LST_BrushPattern:{
					$m.FuncMode = MODE_EditPattern;
				} break;
			}
		}

		if($m.FuncMode == MODE_MakeWater)
			$m.NewWaterView();

		$m.Preview.pModel = P_Null;
		$m.MenuFrame = URect{};
	}
	_open $VOID SetPreviewObject(UNS32 Ident){
		SIZE Scale;
		DWORD Type;
		UNS32 PsoId;
		CMeshAssy *pModel;
		MGStructClass *pClass;

		if($m.FuncMode ==  MODE_AddUnit){
			pModel = mgGetModel(ID_MDL_Actor, Ident);
			Type = pModel->lpFirst->VtxFormat;

			if(gMyShaderArr[ID_PSO_Animal].VtxFlag == Type){
				PsoId = ID_PSO_Animal;
			} else if(gMyShaderArr[ID_PSO_Struct].VtxFlag == Type){
				PsoId = ID_PSO_Struct;
				pClass = mgGetStructClass(Ident);
				Scale.cx = pClass->SizeX;
				Scale.cy = pClass->SizeZ;
			}
		} else if($m.FuncMode ==  MODE_MakeDecor){
			pModel = mgGetModel(ID_MDL_Decor, Ident);
			// To Do
		} else{
			pModel = P_Null;
		}

		$m.Preview.PsoId = PsoId;
		$m.Preview.MdlId = Ident;
		$m.Preview.Scale = Scale;
		$m.Preview.pModel = pModel;
	}
	///地图格子更新
	_secr $VOID ModifyRegionMask(RECT &rcScope, INT32 Step){
		INT32 CellBase = $m.Ground.ColCount * (rcScope.bottom - 1);
		NCellInfo *prgCell = $m.lprgCellInfo + CellBase;

		for(INT32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			prgCell += $m.Ground.ColCount;
			for(INT32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				prgCell[iCol].iHolder += Step;
		}
	}
	///过程图应用
	_open $VOID ApplyColorMap(WCHAR *psFilePath, UNS32 Repeat){
		PVColor *prgPixel;
		IWICBitmapLock *lpLock;
		IWICBitmap *lpBitmap;
		UNS32 Size, Pitch;
		UNS32 NumCol, NumRow, MaxRow;

		//读取文件
		{
			lpBitmap = LoadWicBitmap(GE2d::lpWicFactory, psFilePath);
			lpBitmap->Lock(P_Null, WICBitmapLockRead, &lpLock);

			lpLock->GetDataPointer(&Size, (BYTET**)&prgPixel);
			lpLock->GetSize(&NumCol, &NumRow);
			lpLock->GetStride(&Pitch);

			MaxRow = NumRow - 1;
			Pitch /= 4;
		}

		//读取像素
		{
			SPFPN ScaleX = (SPFPN)NumCol / (SPFPN)$m.NumMapCol;
			SPFPN ScaleY = (SPFPN)NumRow / (SPFPN)$m.NumMapRow;

			for(UNS32 iDstRow = 0; iDstRow < $m.NumMapRow; ++iDstRow){
				UNS32 iDstLine = iDstRow * $m.NumMapCol;
				for(UNS32 iDstCol = 0; iDstCol < $m.NumMapCol; ++iDstCol){
					UNS32 iSrcCol = UNS32(iDstCol * ScaleX);
					UNS32 iSrcRow = MaxRow - UNS32(iDstRow * ScaleY);
					UNS32 iSource = (iSrcRow * Pitch) + iSrcCol;
					UNS32 iDest = iDstLine + iDstCol;
					$m.psCanvPx[iDest] = prgPixel[iSource];
				}
			}

			$m.TexFrame.left = 0;
			$m.TexFrame.right = $m.NumMapCol;
			$m.TexFrame.top = 0;
			$m.TexFrame.bottom = $m.NumMapRow;
			$m.TexFrame.front = 0;
			$m.TexFrame.back = 1;
		}

		//释放空间

		lpLock->Release();
		lpBitmap->Release();
	}
	_open $VOID ApplyHeightMap(WCHAR *psFilePath, SPFPN Min, SPFPN Max){
		VUint2 Size;
		PVColor *prgPixel;
		IWICBitmapLock *lpLock;
		IWICBitmap *lpBitmap;
		UNS32 ByteCnt, Pitch, MaxRow;

		//读取文件
		{
			lpBitmap = LoadWicBitmap(GE2d::lpWicFactory, psFilePath);
			lpBitmap->Lock(P_Null, WICBitmapLockRead, &lpLock);

			lpLock->GetDataPointer(&ByteCnt, (BYTET**)&prgPixel);
			lpLock->GetSize(&Size.x, &Size.y);
			lpLock->GetStride(&Pitch);

			MaxRow = Size.y - 1;
			Pitch /= 4;
		}

		//解析高度值
		{
			SPFPN Scaling = (Max - Min) / 255.f;
			SPFPN *lprgValue = new SPFPN[Size.x * Size.y];

			for(UNS32 iRow = 0; iRow < Size.y; ++iRow){
				UNS32 iDstLine = iRow * Size.x;
				UNS32 iSrcLine = (MaxRow - iRow) * Pitch;

				for(UNS32 iCol = 0; iCol < Size.x; ++iCol){
					UNS32 iDest = iDstLine + iCol;
					UNS32 iSource = iSrcLine + iCol;
					SPFPN Sample = (SPFPN)prgPixel[iSource].g;
					lprgValue[iDest] = Min + (Sample * Scaling);
				}
			}

			$m.Ground.ApplyHeightMap(lprgValue, Size);
			delete[] lprgValue;
		}

		//释放空间

		lpLock->Release();
		lpBitmap->Release();
	}
	///纹理、地形创建
	_secr $VOID CreateTextures(DXTKUploader &Uploader, VUint2 Size, ARGB8 Color, ARGB8 Matl){
		UNS32 SliceSize = Size.x * Size.y; //单个画布大小

		//纹理画布
		{
			UNS32 Size = SliceSize * 4; //四个画布合并
			UNS32 ByteCnt = Size * sizeof(ARGB8);

			$m.lpMapCanvas = CreateUploadBuffer(GE3d::lpD3dDevice, ByteCnt, B_False);
			$m.lpMapCanvas->Map(0, P_Null, ($VOID**)&$m.psCanvPx);

			DMem_Fill($m.psCanvPx, Color, SliceSize); //基础颜色画布
			DMem_Fill($m.psCanvPx + SliceSize, Matl, SliceSize); //金属性-粗糙度画布
			DMem_Fill($m.psCanvPx + (SliceSize * 2), 0, SliceSize * 2); //纹理遮罩画布
		}

		//基础纹理,纹理遮罩
		{
			GE3d::lpD3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, Size.x, Size.y, 4, 1),
				D3D12_RESOURCE_STATE_COPY_DEST, P_Null, IID_PPV_ARGS(&$m.rgMapTex[0]));

			D3D12_SUBRESOURCE_DATA Data0 = { new ARGB8[SliceSize], 0, 0 };
			D3D12_SUBRESOURCE_DATA Data1 = { new ARGB8[SliceSize], 0, 0 };

			DMem_Fill(Data0.pData, Color, SliceSize);
			DMem_Fill(Data1.pData, Matl, SliceSize);

			Uploader.Upload($m.rgMapTex[0], 0, &Data0, 1);
			Uploader.Upload($m.rgMapTex[0], 1, &Data1, 1);
			Uploader.Transition($m.rgMapTex[0], D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
		}

		//细节纹理,投影纹理
		{
			dx::CreateDDSTextureFromFile(GE3d::lpD3dDevice, Uploader,
				L".\\texture\\Terrain\\Land.dds", &$m.rgMapTex[1]);
			dx::CreateDDSTextureFromFile(GE3d::lpD3dDevice, Uploader,
				L".\\texture\\Terrain\\Land(norm).dds", &$m.rgMapTex[2]);
			dx::CreateDDSTextureFromFile(GE3d::lpD3dDevice, Uploader,
				L".\\texture\\Terrain\\Land(disp).dds", &$m.rgMapTex[3]);
			dx::CreateDDSTextureFromFile(GE3d::lpD3dDevice, Uploader,
				L".\\texture\\Terrain\\Marquee.dds", &$m.rgMapTex[4]);
		}

		//纹理视图
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE hTexView;
			$m.lpTexViews = CreateSrvHeap(GE3d::lpD3dDevice, 6);
			hTexView = $m.lpTexViews->GetCPUDescriptorHandleForHeapStart();

			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[0], hTexView);
			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[1], hTexView.Offset(GE3d::cbSRView));
			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[1], hTexView.Offset(GE3d::cbSRView));
			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[2], hTexView.Offset(GE3d::cbSRView));
			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[3], hTexView.Offset(GE3d::cbSRView));
			dx::CreateShaderResourceView(GE3d::lpD3dDevice, $m.rgMapTex[4], hTexView.Offset(GE3d::cbSRView));
		}
	}
	_open $VOID CreateTerrain(UNS32 NumCol, UNS32 NumRow, ARGB8 Color, ARGB8 Matl){
		TStlFuture<$VOID> Thread;
		DXTKUploader Uploader(GE3d::lpD3dDevice);

		//地形纹理
		{
			UNS32 Width = NumCol * CELL_SPLIT;
			UNS32 Height = NumRow * CELL_SPLIT;

			Uploader.Begin();
			$m.CleanTexture();
			$m.CreateTextures(Uploader, VUint2{ Width, Height }, Color, Matl);
			Thread = Uploader.End(gMyGfxCmd.pCmdQueue);
		}

		//地形模型
		{
			CHeightField::NDesc LandDesc = {};
			LandDesc.bSmooth = B_True;
			LandDesc.ColCount = NumCol;
			LandDesc.RowCount = NumRow;
			LandDesc.CellWidth = CELL_SIZEH;
			LandDesc.CellDepth = CELL_SIZEH;

			$m.Ground.Release();
			$m.Ground.Initialize(GE3d::lpD3dDevice, LandDesc);
		}

		//地形特征
		{
			UNS32 CellCnt = NumCol * NumRow;

			delete[] $m.lprgCellInfo;
			$m.lprgCellInfo = new NCellInfo[CellCnt]{};

			$m.Roads.SetCount(0);
			$m.Paths.clear();

			$m.WavViews.clear();
			$m.WavViews.resize(1);

			$m.Water.Release();
			$m.Water.Initialize(GE3d::lpD3dDevice, CellCnt, sizeof(VUint2));
		}

		//场景信息
		{
			$m.AreaWidth = $m.Ground.GetWidth();
			$m.AreaDepth = $m.Ground.GetDepth();
			$m.NumMapCol = $m.Ground.ColCount * CELL_SPLIT;
			$m.NumMapRow = $m.Ground.RowCount * CELL_SPLIT;
			$m.NumMeridian = $m.Ground.ColCount + 1;
			$m.NumParallel = $m.Ground.RowCount + 1;
		}

		//视域信息
		{
			$m.EyeScopeY1 = MAX_($m.AreaWidth, $m.AreaDepth) * 0.5f;
			$m.EyeScopeY1 = MAX_($m.EyeScopeY1, EYE_POS_Y);
			$m.EyeScopeY0 = 5.f;
		}

		//相机设置
		{
			SPFPN PosY = EYE_POS_Y;
			SPFPN PosX = $m.AreaWidth * 0.5f;
			SPFPN PosZ = $m.AreaDepth * 0.5f;
			SPFPN Aspect = gMyWndW / gMyWndH;

			$m.Camera.Reset(dx::XMVectorSet(PosX, PosY, PosZ, 0.f));
			$m.Camera.Pitch(TO_RAD(EYE_SLOPE));
			$m.Camera.Perspective(TO_RAD(gMyFovAngX), Aspect, 0.1f, 1000.f);
			$m.Prjctr.LookTo(dx::g_XMNegIdentityR1, dx::g_XMIdentityR2);
		}

		//视角约束
		{
			$m.ResetFov();
			$m.RestrictView();
			$m.Camera.UpdateView(B_True);
		}

		Thread.wait();
	}
	///模式检查
	_open IBOOL InLandEditMode(){
		if($m.FuncMode < FIRST_TERR_MODE) return B_False;
		if($m.FuncMode > LAST_TERR_MODE) return B_False;
		return B_True;
	}
	_open IBOOL InLandPaintMode(){
		switch($m.FuncMode){
			case MODE_EditColor:
			case MODE_EditPattern:
			case MODE_EditMetal:
			case MODE_EditRough: return B_True;
			default: return B_False;
		}
	}
	_open IBOOL InActorSetupMode(){
		if($m.FuncMode < FIRST_ACTOR_MODE) return B_False;
		if($m.FuncMode > LAST_ACTOR_MODE) return B_False;
		return B_True;
	}
	_open IBOOL InSceneSetupMode(){
		if($m.FuncMode < FIRST_DECOR_MODE) return B_False;
		if($m.FuncMode > LAST_DECOR_MODE) return B_False;
		return B_True;
	}
	_open IBOOL InAdjustmentMode(){
		return ($m.FuncMode == MODE_AdjustActor);
	}
	///框选范围
	_secr NMarquee GetPickedCanvas(){
		FRect Frame = $m.MapFrame;
		SPFPN CellSize = CELL_SIZEH / CELL_SPLIT;

		INT32 DivL = INT32(Frame.left / CellSize);
		INT32 DivB = INT32(Frame.bottom / CellSize);
		INT32 DivR = INT32(Frame.right / CellSize) - 1;
		INT32 DivT = INT32(Frame.top / CellSize) - 1;

		UNS32 ColStart = CLAMP_(DivL, 0, $m.NumMapCol - 1);
		UNS32 RowStart = CLAMP_(DivB, 0, $m.NumMapRow - 1);
		UNS32 ColEnd = CLAMP_(DivR, 0, $m.NumMapCol - 1);
		UNS32 RowEnd = CLAMP_(DivT, 0, $m.NumMapRow - 1);

		NMarquee Marquee;
		Marquee.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		Marquee.Rect = ToRectF(Marquee.RectU);
		Marquee.Cen = RECT_CENTER(Marquee.Rect);
		Marquee.Rad = (ColEnd - ColStart + 1) * 0.5f;
		Marquee.RadSq = Marquee.Rad * Marquee.Rad;
		Marquee.bRect = ($m.BrushShape == BR_TYPE_Rect);
		Marquee.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		Marquee.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return Marquee;
	}
	_secr NMarquee GetPickedRegion(){
		INT32 MaxCol = $m.Ground.ColCount - 1;
		INT32 MaxRow = $m.Ground.RowCount - 1;

		INT32 DivL = INT32($m.MapFrame.left / CELL_SIZEH);
		INT32 DivB = INT32($m.MapFrame.bottom / CELL_SIZEH);
		INT32 DivR = INT32(($m.MapFrame.right - CELL_SIZEH) / CELL_SIZEH);
		INT32 DivT = INT32(($m.MapFrame.top - CELL_SIZEH) / CELL_SIZEH);

		UNS32 ColStart = CLAMP_(DivL, 0, MaxCol);
		UNS32 RowStart = CLAMP_(DivB, 0, MaxRow);
		UNS32 ColEnd = CLAMP_(DivR, 0, MaxCol);
		UNS32 RowEnd = CLAMP_(DivT, 0, MaxRow);

		NMarquee Marquee;
		Marquee.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		Marquee.Rect = ToRectF(Marquee.RectU);
		Marquee.Cen = RECT_CENTER(Marquee.Rect);
		Marquee.Rad = (ColEnd - ColStart + 1) * 0.5f;
		Marquee.RadSq = Marquee.Rad * Marquee.Rad;
		Marquee.bRect = ($m.BrushShape == BR_TYPE_Rect);
		Marquee.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		Marquee.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return Marquee;
	}
	_secr NMarquee GetPickedVertices(){
		INT32 MaxCol = $m.Ground.ColCount;
		INT32 MaxRow = $m.Ground.RowCount;

		INT32 DivL = INT32(($m.MapFrame.left + CELL_SIZEH) / CELL_SIZEH);
		INT32 DivB = INT32(($m.MapFrame.bottom + CELL_SIZEH) / CELL_SIZEH);
		INT32 DivR = INT32(($m.MapFrame.right - CELL_SIZEH) / CELL_SIZEH);
		INT32 DivT = INT32(($m.MapFrame.top - CELL_SIZEH) / CELL_SIZEH);

		UNS32 ColStart = CLAMP_(DivL, 0, MaxCol);
		UNS32 RowStart = CLAMP_(DivB, 0, MaxRow);
		UNS32 ColEnd = CLAMP_(DivR, 0, MaxCol);
		UNS32 RowEnd = CLAMP_(DivT, 0, MaxRow);

		NMarquee Marquee;
		Marquee.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		Marquee.Rect = $m.MapFrame;
		Marquee.Cen = RECT_CENTER($m.MapFrame);
		Marquee.Rad = $m.BrushRad0;
		Marquee.RadSq = $m.BrushRad0 * $m.BrushRad0;
		Marquee.bRect = ($m.BrushShape == BR_TYPE_Rect);
		Marquee.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		Marquee.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return Marquee;
	}
	///地形噪波计算
	_secr SPFPN CalcTerrainNoise(VFloat2 &Coord){
		SPFPN Noise = PerlinNoise(Coord.x * 0.6f, Coord.y * 0.6f);
		return $m.BrushPower * Noise * 0.2f;
	}
	///地形高度统计
	_secr INT32 CalcUniformCliff(URect &rcScope){
		UNS32 MaxCount = 0;
		UNS32 NumValue = 0;
		INT32 Result, rgValue[MAX_BRUSH * MAX_BRUSH];
		NFaceFeat *prgSource = $m.GetTerrainFeature(0);

		for(UNS32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			for(UNS32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				rgValue[NumValue++] = prgSource[iCol].Extrusion;
			prgSource += $m.Ground.ColCount;;
		}

		for(UNS32 iVal0 = 0; iVal0 < (NumValue - 1); ++iVal0){
			UNS32 Count = 0;
			INT32 Value = rgValue[iVal0];

			if(Value != INT16_MAX){
				for(UNS32 iVal1 = iVal0 + 1; iVal1 < NumValue; ++iVal1){
					if(NEAR_EQUAL(Value, rgValue[iVal1], 0.01f)){
						rgValue[iVal1] = INT16_MAX;
						Count += 1;
					}
				}
				if(Count > MaxCount){
					MaxCount = Count;
					Result = Value;
				}
			}
		}

		if(MaxCount == 0)
			return rgValue[0];
		return Result;
	}
	_secr SPFPN CalcUniformAltitude(URect &rcScope){
		UNS32 MaxCount = 0;
		UNS32 NumValue = 0;
		SPFPN Result, rgValue[MAX_BRUSH * MAX_BRUSH];
		NLandVert *prgVert = $m.Ground.prgVertex;

		for(UNS32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			for(UNS32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				rgValue[NumValue++] = prgVert[iCol].CoordY;
			prgVert += $m.NumMeridian;
		}

		for(UNS32 iVal0 = 0; iVal0 < NumValue; ++iVal0){
			UNS32 Count = 0;
			SPFPN Value = rgValue[iVal0];

			if(Value != FLT_MAX){
				for(UNS32 iVal1 = iVal0 + 1; iVal1 < NumValue; ++iVal1){
					if(NEAR_EQUAL(Value, rgValue[iVal1], 0.01f)){
						rgValue[iVal1] = FLT_MAX;
						Count += 1;
					}
				}
				if(Count > MaxCount){
					MaxCount = Count;
					Result = Value;
				}
			}
		}

		if(MaxCount != 0) return Result;
		return $m.CalcAverageAltitude(rcScope);
	}
	_secr SPFPN CalcAverageAltitude(URect &rcScope){
		NLandVert *prgVert = $m.Ground.prgVertex;

		UNS32 NumCol = rcScope.right - rcScope.left + 1;
		UNS32 NumRow = rcScope.top - rcScope.bottom + 1;

		SPFPN RowSum = 0;
		SPFPN Result = 0;

		for(UNS32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			for(UNS32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				RowSum += prgVert[iCol].CoordY;

			prgVert += $m.NumMeridian;
			Result += RowSum / NumCol;
			RowSum = 0;
		}

		return Result /= NumRow;
	}
	///占地范围
	_secr RECT GetOccupiedRegion(VFloat3 &Center, SIZE &Scale){
		SPFPN HalfW = CELL_SIZEH * Scale.cx * 0.5f;
		SPFPN HalfH = CELL_SIZEH * Scale.cy * 0.5f;

		INT32 DivL = INT32((Center.x - HalfW) / CELL_SIZEH);
		INT32 DivB = INT32((Center.z - HalfH) / CELL_SIZEH);
		INT32 DivR = INT32((Center.x + HalfW) / CELL_SIZEH) - 1;
		INT32 DivT = INT32((Center.z + HalfH) / CELL_SIZEH) - 1;

		return RECT{ DivL, DivT, DivR, DivB };
	}
	///消息处理
	_open IBOOL OnMessage(DWORD MsgCode, WPARAM Param0, LPARAM Param1){
		if(!$m.bPicking) return B_False;

		if(MsgCode == WM_LBUTTONDOWN){
			switch($m.FuncMode){
				case MODE_Uplift:
				case MODE_Steepen:
				case MODE_Uniform:{
					$m.EditLandHeight();
				} break;
				case MODE_Sharpen:{
					$m.EditLandShading();
				} break;
				case MODE_Wiring:{
					$m.EditLandEdge();
				} break;
				case MODE_EditColor:{
					$m.EditLandColor();
				} break;
				case MODE_EditPattern:{
					$m.EditLandTexture();
				} break;
				case MODE_EditMetal:
				case MODE_EditRough:{
					$m.EditLandMaterial();
				} break;
				case MODE_MakeAbyss:{
					$m.MakeAbyss();
				} break;
				case MODE_MakeCliff:{
					$m.MakeCliff();
				} break;
				case MODE_MakeWater:{
					$m.MakeWater();
				} break;
				case MODE_MakeClosed:{
					$m.MakeClosed();
				} break;
				case MODE_MakeRoad:{
					$m.MakeRoad(B_False);
				} break;
				case MODE_MakeDecor:{
					$m.AddDecoration();
				} break;
				case MODE_AddUnit:{
					$m.AddActor();
				} break;
				case MODE_AdjustActor:{
					$m.bPrepIdImg = B_True;
				} break;
			}
		} else if(MsgCode == WM_RBUTTONDOWN){
			if($m.FuncMode == MODE_MakeRoad)
				$m.MakeRoad(B_True);
		} else if(MsgCode == WM_KEYDOWN){
			if((Param0 == VK_ESCAPE) && $m.InActorSetupMode())
				$m.SetFunctionMode(MODE_AdjustActor);
			else if(Param0 == 'G')
				$m.bHideGrid = !$m.bHideGrid;
		}

		if(MsgCode < WM_MOUSEFIRST) return B_False;
		if(MsgCode > WM_MOUSELAST) return B_False;
		return B_True;
	}
	///选区边缘过滤
	_secr IBOOL TerrainInCircle(UNS32 RowId, UNS32 ColId, UNS32 FaceId){
		VFloat2 Center = RECT_CENTER($m.MapFrame);
		FCircle2 Circle(Center, $m.BrushRad0);
		FTriangle2 Tri = $m.GetTerrainFace(RowId, ColId, FaceId);
		return Intersects(Circle, Tri);
	}
	_secr IBOOL TerrainInRhombus(UNS32 RowId, UNS32 ColId, UNS32 FaceId){
		FTriangle2 Tri = $m.GetTerrainFace(RowId, ColId, FaceId);

		FVector2 Pa = (Tri.Pt0 + Tri.Pt1 + Tri.Pt2) / 3.f;
		FVector2 Pb0($m.MapFrame.left, $m.MapFrame.top);
		FVector2 Pb1($m.MapFrame.right, $m.MapFrame.bottom);
		FVector2 Pb2($m.MapFrame.right, $m.MapFrame.top);
		FVector2 Pb3($m.MapFrame.left, $m.MapFrame.bottom);

		SPFPN Limit = sqrt($m.BrushRad0 * $m.BrushRad0 * 2.f) * 0.5f;
		SPFPN Dist0 = VtoF_(dx::XMVector3LinePointDistance(Pb0, Pb1, Pa));
		SPFPN Dist1 = VtoF_(dx::XMVector3LinePointDistance(Pb2, Pb3, Pa));

		if(Dist0 > Limit) return B_False;
		if(Dist1 > Limit) return B_False;
		return B_True;
	}
	///焦点吸附
	_secr VFloat3 GetSnapedLocation(SIZE &Alignment){
		SPFPN PosX = $m.SelPoint.x - fmod($m.SelPoint.x, CELL_SIZEH);
		if((Alignment.cx % 2) != 0) PosX += CELL_SIZEH / 2.f;
		SPFPN PosZ = $m.SelPoint.z - fmod($m.SelPoint.z, CELL_SIZEH);
		if((Alignment.cy % 2) != 0) PosZ += CELL_SIZEH / 2.f;
		SPFPN PosY = $m.Ground.SampleHeight(PosX, PosZ);
		return VFloat3(PosX, PosY, PosZ);
	}
	///地形顶点
	_secr VFloat2 GetTerrainVertex(UNS32 RowId, UNS32 ColId){
		return VFloat2(ColId * CELL_SIZEH, RowId * CELL_SIZEH);
	}
	_secr FTriangle2 GetTerrainFace(UNS32 RowId, UNS32 ColId, UNS32 FaceId){
		IBOOL bEvenId = ((FaceId % 2) == 0);
		IBOOL bBslash = $m.GetTerrainFeature(bEvenId ? FaceId : (FaceId - 1))->bBslash;

		VUint2 *pVerts;
		VUint2 rgVerts[4][3] = {
			{ { 0, 0 }, { 0, 1 }, { 1, 1 } },
			{ { 0, 0 }, { 1, 1 }, { 1, 0 } },
			{ { 0, 0 }, { 0, 1 }, { 1, 0 } },
			{ { 0, 1 }, { 1, 1 }, { 1, 0 } }
		};

		if(bEvenId){
			if(bBslash) pVerts = rgVerts[2];
			else pVerts = rgVerts[0];
		} else{
			if(bBslash) pVerts = rgVerts[3];
			else pVerts = rgVerts[1];
		}

		VFloat2 P0 = $m.GetTerrainVertex(RowId + pVerts[0].y, ColId + pVerts[0].x);
		VFloat2 P1 = $m.GetTerrainVertex(RowId + pVerts[1].y, ColId + pVerts[1].x);
		VFloat2 P2 = $m.GetTerrainVertex(RowId + pVerts[2].y, ColId + pVerts[2].x);

		return FTriangle2(P0, P1, P2);
	}
	///地形特征
	_secr NFaceFeat *GetTerrainFeature(UNS32 FaceId){
		return (NFaceFeat*)&$m.Ground.prgTriFeat[FaceId];
	}
	///资源获取
	_secr D3D12_GPU_VIRTUAL_ADDRESS GetToneMaterial(UNS32 Index){
		return $m.hToneMatls + (Index * sizeof(GMtlBase));
	}
	_secr D3D12_GPU_VIRTUAL_ADDRESS GetWaterMaterial(UNS32 Index){
		return $m.hWavMatls + (Index * sizeof(GMtlLite));
	}
	_secr D3D12_GPU_VIRTUAL_ADDRESS GetLandmarkProjection(UNS32 Index){
		return $m.hMarkProjs + (Index * sizeof(MFlt4x4));
	}

#undef EYE_POS_Y
#undef EYE_SLOPE
#undef TILE_SIZE
#undef CELL_SIZEH
#undef CELL_SIZEV
#undef CELL_SPLIT
#undef MAX_BRUSH
#undef MAX_PATH_PT
#undef MIN_UNIT_ID
#undef MIN_STRUCT_ID
};