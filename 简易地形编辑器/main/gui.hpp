//-------------------- 全局变量 --------------------//

MGPortal *gMyPortal;
MGMaster *gMyMaster;
MGEditor *gMyEditor;

//----------------------------------------//


//-------------------- 函数声明 --------------------//

CPanel *mgInitPortalGui();
CPanel *mgInitMasterGui();
CPanel *mgInitEditorGui();

//----------------------------------------//


//-------------------- 辅助函数 --------------------//

inline $VOID CenterTheCursor(){
	SetCursorPos(gMyWndEx.Width / 2, gMyWndEx.Height / 2);
}
inline IBOOL ConfirmMessage(_in WCHAR *psMessage){
	CenterTheCursor();
	INT MsgCode = MessageBoxW(gMyWindow, psMessage, L"Hint", MB_OKCANCEL);
	return MsgCode == IDOK;
}

//----------------------------------------//


//-------------------- 绘制回调 --------------------//

$VOID _CBACK mgDrawClock(CWidget *pCtrl, _in RECTF &rcDest){
	RECTF rcInner = rcDest;
	SPFPN dxMargin = pCtrl->SizeX * 0.2f;

	rcInner.top += dxMargin;
	rcInner.left += dxMargin;
	rcInner.right -= dxMargin;
	rcInner.bottom -= dxMargin;

	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Ico-Clock"), rcDest);
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Ico-Moon"), rcInner);
}
$VOID _CBACK mgDrawMinimap(CWidget *pCtrl, _in RECTF &rcDest){
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.8f, 0.8f, 0.8f));
	GE2d::lpD2dDevCtx->FillRectangle(rcDest, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.f, 0.f, 0.5f));
	GE2d::lpD2dDevCtx->DrawRectangle(rcDest, GE2d::lpPureBrush, 1.f);
}
$VOID _CBACK mgDrawButton0(CWidget *pCtrl, _in RECTF &rcDest){
	CButton *pButton = (CButton*)pCtrl;
	CLabel *pLabel = pButton->lpLabel;

	if(pCtrl->IsFocused()){
		pLabel->TxtColor = 0xFF0000;
		pButton->DrawLabel(rcDest);
	} else if(pCtrl->IsHighkey()){
		pLabel->TxtColor = 0xFFFF00;
		pButton->DrawLabel(rcDest);
	} else{
		pLabel->TxtColor = 0xFFFFFF;
		pButton->DrawLabel(rcDest);
	}
}
$VOID _CBACK mgDrawButton1(CWidget *pCtrl, _in RECTF &rcDest){
	if(!pCtrl->IsFocused()){
		((CButton*)pCtrl)->DrawBackground(rcDest);
	} else{
		RECTF rcDest0 = rcDest;
		SPFPN dxMargin = pCtrl->SizeX * 0.04f;

		rcDest0.top += dxMargin;
		rcDest0.left += dxMargin;
		rcDest0.right -= dxMargin;
		rcDest0.bottom -= dxMargin;

		((CButton*)pCtrl)->DrawBackground(rcDest0);
	}
}
$VOID _CBACK mgDrawButton2(CWidget *pCtrl, _in RECTF &rcDest){
	D2DX1COLOR BgColor0(0.4f, 0.4f, 0.8f);
	D2DX1COLOR BgColor1(0.f, 0.12f, 0.7f);
	D2DX1COLOR BgColor = pCtrl->IsFocused() ? BgColor0 : BgColor1;

	GE2d::lpPureBrush->SetColor(BgColor);
	GE2d::lpD2dDevCtx->FillRectangle(rcDest, GE2d::lpPureBrush);
	((CButton*)pCtrl)->DrawBackground(rcDest);
}
$VOID _CBACK mgDrawResInfo0(CWidget *pCtrl, _in RECTF &rcDest){
	RECTF rcPart0 = rcDest, rcPart1 = rcDest;
	rcPart0.left = rcPart0.right - pCtrl->SizeY;
	rcPart1.right = rcPart0.left + 2.f;
	rcPart1.bottom -= pCtrl->SizeY * 0.2f;

	GE2d::lpPureBrush->SetColor(D2DX1COLOR(1.f, 1.f, 1.f));
	GE2d::lpD2dDevCtx->FillRectangle(rcPart0, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.1f, 0.1f, 0.1f));
	GE2d::lpD2dDevCtx->DrawRectangle(rcPart0, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.5f, 0.5f, 0.5f));
	GE2d::lpD2dDevCtx->FillRectangle(rcPart1, GE2d::lpPureBrush);
}
$VOID _CBACK mgDrawResInfo1(CWidget *pCtrl, _in RECTF &rcDest){
	//条目底色

	GE2d::lpPureBrush->SetOpacity(0.6f);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.6f, 0.6f, 0.6f));
	GE2d::lpD2dDevCtx->FillRectangle(rcDest, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetOpacity(1.f);

	//资源图标

	RECTF rcPart = rcDest;
	rcPart.left += pCtrl->SizeY + 4.f;
	rcPart.right = rcPart.left + pCtrl->SizeY;
	DrawBitmap(GE2d::lpD2dDevCtx, ((CPanel*)pCtrl)->pBgImage, rcPart);
}
$VOID _CBACK mgDrawResInfo2(CWidget *pCtrl, _in RECTF &rcDest){
	RECTF rcPart0 = rcDest, rcPart1 = rcDest;
	rcPart0.right = rcPart0.left + pCtrl->SizeY;
	rcPart1.left = rcPart0.right + 4.f;
	rcPart1.right = rcPart1.left + pCtrl->SizeY;

	//条目底色

	GE2d::lpPureBrush->SetOpacity(0.6f);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.6f, 0.6f, 0.6f));
	GE2d::lpD2dDevCtx->FillRectangle(rcDest, GE2d::lpPureBrush);

	//参与者数量

	GE2d::lpPureBrush->SetOpacity(1.f);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.1f, 0.1f, 0.1f));
	GE2d::lpD2dDevCtx->DrawRectangle(rcPart0, GE2d::lpPureBrush);

	//资源图标

	DrawBitmap(GE2d::lpD2dDevCtx, ((CPanel*)pCtrl)->pBgImage, rcPart1);
}
$VOID _CBACK mgDrawMenuPanel(CWidget *pCtrl, _in RECTF &rcDest){
	D2D1ROUNDEDRECT rcDest0 = { rcDest, 6.f, 6.f };
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.14f, 0.16f, 0.18f));
	GE2d::lpPureBrush->SetOpacity(0.8f);
	GE2d::lpD2dDevCtx->FillRoundedRectangle(rcDest0, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetOpacity(1.f);
	GE2d::lpD2dDevCtx->DrawRoundedRectangle(rcDest0, GE2d::lpPureBrush, 2.f);
}
$VOID _CBACK mgDrawEditPanel(CWidget *pCtrl, _in RECTF &rcDest){
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Px-Panel"), rcDest);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.1f, 0.1f, 0.1f));
	GE2d::lpD2dDevCtx->DrawRectangle(rcDest, GE2d::lpPureBrush, 2.f);
}
$VOID _CBACK mgDrawMainPanel(CWidget *pCtrl, _in RECTF &rcDest){
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.6f, 0.62f, 0.66f));
	GE2d::lpD2dDevCtx->FillRectangle(rcDest, GE2d::lpPureBrush);
	GE2d::lpPureBrush->SetColor(D2DX1COLOR(0.14f, 0.16f, 0.18f));
	GE2d::lpD2dDevCtx->DrawRectangle(rcDest, GE2d::lpPureBrush, 2.f);
}
$VOID _CBACK mgDrawListItem(CWidget *pCtrl, _in RECTF &rcDest){
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Px-ListItem"), rcDest);
	CListBox::DrawDefText(pCtrl, rcDest);
}
$VOID _CBACK mgDrawListFocus(CWidget *pCtrl, _in RECTF &rcDest){
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Px-ListFocus"), rcDest);
	CListBox::DrawDefText(pCtrl, rcDest);
}
$VOID _CBACK mgDrawComboItem(CWidget *pCtrl, _in RECTF &rcDest){
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Px-CmbItem"), rcDest);
	CListBox::DrawDefText(pCtrl, rcDest);
}
$VOID _CBACK mgDrawComboFocus(CWidget *pCtrl, _in RECTF &rcDest){
	DrawBitmap(GE2d::lpD2dDevCtx, mgGetImage("Px-CmbFocus"), rcDest);
	CListBox::DrawDefText(pCtrl, rcDest);
}

//----------------------------------------//


//-------------------- 消息回调 --------------------//

$VOID _CBACK mgProcPortalGui(CWidget *pCtrl, DWORD MsgCode, WPARAM Param0, LPARAM Param1){
	if(MsgCode != WM_LBUTTONUP) return;

	switch(pCtrl->Ident){
		case ID_BTN_Start:{
			delete gMyGuiRoot;
			delete gMyAppPage;
			gMyGuiRoot = mgInitMasterGui();
			gMyAppPage = gMyMaster = new MGMaster;
			break;
		}
		case ID_BTN_Edit:{
			delete gMyGuiRoot;
			delete gMyAppPage;
			gMyGuiRoot = mgInitEditorGui();
			gMyAppPage = gMyEditor = new MGEditor;
			break;
		}
		case ID_BTN_Quit:{
			if(ConfirmMessage(L"Quit game?"))
				PostQuitMessage(0);
			break;
		}
	}
}
$VOID _CBACK mgProcMasterGui(CWidget *pCtrl, DWORD MsgCode, WPARAM Param0, LPARAM Param1){
	if(MsgCode != WM_LBUTTONDOWN) return;

	switch(pCtrl->Ident){
		case ID_BTN_Quit:{
			if(ConfirmMessage(L"Back to home?")){
				delete gMyGuiRoot;
				delete gMyAppPage;
				gMyGuiRoot = mgInitPortalGui();
				gMyAppPage = gMyPortal = new MGPortal;
			}
			break;
		}
	}
}
$VOID _CBACK mgProcEditorGui(CWidget *pCtrl, DWORD MsgCode, WPARAM Param0, LPARAM Param1){
	static WORDT tTerrMode = MGEditor::MODE_Uplift;
	static WORDT tBrushFunc = MGEditor::MODE_EditColor;

	auto pfnPainting = []()->IBOOL{
		if(gMyEditor->FuncMode == MGEditor::MODE_EditColor)
			return B_True;
		if(gMyEditor->FuncMode == MGEditor::MODE_EditPattern)
			return B_True;

		return B_False;
	};
	auto pfnUnselect = [](DWORD Filter){
		CLabel *pLabel;
		CBtnGroup *pBtnGrp;

		if(Filter != ID_GRP_SculptScape){
			pBtnGrp = mgGetRadios(ID_GRP_SculptScape);
			pLabel = mgGetLabel(ID_LBL_SculptScape);
			pBtnGrp->CheckItem(UINT_MAX);
			pLabel->SetValue(L"");
		}
		if(Filter != ID_GRP_PaintScape){
			pBtnGrp = mgGetRadios(ID_GRP_PaintScape);
			pLabel = mgGetLabel(ID_LBL_PaintScape);
			pBtnGrp->CheckItem(UINT_MAX);
			pLabel->SetValue(L"");
		}
		if(Filter != ID_GRP_MakeScape){
			pBtnGrp = mgGetRadios(ID_GRP_MakeScape);
			pLabel = mgGetLabel(ID_LBL_MakeScape);
			pBtnGrp->CheckItem(UINT_MAX);
			pLabel->SetValue(L"");
		}
	};
	auto pfnPrompt = [](DWORD TipId){
		CLabel *pLabel = mgGetLabel(ID_LBL_ToolTip);
		pLabel->SetValue(mgGetText(TipId));
		pLabel->PosY = gMyWndH - pLabel->PosX - pLabel->SizeY;
	};

	if(MsgCode != WM_LBUTTONDOWN)
		return;

	switch(pCtrl->Ident){
		case ID_BTN_New:{
			// To do
		} break;
		case ID_BTN_Set:{
			// To do
		} break;
		case ID_BTN_Open:{
			// To do
		} break;
		case ID_BTN_Save:{
			// To do
		} break;
		case ID_BTN_SaveAs:{
			// To do
		} break;
		case ID_BTN_Quit:{
			if(ConfirmMessage(L"Back to home?")){
				delete gMyGuiRoot;
				delete gMyAppPage;
				gMyGuiRoot = mgInitPortalGui();
				gMyAppPage = gMyPortal = new MGPortal;
			}
		} break;
		case ID_BTN_BrushSize:{
			const WCHAR *psValue = pCtrl->GetValue();
			gMyEditor->SetBrushSize(WtoI(psValue));
			mgGetLabel(ID_LBL_BrushSize)->SetValue(psValue);
			pfnPrompt(IDS_Hint8);
		} break;
		case ID_BTN_BrushPower:{
			const WCHAR *psValue = pCtrl->GetValue();
			gMyEditor->SetBrushPower((SPFPN)WtoD(psValue));
			mgGetLabel(ID_LBL_BrushPower)->SetValue(psValue);
			pfnPrompt(IDS_Hint9);
		} break;
		case ID_RDO_RectBrush:{
			gMyEditor->SetBrushShape(MGEditor::BR_TYPE_Rect);
		} break;
		case ID_RDO_CircleBrush:{
			gMyEditor->SetBrushShape(MGEditor::BR_TYPE_Circle);
		} break;
		case ID_RDO_RhombusBrush:{
			gMyEditor->SetBrushShape(MGEditor::BR_TYPE_Rhombus);
		} break;
		case ID_RDO_Uplift:{
			pfnPrompt(IDS_Hint1);
			pfnUnselect(ID_GRP_SculptScape);
			tTerrMode = MGEditor::MODE_Uplift;
			gMyEditor->SetFunctionMode(MGEditor::MODE_Uplift);
			mgGetLabel(ID_LBL_SculptScape)->SetValue(mgGetText(IDS_Uplift));
		} break;
		case ID_RDO_Steepen:{
			pfnPrompt(IDS_Hint2);
			pfnUnselect(ID_GRP_SculptScape);
			tTerrMode = MGEditor::MODE_Steepen;
			gMyEditor->SetFunctionMode(MGEditor::MODE_Steepen);
			mgGetLabel(ID_LBL_SculptScape)->SetValue(mgGetText(IDS_Steepen));
		} break;
		case ID_RDO_Flatten:{
			pfnPrompt(IDS_Hint3);
			pfnUnselect(ID_GRP_SculptScape);
			tTerrMode = MGEditor::MODE_Uniform;
			gMyEditor->SetFunctionMode(MGEditor::MODE_Uniform);
			mgGetLabel(ID_LBL_SculptScape)->SetValue(mgGetText(IDS_Uniform));
		} break;
		case ID_RDO_Sharpen:{
			pfnPrompt(IDS_Hint4);
			pfnUnselect(ID_GRP_SculptScape);
			tTerrMode = MGEditor::MODE_Sharpen;
			gMyEditor->SetFunctionMode(MGEditor::MODE_Sharpen);
			mgGetLabel(ID_LBL_SculptScape)->SetValue(mgGetText(IDS_Sharpen));
		} break;
		case ID_RDO_Wiring:{
			pfnPrompt(IDS_Hint5);
			pfnUnselect(ID_GRP_SculptScape);
			tTerrMode = MGEditor::MODE_Wiring;
			gMyEditor->SetFunctionMode(MGEditor::MODE_Wiring);
			mgGetLabel(ID_LBL_SculptScape)->SetValue(mgGetText(IDS_Wiring));
		} break;
		case ID_RDO_EditTex:{
			pfnPrompt(IDS_Hint0);
			pfnUnselect(ID_GRP_PaintScape);
			tTerrMode = tBrushFunc;
			gMyEditor->SetFunctionMode(tBrushFunc);
			mgGetLabel(ID_LBL_PaintScape)->SetValue(mgGetText(IDS_Texture));
		} break;
		case ID_RDO_EditMetal:{
			pfnPrompt(IDS_Hint0);
			pfnUnselect(ID_GRP_PaintScape);
			tTerrMode = MGEditor::MODE_EditMetal;
			gMyEditor->SetFunctionMode(MGEditor::MODE_EditMetal);
			mgGetLabel(ID_LBL_PaintScape)->SetValue(mgGetText(IDS_Metalness));
		} break;
		case ID_RDO_EditRough:{
			pfnPrompt(IDS_Hint0);
			pfnUnselect(ID_GRP_PaintScape);
			tTerrMode = MGEditor::MODE_EditRough;
			gMyEditor->SetFunctionMode(MGEditor::MODE_EditRough);
			mgGetLabel(ID_LBL_PaintScape)->SetValue(mgGetText(IDS_Roughness));
		} break;
		case ID_RDO_MakeClosed:{
			pfnPrompt(IDS_Hint6);
			pfnUnselect(ID_GRP_MakeScape);
			tTerrMode = MGEditor::MODE_MakeClosed;
			gMyEditor->SetFunctionMode(MGEditor::MODE_MakeClosed);
			mgGetLabel(ID_LBL_MakeScape)->SetValue(mgGetText(IDS_Closed));
		} break;
		case ID_RDO_MakeWater:{
			pfnPrompt(IDS_Hint6);
			pfnUnselect(ID_GRP_MakeScape);
			tTerrMode = MGEditor::MODE_MakeWater;
			gMyEditor->SetFunctionMode(MGEditor::MODE_MakeWater);
			mgGetLabel(ID_LBL_MakeScape)->SetValue(mgGetText(IDS_Water));
		} break;
		case ID_RDO_MakeAbyss:{
			pfnPrompt(IDS_Hint6);
			pfnUnselect(ID_GRP_MakeScape);
			tTerrMode = MGEditor::MODE_MakeAbyss;
			gMyEditor->SetFunctionMode(MGEditor::MODE_MakeAbyss);
			mgGetLabel(ID_LBL_MakeScape)->SetValue(mgGetText(IDS_Abyss));
		} break;
		case ID_RDO_MakeCliff:{
			pfnPrompt(IDS_Hint7);
			pfnUnselect(ID_GRP_MakeScape);
			tTerrMode = MGEditor::MODE_MakeCliff;
			gMyEditor->SetFunctionMode(MGEditor::MODE_MakeCliff);
			mgGetLabel(ID_LBL_MakeScape)->SetValue(mgGetText(IDS_Cliff));
		} break;
		case ID_LST_BrushPattern:{
			CListBox::ITEM *pItem = ((CListBox*)pCtrl)->pFocusItem;
			CListBox *pMutex = mgGetListBox(ID_LST_BrushColor);
			if(!pItem) break;
			pMutex->pFocusItem = P_Null;
			gMyEditor->SetBrushMapping(pItem->Number);
			tBrushFunc = MGEditor::MODE_EditPattern;
			if(pfnPainting()) gMyEditor->SetFunctionMode(tBrushFunc);
		} break;
		case ID_LST_BrushColor:{
			CListBox::ITEM *pItem = ((CListBox*)pCtrl)->pFocusItem;
			CListBox *pMutex = mgGetListBox(ID_LST_BrushPattern);
			if(!pItem) break;
			pMutex->pFocusItem = P_Null;
			tBrushFunc = MGEditor::MODE_EditColor;
			if(pfnPainting()) gMyEditor->SetFunctionMode(tBrushFunc);
			if(pItem->Ident != 0) gMyEditor->SetBrushColor(pItem->Color);
		} break;
		case ID_LST_Decorat:{
			DWORD ObjId = ((CListBox*)pCtrl)->GetChoice();
			DWORD ObjType = mgGetComboBox(ID_CMB_DecorType)->GetChoice();

			switch(ObjType){
				case UINT_MAX:
				case 0:{
					gMyEditor->SetFunctionMode(MGEditor::MODE_MakeRoad);
				} break;
				default:{
					gMyEditor->SetFunctionMode(MGEditor::MODE_MakeDecor);
					gMyEditor->SetPreviewObject(ObjId);
				}
			}			
		} break;
		case ID_LST_Actor:{
			DWORD ObjId = ((CListBox*)pCtrl)->GetChoice();
			gMyEditor->SetFunctionMode(MGEditor::MODE_AddUnit);
			gMyEditor->SetPreviewObject(ObjId);
		} break;
		case ID_CMB_SceneType:{
			// To do
		} break;
		case ID_CMB_DecorType:{
			CHAR8 *psType = W_TO_A(pCtrl->GetValue());
			CListBox *pList = mgGetListBox(ID_LST_Decorat);
			CJsonNode *pItems = gMyRoster->GetChild("decoration");
			CJsonNode *pItem = pItems->GetChild(psType)->GetChild();

			pList->ClearItems();
			for(; pItem; pItem = pItem->GetNext()){
				USINT Ident = StrHash32(pItem->GetValS());
				WCHAR *psText = A_TO_W(pItem->GetKey());
				pList->AddTextItem(psText, Ident);
			}
		} break;
		case ID_CMB_ActorTeam:{
			// To do
		} break;
		case ID_CMB_ActorEthnic:{
			CHAR8 *psType = W_TO_A(pCtrl->GetValue());
			CListBox *pList = mgGetListBox(ID_LST_Actor);
			CJsonNode *pItems = gMyRoster->GetChild("actor");

			pList->ClearItems();
			pItems = pItems->GetChild(psType)->GetChild();

			for(; pItems; pItems = pItems->GetNext()){
				CJsonNode *pItem = pItems->GetChild();
				for(; pItem; pItem = pItem->GetNext()){
					USINT Ident = StrHash32(pItem->GetValS());
					WCHAR *psText = A_TO_W(pItem->GetKey());
					pList->AddTextItem(psText, Ident);
				}
			}
		} break;
		case ID_CMB_Module:{
			CComboBox *pCmbBox = (CComboBox*)pCtrl->pParent;
			switch(pCmbBox->GetChoice()){
				case IDS_TerrModule:{
					mgGetWidget(ID_PNL_ActorTool)->Hide(B_True);
					mgGetWidget(ID_PNL_TerrTool)->Hide(B_False);
					mgGetWidget(ID_PNL_DecorTool)->Hide(B_True);

					if(!gMyEditor->InLandEditMode())
						gMyEditor->SetFunctionMode(tTerrMode);
				} break;
				case IDS_DecorModule:{
					mgGetWidget(ID_PNL_ActorTool)->Hide(B_True);
					mgGetWidget(ID_PNL_TerrTool)->Hide(B_True);
					mgGetWidget(ID_PNL_DecorTool)->Hide(B_False);

					if(!gMyEditor->InSceneSetupMode())
						gMyEditor->SetFunctionMode(MGEditor::MODE_None);
				} break;
				case IDS_ActorModule:{
					mgGetWidget(ID_PNL_ActorTool)->Hide(B_False);
					mgGetWidget(ID_PNL_TerrTool)->Hide(B_True);
					mgGetWidget(ID_PNL_DecorTool)->Hide(B_True);

					if(!gMyEditor->InActorSetupMode())
						gMyEditor->SetFunctionMode(MGEditor::MODE_AdjustActor);
				} break;
			}
		} break;
	}
}

//----------------------------------------//


//-------------------- 模块细分 --------------------//

CPanel *mgInitPortalGui(){
	//顶层面板

	USINT NumCtrl = 3;
	CPanel *pTopPanel;
	CPanel::DESC PnlDesc;

	PnlDesc.SizeX = gMyWndH * 0.28f;
	PnlDesc.SizeY = PnlDesc.SizeX * NumCtrl * 0.25f;
	PnlDesc.PosX = (gMyWndW - PnlDesc.SizeX) * 0.5f;
	PnlDesc.PosY = (gMyWndH - PnlDesc.SizeY) * 0.5f;
	PnlDesc.pfnDrawUdf = mgDrawMenuPanel;
	PnlDesc.pfnRespond = mgProcPortalGui;
	pTopPanel = new CPanel(PnlDesc);

	//子模块

	CLabel::DESC LblDesc;
	CButton::DESC BtnDesc;

	USINT rgCtlId[] = { ID_BTN_Start, ID_BTN_Edit, ID_BTN_Quit };
	USINT rgTxtId[] = { IDS_StartGame, IDS_EditScene, IDS_QuitApp };

	BtnDesc.SizeX = PnlDesc.SizeX;
	BtnDesc.SizeY = PnlDesc.SizeY / NumCtrl;
	BtnDesc.pfnDrawUdf = mgDrawButton0;
	BtnDesc.pLblDesc = &LblDesc;

	mgAddFont("TF-Home", BtnDesc.SizeY * 0.6f);
	LblDesc.pTxtFormat = mgGetFont("TF-Home");

	for(USINT iCtl = 0; iCtl < NumCtrl; ++iCtl){
		BtnDesc.Ident = rgCtlId[iCtl];
		LblDesc.psText = mgGetText(rgTxtId[iCtl]);
		pTopPanel->AddChild(new CButton(BtnDesc));
		BtnDesc.PosY += BtnDesc.SizeY;
	}

	//返回

	return pTopPanel;
}
CPanel *mgInitMasterGui(){
	SPFPN EdgeSize = 2.f;
	SPFPN EdgeDistX = 12.f;
	CPanel *pTopPanel = P_Null;

	auto pfnInitCmdBar = [&](SPFPN MaxX)->SPFPN{
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = MaxX - gMyPanelH;
		PnlDesc.PosY = 0.f;
		PnlDesc.SizeX = gMyPanelH;
		PnlDesc.SizeY = gMyPanelH;
		PnlDesc.pfnDrawUdf = mgDrawMainPanel;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		//按钮
		{
			SPFPN GridSize = (gMyPanelH - EdgeDistX) / 4.f;
			SPFPN CtlSpan = 4.f;
			SPFPN CtlSize = GridSize - CtlSpan;
			SPFPN BasePos = (EdgeDistX + CtlSpan) * 0.5f;

			CButton::DESC CtlDesc = { 0, BasePos, BasePos, CtlSize, CtlSize,
				1, BFM_Fill, P_Null, mgDrawButton2, P_Null };

			CHAR8 *rgImgId[] = {
				"Ico-Move", "Ico-Stop", "Ico-Defense", "Ico-Attack",
				"Ico-Patrol", P_Null, P_Null, P_Null,
				P_Null, P_Null, P_Null, P_Null, P_Null, P_Null, P_Null, P_Null };

			for(USINT iRow = 0; iRow < 4; ++iRow){
				for(USINT iCol = 0; iCol < 4; ++iCol){
					CtlDesc.pBgImage = mgGetImage(rgImgId[iRow*4 + iCol]);
					pPanel->AddChild(new CButton(CtlDesc));
					CtlDesc.PosX += GridSize;
				}
				CtlDesc.PosX = BasePos;
				CtlDesc.PosY += GridSize;
			}
		}

		//返回

		return PnlDesc.PosX - EdgeSize;
	};
	auto pfnInitItemBar = [&](SPFPN MaxX)->SPFPN{
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.SizeX = gMyPanelH * 0.66f;
		PnlDesc.SizeY = gMyPanelH;
		PnlDesc.PosX = MaxX - PnlDesc.SizeX;
		PnlDesc.PosY = 0.f;
		PnlDesc.pfnDrawUdf = mgDrawMainPanel;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		//公共数据

		SPFPN GridSize = (PnlDesc.SizeX - EdgeDistX) / 3.f;
		SPFPN CtlSpan = 4.f;
		SPFPN CtlSize = GridSize - CtlSpan;
		SPFPN CenterX = PnlDesc.SizeX * 0.5f;
		SPFPN RuleY = gMyPanelH - EdgeDistX - (GridSize * 4.f);
		SPFPN Dist0 = (EdgeDistX + CtlSpan) * 0.5f;
		SPFPN Dist1 = RuleY + Dist0;

		//切页按钮
		{
			CButton::DESC CtlDesc;
			CtlDesc.SizeX = PnlDesc.SizeX * 0.32f;
			CtlDesc.SizeY = RuleY - Dist0;
			CtlDesc.BkgdMode = BFM_Contain;
			CtlDesc.pfnDrawUdf = mgDrawButton2;

			CtlDesc.PosY = Dist0;
			CtlDesc.PosX = CenterX - CtlSpan - CtlDesc.SizeX;
			CtlDesc.pBgImage = mgGetImage("Ico-Prev");
			pPanel->AddChild(new CButton(CtlDesc));

			CtlDesc.PosX = CenterX + CtlSpan;
			CtlDesc.pBgImage = mgGetImage("Ico-Next");
			pPanel->AddChild(new CButton(CtlDesc));
		}

		//物品按钮
		{
			CButton::DESC CtlDesc = { 0, Dist0, Dist1, CtlSize, CtlSize,
				1, BFM_Fill, P_Null, mgDrawButton2, P_Null };

			CHAR8 *rgImgId[] = {
				P_Null, P_Null, P_Null, P_Null, P_Null, P_Null,
				P_Null, P_Null, P_Null, P_Null, P_Null, P_Null, };

			for(USINT iRow = 0; iRow < 4; ++iRow){
				for(USINT iCol = 0; iCol < 3; ++iCol){
					CtlDesc.pBgImage = mgGetImage(rgImgId[iRow*3 + iCol]);
					pPanel->AddChild(new CButton(CtlDesc));
					CtlDesc.PosX += GridSize;
				}
				CtlDesc.PosX = Dist0;
				CtlDesc.PosY += GridSize;
			}
		}

		//返回

		return PnlDesc.PosX - EdgeSize;
	};
	auto pfnInitPropBar = [&](SPFPN MaxX)->SPFPN{
		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.SizeY = gMyPanelH;
		PnlDesc.SizeX = gMyPanelH * 2.4f;
		PnlDesc.PosX = MaxX - PnlDesc.SizeX;
		PnlDesc.PosY = 0.f;
		PnlDesc.pfnDrawUdf = mgDrawMainPanel;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		return PnlDesc.PosX - EdgeSize;
	};
	auto pfnInitResBar = [&](SPFPN MinX)->SPFPN{
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = MinX + 10.f;
		PnlDesc.PosY = 0.f;
		PnlDesc.SizeX = gMyPanelH * 0.5f;
		PnlDesc.SizeY = gMyPanelH;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		//主城按钮
		{
			CButton::DESC CtlDesc;
			CtlDesc.SizeX = PnlDesc.SizeX;
			CtlDesc.SizeY = PnlDesc.SizeX * 0.5f;
			CtlDesc.pfnDrawUdf = mgDrawButton1;
			CtlDesc.pBgImage = mgGetImage("Britain");
			pPanel->AddChild(new CButton(CtlDesc));
		}

		//资源状态
		{
			SPFPN BaseY = PnlDesc.SizeX * 0.5f;
			SPFPN RowDimY = (PnlDesc.SizeY - BaseY) * 0.2f;

			CPanel::DESC CtlDesc;
			CtlDesc.PosX = 0.f;
			CtlDesc.PosY = BaseY + 2.f;
			CtlDesc.SizeX = PnlDesc.SizeX;
			CtlDesc.SizeY = RowDimY * 0.6f;

			CWidget::PFNDRAWUDF rgCback[] = {
				mgDrawResInfo0, mgDrawResInfo1,
				mgDrawResInfo2, mgDrawResInfo2, mgDrawResInfo2 };

			ID2DBitmap *rgImage[] = { P_Null,
				mgGetImage("Ico-House"), mgGetImage("Ico-Food"),
				mgGetImage("Ico-Wood"), mgGetImage("Ico-Gold") };

			for(USINT iCtl = 0; iCtl < 5; ++iCtl){
				CtlDesc.pBgImage = rgImage[iCtl];
				CtlDesc.pfnDrawUdf = rgCback[iCtl];
				pPanel->AddChild(new CPanel(CtlDesc));
				CtlDesc.PosY += RowDimY;
			}
		}

		//返回

		return PnlDesc.PosX + PnlDesc.SizeX;
	};
	auto pfnInitTaskBar = [&](SPFPN MinX)->SPFPN{
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = MinX + 10.f;
		PnlDesc.PosY = 0.f;
		PnlDesc.SizeX = gMyPanelH * 0.4f;
		PnlDesc.SizeY = gMyPanelH;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		//时钟
		{
			CPanel::DESC CtlDesc;
			CtlDesc.PosX = 0.f;
			CtlDesc.PosY = 0.f;
			CtlDesc.SizeX = PnlDesc.SizeX;
			CtlDesc.SizeY = PnlDesc.SizeX;
			CtlDesc.pfnDrawUdf = mgDrawClock;
			pPanel->AddChild(new CPanel(CtlDesc));
		}

		//按钮
		{
			CButton::DESC CtlDesc;
			CtlDesc.SizeX = PnlDesc.SizeX * 0.5f;
			CtlDesc.SizeY = CtlDesc.SizeX;
			CtlDesc.pfnDrawUdf = mgDrawButton1;
			CtlDesc.pBgImage = mgGetImage("Ico-Circle");

			CtlDesc.Ident = 0;
			CtlDesc.PosY = PnlDesc.SizeX + (CtlDesc.SizeY * 0.5f);
			pPanel->AddChild(new CButton(CtlDesc));

			CtlDesc.Ident = 0;
			CtlDesc.PosY += CtlDesc.SizeY;
			pPanel->AddChild(new CButton(CtlDesc));

			CtlDesc.Ident = 0;
			CtlDesc.PosX = CtlDesc.SizeX + 1.f;
			CtlDesc.PosY = PnlDesc.SizeX + 1.f;
			pPanel->AddChild(new CButton(CtlDesc));

			CtlDesc.Ident = 0;
			CtlDesc.PosY += CtlDesc.SizeY;
			pPanel->AddChild(new CButton(CtlDesc));

			CtlDesc.Ident = ID_BTN_Quit;
			CtlDesc.PosY += CtlDesc.SizeY;
			pPanel->AddChild(new CButton(CtlDesc));
		}

		//返回

		return PnlDesc.PosX + PnlDesc.SizeX;
	};
	auto pfnInitMapMenu = [&](SPFPN MinX)->SPFPN{
		SPFPN CtlSpan = 4.f;
		SPFPN GridSize = gMyPanelH * 0.12f;

		CButton::DESC CtlDesc;
		CtlDesc.PosY = CtlSpan * 0.5f;
		CtlDesc.PosX = MinX + CtlDesc.PosY;
		CtlDesc.SizeX = GridSize - CtlSpan;
		CtlDesc.SizeY = CtlDesc.SizeX;
		CtlDesc.pfnDrawUdf = mgDrawButton2;

		CHAR8 *rgImgId[] = { P_Null, P_Null, P_Null, P_Null };

		for(USINT iCtl = 0; iCtl < 4; ++iCtl){
			CtlDesc.pBgImage = mgGetImage(rgImgId[iCtl]);
			pTopPanel->AddChild(new CButton(CtlDesc));
			CtlDesc.PosY += GridSize;
		}

		return GridSize;
	};
	auto pfnInitMinimap = [&](SPFPN MinX)->SPFPN{
		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = MinX + 1.f;
		PnlDesc.PosY = 0.f;
		PnlDesc.SizeX = gMyPanelH;
		PnlDesc.SizeY = gMyPanelH;
		PnlDesc.pfnDrawUdf = mgDrawMinimap;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		return PnlDesc.PosX + PnlDesc.SizeX;
	};

	//顶层面板

	CPanel::DESC PnlDesc;
	PnlDesc.PosX = 0.f;
	PnlDesc.PosY = gMyWndH - gMyPanelH;
	PnlDesc.SizeX = gMyWndW;
	PnlDesc.SizeY = gMyPanelH;
	PnlDesc.pfnRespond = mgProcMasterGui;

	pTopPanel = new CPanel(PnlDesc);

	//子模块

	SPFPN MaxX = gMyWndW;
	MaxX = pfnInitCmdBar(MaxX);
	MaxX = pfnInitItemBar(MaxX);
	MaxX = pfnInitPropBar(MaxX);
	SPFPN MinX = 0;
	MinX = pfnInitMapMenu(MinX);
	MinX = pfnInitMinimap(MinX);
	MinX = pfnInitResBar(MinX);
	MinX = pfnInitTaskBar(MinX);

	//返回顶层面板

	return pTopPanel;
}
CPanel *mgInitEditorGui(){
	SPFPN FontSize = 0.f;
	SPFPN EdgeDistY = 8.f;
	SPFPN EdgeDistX = 12.f;
	CPanel *pTopPanel = P_Null;
	IDWTxtFormat *pTxtFmt = P_Null;

	auto pfnInitFileMenu = [&](SPFPN MinY)->SPFPN{
		//按钮

		USINT rgTxtId[] = { IDS_New, IDS_Open, IDS_Quit, IDS_Save, IDS_SaveAs, IDS_Set };
		USINT rgCtlId[] = { ID_BTN_New, ID_BTN_Open, ID_BTN_Quit, ID_BTN_Save, ID_BTN_SaveAs, ID_BTN_Set };

		USINT NumCtrl = ARR_LEN(rgCtlId);
		USINT NumCol = 3;

		SPFPN CtlSpan = 4.f;
		SPFPN GridDimX = (gMyPanelW - (EdgeDistX * 2.f)) / NumCol;
		SPFPN GridDimY = (FontSize * 1.4f) + CtlSpan;
		SPFPN BaseX = EdgeDistX + (CtlSpan * 0.5f);
		SPFPN BaseY = MinY + EdgeDistY;

		CLabel::DESC LblDesc;
		CButton::DESC BtnDesc;

		BtnDesc.PosX = BaseX;
		BtnDesc.PosY = BaseY;
		BtnDesc.SizeX = GridDimX - CtlSpan;
		BtnDesc.SizeY = GridDimY - CtlSpan;
		BtnDesc.pLblDesc = &LblDesc;
		BtnDesc.pBgImage = mgGetImage("Px-Button");
		BtnDesc.BkgdCnt = 3;

		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;

		for(USINT iCol = 0, iCtl = 0; iCtl < NumCtrl; ++iCtl){
			if(iCol++ == NumCol){
				iCol = 0;
				BtnDesc.PosX = BaseX;
				BtnDesc.PosY += GridDimY;
			}
			BtnDesc.Ident = rgCtlId[iCtl];
			LblDesc.psText = mgGetText(rgTxtId[iCtl]);
			pTopPanel->AddChild(new CButton(BtnDesc));
			BtnDesc.PosX += GridDimX;
		}

		//分隔线

		CSeparator::DESC LineDesc;
		LineDesc.Color = 0xB0B0B0;
		LineDesc.StartX = EdgeDistX;
		LineDesc.StartY = BtnDesc.PosY + BtnDesc.SizeY + EdgeDistY;
		LineDesc.EndX = gMyPanelW - EdgeDistX;
		LineDesc.EndY = LineDesc.StartY;
		pTopPanel->AddChild(new CSeparator(LineDesc));

		//返回

		return LineDesc.EndY;
	};
	auto pfnInitToolList = [&](SPFPN MinY)->SPFPN{
		USINT rgTxtId[] = { IDS_TerrModule, IDS_DecorModule, IDS_ActorModule };
		USINT NumText = ARR_LEN(rgTxtId);

		CComboBox *pComboBox;
		CLabel::DESC LblDesc;
		CButton::DESC BtnDesc;
		CListBox::DESC ListDesc;
		CComboBox::DESC CmbDesc;

		BtnDesc.BkgdCnt = 3;
		BtnDesc.pBgImage = mgGetImage("Px-Button");

		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;
		LblDesc.psText = mgGetText(rgTxtId[0]);

		ListDesc.SizeY = FontSize * NumText * 1.4f;
		ListDesc.PageSizeY = NumText;
		ListDesc.TxtColor = 0xFFFFFF;
		ListDesc.pTxtFormat = pTxtFmt;
		ListDesc.pfnDrawItem = mgDrawComboItem;
		ListDesc.pfnDrawFocus = mgDrawComboFocus;

		CmbDesc.Ident = ID_CMB_Module;
		CmbDesc.SizeX = gMyPanelW * 0.6f;
		CmbDesc.SizeY = FontSize * 1.4f;
		CmbDesc.PosX = (gMyPanelW - CmbDesc.SizeX) * 0.5f;
		CmbDesc.PosY = MinY + EdgeDistY;
		CmbDesc.pLblDesc = &LblDesc;
		CmbDesc.pBtnDesc = &BtnDesc;
		CmbDesc.pListDesc = &ListDesc;

		pComboBox = new CComboBox(CmbDesc);
		pTopPanel->AddChild(pComboBox);

		for(USINT iTxt : rgTxtId)
			pComboBox->AddItem(mgGetText(iTxt), iTxt);

		return CmbDesc.PosY + CmbDesc.SizeY;
	};
	auto pfnInitTerrTool = [&](SPFPN MinY){
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = 0.f;
		PnlDesc.PosY = MinY + EdgeDistY;
		PnlDesc.Ident = ID_PNL_TerrTool;
		PnlDesc.SizeX = pTopPanel->SizeX;
		PnlDesc.SizeY = pTopPanel->SizeY - PnlDesc.PosY;

		pPanel = new CPanel(PnlDesc);
		pTopPanel->AddChild(pPanel);

		//公共数据

		SPFPN RowDimY = FontSize * 1.5f;
		SPFPN GridDimX = (gMyPanelW - (EdgeDistX * 2.f)) / 5.f;

		CLabel *rgLabel[16];
		CListBox *pListBox;
		CBtnGroup *pBtnGrp;

		CLabel::DESC LblDesc;
		CButton::DESC BtnDesc;
		CListBox::DESC ListDesc;

		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;

		BtnDesc.SizeY = FontSize * 1.2f;
		BtnDesc.SizeX = GridDimX - 2.f;
		BtnDesc.BkgdCnt = 3;
		BtnDesc.pBgImage = mgGetImage("Px-Button");
		BtnDesc.pLblDesc = &LblDesc;

		//标签
		{
			USINT rgTxtId[] = {
				IDS_SculptScape, IDS_PaintScape, IDS_MakeScape,
				IDS_MarqueeShape, IDS_MarqueeSize,
				IDS_BrushStrength, IDS_BrushColor, IDS_BrushPattern };

			LblDesc.PosY = 0.f;
			LblDesc.PosX = EdgeDistX;

			for(USINT iCtl = 0; iCtl < ARR_LEN(rgTxtId); ++iCtl){
				LblDesc.psText = mgGetText(rgTxtId[iCtl]);
				rgLabel[iCtl] = new CLabel(LblDesc);
				pPanel->AddChild(rgLabel[iCtl]);
				LblDesc.PosY += RowDimY * 2.f;
			}
		}

		//指示
		{
			LblDesc.psText = mgGetText(IDS_Uplift);
			LblDesc.Ident = ID_LBL_SculptScape;
			LblDesc.BufSize = 24;
			LblDesc.PosY = 0.f;
			LblDesc.PosX = EdgeDistX + rgLabel[0]->SizeX + 4.f;
			pPanel->AddChild(new CLabel(LblDesc));

			LblDesc.psText = L"";
			LblDesc.Ident = ID_LBL_PaintScape;
			LblDesc.BufSize = 24;
			LblDesc.PosY += RowDimY * 2.f;
			LblDesc.PosX = EdgeDistX + rgLabel[1]->SizeX + 4.f;
			pPanel->AddChild(new CLabel(LblDesc));

			LblDesc.psText = L"";
			LblDesc.Ident = ID_LBL_MakeScape;
			LblDesc.BufSize = 24;
			LblDesc.PosY += RowDimY * 2.f;
			LblDesc.PosX = EdgeDistX + rgLabel[2]->SizeX + 4.f;
			pPanel->AddChild(new CLabel(LblDesc));

			LblDesc.psText = L"4";
			LblDesc.Ident = ID_LBL_BrushSize;
			LblDesc.BufSize = 4;
			LblDesc.PosY += RowDimY * 4.f;
			LblDesc.PosX = EdgeDistX + rgLabel[5]->SizeX + 4.f;
			pPanel->AddChild(new CLabel(LblDesc));

			LblDesc.psText = L"0.5";
			LblDesc.Ident = ID_LBL_BrushPower;
			LblDesc.BufSize = 8;
			LblDesc.PosY += RowDimY * 2.f;
			LblDesc.PosX = EdgeDistX + rgLabel[6]->SizeX + 4.f;
			pPanel->AddChild(new CLabel(LblDesc));

			LblDesc.Ident = 0;
			LblDesc.BufSize = 0;
		}

		//按钮组
		{
			USINT rgGrpId[] = {
				ID_GRP_SculptScape, ID_GRP_PaintScape, ID_GRP_MakeScape, ID_GRP_BrushShape };
			USINT rgCtlId[][5] = {
				{ ID_RDO_Uplift, ID_RDO_Steepen, ID_RDO_Flatten, ID_RDO_Sharpen, ID_RDO_Wiring },
				{ ID_RDO_EditTex, ID_RDO_EditMetal, ID_RDO_EditRough, 0L, 0L },
				{ ID_RDO_MakeWater, ID_RDO_MakeCliff, ID_RDO_MakeAbyss, ID_RDO_MakeClosed },
				{ ID_RDO_RectBrush, ID_RDO_CircleBrush, ID_RDO_RhombusBrush, 0L, 0L } };
			WCHAR *rgCtlVal[][5] = {
				{ L"↥↥", L"↜↜", L"⏥", L"︿", L"☒"},
				{ L"Tx", L"Mt", L"Rg", P_Null, P_Null},
				{ L"≋≋", L"╓┐", L"┐╓", L"⛔"},
				{ L"▥", L"◍", L"❖", P_Null, P_Null} };

			BtnDesc.PosY = RowDimY;
			BtnDesc.PosX = EdgeDistX;

			for(USINT iSet = 0; iSet < ARR_LEN(rgGrpId); ++iSet){
				pBtnGrp = new CBtnGroup(rgGrpId[iSet]);
				pPanel->AddChild(pBtnGrp);

				for(USINT iCtl = 0; iCtl < 5; ++iCtl){
					BtnDesc.Ident = rgCtlId[iSet][iCtl];
					LblDesc.psText = rgCtlVal[iSet][iCtl];
					if(!LblDesc.psText) break;
					pBtnGrp->AddItem(new CRadio(BtnDesc));
					BtnDesc.PosX += GridDimX;
				}

				BtnDesc.PosX = EdgeDistX;
				BtnDesc.PosY += RowDimY * 2.f;
			}

			pBtnGrp = (CBtnGroup*)pPanel->GetChildById(ID_GRP_BrushShape);
			pBtnGrp->CheckItem(ID_RDO_RectBrush);
			pBtnGrp = (CBtnGroup*)pPanel->GetChildById(ID_GRP_SculptScape);
			pBtnGrp->CheckItem(ID_RDO_Uplift);
		}

		//按钮
		{
			USINT rgCtlId[2] = { ID_BTN_BrushSize, ID_BTN_BrushPower };
			WCHAR *rgCtlVal[2][5] = {
				{ L"2", L"4", L"8", L"16", L"32" },
				{ L"0.1", L"0.2", L"0.5", L"0.8", L"1" } };

			BtnDesc.PosY = RowDimY * 7.f;
			LblDesc.AliMode = LOC_Center;

			for(USINT iRow = 0; iRow < 2; ++iRow){
				BtnDesc.PosX = EdgeDistX;
				BtnDesc.PosY += RowDimY * 2.f;
				BtnDesc.Ident = rgCtlId[iRow];

				for(USINT iCol = 0; iCol < 5; ++iCol){
					LblDesc.psText = rgCtlVal[iRow][iCol];
					pPanel->AddChild(new CButton(BtnDesc));
					BtnDesc.PosX += GridDimX;
				}
			}
		}

		//颜色列表
		{
			ListDesc.PosX = EdgeDistX;
			ListDesc.PosY = RowDimY * 13.f;
			ListDesc.SizeX = gMyPanelW - (EdgeDistX * 2.f);
			ListDesc.SizeY = ListDesc.SizeX * 0.5f;
			ListDesc.Ident = ID_LST_BrushColor;
			ListDesc.BkgdColor = 0xD6DCDE;
			ListDesc.PageSizeX = 6;
			ListDesc.PageSizeY = 3;

			pListBox = new CListBox(ListDesc);
			pListBox->AddIconItem(mgGetImage("Px-Add"));
			pPanel->AddChild(pListBox);

			CJsonNode *pItems = gMyOption->GetChild("DefColor");
			USINT NumItem = pItems->GetSize();

			for(USINT iItem = 0; iItem < NumItem; ++iItem){
				const CHAR8 *psValue = pItems->GetElemS(iItem);
				pListBox->AddColorItem(AtoUx(psValue, P_Null, 16) | 0xFF000000);
			}
		}

		//纹理列表
		{
			CSprite *pSheet = mgGetSprite("Ground");

			rgLabel[7]->PosY = ListDesc.PosY + ListDesc.SizeY;
			rgLabel[7]->PosY += FontSize * 0.3f;

			ListDesc.Ident = ID_LST_BrushPattern;
			ListDesc.PosY = rgLabel[7]->PosY + RowDimY;
			ListDesc.SizeY = ListDesc.SizeX  / 2.f;
			ListDesc.PageSizeX = 4;
			ListDesc.PageSizeY = 2;
			ListDesc.pIconSheet = pSheet;

			pListBox = new CListBox(ListDesc);
			pPanel->AddChild(pListBox);

			for(USINT iItem = 0; iItem < pSheet->GetFrameCount(); ++iItem)
				pListBox->AddNumberItem(iItem);
		}
	};
	auto pfnInitDecorTool = [&](SPFPN MinY){
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = 0.f;
		PnlDesc.PosY = MinY + EdgeDistY;
		PnlDesc.Ident = ID_PNL_DecorTool;
		PnlDesc.SizeX = pTopPanel->SizeX;
		PnlDesc.SizeY = pTopPanel->SizeY - PnlDesc.PosY;

		pPanel = new CPanel(PnlDesc);
		pPanel->Hide(B_True);
		pTopPanel->AddChild(pPanel);

		//公共数据

		SPFPN RowDimY = FontSize * 1.5f;
		SPFPN CtlDimY = FontSize * 1.2f;
		SPFPN CtlSpan = FontSize * 0.3f;

		CListBox *pListBox;
		CComboBox *pComboBox;

		CLabel::DESC LblDesc;
		CButton::DESC BtnDesc;
		CListBox::DESC ListDesc;
		CComboBox::DESC CmbDesc;

		BtnDesc.BkgdCnt = 3;
		BtnDesc.pBgImage = mgGetImage("Px-Button");

		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;

		ListDesc.TxtColor = 0xFFFFFF;
		ListDesc.pTxtFormat = pTxtFmt;
		ListDesc.pfnDrawItem = mgDrawComboItem;
		ListDesc.pfnDrawFocus = mgDrawComboFocus;

		CmbDesc.SizeY = CtlDimY;
		CmbDesc.SizeX = gMyPanelW - (EdgeDistX * 2.f);
		CmbDesc.pBtnDesc = &BtnDesc;
		CmbDesc.pLblDesc = &LblDesc;
		CmbDesc.pListDesc = &ListDesc;

		//
		{
			CmbDesc.PosY = 0.f;
			CmbDesc.PosX = EdgeDistX;
			CmbDesc.Ident = ID_CMB_SceneType;
			ListDesc.PageSizeY = MAX_SCENE_ID - MIN_SCENE_ID + 1;
			ListDesc.SizeY = CmbDesc.SizeY * ListDesc.PageSizeY;
			LblDesc.psText = mgGetText(MIN_SCENE_ID);

			pComboBox = new CComboBox(CmbDesc);
			pPanel->AddChild(pComboBox);

			for(USINT iTxt = MIN_SCENE_ID; iTxt <= MAX_SCENE_ID; ++iTxt)
				pComboBox->AddItem(mgGetText(iTxt));
		}

		//
		{
			CJsonNode *pItems = gMyRoster->GetChild("decoration");
			CJsonNode *pItem = pItems->GetChild();

			CmbDesc.PosY += RowDimY;
			CmbDesc.Ident = ID_CMB_DecorType;
			ListDesc.PageSizeY = pItems->GetSize();
			ListDesc.SizeY = CmbDesc.SizeY * ListDesc.PageSizeY;
			LblDesc.psText = A_TO_W(pItem->GetKey());

			pComboBox = new CComboBox(CmbDesc);
			pPanel->AddChild(pComboBox);

			for(; pItem; pItem = pItem->GetNext())
				pComboBox->AddItem(A_TO_W(pItem->GetKey()));
		}

		//
		{
			ListDesc.PosX = EdgeDistX;
			ListDesc.PosY = CmbDesc.PosY + CmbDesc.SizeY + CtlSpan;
			ListDesc.SizeX = CmbDesc.SizeX;
			ListDesc.SizeY = CtlDimY * 36;
			ListDesc.Ident = ID_LST_Decorat;
			ListDesc.PageSizeY = 30;
			ListDesc.BkgdColor = 0x707C82;
			ListDesc.pfnDrawItem = mgDrawListItem;
			ListDesc.pfnDrawFocus = mgDrawListFocus;

			pListBox = new CListBox(ListDesc);
			pPanel->AddChild(pListBox);

			CJsonNode *pItems = gMyRoster->GetChild("decoration");
			CJsonNode *pItem = pItems->GetChild()->GetChild();

			for(; pItem; pItem = pItem->GetNext()){
				USINT Ident = StrHash32(pItem->GetValS());
				WCHAR *psText = A_TO_W(pItem->GetKey());
				pListBox->AddTextItem(psText, Ident);
			}
		}

		//
		{
			BtnDesc.PosX = EdgeDistX;
			BtnDesc.PosY = ListDesc.PosY + ListDesc.SizeY + CtlSpan;
			BtnDesc.SizeX = CtlDimY;
			BtnDesc.SizeY = CtlDimY;
			BtnDesc.pBgImage = mgGetImage("Px-Check");
			BtnDesc.pLblDesc = &LblDesc;
			LblDesc.AliMode = LOC_Right;

			BtnDesc.Ident = ID_RDO_RandDir;
			LblDesc.psText = mgGetText(IDS_RandDir);
			pPanel->AddChild(new CCheckBox(BtnDesc));

			BtnDesc.PosX += CmbDesc.SizeX * 0.5f;
			BtnDesc.Ident = ID_RDO_RandStyle;
			LblDesc.psText = mgGetText(IDS_RandStyle);
			pPanel->AddChild(new CCheckBox(BtnDesc));
		}
	};
	auto pfnInitActorTool = [&](SPFPN MinY){
		//面板

		CPanel *pPanel;
		CPanel::DESC PnlDesc;

		PnlDesc.PosX = 0.f;
		PnlDesc.PosY = MinY + EdgeDistY;
		PnlDesc.Ident = ID_PNL_ActorTool;
		PnlDesc.SizeX = pTopPanel->SizeX;
		PnlDesc.SizeY = pTopPanel->SizeY - PnlDesc.PosY;

		pPanel = new CPanel(PnlDesc);
		pPanel->Hide(B_True);
		pTopPanel->AddChild(pPanel);

		//公共数据

		SPFPN RowDimY = FontSize * 1.5f;
		SPFPN CtlDimY = FontSize * 1.2f;
		SPFPN CtlSpan = FontSize * 0.3f;

		CListBox *pListBox;
		CComboBox *pComboBox;

		CLabel::DESC LblDesc;
		CButton::DESC BtnDesc;
		CListBox::DESC ListDesc;
		CComboBox::DESC CmbDesc;

		BtnDesc.BkgdCnt = 3;
		BtnDesc.pBgImage = mgGetImage("Px-Button");

		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;

		ListDesc.TxtColor = 0xFFFFFF;
		ListDesc.pTxtFormat = pTxtFmt;
		ListDesc.pfnDrawItem = mgDrawComboItem;
		ListDesc.pfnDrawFocus = mgDrawComboFocus;

		CmbDesc.SizeY = CtlDimY;
		CmbDesc.SizeX = gMyPanelW - (EdgeDistX * 2.f);
		CmbDesc.pBtnDesc = &BtnDesc;
		CmbDesc.pLblDesc = &LblDesc;
		CmbDesc.pListDesc = &ListDesc;

		//
		{
			CmbDesc.PosY = 0.f;
			CmbDesc.PosX = EdgeDistX;
			CmbDesc.Ident = ID_CMB_ActorTeam;
			ListDesc.PageSizeY = MAX_TEAM_ID - MIN_TEAM_ID + 1;
			ListDesc.SizeY = CmbDesc.SizeY * ListDesc.PageSizeY;
			LblDesc.psText = mgGetText(MIN_TEAM_ID);

			pComboBox = new CComboBox(CmbDesc);
			pPanel->AddChild(pComboBox);

			for(USINT iTxt = MIN_TEAM_ID; iTxt <= MAX_TEAM_ID; ++iTxt)
				pComboBox->AddItem(mgGetText(iTxt));
		}

		//
		{
			CJsonNode *pItems = gMyRoster->GetChild("actor");
			CJsonNode *pItem = pItems->GetChild();

			CmbDesc.PosY += RowDimY;
			CmbDesc.Ident = ID_CMB_ActorEthnic;
			ListDesc.PageSizeY = pItems->GetSize();
			ListDesc.SizeY = CmbDesc.SizeY * ListDesc.PageSizeY;
			LblDesc.psText = A_TO_W(pItem->GetKey());

			pComboBox = new CComboBox(CmbDesc);
			pPanel->AddChild(pComboBox);

			for(; pItem; pItem = pItem->GetNext())
				pComboBox->AddItem(A_TO_W(pItem->GetKey()));
		}

		//
		{
			ListDesc.PosX = EdgeDistX;
			ListDesc.PosY = CmbDesc.PosY + CmbDesc.SizeY + CtlSpan;
			ListDesc.SizeX = CmbDesc.SizeX;
			ListDesc.SizeY = CtlDimY * 36;
			ListDesc.Ident = ID_LST_Actor;
			ListDesc.PageSizeY = 30;
			ListDesc.BkgdColor = 0x707C82;
			ListDesc.pfnDrawItem = mgDrawListItem;
			ListDesc.pfnDrawFocus = mgDrawListFocus;

			pListBox = new CListBox(ListDesc);
			pPanel->AddChild(pListBox);

			CJsonNode *pItems = gMyRoster->GetChild("actor")->GetChild()->GetChild();
			for(; pItems; pItems = pItems->GetNext()){
				CJsonNode *pItem = pItems->GetChild();
				for(; pItem; pItem = pItem->GetNext()){
					USINT Ident = StrHash32(pItem->GetValS());
					WCHAR *psText = A_TO_W(pItem->GetKey());
					pListBox->AddTextItem(psText, Ident);
				}
			}
		}
	};
	auto pfnInitToolHint = [&](){
		CLabel *pLabel;
		CLabel::DESC LblDesc;

		LblDesc.PosX = EdgeDistX;
		LblDesc.PosY = gMyWndH - EdgeDistX - FontSize;
		LblDesc.Ident = ID_LBL_ToolTip;
		LblDesc.psText = L"";
		LblDesc.TxtColor = 0xFFFFFF;
		LblDesc.pTxtFormat = pTxtFmt;

		pLabel = new CLabel(LblDesc);
		pTopPanel->AddChild(pLabel);
	};

	//顶层面板

	CPanel::DESC PnlDesc;
	PnlDesc.SizeX = gMyPanelW;
	PnlDesc.SizeY = gMyWndH;
	PnlDesc.pfnDrawUdf = mgDrawEditPanel;
	PnlDesc.pfnRespond = mgProcEditorGui;

	pTopPanel = new CPanel(PnlDesc);

	//字体

	mgAddFont("TF-Editer", PnlDesc.SizeX * 0.062f);
	pTxtFmt = mgGetFont("TF-Editer");
	FontSize = pTxtFmt->GetFontSize();

	//子模块

	SPFPN MinY = 0.f;
	MinY = pfnInitFileMenu(MinY);
	MinY = pfnInitToolList(MinY);
	pfnInitTerrTool(MinY);
	pfnInitDecorTool(MinY);
	pfnInitActorTool(MinY);
	pfnInitToolHint();

	//返回

	return pTopPanel;
}

//----------------------------------------//


//-------------------- 模块初始化 --------------------//

$VOID mgInitGui(){
	///

	gMyPanelW = gMyWndH * 0.26f;
	gMyPanelH = gMyWndH * 0.22f;

	///

	gMyGuiRoot = mgInitPortalGui();
	gMyAppPage = gMyPortal = new MGPortal;
};
$VOID mgFreeGui(){
	SAFE_DELETE(gMyGuiRoot);
	SAFE_DELETE(gMyAppPage);
}

//----------------------------------------//