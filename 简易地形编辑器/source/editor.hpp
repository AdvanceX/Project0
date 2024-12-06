class MGEditor: public IAppBase{
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

	_secr using LANDVERT = CHeightField::VERTEX;
	//////
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
	_secr struct SELBOX{
		union{
			RECTU RectU;
			struct{
				USINT L;
				USINT T;
				USINT R;
				USINT B;
			};
		};
		RECTF Rect;
		POINT2 Cen;
		SPFPN Rad;
		SPFPN RadSq;
		IBOOL bRhom : 1;
		IBOOL bRect : 1;
		IBOOL bCir : 1;
	};
	_secr struct PREVIEW{
		SIZE Scale;
		USINT PsoId;
		USINT MdlId;
		VECTOR3 Coord;
		CMeshAssy *pModel;
	};
	_secr struct CELLINFO{
		USINT iHolder = 0;
		USINT iWavView = 0;
	};
	_secr struct WAVEVIEW{
		SPFPN Level;
		USINT ClassId;
		USINT NumCell;
		USINT CellBase;
	};
	_secr struct WAVECLASS{
		SPFPN rgWavDirX[4];
		SPFPN rgWavDirZ[4];
		SPFPN rgWavPhase[4];
		SPFPN rgWavSlope[4];
		SPFPN rgWavAmplit[4];
		SPFPN rgWavLength[4];
	};

	_secr DWORD vHeader;
	///编辑状态
	_open BYTET FuncMode;
	_secr BYTET LayupDir;
	_secr BOOL8 bFirstOp: 4;
	_secr BOOL8 bPicking: 4;
	_secr BOOL8 bConflict: 4;
	_secr BOOL8 bPrepIdImg: 2;
	_secr BOOL8 bPickObjId: 2;
	///场景信息
	_secr SPFPN AreaWidth;
	_secr SPFPN AreaDepth;
	_secr UHALF NumMapPxX;
	_secr UHALF NumMapPxY;
	_secr UHALF NumMeridian; //经线
	_secr UHALF NumParallel; //纬线
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
	_open UHALF BrushMapId;
	_open ARGB8 BrushColor;
	_secr SPFPN BrushPower;
	_secr SPFPN BrushRad1;
	_secr SPFPN BrushRad0;
	///水域信息
	_secr SPFPN WavLevel;
	_secr UHALF iWavView;
	_secr UHALF iWavClass;
	_secr WAVECLASS rgWavClass[8];
	///选区记录
	_secr D3D12BOX TexFrame;
	_secr D2D1RECTF MapFrame;
	_secr D2D1RECTU CpyFrame;
	_secr D2D1RECTU MenuFrame;
	///杂项
	_secr VECTOR3 SelPoint;
	_secr PREVIEW Preview;
	_secr CELLINFO *lprgCellInfo;
	//资源句柄
	_secr COLOR32 *psCanvBuf;
	_secr MATBASE *prgLedMatl;
	_secr MATLITE *prgWavMatl;
	_secr MATRIX4 *prgTexProj;
	_secr HD3D12GPURES hLedMatls;
	_secr HD3D12GPURES hWavMatls;
	_secr HD3D12GPURES hTexProjs;
	///摄影机
	_secr CCamera Camera;
	_secr CCamera Prjctr;
	///路径&水域
	_secr CSeqList Paths;
	_secr CSeqList WavViews;
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
	_secr CHeightField Ground;
	_secr CUploadBuff Chunks;
	_secr CUploadBuff ResMisc;
	///地形纹理
	_secr ID3DResource *rgMapTex[5];
	_secr ID3DResource *lpMapCanvas;
	_secr ID3DViewHeap *lpTexViews;
	//////
	_secr DWORD vFooter;

	_open ~MGEditor(){
		$m.Finalize();
	}
	_open MGEditor(){
		$m.Initialize();
	}
	//////
	_secr $VOID Initialize(){
		//归零
		{
			BStr_Zero(&$m.vHeader, (BYTET*)&$m.vFooter - (BYTET*)&$m.vHeader);
		}

		//视域信息
		{
			SPFPN Alpha = EYE_SLOPE - (gMyFovAngY * 0.5f);
			SPFPN Gamma = 180.f - gMyFovAngY - Alpha;
			SPFPN Theta = 90.f - (gMyFovAngX * 0.5f);

			$m.TanAlpha = Tan(TO_RAD(Alpha));
			$m.TanGamma = Tan(TO_RAD(Gamma));
			$m.TanTheta = Tan(TO_RAD(Theta)) / $m.TanAlpha;
		}

		//地形模型
		{
			$m.WavViews.Init(8, sizeof(WAVEVIEW));
			$m.Paths.Init(16, sizeof(VECTOR3));
			$m.Roads.Initialize(GE3d::lpD3dDevice, MAX_PATH_PT, sizeof(POINT3));
			$m.CreateTerrain(128, 128, 0xFFE4A866, 0x800000);
		}

		//地形着色器
		{
			$m.RoadShader.Initialize(GE3d::lpD3dDevice, VFB_Coord, ASSET_TYPE_Spline);
			$m.LandShader.Initialize(GE3d::lpD3dDevice, 0x0, ASSET_TYPE_Terrain | ASSET_TYPE_ColorMap | ASSET_TYPE_NormMap);
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
			const USINT NumLedMatl = 4;
			const USINT NumWavMatl = 8;

			USINT cbLedMatls = sizeof(MATBASE) * NumLedMatl;
			USINT cbWavMatls = sizeof(MATLITE) * NumWavMatl;
			USINT cbTexProjs = sizeof(MATRIX4) * gMyGfxCmd.MaxFrame;
			USINT cbBuffer = cbLedMatls + cbWavMatls + cbTexProjs;

			$m.ResMisc.Initialize(GE3d::lpD3dDevice, cbBuffer, 1, B_True);
			$m.ResMisc.EnableUpdate(B_True, B_True);

			$m.hLedMatls = $m.ResMisc.GetVram(0);
			$m.hWavMatls = $m.hLedMatls + cbLedMatls;
			$m.hTexProjs = $m.hWavMatls + cbWavMatls;

			$m.prgLedMatl = (MATBASE*)$m.ResMisc.GetMemory(0);
			$m.prgWavMatl = (MATLITE*)($m.prgLedMatl + NumLedMatl);
			$m.prgTexProj = (MATRIX4*)($m.prgWavMatl + NumWavMatl);

			$m.prgLedMatl[0].Opacity = 1.f;
			$m.prgLedMatl[1].Opacity = 1.f;
			$m.prgLedMatl[2].Opacity = 1.f;
			$m.prgLedMatl[3].Opacity = 1.f;

			$m.prgLedMatl[0].TexAttrs[0] = ASSET_TYPE_ColorMap;
			$m.prgLedMatl[1].TexAttrs[0] = ASSET_TYPE_ColorMap;
			$m.prgLedMatl[2].TexAttrs[0] = ASSET_TYPE_ColorMap;
			$m.prgLedMatl[3].TexAttrs[0] = ASSET_TYPE_ColorMap;

			$m.prgLedMatl[0].Emission = VECTOR3(0.4f, 0.f, 0.f);   //加红
			$m.prgLedMatl[1].Emission = VECTOR3(0.f, 0.4f, 0.f);   //加绿
			$m.prgLedMatl[3].Emission = VECTOR3(0.4f, 0.4f, 0.4f); //加白

			$m.prgLedMatl[0].Albedo = VECTOR3(1.f, 0.5f, 0.5f);  //加红
			$m.prgLedMatl[1].Albedo = VECTOR3(0.5f, 1.f, 0.5f);  //加绿
			$m.prgLedMatl[2].Albedo = VECTOR3(0.5f, 0.5f, 0.5f); //加黑
			$m.prgLedMatl[3].Albedo = VECTOR3(1.f, 1.f, 1.f);    //加白

			$m.prgWavMatl[0].Albedo = VECTOR4(0.40f, 0.60f, 0.80f, 1.f);
			$m.prgWavMatl[1].Albedo = VECTOR4(0.32f, 0.62f, 0.52f, 1.f);
			$m.prgWavMatl[2].Albedo = VECTOR4(0.43f, 0.83f, 0.83f, 1.f);
			$m.prgWavMatl[3].Albedo = VECTOR4(0.44f, 0.64f, 0.24f, 1.f);
			$m.prgWavMatl[4].Albedo = VECTOR4(0.45f, 0.35f, 0.75f, 1.f);
			$m.prgWavMatl[5].Albedo = VECTOR4(0.16f, 0.66f, 0.56f, 1.f);
			$m.prgWavMatl[6].Albedo = VECTOR4(0.47f, 0.17f, 0.87f, 1.f);
			$m.prgWavMatl[7].Albedo = VECTOR4(0.88f, 0.68f, 0.88f, 1.f);
		}

		//水波参数
		{
			VECTR3X WavDir;
			WAVECLASS *prgClass = $m.rgWavClass;

			for(USINT iExe = 0; iExe < 4; ++iExe){
				WavDir.x = RandReal(-1.f, 1.f);
				WavDir.z = RandReal(-1.f, 1.f);
				WavDir.Normalize();

				prgClass[0].rgWavDirX[iExe] = WavDir.x;
				prgClass[0].rgWavDirZ[iExe] = WavDir.z;
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
			$m.SetBrushSize(WtoI(pGuiCtrl->GetValue()));

			pGuiCtrl = mgGetWidget(ID_LBL_BrushPower);
			$m.SetBrushPower((SPFPN)WtoD(pGuiCtrl->GetValue()));

			pGuiCtrl = mgGetWidget(ID_RDO_RectBrush);
			$m.SetBrushShape(pGuiCtrl->IsChecked() ? BR_TYPE_Rect : BR_TYPE_Circle);

			pGuiCtrl = mgGetWidget(ID_GRP_SculptScape);
			$m.SetFunctionMode(((CBtnGroup*)pGuiCtrl)->GetChoice());

			pGuiCtrl = mgGetWidget(ID_LST_BrushColor);
			$m.SetBrushColor(((CListBox*)pGuiCtrl)->GetItem(1)->Color);
		}

		//等待GPU

		gMyGfxCmd.WaitForGpu();
		gMyWndEx.rcScissor.left = (USINT)gMyPanelW;
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
		gMyRender.ClearWindow(gMyWndEx, COLOR128(0.f, 0.f, 0.f, 1.0));
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
		if($m.Paths.GetSize() == 0) return;

		GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.f, 1.f, 0.f));

		MMATRIX matViewProj = $m.Camera.GetViewProjMat();
		MMATRIX matViwprt = MatFromViewport(*(VIEWPORT*)&gMyWndEx.Viewport);
		MMATRIX matToScrn = matViewProj * matViwprt;

		VECTR3X *pPoint = (VECTR3X*)$m.Paths.GetFront();
		VECTR3X *pEndPoint = pPoint + $m.Paths.GetSize();
		POINT3 Point0(SPFP_NAN, 0.f, 0.f);

		for(; pPoint != pEndPoint; ++pPoint){
			if(pPoint->x == SPFP_NAN){
				Point0 = *pPoint;
				continue;
			}

			VECTR3X Point1 = Vec3_XformCoord(Pck_V3(pPoint), matToScrn);
			RECTF rcPoint = MAKE_SQUARE(Point1, 5.f, 5.f);
			GE2d::lpD2dDevCtx->FillRectangle(rcPoint, GE2d::lpPureBrush);

			if(Point0.x != SPFP_NAN){
				D2D1POINTF P0 = { Point0.x, Point0.y };
				D2D1POINTF P1 = { Point1.x, Point1.y };
				GE2d::lpD2dDevCtx->DrawLine(P0, P1, GE2d::lpPureBrush, 2);
			}

			Point0 = Point1;
		}
	}
	_secr $VOID PaintRoads(){
		if($m.Roads.GetCount() > 0){
			SDSPLINE RoadDesc = {};
			RoadDesc.Pitch = 2.f;
			RoadDesc.Width = 2.f;
			RoadDesc.WidthB = 0.3f;
			RoadDesc.Height = 0.3f;
			RoadDesc.bShell = B_True;
			RoadDesc.UpDir = VECTR3X::Up;

			gMyRender.SetPipelineState($m.RoadPso);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
			gMyRender.SetSplineAttrs(&RoadDesc);
			gMyRender.DrawSpline($m.Roads, gMyMtlLib.GetTextureSrvById(StrHash32("Highway")));
		}
	}
	_secr $VOID PaintWater(){
		if($m.Water.GetCount() == 0) return;

		SDSURFACE Param = {};
		WAVECLASS *pClass;
		D3D12VBVIEW VtxView;
		HD3D12GPURES hMtlBase;
		D3D12GPUVIEWPTR hTexViews = {};

		WAVEVIEW *pView = (WAVEVIEW*)$m.WavViews.Get(1);
		WAVEVIEW *pEndView = pView + $m.WavViews.GetSize() - 1;

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
			VtxView.SizeInBytes = pView->NumCell * sizeof(POINT2U);

			DStr_Move(&Param, pClass, DW_CNT(WAVECLASS));

			gMyRender.SetSurfaceAttrs(&Param);
			gMyRender.DrawSurface(VtxView, hTexViews, hMtlBase);

			VtxView.BufferLocation += VtxView.SizeInBytes;
		}

		if($m.FuncMode == MODE_MakeWater){
			pView = (WAVEVIEW*)$m.WavViews.Get($m.iWavView);
			if(pView->NumCell == 0) return;

			VtxView = $m.Water.GetSrv();
			VtxView.SizeInBytes = pView->NumCell * sizeof(POINT2U);
			VtxView.BufferLocation += pView->CellBase * sizeof(POINT2U);

			Param.WireColor = 0xFF00FF00;
			Param.LowerLeftY = pView->Level;

			pClass = $m.rgWavClass + pView->ClassId;
			DStr_Move(&Param, pClass, DW_CNT(WAVECLASS));

			gMyRender.SetPipelineState($m.WirePso1);
			gMyRender.SetResourceViewHeap($m.lpTexViews);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
			gMyRender.SetSurfaceAttrs(&Param);
			gMyRender.DrawSurface(VtxView, hTexViews, 0);
		}
	}
	_secr $VOID PaintGround(){
		HD3D12GPURES hLandFeats = $m.Chunks.GetVram(0);
		HD3D12GPURES hTexProj = $m.GetTexProjection(gMyGfxCmd.CurFrame);
		D3D12GPUVIEWPTR hTexView = $m.lpTexViews->GetGPUDescriptorHandleForHeapStart();

		IBOOL bTerrEdit = $m.InLandEditMode();
		USINT NumSlide = ($m.bPicking && bTerrEdit) ? 1 : 0;

		SDTERRAIN LandDesc = {};
		//LandDesc.BumpScale = 0.05f;
		LandDesc.ResAttr = MAKE_XRGB(2, NumSlide, 0);
		LandDesc.SlideIds0 = MAKE_XRGB(0, 0, $m.BrushShape);
		LandDesc.LowerLeftX = $m.Ground.LowerLeftX;
		LandDesc.LowerLeftZ = $m.Ground.LowerLeftZ;
		LandDesc.RowCount = $m.Ground.RowCount;
		LandDesc.ColCount = $m.Ground.ColCount;
		LandDesc.AreaWidth = $m.AreaWidth;
		LandDesc.AreaDepth = $m.AreaDepth;
		LandDesc.TileWidth = TILE_SIZE;
		LandDesc.TileDepth = TILE_SIZE;

		//实体
		gMyRender.SetPipelineState($m.LandPso);
		gMyRender.SetResourceViewHeap($m.lpTexViews);
		gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
		gMyRender.SetTerrainAttrs(&LandDesc);
		gMyRender.DrawTerrain($m.Ground, hTexView, hLandFeats, hTexProj);
		//线框
		LandDesc.CurColor = 0xFFFFFF;
		gMyRender.SetPipelineState($m.WirePso0);
		gMyRender.SetResourceViewHeap($m.lpTexViews);
		gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());
		gMyRender.SetTerrainAttrs(&LandDesc);
		gMyRender.DrawTerrain($m.Ground, hTexView, hLandFeats, hTexProj);
	}
	_secr $VOID PaintObjects(){
		USINT NumAnimal = gMyAnimals.GetSize();
		USINT NumStruct = gMyStructs.GetSize();

		CAnimal *prgAnimal = (CAnimal*)gMyAnimals.GetFront();
		CStructure *prgStruct = (CStructure*)gMyStructs.GetFront();

		if(NumAnimal != 0){
			gMyRender.SetPipelineState(gMyArrPso[ID_PSO_Animal]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			for(USINT iObj = 0; iObj < NumAnimal; ++iObj){
				CAnimal *pObject = prgAnimal + iObj;

				USINT iModel = gMyArrAnimalCls[pObject->iOrgProps].iModel;
				CMeshAssy *pModel = mgGetModel(0, iModel);

				MVECTOR Coord = Pck_V3(&pObject->Position);
				MMATRIX matWorld = Mat_TranslateV(Coord);

				gMyRender.SetModelAttrs(matWorld, P_Null, MIN_UNIT_ID + iObj);
				gMyRender.DrawModels(pModel->lpFirst, &gMyMtlLib);
			}
		}

		if(NumStruct != 0){
			gMyRender.SetPipelineState(gMyArrPso[ID_PSO_Struct]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			for(USINT iObj = 0; iObj < NumStruct; ++iObj){
				CStructure *pObject = prgStruct + iObj;

				USINT iModel = gMyArrStructCls[pObject->iOrgProps].iModel;
				CMeshAssy *pModel = mgGetModel(0, iModel);

				MVECTOR Coord = Pck_V3(&pObject->Position);
				MMATRIX matWorld = Mat_TranslateV(Coord);

				gMyRender.SetModelAttrs(matWorld, P_Null, MIN_STRUCT_ID + iObj);
				gMyRender.DrawModels(pModel->lpFirst, &gMyMtlLib);
			}
		}

		if($m.bPicking && $m.Preview.pModel){
			gMyRender.SetPipelineState(gMyArrPso[$m.Preview.PsoId]);
			gMyRender.SetResourceViewHeap(gMyMtlLib.lpTexSrvs);
			gMyRender.SetLights(gMyLitLib.GetVram(0), gMyLitLib.GetCount());

			MVECTOR Coord = Pck_V3(&$m.Preview.Coord);
			MMATRIX matWorld = Mat_TranslateV(Coord);
			CMesh3D *pModel = $m.Preview.pModel->lpFirst;
			HD3D12GPURES hMatl = $m.GetLedMaterial($m.bConflict ? 0 : 1);

			gMyRender.SetModelAttrs(matWorld);
			gMyRender.DrawModels(pModel, &gMyMtlLib, hMatl);
		}
	}
	_secr $VOID PaintWaterPanel(){
		if($m.FuncMode != MODE_MakeWater) return;

		IBOOL bPressed;
		IM1VEC2 BtnSizeL(34.f, 34.f);
		IM1VEC2 BtnSizeS(30.f, 22.f);
		IM1VEC2 WndSize(164.f, 206.f);
		IM1VEC2 WndPos(gMyWndW - WndSize.x, 0.f);
		MATLITE *prgMatl = $m.prgWavMatl;
		WAVEVIEW *pWavView = (WAVEVIEW*)$m.WavViews.Get($m.iWavView);

		imgui::SetNextWindowPos(WndPos);
		imgui::SetNextWindowSize(WndSize);
		imgui::Begin(u8"Water");

		imgui::Text(u8"Height:");
		imgui::InputFloat("", &pWavView->Level, 0.1f, 0.1f, "%.1f");

		imgui::Text(u8"Style:");
		bPressed = imgui::ColorButton("Style0", *(IM1VEC4*)&prgMatl[0].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 0;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style1", *(IM1VEC4*)&prgMatl[1].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 1;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style2", *(IM1VEC4*)&prgMatl[2].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 2;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style3", *(IM1VEC4*)&prgMatl[3].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 3;
		bPressed = imgui::ColorButton("Style4", *(IM1VEC4*)&prgMatl[4].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 4;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style5", *(IM1VEC4*)&prgMatl[5].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 5;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style6", *(IM1VEC4*)&prgMatl[6].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 6;
		imgui::SameLine(0.f, 4.f);
		bPressed = imgui::ColorButton("Style7", *(IM1VEC4*)&prgMatl[7].Albedo, 0L, BtnSizeL);
		if(bPressed) pWavView->ClassId = 7;

		imgui::Spacing();
		imgui::Separator();
		imgui::Spacing();
		imgui::Indent((WndSize.x - BtnSizeS.x - 8.f) * 0.5f);
		bPressed = imgui::Button(u8"OK", BtnSizeS);

		WndPos = imgui::GetWindowPos();
		WndSize = imgui::GetWindowSize();

		imgui::End();

		$m.MenuFrame.left = (USINT)WndPos.x;
		$m.MenuFrame.top = (USINT)WndPos.y;
		$m.MenuFrame.right = USINT(WndPos.x + WndSize.x);
		$m.MenuFrame.bottom = USINT(WndPos.y + WndSize.y);

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
		VECTOR3 *pEyePos = (VECTOR3*)&$m.Camera.GetPosition();

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
			*pPhase = Mod(*pPhase, 2.f * SIMD_PI);
		}

		///数据同步

		WAVECLASS *pSurf0 = &$m.rgWavClass[0];
		WAVECLASS *pSurf = pSurf0 + 1;

		for(USINT iWav = 1; iWav < 8; ++iWav){
			pSurf->rgWavPhase[0] = pSurf0->rgWavPhase[0];
			pSurf->rgWavPhase[1] = pSurf0->rgWavPhase[1];
			pSurf->rgWavPhase[2] = pSurf0->rgWavPhase[2];
			pSurf->rgWavPhase[3] = pSurf0->rgWavPhase[3];
			pSurf++;
		}
	}
	_secr $VOID UpdateBrush(){
		///尺寸更新
		{
			INT32 Delta = 0;

			if(GEInp::JustPressed(DIK_O))
				Delta = -1;
			else if(GEInp::JustPressed(DIK_P))
				Delta = 1;

			if(Delta != 0){
				CLabel *pCtrl = mgGetLabel(ID_LBL_BrushSize);
				INT32 Value = WtoI(pCtrl->lpsText) + Delta;
				Value = CLAMP_(Value, 1, MAX_BRUSH);
				Wnprintf(pCtrl->lpsText, 4, L"%d", Value);
				$m.SetBrushSize(Value);
			}
		}

		///力度更新
		{
			SPFPN Delta = 0.f;

			if(GEInp::JustPressed(DIK_K))
				Delta = -0.1f;
			else if(GEInp::JustPressed(DIK_L))
				Delta = 0.1f;

			if(Delta != 0.f){
				CLabel *pCtrl = mgGetLabel(ID_LBL_BrushPower);
				SPFPN Value = (SPFPN)WtoD(pCtrl->lpsText) + Delta;
				Value = CLAMP_(Value, 0.f, 1.f);
				Wnprintf(pCtrl->lpsText, 8, L"%g", Value);
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
		VECTOR3 *pCoord = &$m.Preview.Coord;

		if($m.Preview.PsoId == ID_PSO_Struct){
			*pCoord = $m.GetSnapedFocus(*pScale);

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

		POINT2 Cursor(CursorX, CursorY);
		if(PointInRect(Cursor, ToRectF(MenuFrame))) return;

		SPFPN RayDist;
		VECTOR2 WndSize = { gMyWndW, gMyWndH };
		RAY ScrRay = RayFromScreen(Cursor, WndSize, $m.Camera.GetProjMatrix(), $m.Camera.GetViewMatrix());

		$m.bPicking = $m.Ground.IntersectRay(ScrRay, &RayDist);
		if($m.bPicking)	$m.SelPoint = ScrRay.position + (ScrRay.direction * RayDist);
	}
	_secr $VOID UpdateLandPicking(){
		if(!$m.bPicking) return;

		SPFPN BrushRad = $m.BrushRad0;
		SPFPN CellSize = CELL_SIZEH;
		SPFPN AlignF = Mod(BrushRad, CellSize);

		if($m.InLandPaintMode()){
			BrushRad /= CELL_SPLIT;
			CellSize /= CELL_SPLIT;
		}

		$m.SelPoint.x -= Mod($m.SelPoint.x, CellSize);
		$m.SelPoint.z -= Mod($m.SelPoint.z, CellSize);
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
		$m.Prjctr.Locate(Pck_V3(&$m.SelPoint));
		$m.Prjctr.UpdateView(B_False);
		$m.Prjctr.GetViewProjTexMat($m.prgTexProj[gMyGfxCmd.CurFrame]);
	}
	_secr $VOID UpdateBodyPicking(){
		USINT iPickedPx = (gMyWndEx.Width * gCursor->y) + gCursor->x;
		USINT iPickedObj = gMyWndEx.prgIdPixel[iPickedPx];
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

		USINT NumCell = 0;
		USINT NumView = $m.WavViews.GetSize();

		WAVEVIEW *pView = (WAVEVIEW*)$m.WavViews.Get(1);
		WAVEVIEW *pEndView = pView + NumView - 1;

		for(; pView != pEndView; ++pView){
			if(pView->NumCell != 0){
				pView->CellBase = NumCell;
				NumCell += pView->NumCell;
				pView->NumCell = 0;
			}
		}

		//////

		USINT MaxCol = $m.Ground.ColCount;
		USINT MaxRow = $m.Ground.RowCount;

		POINT2U *pVert;
		POINT2U *prgVert = (POINT2U*)$m.Water.GetMemory(0);
		WAVEVIEW *prgView = (WAVEVIEW*)$m.WavViews.GetFront();
		CELLINFO *prgCell = $m.lprgCellInfo;

		for(USINT iRow = 0; iRow < MaxRow; ++iRow){
			for(USINT iCol = 0; iCol < MaxCol; ++iCol){
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
		/*USINT MaxCol = $m.Ground.ColCount;
		USINT MaxRow = $m.Ground.RowCount;

		SELBOX SelBox = $m.GetPickedRegion();
		SDTERRFEAT *prgFeat = (SDTERRFEAT*)$m.Chunks.GetMemory(0);

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * MaxCol;

			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				SDTERRFEAT *pFeat0 = prgFeat + (iLine + iCol)*2;
				SDTERRFEAT *pFeat1 = pFeat0 + 1;

				SDTERRFEAT *pFeatL = pFeat0 - 1;
				SDTERRFEAT *pFeatR = pFeat1 + 1;
				SDTERRFEAT *pFeatF = pFeat0 - (MaxCol * 2);
				SDTERRFEAT *pFeatB = pFeat0 + (MaxCol * 2);

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
		VECTOR3 *pEyePos = (VECTOR3*)&$m.Camera.GetPosition();

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
			USINT DstSize, DstLeft, DstTop;
			D3D12TEXLOCATION Dest, Source;

			//计算布局

			DstTop = $m.TexFrame.top;
			DstLeft = $m.TexFrame.left;
			DstSize = $m.NumMapPxX * $m.NumMapPxY * sizeof(ARGB8);

			Dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			Dest.pResource = $m.rgMapTex[0];

			Source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			Source.pResource = $m.lpMapCanvas;
			Source.PlacedFootprint.Footprint.Depth = 1;
			Source.PlacedFootprint.Footprint.Width = $m.NumMapPxX;
			Source.PlacedFootprint.Footprint.Height = $m.NumMapPxY;
			Source.PlacedFootprint.Footprint.RowPitch = $m.NumMapPxX * sizeof(ARGB8);
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
			for(USINT iTex = 0; iTex < ARR_LEN($m.rgMapTex); ++iTex){
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
		USINT NumView = $m.WavViews.GetSize();
		WAVEVIEW *pView = (WAVEVIEW*)$m.WavViews.Get(1);

		for($m.iWavView = 1; $m.iWavView < NumView; ++$m.iWavView){
			if(pView->NumCell == 0) break;
			pView++;
		}

		if($m.iWavView == NumView)
			pView = (WAVEVIEW*)$m.WavViews.New();

		pView->NumCell = 0;
		pView->CellBase = 0;
		pView->Level = $m.WavLevel;
		pView->ClassId = $m.iWavClass;
	}
	_secr $VOID FitLandWiring(){
		//USINT MaxCol = $m.Ground.ColCount;
		//SELBOX SelBox = $m.GetPickedRegion();
		//RGB10 *prgFeat = $m.Ground.prgNormal;

		//if(SelBox.bCir){  //圆形
		//	POINT2U CenU = RECT_CENTER(SelBox.RectU);
		//	POINT3 Center = RectCenter($m.MapFrame);
		//	SPFPN RadSq = $m.BrushRad0 * $m.BrushRad0;

		//	for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
		//		for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
		//			USINT iFace0 = (MaxCol*iRow + iCol) * 2;
		//			USINT iFace1 = iFace0 + 1;

		//			USINT Count = 0;
		//			POINT3 rgVert[4];

		//			rgVert[0] = { CELL_SIZEH * iCol, 0.f, CELL_SIZEH * iRow };
		//			rgVert[1] = { CELL_SIZEH + rgVert[0].x, 0.f, rgVert[0].z };
		//			rgVert[2] = { rgVert[0].x, 0.f, CELL_SIZEH + rgVert[0].z };
		//			rgVert[3] = { rgVert[1].x, 0.f, rgVert[2].z };

		//			for(USINT iVtx = 0; iVtx < 4; ++iVtx){
		//				if(::PointInCircle(rgVert[iVtx], Center, RadSq))
		//					Count++;
		//			}
		//			if((Count == 0) || (Count == 4))
		//				continue;

		//			if(iCol <= CenU.x){
		//				if(iRow <= CenU.y){
		//					prgFeat[iFace0] |= CHeightField::FACE_FLAG_Bslash;
		//					prgFeat[iFace1] |= CHeightField::FACE_FLAG_Bslash;
		//				} else{
		//					prgFeat[iFace0] &= ~CHeightField::FACE_FLAG_Bslash;
		//					prgFeat[iFace1] &= ~CHeightField::FACE_FLAG_Bslash;
		//				}
		//			} else{
		//				if(iRow <= CenU.y){
		//					prgFeat[iFace0] &= ~CHeightField::FACE_FLAG_Bslash;
		//					prgFeat[iFace1] &= ~CHeightField::FACE_FLAG_Bslash;
		//				} else{
		//					prgFeat[iFace0] |= CHeightField::FACE_FLAG_Bslash;
		//					prgFeat[iFace1] |= CHeightField::FACE_FLAG_Bslash;
		//				}
		//			}
		//		}
		//	}
		//} else if(SelBox.bRect){ //矩形
		//	
		//} else if(SelBox.bRhom){ //菱形
		//	POINT2U Center = RECT_CENTER(SelBox.RectU);

		//	INT32 iRow0 = Center.y + 1;
		//	INT32 iRow1 = Center.y;
		//	for(INT32 iCol = SelBox.L; iCol <= (INT32)Center.x; ++iCol){
		//		{
		//			USINT iFace0 = (MaxCol*iRow0 + iCol) * 2;
		//			USINT iFace1 = iFace0 + 1;

		//			prgFeat[iFace0] &= ~CHeightField::FACE_FLAG_Bslash;
		//			prgFeat[iFace1] &= ~CHeightField::FACE_FLAG_Bslash;
		//		}
		//		{
		//			USINT iFace0 = (MaxCol*iRow1 + iCol) * 2;
		//			USINT iFace1 = iFace0 + 1;

		//			prgFeat[iFace0] |= CHeightField::FACE_FLAG_Bslash;
		//			prgFeat[iFace1] |= CHeightField::FACE_FLAG_Bslash;
		//		}
		//		iRow0++;
		//		iRow1--;
		//	}

		//	INT32 iRow2 = SelBox.T;
		//	INT32 iRow3 = SelBox.B;
		//	for(INT32 iCol = Center.x + 1; iCol <= (INT32)SelBox.R; ++iCol){
		//		{
		//			USINT iFace0 = (MaxCol*iRow2 + iCol) * 2;
		//			USINT iFace1 = iFace0 + 1;

		//			prgFeat[iFace0] |= CHeightField::FACE_FLAG_Bslash;
		//			prgFeat[iFace1] |= CHeightField::FACE_FLAG_Bslash;
		//		}
		//		{
		//			USINT iFace0 = (MaxCol*iRow3 + iCol) * 2;
		//			USINT iFace1 = iFace0 + 1;

		//			prgFeat[iFace0] &= ~CHeightField::FACE_FLAG_Bslash;
		//			prgFeat[iFace1] &= ~CHeightField::FACE_FLAG_Bslash;
		//		}
		//		iRow2--;
		//		iRow3++;
		//	}
		//}

		//$m.Ground.UpdateFlatNormals($m.MapFrame);
		//$m.Ground.UpdateSmoothNormals($m.MapFrame);
	}
	///装饰、角色添加
	_secr $VOID AddActor(){
		if(!$m.bConflict){
			if($m.Preview.PsoId == ID_PSO_Animal){
				ANIMALCLASS *pClass = mgGetAnimalClass($m.Preview.MdlId);
				CAnimal *pUnit = (CAnimal*)gMyAnimals.New();
				USINT ClassId = USINT(pClass - gMyArrAnimalCls);
				pUnit->Initialize(ClassId, *pClass, $m.Preview.Coord);
				// end if
			} else if($m.Preview.PsoId == ID_PSO_Struct){
				STRUCTCLASS *pClass = mgGetStructClass($m.Preview.MdlId);
				CStructure *pStruct = (CStructure*)gMyStructs.New();
				USINT ClassId = USINT(pClass - gMyArrStructCls);
				RECT rcScope = $m.GetOccupiedRegion($m.Preview.Coord, $m.Preview.Scale);
				pStruct->Initialize(ClassId, *pClass, $m.Preview.Coord);
				$m.ModifyRegionMask(rcScope, 1);
			}
		}
	}
	_secr $VOID AddDecoration(){

	}
	///特殊地形生成
	_secr $VOID MakeAbyss(){
		/*IBOOL bReverse = gKeysTest[DIK_LSHIFT];
		SELBOX SelBox = $m.GetPickedRegion();
		SDTERRFEAT *prgFeat = (SDTERRFEAT*)$m.Chunks.GetMemory(0);

		$m.FitLandWiring();

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.Ground.ColCount;
			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				USINT iFace = (iLine + iCol) * 2;
				for(USINT iPart = 0; iPart < 2; ++iPart, ++iFace){
					SDTERRFEAT *pFeat = prgFeat + iFace;

					if(SelBox.bCir && !$m.LandFaceInCircle(iFace))
						continue;
					if(SelBox.bRhom && !$m.LandFaceInRhombus(iFace))
						continue;

					if(!bReverse){
						pFeat->End = CELL_SIZEV * -4.f;
						pFeat->bDiscard = B_True;
					} else if(pFeat->bDiscard){
						pFeat->End = 0.f;
						pFeat->bDiscard = B_False;
					}
				}
			}
		}

		$m.ResetChunks();*/
	}
	_secr $VOID MakeCliff(){
		/*SELBOX SelBox = $m.GetPickedRegion();
		SDTERRFEAT *prgFeat = (SDTERRFEAT*)$m.Chunks.GetMemory(0);

		IBOOL bUniform = gKeysTest[DIK_LCONTROL];
		SPFPN Power = gKeysTest[DIK_LSHIFT] ? -CELL_SIZEV : CELL_SIZEV;
		SPFPN Height = bUniform ? $m.CalcUniformCliff(SelBox.RectU) : 0.f;

		$m.FitLandWiring();

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.Ground.ColCount;
			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				USINT iFace = (iLine + iCol) * 2;
				for(USINT iPart = 0; iPart < 2; ++iPart, ++iFace){
					SDTERRFEAT *pFeat = prgFeat + iFace;

					if(SelBox.bCir && !$m.LandFaceInCircle(iFace))
						continue;
					if(SelBox.bRhom && !$m.LandFaceInRhombus(iFace))
						continue;
					if(pFeat->bDiscard)
						continue;

					pFeat->End = bUniform ? Height : pFeat->End + Power;
					pFeat->TexIdent = (UTINY)$m.BrushMapId;
				}
			}
		}

		$m.ResetChunks();*/
	}
	_secr $VOID MakeWater(){
		IBOOL bReverse = gKeysTest[DIK_LSHIFT];
		SELBOX SelBox = $m.GetPickedRegion();
		WAVEVIEW *prgView = (WAVEVIEW*)$m.WavViews.Get(0);
		WAVEVIEW *pCurView = prgView + $m.iWavView;

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLeftCell = iRow * $m.Ground.ColCount;
			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				USINT &rViewId = $m.lprgCellInfo[iLeftCell + iCol].iWavView;

				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				if(bReverse){
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
		static USINT tNumCtrl = 0;

		if(bBreak){
			$m.Paths.PopBack(tNumCtrl % 4);
			if($m.Paths.GetSize() > 0){
				if(((POINT3*)$m.Paths.GetBack())->x != SPFP_NAN)
					$m.Paths.PushBack(&POINT3(SPFP_NAN, 0.f, 0.f));
			}
			tNumCtrl = 0;
			return;
		}

		POINT3 NewPoint = $m.SelPoint;
		USINT DstStart = $m.Roads.GetCount();

		tNumCtrl += 1;
		NewPoint.y += 0.5f;
		$m.Paths.PushBack(&NewPoint);

		if((tNumCtrl > 4) && (tNumCtrl % 4) == 2){
			VECTR3X *prgDest = (VECTR3X*)$m.Roads.GetMemory(DstStart);
			VECTR3X *prgSource = ((VECTR3X*)$m.Paths.New(2)) - 2;
			VECTR3X *pPrev1 = prgDest - 1;
			VECTR3X *pPrev2 = pPrev1 - 1;

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
			POINT3 *prgDest = (POINT3*)$m.Roads.GetMemory(DstStart);
			POINT3 *prgSource = (POINT3*)$m.Paths.GetBack() - 3;

			prgDest[0] = prgSource[0];
			prgDest[1] = prgSource[1];
			prgDest[2] = prgSource[2];
			prgDest[3] = prgSource[3];

			$m.Roads.SetCount(DstStart + 4);
		}
	}
	///地形编辑
	_secr $VOID EditLandEdge(){
		BOOL8 bReverse = gKeysTest[DIK_LSHIFT];
		USINT MaxCol = $m.Ground.ColCount;
		SELBOX SelBox = $m.GetPickedRegion();
		RGB10 *prgFeat = $m.Ground.prgNormal;

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine0 = iRow * $m.NumMeridian;
			USINT iLine1 = iLine0 + $m.NumMeridian;

			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				USINT iNextCol = iCol + 1;

				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				USINT iFace0 = (MaxCol*iRow + iCol) * 2;
				USINT iFace1 = iFace0 + 1;

				if(bReverse){
					prgFeat[iFace0] &= ~CHeightField::FACE_FLAG_Bslash;
					prgFeat[iFace1] &= ~CHeightField::FACE_FLAG_Bslash;
				} else{
					prgFeat[iFace0] |= CHeightField::FACE_FLAG_Bslash;
					prgFeat[iFace1] |= CHeightField::FACE_FLAG_Bslash;
				}
			}
		}

		$m.Ground.UpdateFlatNormals($m.MapFrame);
		$m.Ground.UpdateSmoothNormals($m.MapFrame);
	}
	_secr $VOID EditLandHeight(){
		SELBOX SelBox = $m.GetPickedVertices();

		BOOL8 bUniform = ($m.FuncMode == MODE_Uniform);
		BOOL8 bSteepen = ($m.FuncMode == MODE_Steepen);
		BOOL8 bReverse = gKeysTest[DIK_LSHIFT];
		BOOL8 bFlatten = (bSteepen && bReverse);

		SPFPN Power = $m.BrushPower * 0.5f;
		SPFPN DistSq, Height;

		///计算高度

		if(bUniform)
			Height = $m.CalcUniformHeight(SelBox.RectU);
		else if(bFlatten)
			Height = $m.CalcAverageHeight(SelBox.RectU);
		else if(bReverse)
			Power = -$m.BrushPower;
		else
			Power = $m.BrushPower;

		///更新顶点

		LANDVERT *prgVert = $m.Ground.prgVertex;
		prgVert += SelBox.B * $m.NumMeridian;

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				SPFPN &rCoordY = prgVert[iCol].CoordY;
				POINT2 Coord = $m.GetLandVertex(iRow, iCol);

				if(SelBox.bCir && !$m.PointInCircle(Coord, SelBox, &DistSq))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(Coord, SelBox, -CELL_SIZEH))
					continue;

				if(bFlatten) rCoordY = LERP_(rCoordY, Height, Power);
				else if(bUniform) rCoordY = Height;
				else if(bSteepen) rCoordY += Power * RandReal(-1.f, 1.f);
				else if(!SelBox.bCir) rCoordY += Power;
				else rCoordY += Power * (1.f - DistSq/SelBox.RadSq);
			}
			prgVert += $m.NumMeridian;
		}

		//更新布线

		$m.FitLandWiring();
	}
	_secr $VOID EditLandShading(){
		SELBOX SelBox = $m.GetPickedRegion();
		RGB10 *prgNorm = (RGB10*)$m.Ground.prgNormal;
		IBOOL bReverse = gKeysTest[DIK_LSHIFT];

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.Ground.ColCount;
			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				USINT iFace0 = (iLine + iCol) * 2;
				USINT iFace1 = iFace0 + 1;

				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				if(!bReverse){
					prgNorm[iFace0] |= 0xC0000000;
					prgNorm[iFace1] |= 0xC0000000;
				} else{
					prgNorm[iFace0] &= 0x3FFFFFFF;
					prgNorm[iFace1] &= 0x3FFFFFFF;
				}
			}
		}
	}
	_secr $VOID EditLandColor(){
		SELBOX SelBox = $m.GetPickedCanvas();
		USINT MapSize = $m.NumMapPxX * $m.NumMapPxY;

		COLOR32 *psMap0 = $m.psCanvBuf;
		COLOR32 *psMap1 = psMap0 + (MapSize * 2);
		COLOR32 *psMap2 = psMap1 + MapSize;

		//更新像素

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.NumMapPxX;

			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				USINT iPixel = iLine + iCol;
				COLOR32 *pPixel1 = psMap1 + iPixel;
				COLOR32 *pPixel2 = psMap2 + iPixel;

				USINT Weight1 =
					(USINT)pPixel1->r + (USINT)pPixel1->g + (USINT)pPixel1->b + (USINT)pPixel1->a +
					(USINT)pPixel2->r + (USINT)pPixel2->g +	(USINT)pPixel2->b + (USINT)pPixel2->a;

				USINT Weight0 = USINT($m.BrushPower * 255.f);
				Weight0 = MIN_(Weight0, 255);

				if((Weight0 + Weight1) > 255){
					SPFPN Falloff = SPFPN(255 - Weight0) / 255.f;
					pPixel1->r = UTINY(pPixel1->r * Falloff);
					pPixel1->g = UTINY(pPixel1->g * Falloff);
					pPixel1->b = UTINY(pPixel1->b * Falloff);
					pPixel1->a = UTINY(pPixel1->a * Falloff);
					pPixel2->r = UTINY(pPixel2->r * Falloff);
					pPixel2->g = UTINY(pPixel2->g * Falloff);
					pPixel2->b = UTINY(pPixel2->b * Falloff);
					pPixel2->a = UTINY(pPixel2->a * Falloff);
				}

				psMap0[iPixel] = $m.BrushColor;
			}
		}

		//更新变动范围

		$m.TexFrame.top = SelBox.B;
		$m.TexFrame.left = SelBox.L;
		$m.TexFrame.right = SelBox.R + 1;
		$m.TexFrame.bottom = SelBox.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	_secr $VOID EditLandTexture(){
		SELBOX SelBox = $m.GetPickedCanvas();

		USINT TexId = $m.BrushMapId % 4;
		USINT ChanId = 3 - ((TexId + 1) % 4); //BGRA转RGBA
		USINT CanvId = ($m.BrushMapId < 4) ? 2 : 3;
		USINT MapSize = $m.NumMapPxX * $m.NumMapPxY;

		COLOR32 *psMap0 = $m.psCanvBuf + (MapSize * CanvId);
		COLOR32 *psMap1 = $m.psCanvBuf + (MapSize * (5 - CanvId));

		//更新像素

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.NumMapPxX;

			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				COLOR32 *pPixel0 = psMap0 + iLine + iCol;
				COLOR32 *pPixel1 = psMap1 + iLine + iCol;
				UTINY *pChan = (UTINY*)pPixel0 + ChanId;
				USINT Weight = USINT(*pChan) + USINT($m.BrushPower * 255.f);

				Weight = CLAMP_(Weight, 0, 255);
				*pChan = (UTINY)Weight;

				USINT WeightSum =
					(USINT)pPixel0->r + (USINT)pPixel0->g + (USINT)pPixel0->b + (USINT)pPixel0->a +
					(USINT)pPixel1->r + (USINT)pPixel1->g + (USINT)pPixel1->b + (USINT)pPixel1->a;

				if(WeightSum > 255){
					SPFPN Falloff = SPFPN(255 - Weight) / 255.f;
					pPixel0->r = UTINY(pPixel0->r * Falloff);
					pPixel0->g = UTINY(pPixel0->g * Falloff);
					pPixel0->b = UTINY(pPixel0->b * Falloff);
					pPixel0->a = UTINY(pPixel0->a * Falloff);
					pPixel1->r = UTINY(pPixel1->r * Falloff);
					pPixel1->g = UTINY(pPixel1->g * Falloff);
					pPixel1->b = UTINY(pPixel1->b * Falloff);
					pPixel1->a = UTINY(pPixel1->a * Falloff);
				}

				*pChan = (UTINY)Weight;
			}
		}

		//更新变动范围

		$m.TexFrame.top = SelBox.B;
		$m.TexFrame.left = SelBox.L;
		$m.TexFrame.right = SelBox.R + 1;
		$m.TexFrame.bottom = SelBox.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	_secr $VOID EditLandMaterial(){
		SELBOX SelBox = $m.GetPickedCanvas();
		USINT ChanId = ($m.FuncMode == MODE_EditMetal) ? 1 : 2;
		USINT MapSize = $m.NumMapPxX * $m.NumMapPxY;
		COLOR32 *psMap = $m.psCanvBuf + MapSize;

		//更新像素

		for(USINT iRow = SelBox.B; iRow <= SelBox.T; ++iRow){
			USINT iLine = iRow * $m.NumMapPxX;

			for(USINT iCol = SelBox.L; iCol <= SelBox.R; ++iCol){
				if(SelBox.bCir && !$m.PointInCircle(ToPoint(iCol, iRow), SelBox))
					continue;
				if(SelBox.bRhom && !$m.PointInRhombus(ToPoint(iCol, iRow), SelBox))
					continue;

				COLOR32 *pPixel = psMap + iLine + iCol;
				UTINY *pChan = (UTINY*)pPixel + ChanId;

				*pChan = UTINY($m.BrushPower * 255.f);
				*pChan = MIN_(255, *pChan);
			}
		}

		//更新变动范围

		$m.TexFrame.top = SelBox.B;
		$m.TexFrame.left = SelBox.L;
		$m.TexFrame.right = SelBox.R + 1;
		$m.TexFrame.bottom = SelBox.T + 1;
		$m.TexFrame.front = 0;
		$m.TexFrame.back = 1;
	}
	///编辑器设置
	_open $VOID SetBrushSize(USINT Scale){
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
	_open $VOID SetBrushMapping(USINT Ident){
		$m.BrushMapId = (UHALF)Ident;
	}
	_open $VOID SetBrushShape(BRUSHTYPE Type){
		$m.BrushShape = Type;
		$m.BrushRad0 = $m.BrushRad1;

		if(Type == BR_TYPE_Rhombus){
			if(Mod($m.BrushRad1, CELL_SIZEH) != 0)
				$m.BrushRad0 += CELL_SIZEH * 0.5f;
		}
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
		$m.MenuFrame = RECTU{};
	}
	_open $VOID SetPreviewObject(USINT Ident){
		SIZE Scale;
		DWORD Type;
		USINT PsoId;
		CMeshAssy *pModel;
		STRUCTCLASS *pClass;

		if($m.FuncMode ==  MODE_AddUnit){
			pModel = mgGetModel(ID_MDL_Actor, Ident);
			Type = pModel->lpFirst->VtxFormat;

			if(gMyArrShader[ID_PSO_Animal].VtxFlag == Type){
				PsoId = ID_PSO_Animal;
			} else if(gMyArrShader[ID_PSO_Struct].VtxFlag == Type){
				PsoId = ID_PSO_Struct;
				pClass = mgGetStructClass(Ident);
				Scale.cx = pClass->SizeX;
				Scale.cy = pClass->SizeZ;
			}
		} else if($m.FuncMode ==  MODE_MakeDecor){
			pModel = mgGetModel(ID_MDL_Decor, Ident);
			// To Do
		}

		$m.Preview.PsoId = PsoId;
		$m.Preview.MdlId = Ident;
		$m.Preview.Scale = Scale;
		$m.Preview.pModel = pModel;
	}
	///地图格子设置
	_secr $VOID ModifyRegionMask(RECT &rcScope, INT32 Step){
		INT32 CellBase = $m.Ground.ColCount * (rcScope.bottom - 1);
		CELLINFO *prgCell = $m.lprgCellInfo + CellBase;

		for(INT32 iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			prgCell += $m.Ground.ColCount;
			for(INT32 iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				prgCell[iCol].iHolder += Step;
		}
	}
	///过程图应用
	_open $VOID ApplyColorMap(WCHAR *psFilePath, USINT Repeat){
		COLOR32 *prgPixel;
		IWICBmpLock *lpLock;
		IWICBitmap *lpBitmap;
		USINT Size, Pitch;
		USINT NumCol, NumRow, MaxRow;

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
			SPFPN ScaleX = (SPFPN)NumCol / (SPFPN)$m.NumMapPxX;
			SPFPN ScaleY = (SPFPN)NumRow / (SPFPN)$m.NumMapPxY;

			for(USINT iDstRow = 0; iDstRow < $m.NumMapPxY; ++iDstRow){
				USINT iDstLine = iDstRow * $m.NumMapPxX;
				for(USINT iDstCol = 0; iDstCol < $m.NumMapPxX; ++iDstCol){
					USINT iSrcCol = USINT(iDstCol * ScaleX);
					USINT iSrcRow = MaxRow - USINT(iDstRow * ScaleY);
					USINT iSource = (iSrcRow * Pitch) + iSrcCol;
					USINT iDest = iDstLine + iDstCol;
					$m.psCanvBuf[iDest] = prgPixel[iSource];
				}
			}

			$m.TexFrame.left = 0;
			$m.TexFrame.right = $m.NumMapPxX;
			$m.TexFrame.top = 0;
			$m.TexFrame.bottom = $m.NumMapPxY;
			$m.TexFrame.front = 0;
			$m.TexFrame.back = 1;
		}

		//释放空间

		lpLock->Release();
		lpBitmap->Release();
	}
	_open $VOID ApplyHeightMap(WCHAR *psFilePath, SPFPN Min, SPFPN Max){
		SIZE2U Size;
		COLOR32 *prgPixel;
		IWICBmpLock *lpLock;
		IWICBitmap *lpBitmap;
		USINT ByteCnt, Pitch, MaxRow;

		//读取文件
		{
			lpBitmap = LoadWicBitmap(GE2d::lpWicFactory, psFilePath);
			lpBitmap->Lock(P_Null, WICBitmapLockRead, &lpLock);

			lpLock->GetDataPointer(&ByteCnt, (BYTET**)&prgPixel);
			lpLock->GetSize(&Size.width, &Size.height);
			lpLock->GetStride(&Pitch);

			MaxRow = Size.height - 1;
			Pitch /= 4;
		}

		//解析高度值
		{
			SPFPN Scaling = (Max - Min) / 255.f;
			SPFPN *lprgValue = new SPFPN[Size.width * Size.height];

			for(USINT iRow = 0; iRow < Size.height; ++iRow){
				USINT iDstLine = iRow * Size.width;
				USINT iSrcLine = (MaxRow - iRow) * Pitch;

				for(USINT iCol = 0; iCol < Size.width; ++iCol){
					USINT iDest = iDstLine + iCol;
					USINT iSource = iSrcLine + iCol;
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
	_secr $VOID CreateTextures(DXTKUploader &Uploader, SIZE2U Size, ARGB8 Color, ARGB8 Matl){
		USINT SliceSize = Size.width * Size.height; //单个画布大小

		//纹理画布
		{
			USINT Size = SliceSize * 4; //四个画布合并
			USINT ByteCnt = Size * sizeof(ARGB8);

			$m.lpMapCanvas = CreateUploadBuffer(GE3d::lpD3dDevice, ByteCnt, B_False);
			$m.lpMapCanvas->Map(0, P_Null, ($VOID**)&$m.psCanvBuf);

			DStr_Store($m.psCanvBuf, Color, SliceSize); //基础颜色画布
			DStr_Store($m.psCanvBuf + SliceSize, Matl, SliceSize); //金属性-粗糙度画布
			DStr_Store($m.psCanvBuf + (SliceSize * 2), 0, SliceSize * 2); //纹理遮罩画布
		}

		//基础纹理,纹理遮罩
		{
			GE3d::lpD3dDevice->CreateCommittedResource(
				&D3DX12HEAPPROPS(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&D3DX12RESDESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, Size.width, Size.height, 4, 1),
				D3D12_RESOURCE_STATE_COPY_DEST, P_Null, IID_PPV_ARGS(&$m.rgMapTex[0]));

			D3D12SUBRESDATA Data0 = { new ARGB8[SliceSize], 0, 0 };
			D3D12SUBRESDATA Data1 = { new ARGB8[SliceSize], 0, 0 };

			DStr_Store(Data0.pData, Color, SliceSize);
			DStr_Store(Data1.pData, Matl, SliceSize);

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
			D3DX12CPUVIEWPTR hTexView;
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
	_open $VOID CreateTerrain(USINT NumCol, USINT NumRow, ARGB8 Color, ARGB8 Matl){
		STLFuture<$VOID> Thread;
		DXTKUploader Uploader(GE3d::lpD3dDevice);

		//地形纹理
		{
			USINT Width = NumCol * CELL_SPLIT;
			USINT Height = NumRow * CELL_SPLIT;

			Uploader.Begin();
			$m.CleanTexture();
			$m.CreateTextures(Uploader, SIZE2U{ Width, Height }, Color, Matl);
			Thread = Uploader.End(gMyGfxCmd.pCmdQueue);
		}

		//地形模型
		{
			CHeightField::DESC LandDesc = {};
			LandDesc.bSmooth = B_True;
			LandDesc.ColCount = NumCol;
			LandDesc.RowCount = NumRow;
			LandDesc.CellWidth = CELL_SIZEH;
			LandDesc.CellDepth = CELL_SIZEH;

			$m.Ground.Finalize();
			$m.Ground.Initialize(GE3d::lpD3dDevice, LandDesc);
		}

		//地形特征
		{
			USINT CellCnt = NumCol * NumRow;

			delete[] $m.lprgCellInfo;
			$m.lprgCellInfo = new CELLINFO[CellCnt]{};

			$m.Roads.SetCount(0);
			$m.Paths.Clear(B_False);

			$m.WavViews.Clear(B_False);
			$m.WavViews.New(1);

			$m.Water.Finalize();
			$m.Water.Initialize(GE3d::lpD3dDevice, CellCnt, sizeof(POINT2U));

			$m.Chunks.Finalize();
			$m.Chunks.Initialize(GE3d::lpD3dDevice, CellCnt * 2, sizeof(SDTERRFEAT), B_False);
			$m.Chunks.EnableUpdate(B_True, B_True);
		}

		//场景信息
		{
			$m.AreaWidth = $m.Ground.GetWidth();
			$m.AreaDepth = $m.Ground.GetDepth();
			$m.NumMapPxX = $m.Ground.ColCount * CELL_SPLIT;
			$m.NumMapPxY = $m.Ground.RowCount * CELL_SPLIT;
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

			$m.Camera.Reset(Vec_Set(PosX, PosY, PosZ, 0.f));
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
	///屏幕拾取
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
		}

		if(MsgCode < WM_MOUSEFIRST) return B_False;
		if(MsgCode > WM_MOUSELAST) return B_False;
		return B_True;
	}
	///选区边缘过滤
	_secr IBOOL PointInCircle(POINT2 &Point, SELBOX &SelBox, SPFPN *pDistSq = P_Null){
		SPFPN DistSq = PointsDistSq(Point, SelBox.Cen);
		if(pDistSq) *pDistSq = DistSq;
		return (DistSq <= SelBox.RadSq);
	}
	_secr IBOOL PointInRhombus(POINT2 &Point, SELBOX &SelBox, SPFPN Adjust = 0.f){
		VECTR2X Pa(Point);
		VECTR2X Pb0(SelBox.Rect.left, SelBox.Rect.top);
		VECTR2X Pb1(SelBox.Rect.right, SelBox.Rect.bottom);
		VECTR2X Pb2(SelBox.Rect.right, SelBox.Rect.top);
		VECTR2X Pb3(SelBox.Rect.left, SelBox.Rect.bottom);

		SPFPN Radius = SelBox.Rad + Adjust;
		SPFPN Limit = Sqrt(Radius * Radius * 2.f) * 0.5f;
		SPFPN Dist0 = Vec2_LinePointDist(Pb0, Pb1, Pa);
		SPFPN Dist1 = Vec2_LinePointDist(Pb2, Pb3, Pa);

		if(Dist0 > Limit) return B_False;
		if(Dist1 > Limit) return B_False;
		return B_True;
	}
	_secr IBOOL LandFaceInCircle(USINT FaceId){
		/*SPHERE Sphere(RectCenter($m.MapFrame), $m.BrushRad0);

		INDEX3 *pFace = $m.Ground.prgIndex + FaceId;
		LANDVERT *prgVert = $m.Ground.prgVertex;

		VECTOR3 Vert0 = prgVert[(*pFace)[0]].Coord;
		VECTOR3 Vert1 = prgVert[(*pFace)[1]].Coord;
		VECTOR3 Vert2 = prgVert[(*pFace)[2]].Coord;

		Vert0.y = 0.f;
		Vert1.y = 0.f;
		Vert2.y = 0.f;

		return Sphere.Intersects(
			Pck_V3(&Vert0), Pck_V3(&Vert1), Pck_V3(&Vert2));*/

		return B_False;
	}
	_secr IBOOL LandFaceInRhombus(USINT FaceId){
		/*using namespace dx;

		INDEX3 *pFace = $m.Ground.prgIndex + FaceId;
		LANDVERT *prgVert = $m.Ground.prgVertex;

		MVECTOR Vert0 = Pck_V3(&prgVert[(*pFace)[0]].Coord);
		MVECTOR Vert1 = Pck_V3(&prgVert[(*pFace)[1]].Coord);
		MVECTOR Vert2 = Pck_V3(&prgVert[(*pFace)[2]].Coord);
		MVECTOR Point = (Vert0 + Vert1 + Vert2) / 3.f;

		Vec_SetY(Point, 0.f);

		MVECTOR PointB0 = Vec_Set($m.MapFrame.left, 0.f, $m.MapFrame.top, 0.f);
		MVECTOR PointB1 = Vec_Set($m.MapFrame.right, 0.f, $m.MapFrame.bottom, 0.f);
		MVECTOR PointB2 = Vec_Set($m.MapFrame.right, 0.f, $m.MapFrame.top, 0.f);
		MVECTOR PointB3 = Vec_Set($m.MapFrame.left, 0.f, $m.MapFrame.bottom, 0.f);

		SPFPN Limit = Sqrt($m.BrushRad0 * $m.BrushRad0 * 2.f) * 0.5f;
		SPFPN Dist0 = Vec3_LinePointDist(PointB0, PointB1, Point);
		SPFPN Dist1 = Vec3_LinePointDist(PointB2, PointB3, Point);

		if(Dist0 > Limit) return B_False;
		if(Dist1 > Limit) return B_False;
		return B_True;*/

		return B_False;
	}
	///地形高度统计
	_secr SPFPN CalcAverageHeight(RECTU &rcScope){
		SPFPN Result = 0;
		LANDVERT *prgVert = $m.Ground.prgVertex;

		for(USINT iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			USINT iLine = iRow * $m.NumMeridian;
			for(USINT iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				Result += prgVert[iLine + iCol].CoordY;
		}

		Result /= rcScope.right - rcScope.left + 1;
		Result /= rcScope.top - rcScope.bottom + 1;

		return Result;
	}
	_secr SPFPN CalcUniformHeight(RECTU &rcScope){
		USINT MaxCount = 0;
		USINT NumValue = 0;
		SPFPN Result, rgValue[MAX_BRUSH * MAX_BRUSH];
		LANDVERT *prgSource = $m.Ground.prgVertex;

		for(USINT iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			USINT iLine = iRow * $m.NumMeridian;
			for(USINT iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				rgValue[NumValue++] = prgSource[iLine + iCol].CoordY;
		}

		for(USINT iVtx0 = 0; iVtx0 < (NumValue - 1); ++iVtx0){
			USINT Count = 0;
			SPFPN Value = rgValue[iVtx0];

			if(Value != FLT_MAX){
				for(USINT iVtx1 = iVtx0 + 1; iVtx1 < NumValue; ++iVtx1){
					if(NEAR_EQUAL(Value, rgValue[iVtx1], 0.01f)){
						rgValue[iVtx1] = FLT_MAX;
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
			return $m.CalcAverageHeight(rcScope);
		return Result;
	}
	_secr SPFPN CalcUniformCliff(RECTU &rcScope){
		/*USINT MaxCount = 0;
		USINT NumValue = 0;
		SPFPN Result, rgValue[MAX_BRUSH * MAX_BRUSH];
		SDTERRFEAT *prgSource = (SDTERRFEAT*)$m.Chunks.GetMemory(0);

		for(USINT iRow = rcScope.bottom; iRow <= rcScope.top; ++iRow){
			USINT iLine = iRow * $m.Ground.ColCount;
			for(USINT iCol = rcScope.left; iCol <= rcScope.right; ++iCol)
				rgValue[NumValue++] = prgSource[iLine + iCol].End;
		}

		for(USINT iVal0 = 0; iVal0 < (NumValue - 1); ++iVal0){
			USINT Count = 0;
			SPFPN Value = rgValue[iVal0];

			if(Value != FLT_MAX){
				for(USINT iVal1 = iVal0 + 1; iVal1 < NumValue; ++iVal1){
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

		if(MaxCount == 0)
			return rgValue[0];
		return Result;*/
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
	_secr SELBOX GetPickedCanvas(){
		RECTF Frame = $m.MapFrame;
		SPFPN CellSize = CELL_SIZEH / CELL_SPLIT;

		INT32 DivL = INT32(Frame.left / CellSize);
		INT32 DivB = INT32(Frame.bottom / CellSize);
		INT32 DivR = INT32(Frame.right / CellSize) - 1;
		INT32 DivT = INT32(Frame.top / CellSize) - 1;

		USINT ColStart = CLAMP_(DivL, 0, $m.NumMapPxX - 1);
		USINT RowStart = CLAMP_(DivB, 0, $m.NumMapPxY - 1);
		USINT ColEnd = CLAMP_(DivR, 0, $m.NumMapPxX - 1);
		USINT RowEnd = CLAMP_(DivT, 0, $m.NumMapPxY - 1);

		SELBOX SelBox;
		SelBox.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		SelBox.Rect = ToRectF(SelBox.RectU);
		SelBox.Cen = RECT_CENTER(SelBox.Rect);
		SelBox.Rad = (ColEnd - ColStart + 1) * 0.5f;
		SelBox.RadSq = SelBox.Rad * SelBox.Rad;
		SelBox.bRect = ($m.BrushShape == BR_TYPE_Rect);
		SelBox.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		SelBox.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return SelBox;
	}
	_secr SELBOX GetPickedRegion(){
		INT32 MaxCol = $m.Ground.ColCount - 1;
		INT32 MaxRow = $m.Ground.RowCount - 1;

		INT32 DivL = INT32($m.MapFrame.left / CELL_SIZEH);
		INT32 DivB = INT32($m.MapFrame.bottom / CELL_SIZEH);
		INT32 DivR = INT32(($m.MapFrame.right - CELL_SIZEH) / CELL_SIZEH);
		INT32 DivT = INT32(($m.MapFrame.top - CELL_SIZEH) / CELL_SIZEH);

		USINT ColStart = CLAMP_(DivL, 0, MaxCol);
		USINT RowStart = CLAMP_(DivB, 0, MaxRow);
		USINT ColEnd = CLAMP_(DivR, 0, MaxCol);
		USINT RowEnd = CLAMP_(DivT, 0, MaxRow);

		SELBOX SelBox;
		SelBox.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		SelBox.Rect = ToRectF(SelBox.RectU);
		SelBox.Cen = RECT_CENTER(SelBox.Rect);
		SelBox.Rad = (ColEnd - ColStart + 1) * 0.5f;
		SelBox.RadSq = SelBox.Rad * SelBox.Rad;
		SelBox.bRect = ($m.BrushShape == BR_TYPE_Rect);
		SelBox.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		SelBox.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return SelBox;
	}
	_secr SELBOX GetPickedVertices(){
		INT32 MaxCol = $m.Ground.ColCount;
		INT32 MaxRow = $m.Ground.RowCount;

		INT32 DivL = INT32(($m.MapFrame.left + CELL_SIZEH) / CELL_SIZEH);
		INT32 DivB = INT32(($m.MapFrame.bottom + CELL_SIZEH) / CELL_SIZEH);
		INT32 DivR = INT32(($m.MapFrame.right - CELL_SIZEH) / CELL_SIZEH);
		INT32 DivT = INT32(($m.MapFrame.top - CELL_SIZEH) / CELL_SIZEH);

		USINT ColStart = CLAMP_(DivL, 0, MaxCol);
		USINT RowStart = CLAMP_(DivB, 0, MaxRow);
		USINT ColEnd = CLAMP_(DivR, 0, MaxCol);
		USINT RowEnd = CLAMP_(DivT, 0, MaxRow);

		SELBOX SelBox;
		SelBox.RectU = { ColStart, RowEnd, ColEnd, RowStart };
		SelBox.Rect = $m.MapFrame;
		SelBox.Cen = RECT_CENTER($m.MapFrame);
		SelBox.Rad = $m.BrushRad0;
		SelBox.RadSq = $m.BrushRad0 * $m.BrushRad0;
		SelBox.bRect = ($m.BrushShape == BR_TYPE_Rect);
		SelBox.bRhom = ($m.BrushShape == BR_TYPE_Rhombus);
		SelBox.bCir = ($m.BrushShape == BR_TYPE_Circle);

		return SelBox;
	}
	///占地范围
	_secr RECT GetOccupiedRegion(VECTOR3 &Center, SIZE &Scale){
		SPFPN HalfW = CELL_SIZEH * Scale.cx * 0.5f;
		SPFPN HalfH = CELL_SIZEH * Scale.cy * 0.5f;

		INT32 DivL = INT32((Center.x - HalfW) / CELL_SIZEH);
		INT32 DivB = INT32((Center.z - HalfH) / CELL_SIZEH);
		INT32 DivR = INT32((Center.x + HalfW) / CELL_SIZEH) - 1;
		INT32 DivT = INT32((Center.z + HalfH) / CELL_SIZEH) - 1;

		return RECT{ DivL, DivT, DivR, DivB };
	}
	///
	_secr VECTOR2 GetLandVertex(USINT RowId, USINT ColId){
		return VECTOR2(ColId * CELL_SIZEH, RowId * CELL_SIZEH);
	}
	///坐标吸附
	_secr VECTOR3 GetSnapedFocus(SIZE &Scale){
		SPFPN PosX = $m.SelPoint.x - Mod($m.SelPoint.x, CELL_SIZEH);
		if((Scale.cx % 2) != 0) PosX += CELL_SIZEH / 2.f;
		SPFPN PosZ = $m.SelPoint.z - Mod($m.SelPoint.z, CELL_SIZEH);
		if((Scale.cy % 2) != 0) PosZ += CELL_SIZEH / 2.f;
		SPFPN PosY = $m.Ground.SampleHeight(PosX, PosZ);
		return VECTOR3(PosX, PosY, PosZ);
	}
	///资源获取
	_secr HD3D12GPURES GetLedMaterial(USINT Index){
		return $m.hLedMatls + (Index * sizeof(MATBASE));
	}
	_secr HD3D12GPURES GetWaterMaterial(USINT Index){
		return $m.hWavMatls + (Index * sizeof(MATLITE));
	}
	_secr HD3D12GPURES GetTexProjection(USINT Index){
		return $m.hTexProjs + (Index * sizeof(MATRIX4));
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