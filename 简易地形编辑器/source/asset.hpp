//-------------------- 数据类型 --------------------//

class IAppBase{
public:
	virtual $VOID Initialize() = 0;
	virtual $VOID Finalize() = 0;
	virtual $VOID Update() = 0;
	virtual $VOID Paint() = 0;
	virtual IBOOL OnMessage(DWORD, WPARAM, LPARAM) = 0;
};

enum MYVARID{
	ID_MDL_Actor = 0,
	ID_MDL_Decor = 1,
	//////
	ID_PSO_Struct = 0,
	ID_PSO_Animal = 1,
};
enum MYTEXTID{
	IDS_StartGame,
	IDS_EditScene,
	IDS_QuitApp,
	IDS_New,
	IDS_Set,
	IDS_Open,
	IDS_Quit,
	IDS_Save,
	IDS_SaveAs,
	IDS_TerrModule,
	IDS_DecorModule,
	IDS_ActorModule,
	IDS_SculptScape,
	IDS_PaintScape,
	IDS_MakeScape,
	IDS_MarqueeShape,
	IDS_MarqueeSize,
	IDS_BrushStrength,
	IDS_BrushColor,
	IDS_BrushPattern,
	IDS_Uplift,
	IDS_Steepen,
	IDS_Uniform,
	IDS_Sharpen,
	IDS_Wiring,
	IDS_Texture,
	IDS_Metalness,
	IDS_Roughness,
	IDS_Water,
	IDS_Cliff,
	IDS_Abyss,
	IDS_Closed,
	IDS_RandDir,
	IDS_RandStyle,
	IDS_Hint0,
	IDS_Hint1,
	IDS_Hint2,
	IDS_Hint3,
	IDS_Hint4,
	IDS_Hint5,
	IDS_Hint6,
	IDS_Hint7,
	IDS_Hint8,
	IDS_Hint9,
	IDS_Hint10,
	//////
	MIN_TEAM_ID,
	IDS_Player = MIN_TEAM_ID,
	IDS_Enemy,
	IDS_Neutral,
	IDS_Goods,
	MAX_TEAM_ID = IDS_Goods,
	//////
	MIN_ETHNIC_ID,
	IDS_MainCountry = MIN_ETHNIC_ID,
	IDS_Chartered,
	IDS_Aboriginal,
	IDS_Unattributed,
	IDS_EarthCoalition,
	IDS_GalaxyEmpire,
	IDS_WastelandLegion,
	IDS_Starmind,
	IDS_Cosmosman,
	IDS_TwinBee,
	MAX_ETHNIC_ID = IDS_TwinBee,
	//////
	MIN_SCENE_ID,
	IDS_DryLand = MIN_SCENE_ID,
	IDS_WetLand,
	IDS_PolarRegion,
	IDS_SeaIsland,
	IDS_FlyingIsland,
	IDS_Crypt,
	IDS_City,
	IDS_UndergroundBase,
	IDS_UnderwaterBase,
	IDS_SpacePlatform,
	MAX_SCENE_ID = IDS_SpacePlatform,	
};
enum MYWIDGETID{
	ID_BTN_Start = 100L,
	ID_BTN_Quit,
	ID_BTN_Set,
	ID_BTN_New,
	ID_BTN_Open,
	ID_BTN_Edit,
	ID_BTN_Save,
	ID_BTN_SaveAs,
	ID_BTN_BrushSize,
	ID_BTN_BrushPower,
	ID_GRP_BrushShape,
	ID_GRP_SculptScape,
	ID_GRP_PaintScape,
	ID_GRP_MakeScape,
	ID_RDO_RectBrush,
	ID_RDO_CircleBrush,
	ID_RDO_RhombusBrush,
	ID_RDO_Uplift,
	ID_RDO_Wiring,
	ID_RDO_Steepen,
	ID_RDO_Flatten,
	ID_RDO_Sharpen,
	ID_RDO_EditTex,
	ID_RDO_EditMetal,
	ID_RDO_EditRough,
	ID_RDO_MakeWater,
	ID_RDO_MakeCliff,
	ID_RDO_MakeAbyss,
	ID_RDO_MakeClosed,
	ID_RDO_RandDir,
	ID_RDO_RandStyle,
	ID_LBL_ToolTip,
	ID_LBL_SculptScape,
	ID_LBL_PaintScape,
	ID_LBL_MakeScape,
	ID_LBL_BrushSize,
	ID_LBL_BrushPower,
	ID_LST_BrushColor,
	ID_LST_BrushPattern,
	ID_LST_Decorat,
	ID_LST_Actor,
	ID_CMB_Module,
	ID_CMB_SceneType,
	ID_CMB_DecorType,
	ID_CMB_ActorTeam,
	ID_CMB_ActorEthnic,
	ID_PNL_TerrTool,
	ID_PNL_DecorTool,
	ID_PNL_ActorTool,
};

//----------------------------------------//


//-------------------- 全局变量 --------------------//

POINT *gCursor;
BYTET *gKeysTest;

SPFPN gMyPanelW;
SPFPN gMyPanelH;

SPFPN gMyFovAngX;
SPFPN gMyFovAngY;

UNS32 gMyAnimalClsNum;
UNS32 gMyStructClsNum;

MGAnimalClass *gMyAnimalClsArr;
MGStructClass *gMyStructClsArr;

CJsonNode *gMyOption;
CJsonNode *gMyRoster;

CShader *gMyShaderArr;
CGfxPso *gMyPsoArr;

CPanel *gMyGuiRoot;
IAppBase *gMyAppPage;

CTimer gMyTimer;
CRenderer gMyRender;

CLitRepos gMyLitLib;
CMtlRepos gMyMtlLib;
CStrRepos gMyTxtLib;

TSeqList<CMGAnimal> gMyAnimals;
TSeqList<CMGStructure> gMyStructs;

TSeqMap<CSprite> gMySprites;
TSeqMap<CMeshAssy> gMyModels[2];
TSeqMap<ID2D1Bitmap1*> gMyImages;
TSeqMap<IDWriteTextFormat*> gMyFonts;

//----------------------------------------//


//-------------------- 资源查找 --------------------//

inline WCHAR *mgGetText(UNS32 Index){
	return A_TO_W(gMyTxtLib.Get(Index));
}
inline CSprite *mgGetSprite(CHAR8 *psName){
	if(!psName) return P_Null;
	return gMySprites.Get(psName);
}
inline CMeshAssy *mgGetModel(UNS32 SetId, UNS32 MdlId){
	return gMyModels[SetId].Get(MdlId);
}
inline CMeshAssy *mgGetModel(UNS32 SetId, CHAR8 *psName){
	return gMyModels[SetId].Get(StrHash32(psName));
}
inline ID2D1Bitmap1 *mgGetImage(CHAR8 *psName){
	if(!psName) return P_Null;
	return *gMyImages.Get(psName);
}
inline IDWriteTextFormat *mgGetFont(CHAR8 *psName){
	if(!psName) return P_Null;
	return *gMyFonts.Get(psName);
}
inline MGAnimalClass *mgGetAnimalClass(UNS32 Ident){
	return (MGAnimalClass*)bsearch(&Ident,
		gMyAnimalClsArr, gMyAnimalClsNum, sizeof(MGAnimalClass), tlCompareAsc<UNS32>);
}
inline MGStructClass *mgGetStructClass(UNS32 Ident){
	return (MGStructClass*)bsearch(&Ident,
		gMyStructClsArr, gMyStructClsNum, sizeof(MGStructClass), tlCompareAsc<UNS32>);
}

inline CLabel *mgGetLabel(DWORD Ident){
	return (CLabel*)gMyGuiRoot->GetChildById(Ident);
}
inline CButton *mgGetButton(DWORD Ident){
	return (CButton*)gMyGuiRoot->GetChildById(Ident);
}
inline CWidget *mgGetWidget(DWORD Ident){
	return gMyGuiRoot->GetChildById(Ident);
}
inline CTextBox *mgGetTextBox(DWORD Ident){
	return (CTextBox*)gMyGuiRoot->GetChildById(Ident);
}
inline CListBox *mgGetListBox(DWORD Ident){
	return (CListBox*)gMyGuiRoot->GetChildById(Ident);
}
inline CComboBox *mgGetComboBox(DWORD Ident){
	return (CComboBox*)gMyGuiRoot->GetChildById(Ident);
}
inline CCheckBox *mgGetCheckBox(DWORD Ident){
	return (CCheckBox*)gMyGuiRoot->GetChildById(Ident);
}
inline CBtnGroup *mgGetRadios(DWORD Ident){
	return (CBtnGroup*)gMyGuiRoot->GetChildById(Ident);
}

//----------------------------------------//


//-------------------- 资源创建 --------------------//

$VOID mgAddImages(WCHAR *psCtlg){
	HANDLE hrSeeker;
	WCHAR FilePath[MAX_PATH];
	WIN32_FIND_DATA FileInfo;

	wcscat(wcscpy(FilePath, psCtlg), L"*.png");
	hrSeeker = FindFirstFileW(FilePath, &FileInfo);
	if(INVALID_(hrSeeker)) return;

	CHAR8 ObjName[MAX_PATH];
	UIPTR CtlgLen = wcslen(psCtlg);
	ID2D1Bitmap1 *pObject = P_Null;

	do{
		WideCharToMultiByte(ObjName, FileInfo.cFileName, MAX_PATH);
		wcscpy(&FilePath[CtlgLen], FileInfo.cFileName);
		*strrchr(ObjName, '.') = '\0';
		pObject = LoadD2dBitmap(GE2d::lpWicFactory, GE2d::lpD2dDevCtx, FilePath);
		gMyImages.Insert(ObjName, pObject);
	} while(FindNextFileW(hrSeeker, &FileInfo));

	FindClose(hrSeeker);
}
$VOID mgAddSprites(WCHAR *psCtlg){
	HANDLE hrSeeker;
	WCHAR FilePath[MAX_PATH];
	WIN32_FIND_DATA FileInfo;

	wcscat(wcscpy(FilePath, psCtlg), L"*.png");
	hrSeeker = FindFirstFileW(FilePath, &FileInfo);
	if(INVALID_(hrSeeker)) return;

	CHAR8 ObjName[MAX_PATH];
	UIPTR CtlgLen = wcslen(psCtlg);
	CSprite *pObject = P_Null;

	do{
		WideCharToMultiByte(ObjName, FileInfo.cFileName, MAX_PATH);
		wcscpy(&FilePath[CtlgLen], FileInfo.cFileName);
		*strrchr(ObjName, '.') = '\0';
		pObject = gMySprites.New(ObjName);
		pObject->Initialize(GE2d::lpWicFactory, GE2d::lpD2dDevCtx, FilePath);
	} while(FindNextFileW(hrSeeker, &FileInfo));

	FindClose(hrSeeker);
}
$VOID mgAddActorClasses(WCHAR *psPath, DWORD Type){
#define BUF_SIZE 128

	CHAR8 *pDelim;
	CHAR8 Buffer[BUF_SIZE];
	HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);

	ReadFileByLine(hrFile, Buffer, BUF_SIZE);

	if(Type == 0L){
		sscanf(Buffer, "%d", &gMyAnimalClsNum);

		MGAnimalClass *pItem = gMyAnimalClsArr = new MGAnimalClass[gMyAnimalClsNum];
		MGAnimalClass *pEnd = pItem + gMyAnimalClsNum;

		for(; pItem != pEnd; ++pItem){
			ReadFileByLine(hrFile, Buffer, BUF_SIZE);

			pDelim = strchr(Buffer, ',');
			*pDelim = '\0';
			pItem->iModel = StrHash32(Buffer);

			sscanf(++pDelim, "%d,%d,%f,%f,%d,%d,%f,%f,%hd,%hd",
				&pItem->HpMax, &pItem->EpMax, &pItem->HpRegen, &pItem->EpRegen,
				&pItem->Attack, &pItem->Defense, &pItem->AtkSpeed, &pItem->MovSpeed,
				&pItem->AtkType, &pItem->DefType);
		}

		qsort(gMyAnimalClsArr, gMyAnimalClsNum, sizeof(MGAnimalClass), tlCompareAsc<UNS32>);

		// end if
	} else if(Type == 1L){
		sscanf(Buffer, "%d", &gMyStructClsNum);

		MGStructClass *pItem = gMyStructClsArr = new MGStructClass[gMyStructClsNum];
		MGStructClass *pEnd = pItem + gMyStructClsNum;

		for(; pItem != pEnd; ++pItem){
			ReadFileByLine(hrFile, Buffer, BUF_SIZE);

			pDelim = strchr(Buffer, ',');
			*pDelim = '\0';
			pItem->iModel = StrHash32(Buffer);

			sscanf(++pDelim, "%d,%d,%f,%f,%d,%d,%f,%f,%hd,%hd,%hd,%hd",
				&pItem->HpMax, &pItem->EpMax, &pItem->HpRegen, &pItem->EpRegen,
				&pItem->Attack, &pItem->Defense, &pItem->AtkSpeed, &pItem->MovSpeed,
				&pItem->AtkType, &pItem->DefType, &pItem->SizeX, &pItem->SizeZ);
		}

		qsort(gMyStructClsArr, gMyStructClsNum, sizeof(MGStructClass), tlCompareAsc<UNS32>);
	}

	CloseHandle(hrFile);

#undef BUF_SIZE
}
$VOID mgAddModels(DXTKUploader &Uploader, UNS32 SetId, WCHAR *psCtlg){
	HANDLE hrSeeker;
	WCHAR FilePath[MAX_PATH];
	WIN32_FIND_DATA FileInfo;

	wcscat(wcscpy(FilePath, psCtlg), L"*.dat");
	hrSeeker = FindFirstFileW(FilePath, &FileInfo);
	if(INVALID_(hrSeeker)) return;

	CHAR8 ObjName[MAX_PATH];
	UIPTR CtlgLen = wcslen(psCtlg);
	CMeshAssy *pObject = P_Null;

	do{
		WideCharToMultiByte(ObjName, FileInfo.cFileName, MAX_PATH);
		wcscpy(&FilePath[CtlgLen], FileInfo.cFileName);
		*strrchr(ObjName, '.') = '\0';
		pObject = gMyModels[SetId].New(StrHash32(ObjName));
		pObject->Initialize(Uploader, FilePath);
	} while(FindNextFileW(hrSeeker, &FileInfo));

	FindClose(hrSeeker);
}
$VOID mgAddFont(CHAR8 *psName, SPFPN Height){
	if(!gMyFonts.Get(psName)){
		IDWriteTextFormat *lpFormat;
		GE2d::lpDWFactory->CreateTextFormat(L"Arial", P_Null,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			Height, L"en-US", &lpFormat);

		lpFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		lpFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		lpFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		gMyFonts.Insert(psName, lpFormat);
	}
}

//----------------------------------------//


//-------------------- 模块初始化 --------------------//

$VOID mgInitAsset(){
	TStlFuture<$VOID> Thread;
	DXTKUploader Uploader(GE3d::lpD3dDevice);

	//容器
	{
		gMyFonts.Init(12);
		gMyImages.Init(64);
		gMySprites.Init(32);
		gMyModels[0].Init(32);
		gMyModels[1].Init(32);
	}

	//模型&材质
	{
		Uploader.Begin();

		mgAddModels(Uploader, ID_MDL_Actor, L".\\model\\actor\\");
		mgAddModels(Uploader, ID_MDL_Decor, L".\\model\\decoration\\");

		gMyMtlLib.Initialize(16, 32);
		gMyMtlLib.AddMaterialsFromCatalog(L".\\model\\actor\\");
		gMyMtlLib.AddMaterialsFromCatalog(L".\\model\\decoration\\");
		gMyMtlLib.AddTexturesFromCatalog(Uploader, L".\\texture\\road\\");
		gMyMtlLib.AddTexturesFromCatalog(Uploader, L".\\texture\\unit\\");
		gMyMtlLib.AddTexturesFromCatalog(Uploader, L".\\texture\\common\\");
		gMyMtlLib.UploadToVram(Uploader);

		Thread = Uploader.End(gMyGfxCmd.pCmdQueue);
	}

	//位图
	{
		mgAddImages(L".\\image\\flag\\");
		mgAddImages(L".\\image\\icon\\");
		mgAddImages(L".\\image\\miniicon\\");
		mgAddImages(L".\\image\\backdrop\\");
		mgAddSprites(L".\\image\\sheet\\");
	}

	//缩减内存
	{
		gMyImages.Reserve(gMyImages.GetSize());
		gMySprites.Reserve(gMySprites.GetSize());

		gMyModels[0].Reserve(gMyModels[0].GetSize());
		gMyModels[1].Reserve(gMyModels[1].GetSize());
	}

	//对象设定
	{
		mgAddActorClasses(L".\\value\\Animal.csv", 0L);
		mgAddActorClasses(L".\\value\\Structure.csv", 1L);
	}

	//PSO
	{
		DWORD Format0 = VFB_Coord | VFB_Normal | VFB_TexCoord;
		DWORD Format1 = VFB_Coord | VFB_Normal | VFB_TexCoord | VFB_BoneRig;

		gMyPsoArr = new CGfxPso[2];
		gMyShaderArr = new CShader[2];

		gMyShaderArr[ID_PSO_Struct].Initialize(GE3d::lpD3dDevice, Format0, CShader::OUT_TYPE_IdBuff);
		gMyShaderArr[ID_PSO_Animal].Initialize(GE3d::lpD3dDevice, Format1, CShader::OUT_TYPE_IdBuff);

		gMyPsoArr[ID_PSO_Struct].Initialize(GE3d::lpD3dDevice, gMyShaderArr[ID_PSO_Struct], gMyWndEx);
		gMyPsoArr[ID_PSO_Animal].Initialize(GE3d::lpD3dDevice, gMyShaderArr[ID_PSO_Animal], gMyWndEx);
	}

	//灯光
	{
		GLight Light;
		Light.Intensity = { 3.5f, 3.5f, 3.5f };
		Light.Direction = { 0.6f, -1.f, 0.8f };

		gMyLitLib.Initialize(GE3d::lpD3dDevice, 4);
		gMyLitLib.UpdateDirectionalLight(Light);
	}

	//渲染器
	{
		gMyRender.Initialize(GE3d::lpD3dDevice, gMyGfxCmd.lpCmdList, gMyGfxCmd.MaxFrame);
		gMyRender.SetAmbientLight(VFloat3(0.5f, 0.5f, 0.5f));
	}

	//应用设置
	{
		const CHAR8 *psLang;
		WCHAR TxtPath[MAX_PATH] = L".\\text\\gui\\";
		WCHAR ListPath[MAX_PATH] = L".\\text\\roster\\";

		gMyOption = CJsonNode::CreateByFile(L".\\setting\\option.json");
		psLang = gMyOption->GetMemS("Language");

		StrAppendAt(TxtPath, psLang, wcslen(TxtPath));
		StrAppendAt(TxtPath, L".txt", wcslen(TxtPath));

		gMyTxtLib.Initialize(TxtPath, "\r\n");
		gMyTxtLib.Replace('\\', '\n');

		StrAppendAt(ListPath, psLang, wcslen(ListPath));
		StrAppendAt(ListPath, L".json", wcslen(ListPath));

		gMyRoster = CJsonNode::CreateByFile(ListPath);
	}

	//视域
	{
		gMyFovAngX = 90.f;
		gMyFovAngY = gMyFovAngX * gMyWndH / gMyWndW;
	}

	//静态缓存
	{
		PerlinPerm();
		impl::cec::SetBuffer(128);
	}

	//键盘
	{
		gCursor = &GEPrc::LastMsg.pt;
		gKeysTest = GEInp::KeysState.psF0;
	}

	//等待上传

	Thread.wait();
	gMyLitLib.EnableUpdate(B_False, B_False);
}
$VOID mgFreeAsset(){
	for(UNS32 iExe = 0; iExe < gMyImages.GetSize(); ++iExe){
		ID2D1Bitmap1 *pBitmap = *gMyImages.GetAt(iExe);
		pBitmap->Release();
	}
	for(UNS32 iExe = 0; iExe < gMyFonts.GetSize(); ++iExe){
		IDWriteTextFormat *pFormat = *gMyFonts.GetAt(iExe);
		pFormat->Release();
	}

	SAFE_RELEASE(gMyOption);
	SAFE_RELEASE(gMyRoster);

	SAFE_DELETEA(gMyPsoArr);
	SAFE_DELETEA(gMyShaderArr);

	SAFE_DELETEA(gMyAnimalClsArr);
	SAFE_DELETEA(gMyStructClsArr);

	gMyAnimals.clear();
	gMyStructs.clear();

	gMyFonts.Clear(B_True);
	gMyImages.Clear(B_True);
	gMySprites.Clear(B_True);
	gMyModels[0].Clear(B_True);
	gMyModels[1].Clear(B_True);

	gMyLitLib.Release();
	gMyMtlLib.Release();
	gMyTxtLib.Release();

	gMyRender.Release();
}

//----------------------------------------//