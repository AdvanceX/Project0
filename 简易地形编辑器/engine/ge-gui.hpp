//-------------------- 控件 --------------------//

class CPanel;
class CWidget{
	_rest using CLS = CWidget;
	///回调函数
	_open using PFNDRAWUDF = $VOID(*)(CWidget*, _in RECTF&);
	_open using PFNRESPOND = $VOID(*)(CWidget*, DWORD, WPARAM, LPARAM);
	///样式&状态
	_open struct STYLES{
		WORDT bGrpObject : 1;
		WORDT bMulChoice : 1;
	};
	_open struct STATUS{
		WORDT bPaged : 1;
		WORDT bHidden : 1;
		WORDT bHighkey : 1;
		WORDT bChecked : 1;
		WORDT bFocused : 1;
		WORDT bUnusable : 1;
	};

	_open SPFPN PosX;
	_open SPFPN PosY;
	_open SPFPN SizeX;
	_open SPFPN SizeY;
	_open USINT Ident;
	_secr STYLES Styles;
	_secr STATUS Status;
	_open CWidget *pNext;
	_open CWidget *pPrev;
	_open CWidget *pChild;
	_open CWidget *pParent;

	_open CWidget(){
		$m.Styles = {};
		$m.Status = {};

		$m.pNext = P_Null;
		$m.pPrev = P_Null;
		$m.pChild = P_Null;
		$m.pParent = P_Null;
	}
	_open $VOID Delete(){
		if($m.pNext)
			$m.pNext->pPrev = $m.pPrev;
		if($m.pPrev)
			$m.pPrev->pNext = $m.pNext;
		else if($m.pParent)
			$m.pParent->pChild = $m.pNext;

		$m.DeleteChildren();
		delete this;
	}
	_open $VOID MoveToTop(){
		CWidget *pFirst;

		if(!$m.pPrev) return;
		if($m.pNext) $m.pNext->pPrev = $m.pPrev;

		$m.pPrev->pNext = $m.pNext;
		$m.pParent->pChild = this;

		for(pFirst = $m.pPrev; pFirst->pPrev; pFirst = pFirst->pPrev);
		pFirst->pPrev = this;

		$m.pNext = pFirst;
		$m.pPrev = P_Null;
	}
	_open $VOID DeleteChildren(){
		CWidget *pNode, *pNext;
		DETACH_(pNode, $m.pChild);

		for(pNode = $m.pChild; pNode; pNode = pNext){
			if(pNode->pChild) pNode->DeleteChildren();
			pNext = pNode->pNext;
			delete pNode;
		}
	}
	_open $VOID AddChild(CWidget *pWidget){
		pWidget->pParent = this;
		pWidget->pNext = $m.pChild;
		pWidget->pPrev = P_Null;

		if($m.pChild) $m.pChild->pPrev = pWidget;
		$m.pChild = pWidget;
	}
	_open $VOID ToLocalPoint(_io SPFPN &rPosX, _io SPFPN &rPosY){
		VECTOR2 Origin = $m.GetGlobalCoord();
		rPosX -= Origin.x;
		rPosY -= Origin.y;
	}
	_open IBOOL IsContained(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = $m.GetDestRect();

		if(PosY < rcDest.top) return B_False;
		if(PosX < rcDest.left) return B_False;
		if(PosX > rcDest.right) return B_False;
		if(PosY > rcDest.bottom) return B_False;

		return B_True;
	}
	_open RECTF GetDestRect(){
		VECTOR2 Origin = $m.GetGlobalCoord();
		SPFPN Bottom = Origin.y + $m.SizeY;
		SPFPN Right = Origin.x + $m.SizeX;
		return RECTF{ Origin.x, Origin.y, Right, Bottom };
	}
	_open VECTOR2 GetGlobalCoord(){
		CWidget *pNode = $m.pParent;
		SPFPN PosX = $m.PosX;
		SPFPN PosY = $m.PosY;

		while(pNode){
			PosX += pNode->PosX;
			PosY += pNode->PosY;
			pNode = pNode->pParent;
		}

		return VECTOR2(PosX, PosY);
	}
	_open CWidget *GetChildById(USINT Ident){
		CWidget *pNode, *pTarget;

		for(pNode = $m.pChild; pNode; pNode = pNode->pNext){
			if(pNode->Ident == Ident) return pNode;
			pTarget = pNode->GetChildById(Ident);
			if(pTarget) return pTarget;
		}

		return P_Null;
	}
	_open CWidget *CatchMessage(CPanel *pRoot, DWORD MsgCode, WPARAM Param0, LPARAM Param1){
		if(MsgCode == WM_MOUSEMOVE){
			if(!$m.IsDisabled()){
				SPFPN PosX = (SPFPN)LO_WORD(Param1);
				SPFPN PosY = (SPFPN)HI_WORD(Param1);
				if($m.IsGroupObject() || $m.IsContained(PosX, PosY)){
					CWidget *pTarget = $m.SpreadMessage(pRoot, MsgCode, Param0, Param1);
					if(pTarget) return pTarget;
					if(!$m.IsGroupObject()) return this;
				}
			}
		} else if(MsgCode == WM_PAINT){
			SPFPN PosX = AS_SPFPN(Param0);
			SPFPN PosY = AS_SPFPN(Param1);
			Param0 = AS_DWORD(PosX += $m.PosX);
			Param1 = AS_DWORD(PosY += $m.PosY);
			$m.OnPaint(PosX, PosY);
			$m.SpreadMessage(pRoot, MsgCode, Param0, Param1);
		}

		return P_Null;
	}
	_open CWidget *SpreadMessage(CPanel *pRoot, DWORD MsgCode, WPARAM Param0, LPARAM Param1){
		CWidget *pTarget;
		CWidget *pNode = $m.pChild;

		if(pNode){
			if(MsgCode == WM_PAINT){
				for(; pNode->pNext; pNode = pNode->pNext);
				for(; pNode; pNode = pNode->pPrev){
					if(pNode->IsHidden()) continue;
					pNode->CatchMessage(pRoot, MsgCode, Param0, Param1);
				}
			} else{
				for(; pNode; pNode = pNode->pNext){
					if(pNode->IsHidden()) continue;
					pTarget = pNode->CatchMessage(pRoot, MsgCode, Param0, Param1);
					if(pTarget) return pTarget;
				}
			}
		}

		return P_Null;
	}
	///状态访问
	_open $VOID Hide(BOOL8 bEnable){
		$m.Status.bHidden = bEnable;
	}
	_open $VOID Check(BOOL8 bEnable){
		$m.Status.bChecked = bEnable;
	}
	_open $VOID Focus(BOOL8 bEnable){
		$m.Status.bFocused = bEnable;
	}
	_open $VOID Paging(BOOL8 bEnable){
		$m.Status.bPaged = bEnable;
	}
	_open $VOID Disable(BOOL8 bEnable){
		$m.Status.bUnusable = bEnable;
	}
	_open $VOID Highlight(BOOL8 bEnable){
		$m.Status.bHighkey = bEnable;
	}
	_open BOOL8 IsPaged(){
		return $m.Status.bPaged;
	}
	_open BOOL8 IsHidden(){
		return $m.Status.bHidden;
	}
	_open BOOL8 IsChecked(){
		return $m.Status.bChecked;
	}
	_open BOOL8 IsHighkey(){
		return $m.Status.bHighkey;
	}
	_open BOOL8 IsFocused(){
		return $m.Status.bFocused;
	}
	_open BOOL8 IsDisabled(){
		return $m.Status.bUnusable;
	}
	///样式访问
	_rest $VOID AsGroupObject(){
		$m.Styles.bGrpObject = B_True;
	}
	_rest $VOID AsMultiChoice(){
		$m.Styles.bMulChoice = B_True;
	}
	_open BOOL8 IsGroupObject(){
		return $m.Styles.bGrpObject;
	}
	_open BOOL8 IsMultiChoice(){
		return $m.Styles.bMulChoice;
	}

	_open virtual ~CWidget(){
		$m.DeleteChildren();
	}
	_open virtual const WCHAR *GetValue(){
		return P_Null;
	}
	_open virtual $VOID SetValue(_in WCHAR *psValue){}
	///消息响应
	_open virtual $VOID OnPaint(SPFPN PosX, SPFPN PosY){}
	_open virtual $VOID OnNotify(CPanel *pRoot, LPARAM Extra){}
	_open virtual $VOID OnMouseHover(CPanel *pRoot, IBOOL bHover){}
	_open virtual $VOID OnMouseRoll(CPanel *pRoot, USINT Delta, DWORD State){}
	_open virtual $VOID OnMouseMove(CPanel *pRoot, SPFPN PosX, SPFPN PosY){}
	_open virtual $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){}
	_open virtual $VOID OnMouseUp(CPanel *pRoot, SPFPN PosX, SPFPN PosY){}
	_open virtual $VOID OnKeyUp(CPanel *pRoot, BYTET Key, LPARAM Extra){}
	_open virtual $VOID OnKeyDown(CPanel *pRoot, BYTET Key, LPARAM Extra){}
	_open virtual $VOID OnCharInput(CPanel *pRoot, CHAR8 Char, LPARAM Extra){}

	_rest static IDWFactory *pDWFactory;
	_rest static ID2DDevCtx *pD2dDevCtx;
	_rest static ID2DPureColorBrush *pD2DBrush;

	_open static $VOID InitContext(IDWFactory *pDWFactory, ID2DDevCtx *pD2dDevCtx, ID2DPureColorBrush *pD2DBrush){
		CLS::pD2DBrush = pD2DBrush;
		CLS::pD2dDevCtx = pD2dDevCtx;
		CLS::pDWFactory = pDWFactory;
	}
};

// Panel
class CPanel: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 0.f;
		SPFPN SizeY = 0.f;
		BOXFITMODE BkgdMode = BFM_Fill;
		ID2DBitmap *pBgImage = P_Null;
		PFNDRAWUDF pfnDrawUdf = P_Null;
		PFNRESPOND pfnRespond = P_Null;
	};

	_open CWidget *pHover;
	_open CWidget *pFocus;
	_open CWidget *pPopup;
	_open ID2DBitmap *pBgImage;
	_open BOXFITMODE BkgdMode;
	_secr PFNRESPOND pfnRespond;
	_secr PFNDRAWUDF pfnDrawUdf;

	_open CPanel(_in DESC &Desc){
		$m.pHover = P_Null;
		$m.pFocus = P_Null;
		$m.pPopup = P_Null;
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;
		$m.BkgdMode = Desc.BkgdMode;
		$m.pBgImage = Desc.pBgImage;
		$m.pfnRespond = Desc.pfnRespond;
		$m.pfnDrawUdf = Desc.pfnDrawUdf;
	}
	_open $VOID DrawBackground(_in RECTF &rcDest){
		if(!$m.pBgImage) return;
		DrawBitmap(CLS::pD2dDevCtx, $m.pBgImage, rcDest, $m.BkgdMode);
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);

		if($m.pfnDrawUdf)
			$m.pfnDrawUdf(this, rcDest);
		else if($m.pBgImage)
			$m.DrawBackground(rcDest);
	}
	_open IBOOL OnMessage(USINT MsgCode, WPARAM Param0, LPARAM Param1){
		CWidget *pTarget = P_Null;

		if($m.IsHidden())
			return B_False;

		if(MsgCode == WM_PAINT){
			$m.CatchMessage(this, MsgCode, Param0, Param1);
			return B_False;
		}

		if($m.pFocus){
			if(MsgCode == WM_CHAR){
				pTarget = $m.pFocus;
				pTarget->OnCharInput(this, (CHAR8)Param0, Param1);
			} else if(MsgCode == WM_KEYUP){
				pTarget = $m.pFocus;
				pTarget->OnKeyUp(this, (BYTET)Param0, Param1);
			} else if(MsgCode == WM_KEYDOWN){
				pTarget = $m.pFocus;
				pTarget->OnKeyDown(this, (BYTET)Param0, Param1);
			} else if(MsgCode == WM_LBUTTONUP){
				pTarget = $m.pFocus;
				pTarget->OnMouseUp(this, (SPFPN)LO_WORD(Param1), (SPFPN)HI_WORD(Param1));
			}
		} else{
			switch(MsgCode){
				case WM_CHAR: return B_False;
				case WM_KEYUP: return B_False;
				case WM_LBUTTONUP: return B_False;
			}
		}

		if($m.pHover){
			if(MsgCode == WM_MOUSEMOVE){
				pTarget = $m.pHover->CatchMessage(this, MsgCode, Param0, Param1);
				if(pTarget == $m.pHover){
					SPFPN PosX = (SPFPN)LO_WORD(Param1);
					SPFPN PosY = (SPFPN)HI_WORD(Param1);
					pTarget->OnMouseMove(this, PosX, PosY);
				} else{
					$m.pHover->Highlight(B_False);
					$m.pHover->OnMouseHover(this, B_False);
					$m.pHover = pTarget;
					if(pTarget){
						pTarget->Highlight(B_True);
						pTarget->OnMouseHover(this, B_True);
					}
				}
			} else if(MsgCode == WM_LBUTTONDOWN){
				SPFPN PosX = (SPFPN)LO_WORD(Param1);
				SPFPN PosY = (SPFPN)HI_WORD(Param1);
				if($m.pFocus) $m.pFocus->Focus(B_False);
				pTarget = $m.pFocus = $m.pHover;
				pTarget->MoveToTop();
				pTarget->Focus(B_True);
				pTarget->OnMouseDown(this, PosX, PosY);
			} else if(MsgCode == WM_MOUSEHWHEEL){
				pTarget = $m.pHover;
				pTarget->OnMouseRoll(this, HI_WORD(Param1), LO_WORD(Param1));
			}
		} else{
			if(MsgCode == WM_LBUTTONDOWN){
				if($m.pFocus){
					$m.pFocus->Focus(B_False);
					$m.pFocus = P_Null;
				}
				return B_False;
			}
		}

		if(!pTarget && (MsgCode == WM_MOUSEMOVE)){
			pTarget = $m.CatchMessage(this, MsgCode, Param0, Param1);
			if(!pTarget) return B_False;;
			$m.pHover = pTarget;
			$m.pHover->Highlight(B_True);
			$m.pHover->OnMouseHover(this, B_True);
		}

		if(pTarget && $m.pfnRespond)
			$m.pfnRespond(pTarget, MsgCode, Param0, Param1);

		if(MsgCode < WM_MOUSEFIRST) return B_False;
		if(MsgCode > WM_MOUSELAST) return B_False;

		return (pTarget != P_Null);
	}
};

// Icon
class CIcon: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 0.f;
		SPFPN SizeY = 0.f;
		IBOOL bScale = B_False;
		ID2DBitmap *pImage = P_Null;
	};

	_open IBOOL bScale;
	_open ID2DBitmap *pImage;

	_open CIcon(_in DESC &Desc){
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;
		$m.bScale = Desc.bScale;
		$m.SetImage(Desc.pImage);
	}
	_open $VOID SetImage(ID2DBitmap *pBitmap){
		$m.pImage = pBitmap;
		if($m.bScale) return;
		D2D1SIZEF Size = pBitmap->GetSize();
		$m.SizeY = Size.height;
		$m.SizeX = Size.width;
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);
		DrawBitmap(CLS::pD2dDevCtx, $m.pImage, rcDest);
	}
};

// Label
class CLabel: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		USINT BufSize = 0;
		DWORD AliMode = LOC_Center;
		ARGB8 TxtColor = 0x000000;
		WCHAR *psText = P_Null;
		IDWTxtFormat *pTxtFormat = P_Null;
	};

	_open USINT BufSize;
	_open ARGB8 TxtColor;
	_open WCHAR *lpsText;
	_open IDWTxtFormat *pTxtFormat;

	_open ~CLabel(){
		delete[] $m.lpsText;
	}
	_open CLabel(_in DESC &Desc){
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.Ident = Desc.Ident;
		$m.BufSize = Desc.BufSize;
		$m.TxtColor = Desc.TxtColor;
		$m.pTxtFormat = Desc.pTxtFormat;
		$m.lpsText = new WCHAR[Desc.BufSize];
		$m.SetValue(Desc.psText);
	}
	_open $VOID SetValue(_in WCHAR *psValue){
		USINT TxtLen = (USINT)WcsLen(psValue) + 1;
		if(TxtLen > $m.BufSize) $m.ResetBuffer(TxtLen);
		WcsCpy($m.lpsText, psValue);
		$m.UpdateSize();
	}
	_open $VOID SetTextFormat(IDWTxtFormat *pFormat){
		$m.pTxtFormat = pFormat;
		$m.UpdateSize();
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);

		CLS::pD2DBrush->SetColor(D2DX1COLOR($m.TxtColor));
		CLS::pD2dDevCtx->DrawText($m.lpsText, (USINT)WcsLen($m.lpsText),
			$m.pTxtFormat, rcDest, CLS::pD2DBrush);
	}
	_secr $VOID ResetBuffer(USINT Size){
		WCHAR *lpsBuffer = new WCHAR[Size];
		delete[] $m.lpsText;
		$m.BufSize = Size;
		$m.lpsText = lpsBuffer;
		$m.lpsText[0] = L'\0';
	}
	_secr $VOID UpdateSize(){
		D2D1SIZEF Size = CalcTextSize(CLS::pDWFactory, $m.pTxtFormat, $m.lpsText);
		$m.SizeX = Size.width;
		$m.SizeY = Size.height;
	}
	_open const WCHAR *GetValue(){
		return $m.lpsText;
	}
};

// Button
class CButton: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 0.f;
		SPFPN SizeY = 0.f;
		USINT BkgdCnt = 1;
		BOXFITMODE BkgdMode = BFM_Fill;
		ID2DBitmap *pBgImage = P_Null;
		PFNDRAWUDF pfnDrawUdf = P_Null;
		CLabel::DESC *pLblDesc = P_Null;
	};
	_open struct STYLES{
		WORDT M0;      //标签对齐模式
		UHALF M1;      //背景图片数量
		BOXFITMODE M2; //背景适应模式
	};

	_open STYLES Styles;
	_open CLabel *lpLabel;
	_open ID2DBitmap *pBgImage;
	_secr PFNDRAWUDF pfnDrawUdf;

	_open ~CButton(){
		delete $m.lpLabel;
	}
	_open CButton(_in DESC &Desc){
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;
		$m.Styles.M1 = Desc.BkgdCnt;
		$m.Styles.M2 = Desc.BkgdMode;
		$m.pfnDrawUdf = Desc.pfnDrawUdf;
		$m.pBgImage = Desc.pBgImage;
		$m.lpLabel = P_Null;
		$m.SetLabel(Desc.pLblDesc);
	}
	_open $VOID SetValue(_in WCHAR *psValue){
		$m.lpLabel->SetValue(psValue);
		$m.UpdateLabel();
	}
	_open $VOID SetLabel(_in CLabel::DESC *pDesc){
		SAFE_DELETE($m.lpLabel);
		if(!pDesc) return;
		$m.lpLabel = new CLabel(*pDesc);
		$m.lpLabel->pParent = this;
		$m.Styles.M0 = (WORDT)pDesc->AliMode;
		$m.UpdateLabel();
	}
	_open $VOID DrawLabel(_in RECTF &rcDest){
		if(!$m.lpLabel) return;
		SPFPN PosX = $m.lpLabel->PosX + rcDest.left;
		SPFPN PosY = $m.lpLabel->PosY + rcDest.top;
		$m.lpLabel->OnPaint(PosX, PosY);
	}
	_open $VOID DrawBackground(_in RECTF &rcDest){
		if($m.pBgImage){
			if($m.Styles.M1 <= 1){
				DrawBitmap(CLS::pD2dDevCtx, $m.pBgImage, rcDest, $m.Styles.M2);
			} else{
				D2D1SIZEF Size = $m.pBgImage->GetSize();
				RECTF rcSource = { 0.f, 0.f, 0.f, Size.height };
				USINT Index, Count = $m.Styles.M1;

				if($m.IsFocused() || $m.IsChecked()) Index = Count - 1;
				else if($m.IsHighkey()) Index = Count - 2;
				else Index = 0;

				Size.width /= Count;
				rcSource.left = Size.width * Index;
				rcSource.right = rcSource.left + Size.width - 1.f;

				DrawBitmap(CLS::pD2dDevCtx, $m.pBgImage, rcSource, rcDest, $m.Styles.M2);
			}
		}
	}
	_open $VOID OnMouseHover(CPanel *pRoot, IBOOL bHover){
		if(!bHover && $m.IsFocused()){
			$m.Focus(B_False);
			pRoot->pFocus = P_Null;
		}
	}
	_open $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.pParent->OnNotify(pRoot, (LPARAM)this);
	}
	_open $VOID OnMouseUp(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.Focus(B_False);
		pRoot->pFocus = P_Null;
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);

		if($m.pfnDrawUdf){
			$m.pfnDrawUdf(this, rcDest);
		} else{
			if($m.pBgImage) $m.DrawBackground(rcDest);
			if($m.lpLabel) $m.DrawLabel(rcDest);
		}
	}
	_secr $VOID UpdateLabel(){
		switch($m.Styles.M0){
			case LOC_Center:{
				$m.lpLabel->PosX = ($m.SizeX - $m.lpLabel->SizeX) * 0.5f;
				$m.lpLabel->PosY = ($m.SizeY - $m.lpLabel->SizeY) * 0.5f;
			} break;
			case LOC_Top:{
				$m.lpLabel->PosX = ($m.SizeX - $m.lpLabel->SizeX) * 0.5f;
				$m.lpLabel->PosY = $m.PosY - $m.lpLabel->SizeY;
			} break;
			case LOC_Bottom:{
				$m.lpLabel->PosX = ($m.SizeX - $m.lpLabel->SizeX) * 0.5f;
				$m.lpLabel->PosY = $m.SizeY;
			} break;
			case LOC_Left:{
				$m.lpLabel->PosX = $m.PosX - $m.lpLabel->SizeX;
				$m.lpLabel->PosY = ($m.SizeY - $m.lpLabel->SizeY) * 0.5f;
			} break;
			case LOC_Right:{
				$m.lpLabel->PosX = $m.SizeX;
				$m.lpLabel->PosY = ($m.SizeY - $m.lpLabel->SizeY) * 0.5f;
			} break;
		}
	}
	_open const WCHAR *GetValue(){
		return $m.lpLabel->lpsText;
	}
};

// Check Box
class CCheckBox: public CButton{
	_open CCheckBox(_in DESC &Desc): CButton(Desc){}
	_open $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.Check(!$m.IsChecked());
		$m.pParent->OnNotify(pRoot, (LPARAM)this);
	}
};

// Radio
class CRadio: public CButton{
	_open CRadio(_in DESC &Desc): CButton(Desc){}
};

// Button Group
class CBtnGroup: public CWidget{
	_open CBtnGroup(USINT Ident){
		$m.PosX = 0.f;
		$m.PosY = 0.f;
		$m.SizeX = 0.f;
		$m.SizeY = 0.f;
		$m.Ident = Ident;
		$m.AsGroupObject();
	}
	_open $VOID AddItem(CRadio *pItem){
		$m.AddChild(pItem);
	}
	_open $VOID CheckItem(USINT Ident){
		for(auto pNode = $m.pChild; pNode; pNode = pNode->pNext){
			if(pNode->Ident == Ident) pNode->Check(B_True);
			else pNode->Check(B_False);
		}
	}
	_open $VOID OnNotify(CPanel *pRoot, LPARAM Extra){
		CWidget *pSender = (CWidget*)Extra;
		for(auto pNode = $m.pChild; pNode; pNode = pNode->pNext){
			if(pNode == pSender) pNode->Check(B_True);
			else pNode->Check(B_False);
		}
	}
	_open const WCHAR *GetValue(){
		for(auto pNode = $m.pChild; pNode; pNode = pNode->pNext)
			if(pNode->IsChecked()) return pNode->GetValue();
		return P_Null;
	}
	_open USINT GetChoice(){
		for(auto pNode = $m.pChild; pNode; pNode = pNode->pNext)
			if(pNode->IsChecked()) return pNode->Ident;
		return UINT_MAX;
	}
	_open USINT GetCount(){
		USINT Count = 0;
		CWidget *pNode = $m.pChild;

		while(pNode){
			Count += 1;
			pNode = pNode->pNext;
		}

		return Count;
	}
};

// Text Box
class CTextBox: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 0.f;
		SPFPN SizeY = 0.f;
		SPFPN EdgeDist = 6.f;
		SPFPN EdgeSize = 1.f;
		ARGB8 TxtColor = 0x000000;
		ARGB8 BkgdColor = 0xFFFFFF;
		ARGB8 EdgeColor = 0x808080;
		PFNDRAWUDF pfnDrawBkgd = P_Null;
		PFNDRAWUDF pfnDrawCaret = P_Null;
		IDWTxtFormat *pTxtFormat = P_Null;
	};

	_secr USINT CaretIdx;
	_secr SPFPN CaretPos;
	_open SPFPN EdgeDist;
	_open SPFPN EdgeSize;
	_open ARGB8 TxtColor;
	_open ARGB8 BkgdColor;
	_open ARGB8 EdgeColor;
	_open PFNDRAWUDF pfnDrawBkgd;
	_open PFNDRAWUDF pfnDrawCaret;
	_open IDWTxtFormat *pTxtFormat;
	_open WString Content;

	_open CTextBox(_in DESC &Desc){
		$m.CaretIdx = 0;
		$m.CaretPos = 0.f;

		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;
		$m.EdgeDist = Desc.EdgeDist;
		$m.EdgeSize = Desc.EdgeSize;
		$m.TxtColor = Desc.TxtColor;
		$m.BkgdColor = Desc.BkgdColor;
		$m.EdgeColor = Desc.EdgeColor;
		$m.pTxtFormat = Desc.pTxtFormat;

		$m.pfnDrawBkgd = Desc.pfnDrawBkgd ?
			Desc.pfnDrawBkgd : CTextBox::DrawDefBack;
		$m.pfnDrawCaret = Desc.pfnDrawCaret ?
			Desc.pfnDrawCaret : CTextBox::DrawDefCaret;
	}
	_open $VOID SetText(_in WCHAR *psText){
		$m.Content = psText;
		$m.SetCaretIndex(WcsLen(psText));
	}
	_open $VOID InsertText(_in WCHAR *psText){
		$m.Content.insert($m.CaretIdx, psText);
		$m.SetCaretIndex($m.CaretIdx + WcsLen(psText));
	}
	_open $VOID InsertChar(WCHAR Char){
		$m.Content.insert($m.CaretIdx, 1, Char);
		$m.SetCaretIndex($m.CaretIdx + 1);
	}
	_open $VOID DeleteText(UPINT Count){
		UPINT MaxCount = $m.Content.size() - $m.CaretIdx;
		Count = MIN_(Count, MaxCount);
		if(Count != 0) $m.Content.erase($m.CaretIdx, Count);
	}
	_open $VOID BackSpace(UPINT Count){
		if($m.CaretIdx > 0){
			Count = MIN_(Count, $m.CaretIdx);
			$m.SetCaretIndex($m.CaretIdx - Count);
			$m.Content.erase($m.CaretIdx, Count);
		}
	}
	_open $VOID SetCaretIndex(UPINT Index){
		if(Index <= $m.Content.size()){
			WCHAR Temp = $m.Content[Index];
			$m.Content[Index] = L'\0';
			$m.CaretPos = $m.GetTextWidth($m.Content.c_str());
			$m.CaretIdx = (USINT)Index;
			$m.Content[Index] = Temp;
		}
	}
	_open $VOID SetCaretCoord(SPFPN ClickX, SPFPN ClickY){
		if($m.Content.size() > 0){
			WCHAR Subtext[2] = L"";
			USINT TxtLen = (USINT)$m.Content.size();

			$m.ToLocalPoint(ClickX, ClickY);
			$m.CaretPos = $m.EdgeDist;
			$m.CaretIdx = 0;

			while($m.CaretIdx < TxtLen){
				if(ClickX <= $m.CaretPos) break;
				Subtext[0] = $m.Content[$m.CaretIdx++];
				$m.CaretPos += $m.GetTextWidth(Subtext);
			}
		} else{
			$m.CaretIdx = 0;
			$m.CaretPos = $m.EdgeDist;
		}
	}
	_open $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.SetCaretCoord(PosX, PosY);
	}
	_open $VOID OnCharInput(CPanel *pRoot, CHAR8 Char, LPARAM Extra){
		if(IsPrint(Char)) $m.InsertChar(Char);
	}
	_open $VOID OnKeyDown(CPanel *pRoot, BYTET Key, LPARAM Extra){
		switch(Key){
			case VK_BACK:{
				$m.BackSpace(1);
			} break;
			case VK_DELETE:{
				$m.DeleteText(1);
			} break;
			case VK_LEFT:{
				$m.SetCaretIndex($m.CaretIdx - 1);
			} break;
			case VK_RIGHT:{
				$m.SetCaretIndex($m.CaretIdx + 1);
			} break;
		}
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcBox = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);
		RECTF rcCaret = rcBox;

		$m.pfnDrawBkgd(this, rcBox);
		$m.DrawText(rcBox);
		$m.GetCaretDest(rcCaret);
		$m.pfnDrawCaret(this, rcCaret);
	}
	_open $VOID DrawText(_in RECTF &rcDest){
		const WCHAR *psText = $m.Content.c_str();
		USINT TxtLen = (USINT)$m.Content.size();
		RECTF rcText = { rcDest.left + $m.EdgeDist, rcDest.top, rcDest.right, rcDest.bottom };

		CLS::pD2DBrush->SetColor(D2DX1COLOR($m.TxtColor));
		CLS::pD2dDevCtx->DrawText(psText, TxtLen, $m.pTxtFormat, rcText, CLS::pD2DBrush);
	}
	_secr $VOID GetCaretDest(_io RECTF &rcDest){
		SPFPN Height = $m.pTxtFormat->GetFontSize();
		SPFPN LocalX = $m.CaretPos;
		SPFPN LocalY = ($m.SizeY - Height) * 0.5f;

		rcDest.left += LocalX;
		rcDest.top += LocalY;
		rcDest.right = rcDest.left;
		rcDest.bottom = rcDest.top + Height;
	}
	_secr SPFPN GetTextWidth(_in WCHAR *psText){
		return CalcTextSize(CLS::pDWFactory, $m.pTxtFormat, psText).width;
	}
	_open const WCHAR *GetValue(){
		return (WCHAR*)$m.Content.c_str();
	}

	_secr static $VOID DrawDefBack(CWidget *pWidget, _in RECTF &rcDest){
		CTextBox *pTextBox = (CTextBox*)pWidget;
		CLS::pD2DBrush->SetColor(D2DX1COLOR(pTextBox->BkgdColor));
		CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
		CLS::pD2DBrush->SetColor(D2DX1COLOR(pTextBox->EdgeColor));
		CLS::pD2dDevCtx->DrawRectangle(rcDest, CLS::pD2DBrush, pTextBox->EdgeSize);
	}
	_secr static $VOID DrawDefCaret(CWidget *pWidget, _in RECTF &rcDest){
		if(pWidget->IsFocused()){
			D2D1POINTF Points[2] = { { rcDest.left, rcDest.top }, { rcDest.left, rcDest.bottom } };
			CLS::pD2dDevCtx->DrawLine(Points[0], Points[1], CLS::pD2DBrush, 2.f);
		}
	}
};

// Scroll Bar
class CScrollBar: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 16.f;
		SPFPN SizeY = 16.f;
		USINT PageCnt = 1;
		PFNDRAWUDF pfnDrawTrack = P_Null;
		PFNDRAWUDF pfnDrawSlider = P_Null;
	};

	_open USINT CurPage;
	_open USINT MaxPage;
	_secr SPFPN CellSize;
	_open PFNDRAWUDF pfnDrawTrack;
	_open PFNDRAWUDF pfnDrawSlider;

	_open CScrollBar(_in DESC &Desc = DESC()){
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;

		$m.CurPage = 0;
		$m.MaxPage = Desc.PageCnt;
		$m.CellSize = Desc.SizeY / Desc.PageCnt;

		$m.pfnDrawTrack = Desc.pfnDrawTrack ?
			Desc.pfnDrawTrack : CScrollBar::DrawDefTrack;
		$m.pfnDrawSlider = Desc.pfnDrawSlider ?
			Desc.pfnDrawSlider : CScrollBar::DrawDefSlider;
	}
	_open $VOID SetPageCount(USINT Count){
		$m.MaxPage = MAX_(1, Count);
		$m.CurPage = MIN_($m.CurPage, $m.MaxPage - 1);
		$m.CellSize = $m.SizeY / $m.MaxPage;
	}
	_open $VOID OnMouseMove(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		if($m.IsChecked()){
			$m.ToLocalPoint(PosX, PosY);
			$m.CurPage = USINT(PosY / $m.CellSize);
		}
	}
	_open $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.ToLocalPoint(PosX, PosY);
		USINT Location = USINT(PosY / $m.CellSize);

		if(Location < $m.CurPage)
			$m.CurPage -= 1;
		else if(Location > $m.CurPage)
			$m.CurPage += 1;
		else
			$m.Check(B_True);
	}
	_open $VOID OnMouseUp(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.Check(B_False);
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);
		$m.pfnDrawTrack(this, rcDest);
		rcDest.top += $m.CellSize * $m.CurPage;
		rcDest.bottom = rcDest.top + $m.CellSize;
		$m.pfnDrawSlider(this, rcDest);
	}

	_secr static $VOID DrawDefTrack(CWidget *pWidget, _in RECTF &rcDest){
		CLS::pD2DBrush->SetColor(D2DX1COLOR(1.f, 1.f, 1.f));
		CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
		CLS::pD2DBrush->SetColor(D2DX1COLOR(0.5f, 0.5f, 0.5f));
		CLS::pD2dDevCtx->DrawRectangle(rcDest, CLS::pD2DBrush);
	}
	_secr static $VOID DrawDefSlider(CWidget *pWidget, _in RECTF &rcDest){
		ARGB8 Color = pWidget->IsChecked() ? 0x808080 : 0xD0D0D0;
		CLS::pD2DBrush->SetColor(D2DX1COLOR(Color));
		CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
	}
};

// List Box
class CListBox: public CWidget{
	_open enum ITEMTYPE: WORDT{
		ITEM_TYPE_Null,
		ITEM_TYPE_Text,
		ITEM_TYPE_Icon,
		ITEM_TYPE_Color,
		ITEM_TYPE_Number,
	};
	_open struct ITEM{
		DWORD Ident;
		WORDT bChosen;
		ITEMTYPE Type;
		union{
			ARGB8 Color;
			USINT Number;
			WCHAR Text[2];
			ID2DBitmap *pIcon;
		};
	};
	_secr struct ITEMROW{
		ITEMROW *pNext;
		ITEMROW *pPrev;
		ITEM rgItem[0];

		PVOID operator new(UPINT cbHeader, _in WCHAR *psText){
			UPINT cbInfo = sizeof(ITEM);
			UPINT cbText = WcsLen(psText) * sizeof(WCHAR);
			UPINT cbObject = cbHeader + cbInfo + cbText;
			ITEMROW *pObject = (ITEMROW*)Mcalloc(1, cbObject);
			pObject->rgItem[0].Type = ITEM_TYPE_Text;
			WcsCpy(pObject->rgItem[0].Text, psText);
			return pObject;
		}
		PVOID operator new(UPINT cbHeader, USINT ColCount){
			return Mcalloc(1, cbHeader + (ColCount * sizeof(ITEM)));
		}
		$VOID operator delete(PVOID Pointer){
			Mfree(Pointer);
		}
	};
	_open struct DESC{
		USINT Ident = 0;
		SPFPN PosX = 0.f;
		SPFPN PosY = 0.f;
		SPFPN SizeX = 0.f;
		SPFPN SizeY = 0.f;
		USINT PageSizeX = 1;
		USINT PageSizeY = 1;
		ARGB8 TxtColor = 0x000000;
		ARGB8 BkgdColor = 0xFFFFFF;
		PFNDRAWUDF pfnDrawBkgd = P_Null;
		PFNDRAWUDF pfnDrawItem = P_Null;
		PFNDRAWUDF pfnDrawText = P_Null;
		PFNDRAWUDF pfnDrawFocus = P_Null;
		CSprite *pIconSheet = P_Null;
		IDWTxtFormat *pTxtFormat = P_Null;
		CScrollBar::DESC *pScrollDesc = P_Null;
	};

	_open USINT RowCount;
	_open USINT ItemCount;
	_open USINT PageSizeX;
	_open USINT PageSizeY;
	_open SPFPN ItemWidth;
	_open SPFPN ItemHeight;
	_open ARGB8 TxtColor;
	_open ARGB8 BkgdColor;
	_open ITEM *pHoverItem;
	_open ITEM *pFocusItem;
	_open ITEMROW *pLastRow;
	_open ITEMROW *pFirstRow;
	_open PFNDRAWUDF pfnDrawBkgd;
	_open PFNDRAWUDF pfnDrawItem;
	_open PFNDRAWUDF pfnDrawText;
	_open PFNDRAWUDF pfnDrawFocus;
	_open IDWTxtFormat *pTxtFormat;
	_open CScrollBar *pScrollBar;
	_open CSprite *pIconSheet;

	_open ~CListBox(){
		$m.ClearItems();
	}
	_open CListBox(_in DESC &Desc){
		$m.RowCount = 0;
		$m.ItemCount = 0;

		$m.pLastRow = P_Null;
		$m.pFirstRow = P_Null;
		$m.pHoverItem = P_Null;
		$m.pFocusItem = P_Null;

		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;
		$m.TxtColor = Desc.TxtColor;
		$m.BkgdColor = Desc.BkgdColor;
		$m.PageSizeX = Desc.PageSizeX;
		$m.PageSizeY = Desc.PageSizeY;
		$m.pIconSheet = Desc.pIconSheet;
		$m.pTxtFormat = Desc.pTxtFormat;

		$m.ItemWidth = $m.SizeX / $m.PageSizeX;
		$m.ItemHeight = $m.SizeY / $m.PageSizeY;

		$m.pfnDrawBkgd = Desc.pfnDrawBkgd ?
			Desc.pfnDrawBkgd : CListBox::DrawDefBack;
		$m.pfnDrawItem = Desc.pfnDrawItem ?
			Desc.pfnDrawItem : CListBox::DrawDefItem;
		$m.pfnDrawText = Desc.pfnDrawText ?
			Desc.pfnDrawText : CListBox::DrawDefText;
		$m.pfnDrawFocus = Desc.pfnDrawFocus ?
			Desc.pfnDrawFocus : CListBox::DrawDefFocus;

		$m.pScrollBar = Desc.pScrollDesc ?
			new CScrollBar(*Desc.pScrollDesc) :
			new CScrollBar;

		$m.pScrollBar->PosY = 0.f;
		$m.pScrollBar->PosX = $m.SizeX - $m.pScrollBar->SizeX;
		$m.pScrollBar->SizeY = $m.SizeY;
		$m.pScrollBar->Hide(B_True);
		$m.AddChild($m.pScrollBar);
	}
	_open $VOID OnMouseMove(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		$m.ToLocalPoint(PosX, PosY);
		$m.pHoverItem = $m.GetItem(VECTOR2(PosX, PosY));
	}
	_open $VOID OnMouseDown(CPanel *pRoot, SPFPN PosX, SPFPN PosY){
		if(!$m.IsMultiChoice()){
			$m.pFocusItem = $m.pHoverItem;
			$m.pParent->OnNotify(pRoot, (LPARAM)this);
		} else if($m.pHoverItem){
			$m.pHoverItem->bChosen = !$m.pHoverItem->bChosen;
			$m.pParent->OnNotify(pRoot, (LPARAM)this);
		}
	}
	_open $VOID OnMouseHover(CPanel *pRoot, IBOOL bHover){
		if(!bHover) $m.pHoverItem = P_Null;
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		RECTF rcDest = MAKE_RECT(PosX, PosY, $m.SizeX, $m.SizeY);

		$m.pfnDrawBkgd(this, rcDest);
		$m.DrawItems(rcDest);
	}
	_open $VOID DrawItems(_in RECTF &rcDest){
		ITEMROW *pRow = $m.GetRow($m.GetRowStart());
		ITEM *pFocus = $m.pFocusItem;
		RECTF rcItem = { rcDest.left, rcDest.top,
			rcDest.left + $m.ItemWidth, rcDest.top + $m.ItemHeight };

		for(USINT iRow = 0; iRow < $m.PageSizeY; ++iRow){
			if(!pRow) break;

			for(USINT iCol = 0; iCol < $m.PageSizeX; ++iCol){
				ITEM *pItem = &pRow->rgItem[iCol];

				if(pItem->Type == ITEM_TYPE_Null) break;
				else $m.pFocusItem = pItem;

				if((pItem == $m.pHoverItem) || (pItem == pFocus) || pItem->bChosen)
					$m.pfnDrawFocus(this, rcItem);
				else  $m.pfnDrawItem(this, rcItem);

				rcItem.left += $m.ItemWidth;
				rcItem.right += $m.ItemWidth;
			}

			rcItem.left = rcDest.left;
			rcItem.right = rcItem.left + $m.ItemWidth;
			rcItem.top += $m.ItemHeight;
			rcItem.bottom += $m.ItemHeight;

			pRow = pRow->pNext;
		}

		$m.pFocusItem = pFocus;
	}
	_open $VOID ClearItems(){
		//清理条目

		for(auto pNode = $m.pFirstRow; pNode;){
			auto pNext = pNode->pNext;
			delete pNode;
			pNode = pNext;
		}

		//归零

		$m.RowCount = 0;
		$m.ItemCount = 0;
		$m.pLastRow = P_Null;
		$m.pFirstRow = P_Null;
	}
	_secr $VOID UpdateScrollBar(){
		USINT PageCnt = CEIL_DIV($m.RowCount, $m.PageSizeY);

		if(PageCnt > 1){
			$m.Paging(B_True);
			$m.pScrollBar->Hide(B_False);
			$m.pScrollBar->SetPageCount(PageCnt);
			$m.ItemWidth = ($m.SizeX - $m.pScrollBar->SizeX) / $m.PageSizeX;
		} else{
			$m.Paging(B_False);
			$m.pScrollBar->Hide(B_True);
			$m.pScrollBar->SetPageCount(PageCnt);
			$m.ItemWidth = $m.SizeX / $m.PageSizeX;
		}
	}
	///添加&查找
	_open ITEM *FindColorItem(ARGB8 Color){
		for(auto pRow = $m.pFirstRow; pRow; pRow = pRow->pNext){
			for(USINT iCol = 0; iCol < $m.PageSizeX; ++iCol){
				ITEM *pItem = &pRow->rgItem[iCol];
				if(pItem->Type != ITEM_TYPE_Color) continue;
				if(pItem->Color == Color) return pItem;
			}
		}
		return P_Null;
	}
	_open ITEM *FindNumberItem(USINT Number){
		for(auto pRow = $m.pFirstRow; pRow; pRow = pRow->pNext){
			for(USINT iCol = 0; iCol < $m.PageSizeX; ++iCol){
				ITEM *pItem = &pRow->rgItem[iCol];
				if(pItem->Type != ITEM_TYPE_Number) continue;
				if(pItem->Number == Number) return pItem;
			}
		}
		return P_Null;
	}
	_open ITEM *FindTextItem(_in WCHAR *psText){
		for(auto pRow = $m.pFirstRow; pRow; pRow = pRow->pNext){
			ITEM *pItem = &pRow->rgItem[0];
			INT32 Diff = WcsCmp(pItem->Text, psText);
			if(!Diff) return pItem;
		}
		return P_Null;
	}
	_open ITEM *FindIconItem(ID2DBitmap *pIcon){
		for(auto pRow = $m.pFirstRow; pRow; pRow = pRow->pNext){
			for(USINT iCol = 0; iCol < $m.PageSizeX; ++iCol){
				ITEM *pItem = &pRow->rgItem[iCol];
				if(pItem->Type != ITEM_TYPE_Icon) continue;
				if(pItem->pIcon == pIcon) return pItem;
			}
		}
		return P_Null;
	}
	_open ITEM *AddColorItem(ARGB8 Color, USINT Ident = UINT_MAX){
		if(Ident == UINT_MAX) Ident = $m.ItemCount;
		ITEM *pItem = $m.NewItem(P_Null);
		pItem->Type = ITEM_TYPE_Color;
		pItem->Ident = Ident;
		pItem->Color = Color;
		return pItem;
	}
	_open ITEM *AddNumberItem(USINT Number, USINT Ident = UINT_MAX){
		if(Ident == UINT_MAX) Ident = $m.ItemCount;
		ITEM *pItem = $m.NewItem(P_Null);
		pItem->Type = ITEM_TYPE_Number;
		pItem->Ident = Ident;
		pItem->Number = Number;
		return pItem;
	}
	_open ITEM *AddTextItem(_in WCHAR *psText, USINT Ident = UINT_MAX){
		if(Ident == UINT_MAX) Ident = $m.ItemCount;
		ITEM *pItem = $m.NewItem(psText);
		pItem->Ident = Ident;
		return pItem;
	}
	_open ITEM *AddIconItem(ID2DBitmap *pIcon, USINT Ident = UINT_MAX){
		if(Ident == UINT_MAX) Ident = $m.ItemCount;
		ITEM *pItem = $m.NewItem(P_Null);
		pItem->Type = ITEM_TYPE_Icon;
		pItem->Ident = Ident;
		pItem->pIcon = pIcon;
		return pItem;
	}
	///新建&获取
	_open ITEM *GetItem(USINT Index){
		DIV Split = Div((INT32)Index, (INT32)$m.PageSizeX);
		ITEMROW *pRow = $m.pFirstRow;
		for(INT32 iRow = 0; iRow < Split.quot; ++iRow) pRow = pRow->pNext;
		return &pRow->rgItem[Split.rem];
	}
	_secr ITEM *GetItem(_in VECTOR2 &Coord){
		USINT ColId = USINT(Coord.x / $m.ItemWidth);
		USINT RowId = USINT(Coord.y / $m.ItemHeight) + $m.GetRowStart();

		if(RowId >= $m.RowCount) return P_Null;

		ITEMROW *pRow = $m.GetRow(RowId);
		ITEM *pItem = &pRow->rgItem[ColId];

		if(pItem->Type == ITEM_TYPE_Null) return P_Null;
		return pItem;
	}
	_secr ITEM *NewItem(_in WCHAR *psText){
		USINT ColId = $m.ItemCount++ % $m.PageSizeX;
		if(ColId == 0) $m.NewRow(psText);
		return &$m.pLastRow->rgItem[ColId];
	}
	_secr ITEMROW *NewRow(_in WCHAR *psText){
		ITEMROW *pNewRow = psText ? new(psText)ITEMROW : new($m.PageSizeX)ITEMROW;

		if(!$m.pFirstRow) $m.pLastRow = $m.pFirstRow = pNewRow;
		else $m.pLastRow = $m.pLastRow->pNext = pNewRow;

		$m.RowCount += 1;
		$m.UpdateScrollBar();

		return pNewRow;
	}
	_secr ITEMROW *GetRow(USINT Index){
		USINT iRow = 0;
		ITEMROW *pRow = $m.pFirstRow;
		while(iRow++ != Index) pRow = pRow->pNext;
		return pRow;
	}
	_secr USINT GetRowStart(){
		if(!$m.IsPaged()) return 0;
		USINT MaxStart = $m.RowCount - $m.PageSizeY;
		USINT Start = $m.pScrollBar->CurPage * $m.PageSizeY;
		return MIN_(Start, MaxStart);
	}
	_open USINT GetChoice(){
		if(!$m.pFocusItem) return UINT_MAX;
		return $m.pFocusItem->Ident;
	}
	_open const WCHAR *GetValue(){
		return $m.pFocusItem->Text;
	}

	_open static $VOID DrawDefBack(CWidget *pWidget, _in RECTF &rcDest){
		CLS::pD2DBrush->SetColor(D2DX1COLOR(((CListBox*)pWidget)->BkgdColor));
		CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
		CLS::pD2DBrush->SetColor(D2DX1COLOR(0.5f, 0.5f, 0.5f));
		CLS::pD2dDevCtx->DrawRectangle(rcDest, CLS::pD2DBrush);
	}
	_open static $VOID DrawDefItem(CWidget *pWidget, _in RECTF &rcDest){
		CListBox *pList = (CListBox*)pWidget;
		ITEM *pItem = pList->pFocusItem;

		if(pItem->Type == ITEM_TYPE_Icon){
			DrawBitmap(CLS::pD2dDevCtx, pItem->pIcon, rcDest);
		} else if(pItem->Type == ITEM_TYPE_Color){
			CLS::pD2DBrush->SetColor(D2DX1COLOR(pItem->Color));
			CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
		} else if(pItem->Type == ITEM_TYPE_Number){
			CSprite *pSheet = pList->pIconSheet;
			RECTF rcSouce = pSheet->GetFrame(pItem->Number);
			DrawBitmap(CLS::pD2dDevCtx, pSheet->GetSheet(), rcSouce, rcDest);
		} else{
			CLS::pD2DBrush->SetColor(D2DX1COLOR(1.f, 1.f, 1.f));
			CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
			pList->pfnDrawText(pWidget, rcDest);
		}
	}
	_open static $VOID DrawDefText(CWidget *pWidget, _in RECTF &rcDest){
		CListBox *pListBox = (CListBox*)pWidget;
		SPFPN LocalX = pListBox->pTxtFormat->GetFontSize() * 0.5f;
		RECTF rcText = { rcDest.left + LocalX, rcDest.top, rcDest.right, rcDest.bottom };
		WCHAR *psText = pListBox->pFocusItem->Text;

		CLS::pD2DBrush->SetColor(D2DX1COLOR(pListBox->TxtColor));
		CLS::pD2dDevCtx->DrawText(psText, (USINT)WcsLen(psText),
			pListBox->pTxtFormat, rcText, CLS::pD2DBrush);
	}
	_open static $VOID DrawDefFocus(CWidget *pWidget, _in RECTF &rcDest){
		CListBox *pList = (CListBox*)pWidget;
		ITEM *pItem = pList->pFocusItem;

		if(pItem->Type == ITEM_TYPE_Text){
			CLS::pD2DBrush->SetColor(D2DX1COLOR(0.38f, 0.62f, 0.62f));
			CLS::pD2dDevCtx->FillRectangle(rcDest, CLS::pD2DBrush);
			pList->pfnDrawText(pWidget, rcDest);
		} else{
			RECTF rcFrame = { rcDest.left + 2.f, rcDest.top + 2.f, rcDest.right - 2.f, rcDest.bottom - 2.f };
			pList->pfnDrawItem(pWidget, rcDest);
			CLS::pD2DBrush->SetColor(D2DX1COLOR(0.38f, 0.62f, 0.62f));
			CLS::pD2dDevCtx->DrawRectangle(rcFrame, CLS::pD2DBrush, 3.f);
		}
	}
};

// Combo Box
class CComboBox: public CWidget{
	_open struct DESC{
		USINT Ident;
		SPFPN PosX;
		SPFPN PosY;
		SPFPN SizeX;
		SPFPN SizeY;
		CLabel::DESC *pLblDesc;
		CButton::DESC *pBtnDesc;
		CListBox::DESC *pListDesc;
	};

	_secr CButton *pButton;
	_secr CListBox *pListBox;

	_open CComboBox(_in DESC &Desc){
		$m.PosX = Desc.PosX;
		$m.PosY = Desc.PosY;
		$m.SizeX = Desc.SizeX;
		$m.SizeY = Desc.SizeY;
		$m.Ident = Desc.Ident;

		CButton::DESC BtnDesc = *Desc.pBtnDesc;
		BtnDesc.PosX = 0.f;
		BtnDesc.PosY = 0.f;
		BtnDesc.SizeX = Desc.SizeX;
		BtnDesc.SizeY = Desc.SizeY;
		BtnDesc.Ident = Desc.Ident;
		BtnDesc.pLblDesc = Desc.pLblDesc;
		$m.pButton = new CButton(BtnDesc);
		$m.AddChild($m.pButton);

		CListBox::DESC ListDesc = *Desc.pListDesc;
		ListDesc.PosX = 0.f;
		ListDesc.PosY = Desc.SizeY;
		ListDesc.SizeX = Desc.SizeX;
		ListDesc.Ident = Desc.Ident;
		$m.pListBox = new CListBox(ListDesc);
		$m.pListBox->Hide(B_True);
		$m.AddChild($m.pListBox);
	}
	_open $VOID SetValue(_in WCHAR *psValue){
		$m.pButton->SetValue(psValue);
	}
	_open $VOID AddItem(_in WCHAR *psValue, USINT Ident = UINT_MAX){
		$m.pListBox->AddTextItem(psValue, Ident);
	}
	_open $VOID OnNotify(CPanel *pRoot, LPARAM Extra){
		CWidget *pSender = (CWidget*)Extra;
		if(pSender == $m.pButton){
			$m.MoveToTop();
			$m.DropDown(!$m.IsChecked());
		} else if(pSender == $m.pListBox){
			$m.MoveToTop();
			$m.SetValue($m.pListBox->GetValue());
		}
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		if(!$m.IsChecked()) return;
		if($m.pButton->IsHighkey()) return;
		if($m.pListBox->IsHighkey()) return;
		if($m.pListBox->pScrollBar->IsHighkey()) return;
		$m.DropDown(B_False);
	}
	_open $VOID DropDown(BOOL8 bEnable){
		$m.Check(bEnable);
		$m.pListBox->Hide(!bEnable);
		$m.SizeY = $m.pButton->SizeY;
		if(bEnable) $m.SizeY += $m.pListBox->SizeY;
	}
	_open const WCHAR *GetValue(){
		return $m.pButton->GetValue();
	}
	_open USINT GetChoice(){
		return $m.pListBox->GetChoice();
	}
};

// Separator
class CSeparator: public CWidget{
	_open struct DESC{
		USINT Ident = 0;
		SPFPN StartX = 0.f;
		SPFPN StartY = 0.f;
		SPFPN EndX = 0.f;
		SPFPN EndY = 0.f;
		SPFPN Weight = 1.f;
		ARGB8 Color = 0x808080;
	};

	_open ARGB8 Color;
	_open SPFPN Weight;

	_open CSeparator(_in DESC &Desc){
		$m.Ident = Desc.Ident;
		$m.Color = Desc.Color;
		$m.Weight = Desc.Weight;
		$m.SizeX = Desc.EndX - Desc.StartX;
		$m.SizeY = Desc.EndY - Desc.StartY;
		$m.PosX = Desc.StartX;
		$m.PosY = Desc.StartY;
		$m.Disable(B_True);
	}
	_open $VOID OnPaint(SPFPN PosX, SPFPN PosY){
		D2D1POINTF Start = { PosX, PosY };
		D2D1POINTF End = { PosX + $m.SizeX, PosY + $m.SizeY };

		CLS::pD2DBrush->SetColor(D2DX1COLOR($m.Color));
		CLS::pD2dDevCtx->DrawLine(Start, End, CLS::pD2DBrush, $m.Weight);
	}
};

//----------------------------------------//


//-------------------- 控件上下文 --------------------//

IDWFactory *CWidget::pDWFactory;
ID2DDevCtx *CWidget::pD2dDevCtx;
ID2DPureColorBrush *CWidget::pD2DBrush;

//----------------------------------------//