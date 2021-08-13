//--------------------控件--------------------//

//控件
struct Control{
	SPFP x, y;
	SPFP width;
	SPFP height;
	DWRD ident;
	BYTE bPoint;
	BYTE bFocus;
	BYTE bHidden;
	BYTE bDisable;
	Control *next;
	Control *prev;
	Control *child;
	Control *parent;

	Control(){
		self.next = NULL;
		self.prev = NULL;
		self.child = NULL;
		self.parent = NULL;

		self.bPoint = FALSE;
		self.bFocus = FALSE;
		self.bHidden = FALSE;
		self.bDisable = FALSE;
	}
	VOID Release(){
		Control *prev = self.prev;
		Control *next = self.next;
		Control *parent = self.parent;

		if(next) next->prev = prev;
		if(prev) prev->next = next;
		else if(parent) parent->child = next;

		self.RemoveChildren();
		delete this;
	}
	VOID MoveToTop(){
		if(!self.prev) return;

		self.prev->next = self.next;
		if(self.next) self.next->prev = self.prev;

		Control *parent = self.parent;
		self.prev = NULL;
		self.next = parent->child;
		parent->child->prev = this;
		parent->child = this;
	}
	VOID RemoveChildren(){
		Control *pNode, *next;

		pNode = self.child;
		self.child = NULL;

		while(pNode){
			if(pNode->child)
				pNode->RemoveChildren();
			next = pNode->next;
			delete pNode;
			pNode = next;
		}
	}
	VOID AddChild(Control* pObject){
		pObject->parent = this;
		pObject->prev = NULL;
		pObject->next = self.child;
		if(self.child) self.child->prev = pObject;
		self.child = pObject;
	}
	VOID GetFillRect(D2DRectF &result){
		VECTOR2 position;
		self.GetGlobalCoord(position);

		result.left = position.x;
		result.top = position.y;
		result.right = result.left + self.width;
		result.bottom = result.top + self.height;
	}
	VOID GetGlobalCoord(VECTOR2 &result){
		Control *pNode = self.parent;

		result.x = self.x;
		result.y = self.y;

		while(pNode){
			result.x += pNode->x;
			result.y += pNode->y;
			pNode = pNode->parent;
		}
	}
	VOID GetLocalCursor(UINT x, UINT y, VECTOR2 &result){
		VECTOR2 referent;
		self.GetGlobalCoord(referent);

		result.x = x - referent.x;
		result.y = y - referent.y;
	}
	UINT GetChildCount(){
		UINT count = 0;
		Control *pNode = self.child;

		while(pNode){
			count++;
			count += pNode->GetChildCount();
			pNode = pNode->next;
		}

		return count;
	}
	BOOL CursorIntersect(UINT x, UINT y){
		VECTOR2 control, cursor((SPFP)x, (SPFP)y);
		self.GetGlobalCoord(control);

		if((cursor.x >= control.x) && (cursor.x <= (control.x + self.width)) &&
			(cursor.y >= control.y) && (cursor.y <= (control.y + self.height))) return TRUE;
		else return FALSE;
	}
	Control* GetPointChild(){
		Control *pPoint;
		Control *pNode = self.child;

		while(pNode){
			if(pNode->bPoint) return pNode;
			pPoint = pNode->GetPointChild();
			if(pPoint) return pPoint;
			pNode = pNode->next;
		}

		return NULL;
	}
	Control* TestMessage(DWRD message, WPARAM param0, LPARAM param1){
		switch(message){
			case WM_MOUSEMOVE:{
				if(self.CursorIntersect(LOWORD(param1), HIWORD(param1))){
					Control *pObject = self.SpreadMessage(message, param0, param1);
					if(pObject){
						if(self.bPoint) self.OnMouseOver(FALSE);
						return pObject;
					} else{
						if(!self.bPoint) self.OnMouseOver(TRUE);
						self.OnMouseMove(LOWORD(param1), HIWORD(param1));
						return this;
					}
				} else{
					Control *pPoint;
					if(self.bPoint) pPoint = this;
					else pPoint = self.GetPointChild();
					if(pPoint) pPoint->OnMouseOver(FALSE);
					return NULL;
				}
			}
			case WM_LBUTTONDOWN:{
				if(self.bPoint){
					self.OnMouseDown(message, LOWORD(param1), HIWORD(param1));
					self.MoveToTop();
					return this;
				} else break;
			}
			case WM_LBUTTONUP:{
				if(self.bPoint){
					self.OnMouseUp(message, LOWORD(param1), HIWORD(param1));
					return this;
				} else break;
			}
			case WM_PAINT:{
				if(!self.bHidden) self.OnRender();
				break;
			}
		}

		return self.SpreadMessage(message, param0, param1);
	}
	Control* SpreadMessage(DWRD message, WPARAM param0, LPARAM param1){
		Control *pNode, *pObject;

		for(pNode = self.child; pNode; pNode = pNode->next){
			pObject = pNode->TestMessage(message, param0, param1);
			if(pObject) return pObject;
		}

		return NULL;
	}

	virtual VOID OnCharInput(WPARAM chr, LPARAM extra) = 0;
	virtual VOID OnKeyDown(WPARAM key, LPARAM extra) = 0;
	virtual VOID OnKeyUp(WPARAM key, LPARAM extra) = 0;
	virtual VOID OnMouseDown(DWRD button, UINT x, UINT y) = 0;
	virtual VOID OnMouseUp(DWRD button, UINT x, UINT y) = 0;
	virtual VOID OnMouseMove(UINT x, UINT y) = 0;
	virtual VOID OnMouseOver(BOOL bOver) = 0;
	virtual VOID OnRender() = 0;
	virtual ~Control(){}
};

//面板
struct Panel: public Control{
	using ProcFunc = VOID(*)(Control*, DWRD, WPARAM, LPARAM);

	Control *pPoint;
	Control *pFocus;
	ID2DDevCtx *lpD2DDC;
	ID2DBitmap *lpBGImage;
	IDWFactory *lpDWFactory;
	IWICFactory *lpWICFactory;
	VOID(*fnOnEvent)(Control*, DWRD, WPARAM, LPARAM);

	Panel(){}
	Panel(ProcFunc fnOnEvent, IWICFactory* lpWICFactory, IDWFactory* lpDWFactory, ID2DDevCtx* lpD2DDC, ID2DBitmap* lpBGImage = NULL){
		self.Initialize(fnOnEvent, lpWICFactory, lpDWFactory, lpD2DDC, lpBGImage);
	}
	BOOL ProcMessage(DWRD message, WPARAM param0, LPARAM param1){
		Control *pNode, *pObject = NULL;

		if(self.pFocus){
			if(message == WM_KEYDOWN){
				pObject = self.pFocus;
				pObject->OnKeyDown(param0, param1);
				goto done;
			} else if(message == WM_KEYUP){
				pObject = self.pFocus;
				pObject->OnKeyUp(param0, param1);
				goto done;
			} else if(message == WM_CHAR){
				pObject = self.pFocus;
				pObject->OnCharInput(param0, param1);
				goto done;
			} else if(message == WM_LBUTTONDOWN){
				if(self.pFocus->CursorIntersect(LOWORD(param1), HIWORD(param1))){
					pObject = self.pFocus;
					pObject->OnMouseDown(message, LOWORD(param1), HIWORD(param1));
					goto done;
				} else{
					self.pFocus->bFocus = FALSE;
					self.pFocus = NULL;
				}
			}
		}

		for(pNode = self.child; pNode; pNode = pNode->next){
			pObject = pNode->TestMessage(message, param0, param1);
			if(pObject) break;
		}

done:
		if(pObject){
			if(self.fnOnEvent) self.fnOnEvent(pObject, message, param0, param1);
			return TRUE;
		} else return FALSE;
	}
	VOID Initialize(ProcFunc fnOnEvent, IWICFactory* lpWICFactory, IDWFactory* lpDWFactory, ID2DDevCtx* lpD2DDC, ID2DBitmap* lpBGImage = NULL){
		self.pPoint = NULL;
		self.pFocus = NULL;
		self.lpD2DDC = lpD2DDC;
		self.lpBGImage = lpBGImage;
		self.lpDWFactory = lpDWFactory;
		self.lpWICFactory = lpWICFactory;
		self.fnOnEvent = fnOnEvent;
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){}
	VOID OnMouseMove(UINT x, UINT y){}
	VOID OnMouseOver(BOOL bOver){}
	VOID OnRender(){
		if(self.lpBGImage){
			D2DRectF rcFill;
			self.GetFillRect(rcFill);
			self.lpD2DDC->DrawBitmap(self.lpBGImage, rcFill, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR);
		}
	}
};

//扩展控件
struct CtrlEx: public Control{
	Panel *pPanel;

	ID2DDevCtx* GetD2DDC(){
		return self.pPanel->lpD2DDC;
	}
	IDWFactory* GetDWFactory(){
		return self.pPanel->lpDWFactory;
	}
	IWICFactory* GetWICFactory(){
		return self.pPanel->lpWICFactory;
	}
};

//标签
struct Label: public CtrlEx{
	WCHR *caption;
	ID2DBrush *lpTxtBrush;
	IDWTxtFormat *lpTxtFormat;

	~Label(){
		DELETE_ARR(self.caption);
	}
	Label(){
		self.caption = NULL;
	}
	Label(Label &label){
		*this = label;
		self.caption = new WCHR[StrLenW(label.caption) + 1];
		StrCpyW(label.caption, self.caption);
	}
	Label(WCHR* caption, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush){
		self.InitAsSample(caption, lpTxtFormat, lpTxtBrush);
	}
	Label(Panel* pPanel, SPFP x, SPFP y, WCHR* caption, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush){
		self.Initialize(pPanel, x, y, caption, lpTxtFormat, lpTxtBrush);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, WCHR* caption, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush){
		self.x = x;
		self.y = y;

		self.pPanel = pPanel;
		self.lpTxtBrush = lpTxtBrush;
		self.lpTxtFormat = lpTxtFormat;

		self.caption = NULL;
		self.SetCaption(caption);
	}
	VOID InitAsSample(WCHR* caption, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush){
		self.x = 0.0f;
		self.y = 0.0f;
		self.width = 1.0f;
		self.height = 1.0f;

		self.pPanel = NULL;
		self.lpTxtBrush = lpTxtBrush;
		self.lpTxtFormat = lpTxtFormat;

		self.caption = new WCHR[StrLenW(caption) + 1];
		StrCpyW(caption, self.caption);
	}
	VOID SetCaption(WCHR* caption){
		DELETE_ARR(self.caption);
		self.caption = new WCHR[StrLenW(caption) + 1];
		StrCpyW(caption, self.caption);
		self.UpdateSize();
	}
	VOID SetTextFormat(IDWTxtFormat* lpTxtFormat){
		self.lpTxtFormat = lpTxtFormat;
		self.UpdateSize();
	}
	VOID UpdateSize(){
		D2DSizeF size;
		GetTextSize(self.GetDWFactory(), self.lpTxtFormat, self.caption, size);

		self.width = size.width;
		self.height = size.height;
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){}
	VOID OnMouseMove(UINT x, UINT y){}
	VOID OnMouseOver(BOOL bOver){}
	VOID OnRender(){
		D2DRectF rcFill;
		self.GetFillRect(rcFill);
		self.GetD2DDC()->DrawText(self.caption, (UINT)StrLenW(self.caption),
			self.lpTxtFormat, rcFill, self.lpTxtBrush);
	}
};

//按钮
struct Button: public CtrlEx{
	Label *pLabel;
	ID2DBitmap *lpBGImage;

	~Button(){
		DELETE_PTR(self.pLabel);
	}
	Button(){
		self.pLabel = NULL;
	}
	Button(Button &button){
		*this = button;
		if(button.pLabel){
			self.pLabel = NULL;
			self.SetLabel(button.pLabel);
		}
	}
	Button(ID2DBitmap* lpBGImage){
		self.InitAsSample(lpBGImage);
	}
	Button(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, ID2DBitmap* lpBGImage, Label* pLabel = NULL){
		self.Initialize(pPanel, x, y, width, height, lpBGImage, pLabel);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, ID2DBitmap* lpBGImage, Label* pLabel = NULL){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;

		self.pPanel = pPanel;
		self.lpBGImage = lpBGImage;

		self.pLabel = NULL;
		self.SetLabel(pLabel);
	}
	VOID InitAsSample(ID2DBitmap* lpBGImage){
		self.x = 0.0f;
		self.y = 0.0f;
		self.width = 1.0f;
		self.height = 1.0f;

		self.pPanel = NULL;
		self.pLabel = NULL;
		self.lpBGImage = lpBGImage;
	}
	VOID LoadBackground(WCHR* szFilePath){
		self.lpBGImage = CreateBitmapFromFile(
			self.GetD2DDC(), self.GetWICFactory(), szFilePath);
	}
	VOID SetLabel(Label* pLabel){
		if(pLabel){
			DELETE_PTR(self.pLabel);
			self.pLabel = new Label(*pLabel);

			self.pLabel->parent = this;
			self.pLabel->pPanel = self.pPanel;
			self.pLabel->UpdateSize();

			self.pLabel->x = (self.width - self.pLabel->width) * 0.5f;
			self.pLabel->y = (self.height - self.pLabel->height) * 0.5f;
		}
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		if(button == WM_LBUTTONDOWN){
			self.bFocus = TRUE;
			self.pPanel->pFocus = this;
		}
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){
		if(button == WM_LBUTTONUP){
			if(self.bFocus){
				self.bFocus = FALSE;
				self.pPanel->pFocus = NULL;
			}
		}
	}
	VOID OnMouseMove(UINT x, UINT y){}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		if(bOver)
			self.pPanel->pPoint = this;
		else{
			if(self.pPanel->pPoint == this)
				self.pPanel->pPoint = NULL;
			if(self.bFocus){
				self.bFocus = FALSE;
				self.pPanel->pFocus = NULL;
			}
		}
	}
	VOID OnRender(){
		UINT index;
		D2DSizeF size;
		D2DRectF rcCopy;
		D2DRectF rcFill;

		if(self.bFocus) index = 2;
		else if(self.bPoint) index = 1;
		else index = 0;

		size = self.lpBGImage->GetSize();
		size.width /= 3.0f;

		rcCopy.left = size.width * index;
		rcCopy.top = 0.0f;
		rcCopy.right = size.width * (index + 1);
		rcCopy.bottom = size.height;

		self.GetFillRect(rcFill);
		self.GetD2DDC()->DrawBitmap(self.lpBGImage,
			rcFill, 1.0f, D2D1_INTERPOLATION_MODE_LINEAR, &rcCopy);
		if(self.pLabel) self.pLabel->OnRender();
	}
};

//复选框
struct CheckBox: public CtrlEx{
	BOOL bChecked;
	Label *pLabel;
	Button *pTickBox;

	~CheckBox(){
		DELETE_PTR(self.pLabel);
		DELETE_PTR(self.pTickBox);
	}
	CheckBox(){
		self.pLabel = NULL;
		self.pTickBox = NULL;
	}
	CheckBox(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, Label* pLabel, Button* pTickBox){
		self.Initialize(pPanel, x, y, width, height, pLabel, pTickBox);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, Label* pLabel, Button* pTickBox){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;

		self.pPanel = pPanel;
		self.bChecked = FALSE;

		self.pLabel = NULL;
		self.pTickBox = NULL;

		self.SetLabel(pLabel);
		self.SetTickBox(pTickBox);
	}
	VOID SetLabel(Label* pLabel){
		DELETE_PTR(self.pLabel);
		self.pLabel = new Label(*pLabel);

		self.pLabel->parent = this;
		self.pLabel->pPanel = self.pPanel;

		self.pLabel->UpdateSize();
		self.pLabel->x = self.width;
		self.pLabel->y = (self.height - self.pLabel->height) * 0.5f;
	}
	VOID SetTickBox(Button* pTickBox){
		DELETE_PTR(self.pTickBox);
		self.pTickBox = new Button(*pTickBox);

		self.pTickBox->x = 0.0f;
		self.pTickBox->y = 0.0f;
		self.pTickBox->width = self.width;
		self.pTickBox->height = self.height;
		self.pTickBox->pPanel = self.pPanel;
		self.pTickBox->parent = this;
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		if(button == WM_LBUTTONDOWN){
			self.bFocus = TRUE;
			self.pPanel->pFocus = this;

			self.bChecked = !self.bChecked;
			self.pTickBox->bFocus = self.bChecked;
		}
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){
		if(button == WM_LBUTTONUP){
			if(self.bFocus){
				self.bFocus = FALSE;
				self.pPanel->pFocus = NULL;
			}
		}
	}
	VOID OnMouseMove(UINT x, UINT y){}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		self.pTickBox->bPoint = bOver;

		if(bOver) self.pPanel->pPoint = this;
		else if(self.pPanel->pPoint == this)
			self.pPanel->pPoint = NULL;
	}
	VOID OnRender(){
		self.pLabel->OnRender();
		self.pTickBox->OnRender();
	}
};

//文本框
struct TxtBox: public CtrlEx{
	INT4 iCharIndex;
	SPFP rCursorPos;
	SPFP rFrameSize;
	ID2DBrush *lpBGBrush;
	ID2DBrush *lpTxtBrush;
	ID2DBrush *lpFrameBrush;
	IDWTxtFormat *lpTxtFormat;
	STLStringW content;

	TxtBox(){}
	TxtBox(TxtBox &TxtBox){
		self.x = TxtBox.x;
		self.y = TxtBox.y;
		self.width = TxtBox.width;
		self.height = TxtBox.height;

		self.iCharIndex = TxtBox.iCharIndex;
		self.rCursorPos = TxtBox.rCursorPos;
		self.rFrameSize = TxtBox.rFrameSize;

		self.lpBGBrush = TxtBox.lpBGBrush;
		self.lpTxtBrush = TxtBox.lpTxtBrush;
		self.lpFrameBrush = TxtBox.lpFrameBrush;
		self.lpTxtFormat = TxtBox.lpTxtFormat;

		self.pPanel = TxtBox.pPanel;
		self.content = TxtBox.content;
	}
	TxtBox(IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush, ID2DBrush* lpBGBrush, ID2DBrush* lpFrameBrush){
		self.InitAsSample(lpTxtFormat, lpTxtBrush, lpBGBrush, lpFrameBrush);
	}
	TxtBox(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush, ID2DBrush* lpBGBrush, ID2DBrush* lpFrameBrush){
		self.Initialize(pPanel, x, y, width, height, lpTxtFormat, lpTxtBrush, lpBGBrush, lpFrameBrush);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush, ID2DBrush* lpBGBrush, ID2DBrush* lpFrameBrush){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;

		self.iCharIndex = 0;
		self.rCursorPos = 0.0f;
		self.rFrameSize = 1.0f;

		self.pPanel = pPanel;
		self.lpBGBrush = lpBGBrush;
		self.lpTxtBrush = lpTxtBrush;
		self.lpFrameBrush = lpFrameBrush;
		self.lpTxtFormat = lpTxtFormat;
	}
	VOID InitAsSample(IDWTxtFormat* lpTxtFormat, ID2DBrush* lpTxtBrush, ID2DBrush* lpBGBrush, ID2DBrush* lpFrameBrush){
		self.x = 0.0f;
		self.y = 0.0f;
		self.width = 1.0f;
		self.height = 1.0f;

		self.iCharIndex = 0;
		self.rCursorPos = 0.0f;
		self.rFrameSize = 1.0f;

		self.pPanel = NULL;
		self.lpBGBrush = lpBGBrush;
		self.lpTxtBrush = lpTxtBrush;
		self.lpFrameBrush = lpFrameBrush;
		self.lpTxtFormat = lpTxtFormat;
	}
	VOID SetText(WCHR* content){
		self.content = content;
		self.SetCaretPos((INT4)StrLenW(content));
	}
	VOID InsertText(WCHR* content){
		self.content.insert(self.iCharIndex, content);
		self.SetCaretPos(self.iCharIndex + (INT4)StrLenW(content));
	}
	VOID RemoveText(INT4 quantity){
		if(self.iCharIndex < quantity) return;
		self.SetCaretPos(self.iCharIndex - quantity);
		self.content.erase(self.iCharIndex, quantity);
	}
	VOID SetCaretPos(INT4 position){
		if((position >= 0) && (position <= self.content.length())){
			STLStringW string = self.content.substr(0, position);
			self.iCharIndex = position;
			self.rCursorPos = self.GetTextWidth((WCHR*)string.c_str());
		}
	}
	VOID LocateCaretPos(UINT x, UINT y){
		if(self.content.length() == 0){
			self.iCharIndex = 0;
			self.rCursorPos = 0.0f;
			return;
		}

		SPFP rCharPos = 0.0f;
		SPFP rCharWidth = 0.0f;
		UINT nCharIndex = 0;
		UINT nCharCount = (UINT)self.content.length() - 1;

		WCHR chr[2] = L"";
		WCHR *content = (WCHR*)self.content.c_str();

		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		while(TRUE){
			chr[0] = content[nCharIndex];
			rCharWidth = self.GetTextWidth(chr);
			if((cursor.x >= rCharPos) && (cursor.x <= (rCharPos + rCharWidth))) break;
			if(nCharIndex == nCharCount) break;
			nCharIndex++;
			rCharPos += rCharWidth;
		}

		self.iCharIndex = nCharIndex + 1;
		self.rCursorPos = rCharPos + rCharWidth;
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){
		if(IsPrint((INT4)chr)){
			WCHR wchar = (WCHR)chr;
			self.InsertText(&wchar);
		}
	}
	VOID OnKeyDown(WPARAM key, LPARAM extra){
		switch(key){
			case VK_BACK:
			case VK_DELETE:{
				self.RemoveText(1);
				break;
			}
			case VK_LEFT:{
				self.SetCaretPos(self.iCharIndex - 1);
				break;
			}
			case VK_RIGHT:{
				self.SetCaretPos(self.iCharIndex + 1);
				break;
			}
		}
	}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		if(button == WM_LBUTTONDOWN){
			self.bFocus = TRUE;
			self.pPanel->pFocus = this;
			self.LocateCaretPos(x, y);
		}
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){}
	VOID OnMouseMove(UINT x, UINT y){}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		if(bOver) self.pPanel->pPoint = this;
		else if(self.pPanel->pPoint == this)
			self.pPanel->pPoint = NULL;
	}
	VOID OnRender(){
		SPFP padding;
		D2DRectF rcFill;
		D2DPointF line[2];
		ID2DDevCtx *lpID2DDC;

		self.GetFillRect(rcFill);
		lpID2DDC = self.GetD2DDC();

		if(self.lpBGBrush)
			lpID2DDC->FillRectangle(rcFill, self.lpBGBrush);
		if(self.lpFrameBrush)
			lpID2DDC->DrawRectangle(rcFill, self.lpFrameBrush, self.rFrameSize);

		lpID2DDC->DrawText(self.content.c_str(), (UINT)self.content.length(),
			self.lpTxtFormat, rcFill, self.lpTxtBrush);

		if(self.bFocus){
			padding = self.height - self.lpTxtFormat->GetFontSize();
			padding *= 0.5f;

			line[0].x = rcFill.left + self.rCursorPos;
			line[0].y = rcFill.top + padding;
			line[1].x = line[0].x;
			line[1].y = rcFill.bottom - padding;

			lpID2DDC->DrawLine(line[0], line[1], self.lpTxtBrush, 2.0f);
		}
	}
	SPFP GetTextWidth(WCHR* content){
		D2DSizeF tTxtSize;
		GetTextSize(self.GetDWFactory(), self.lpTxtFormat, content, tTxtSize);
		return tTxtSize.width;
	}
};

//滚动条
struct ScrollBar: public CtrlEx{
	UINT dSection;
	UINT nSecFrame;
	UINT curSection;
	UINT maxSection;
	BOOL bDragging;
	Button *pThumb;
	Button *pArrow0;
	Button *pArrow1;
	ID2DBitmap *lpTrack;

	~ScrollBar(){
		DELETE_PTR(self.pThumb);
		DELETE_PTR(self.pArrow0);
		DELETE_PTR(self.pArrow1);
	}
	ScrollBar(){
		self.pThumb = NULL;
		self.pArrow0 = NULL;
		self.pArrow1 = NULL;
	}
	ScrollBar(ScrollBar &tScrollBar){
		*this = tScrollBar;
		self.pThumb = NULL;
		self.pArrow0 = NULL;
		self.pArrow1 = NULL;
		self.SetButton(tScrollBar.pArrow0, tScrollBar.pArrow1, tScrollBar.pThumb);
	}
	ScrollBar(Button* pArrow0, Button* pArrow1, Button* pThumb, ID2DBitmap *lpTrack){
		self.InitAsSample(pArrow0, pArrow1, pThumb, lpTrack);
	}
	ScrollBar(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, Button* pArrow0, Button* pArrow1, Button* pThumb, ID2DBitmap *lpTrack){
		self.Initialize(pPanel, x, y, width, height, pArrow0, pArrow1, pThumb, lpTrack);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, Button* pArrow0, Button* pArrow1, Button* pThumb, ID2DBitmap *lpTrack){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;

		self.dSection = 1;
		self.nSecFrame = 1;
		self.maxSection = 1;
		self.curSection = 0;

		self.bDragging = FALSE;

		self.pPanel = pPanel;
		self.lpTrack = lpTrack;

		self.pThumb = NULL;
		self.pArrow0 = NULL;
		self.pArrow1 = NULL;

		self.SetButton(pArrow0, pArrow1, pThumb);
	}
	VOID InitAsSample(Button* pArrow0, Button* pArrow1, Button* pThumb, ID2DBitmap *lpTrack){
		self.x = 0.0f;
		self.y = 0.0f;
		self.width = 1.0f;
		self.height = 1.0f;

		self.dSection = 1;
		self.nSecFrame = 1;
		self.maxSection = 1;
		self.curSection = 0;

		self.bDragging = FALSE;

		self.pPanel = NULL;
		self.lpTrack = lpTrack;

		self.pThumb = NULL;
		self.pArrow0 = NULL;
		self.pArrow1 = NULL;

		self.SetButton(pArrow0, pArrow1, pThumb);
	}
	VOID SetButton(Button* pArrow0, Button* pArrow1, Button* pThumb){
		DELETE_PTR(self.pThumb);
		DELETE_PTR(self.pArrow0);
		DELETE_PTR(self.pArrow1);

		self.pThumb = new Button(*pThumb);
		self.pArrow0 = new Button(*pArrow0);
		self.pArrow1 = new Button(*pArrow1);

		self.pThumb->x = 0.0f;
		self.pThumb->y = self.width;
		self.pThumb->width = self.width;
		self.pThumb->height = self.GetTrackStep() * self.nSecFrame;
		self.pThumb->pPanel = self.pPanel;
		self.pThumb->parent = this;

		self.pArrow0->x = 0.0f;
		self.pArrow0->y = 0.0f;
		self.pArrow0->width = self.width;
		self.pArrow0->height = self.width;
		self.pArrow0->pPanel = self.pPanel;
		self.pArrow0->parent = this;

		self.pArrow1->x = 0.0f;
		self.pArrow1->y = self.height - self.width;
		self.pArrow1->width = self.width;
		self.pArrow1->height = self.width;
		self.pArrow1->pPanel = self.pPanel;
		self.pArrow1->parent = this;
	}
	VOID SetPanel(Panel* pPanel){
		self.pPanel = pPanel;
		self.pThumb->pPanel = pPanel;
		self.pArrow0->pPanel = pPanel;
		self.pArrow1->pPanel = pPanel;
	}
	VOID SetSize(SPFP width, SPFP height){
		self.width = width;
		self.height = height;

		self.pArrow0->width = width;
		self.pArrow0->height = width;

		self.pArrow1->width = width;
		self.pArrow1->height = width;
		self.pArrow1->y = height - width;

		self.pThumb->width = width;
		self.pThumb->height = self.GetTrackStep() * self.nSecFrame;
		self.pThumb->y = self.pArrow0->height + self.GetTrackStep()*self.curSection;
	}
	VOID SetMaxSection(UINT value){
		if((INT4)value < 1) value = 1;
		self.maxSection = value;
		self.pThumb->height = self.GetTrackStep() * self.nSecFrame;
	}
	VOID SetStartSection(UINT value){
		if((INT4)value < 0) value = 0;
		else value = MIN(value, self.maxSection - self.nSecFrame);

		self.curSection = value;
		self.pThumb->y = self.pArrow0->height + self.GetTrackStep()*value;
	}
	VOID SetSectionFrame(UINT value){
		self.nSecFrame = MIN(value, self.maxSection);
		self.pThumb->height = self.GetTrackStep() * self.nSecFrame;
	}
	VOID LocateSection(SPFP location){
		if((location >= self.pArrow0->height) &&
			(location < (self.height - self.pArrow1->height)))
		{
			SPFP rTrackStep = self.GetTrackStep();
			SPFP rSecTop = self.pArrow0->height;
			SPFP rSecBottom = rSecTop + rTrackStep;

			for(UINT i = 0; i < self.maxSection; ++i){
				if(((rSecTop <= location)) && (rSecBottom >= location)){
					self.SetStartSection(i);
					return;
				}
				rSecTop = rSecBottom;
				rSecBottom += rTrackStep;
			}
		}
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		if(cursor.y <= self.pArrow0->height){
			self.pArrow0->bFocus = TRUE;
			self.SetStartSection(self.curSection - self.dSection);
		} else if(cursor.y >= (self.height - self.pArrow1->height)){
			self.pArrow1->bFocus = TRUE;
			self.SetStartSection(self.curSection + self.dSection);
		} else if((cursor.y >= self.pThumb->y) && (cursor.y <= (self.pThumb->y + self.pThumb->height))){
			self.bDragging = TRUE;
			self.pThumb->bFocus = TRUE;
		} else self.LocateSection(cursor.y);
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){
		self.bDragging = FALSE;

		self.pArrow0->bFocus = FALSE;
		self.pArrow1->bFocus = FALSE;
		self.pThumb->bFocus = FALSE;
	}
	VOID OnMouseMove(UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		self.pThumb->bPoint = FALSE;
		self.pArrow0->bPoint = FALSE;
		self.pArrow1->bPoint = FALSE;

		if(cursor.y <= self.pArrow0->height){
			self.pArrow0->bPoint = TRUE;
			self.pArrow1->bFocus = FALSE;
			self.pThumb->bFocus = FALSE;
		} else if(cursor.y >= (self.height - self.pArrow1->height)){
			self.pArrow1->bPoint = TRUE;
			self.pArrow0->bFocus = FALSE;
			self.pThumb->bFocus = FALSE;
		} else if((cursor.y >= self.pThumb->y) && (cursor.y <= (self.pThumb->y + self.pThumb->height))){
			self.pThumb->bPoint = TRUE;
			self.pArrow0->bFocus = FALSE;
			self.pArrow1->bFocus = FALSE;
		}

		if(self.bDragging) self.LocateSection(cursor.y);
	}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		if(bOver)
			self.pPanel->pPoint = this;
		else{
			self.bDragging = FALSE;
			if(self.pPanel->pPoint == this)
				self.pPanel->pPoint = NULL;

			self.pArrow0->bPoint = FALSE;
			self.pArrow1->bPoint = FALSE;
			self.pThumb->bPoint = FALSE;

			self.pArrow0->bFocus = FALSE;
			self.pArrow1->bFocus = FALSE;
			self.pThumb->bFocus = FALSE;
		}
	}
	VOID OnRender(){
		if(self.lpTrack){
			SPFP step;
			D2DRectF rcFill;
			ID2DDevCtx *lpD2DDC;

			self.GetFillRect(rcFill);
			step = self.GetTrackStep();
			rcFill.top += self.pArrow0->height;
			rcFill.bottom = rcFill.top + step;
			lpD2DDC = self.GetD2DDC();

			for(UINT i = 0; i < self.maxSection; ++i){
				lpD2DDC->DrawBitmap(self.lpTrack, rcFill);
				rcFill.top = rcFill.bottom - 1.0f;
				rcFill.bottom += step;
			}
		}

		self.pThumb->OnRender();
		self.pArrow0->OnRender();
		self.pArrow1->OnRender();
	}
	SPFP GetTrackStep(){
		return((self.height - self.width*2.0f) / self.maxSection);
	}
};

//列表框
struct ListBox: public CtrlEx{
	struct ListItem{
		WCHR *caption;
		BOOL bSelected;
		ListItem *next;
		ListItem *prev;

		~ListItem(){
			DELETE_ARR(self.caption);
		}
		ListItem(){
			self.next = NULL;
			self.prev = NULL;
			self.caption = NULL;
			self.bSelected = FALSE;
		}
		ListItem(WCHR* caption){
			self.next = NULL;
			self.prev = NULL;
			self.caption = NULL;
			self.bSelected = FALSE;
			self.SetCaption(caption);
		}
		VOID SetCaption(WCHR* caption){
			DELETE_ARR(self.caption);
			self.caption = new WCHR[StrLenW(caption) + 1];
			StrCpyW(caption, self.caption);
		}
	};

	UINT nItemStart;
	UINT nItemFrame;
	UINT nItemCount;
	SPFP rItemHeight;
	BOOL bMultiple;
	ListItem *fstItem;
	ListItem *pPoint;
	ListItem *pSelect;
	ID2DBitmap *lpItemBG;
	ID2DBitmap *lpTargetBG;
	ID2DBrush *lpBoxBrush;
	ID2DBrush *lpTxtBrush;
	IDWTxtFormat *lpTxtFormat;
	ScrollBar *pScrollBar;

	~ListBox(){
		self.ClearItems();
		DELETE_PTR(self.pScrollBar);
	}
	ListBox(){
		self.fstItem = NULL;
		self.pScrollBar = NULL;
	}
	ListBox(ListBox &tListBox){
		*this = tListBox;
		self.pScrollBar = NULL;
		self.SetScrollBar(tListBox.pScrollBar);
	}
	ListBox(ID2DBitmap *lpItemBG, ID2DBitmap *lpTargetBG, ID2DBrush* lpBoxBrush, ID2DBrush* lpTxtBrush, IDWTxtFormat* lpTxtFormat, ScrollBar *pScrollBar){
		self.InitAsSample(lpItemBG, lpTargetBG, lpBoxBrush, lpTxtBrush, lpTxtFormat, pScrollBar);
	}
	ListBox(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, ID2DBitmap *lpItemBG, ID2DBitmap *lpTargetBG, ID2DBrush* lpBoxBrush, ID2DBrush* lpTxtBrush, IDWTxtFormat* lpTxtFormat, ScrollBar *pScrollBar){
		self.Initialize(pPanel, x, y, width, height, lpItemBG, lpTargetBG, lpBoxBrush, lpTxtBrush, lpTxtFormat, pScrollBar);
	}
	VOID Initialize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, ID2DBitmap *lpItemBG, ID2DBitmap *lpTargetBG, ID2DBrush* lpBoxBrush, ID2DBrush* lpTxtBrush, IDWTxtFormat* lpTxtFormat, ScrollBar *pScrollBar){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;

		self.nItemStart = 0;
		self.nItemCount = 0;
		self.rItemHeight = lpTxtFormat->GetFontSize();
		self.nItemFrame = (UINT)(height / self.rItemHeight);

		self.pPanel = pPanel;
		self.fstItem = NULL;
		self.pPoint = NULL;
		self.pSelect = NULL;
		self.bMultiple = FALSE;

		self.lpItemBG = lpItemBG;
		self.lpTargetBG = lpTargetBG;
		self.lpBoxBrush = lpBoxBrush;
		self.lpTxtBrush = lpTxtBrush;
		self.lpTxtFormat = lpTxtFormat;

		self.pScrollBar = NULL;
		self.SetScrollBar(pScrollBar);
	}
	VOID InitAsSample(ID2DBitmap *lpItemBG, ID2DBitmap *lpTargetBG, ID2DBrush* lpBoxBrush, ID2DBrush* lpTxtBrush, IDWTxtFormat* lpTxtFormat, ScrollBar *pScrollBar){
		self.x = 0.0f;
		self.y = 0.0f;
		self.width = 1.0f;
		self.height = 1.0f;

		self.nItemStart = 0;
		self.nItemCount = 0;
		self.rItemHeight = lpTxtFormat->GetFontSize();
		self.nItemFrame = (UINT)(self.height / self.rItemHeight);

		self.pPanel = NULL;
		self.fstItem = NULL;
		self.pPoint = NULL;
		self.pSelect = NULL;
		self.bMultiple = FALSE;

		self.lpItemBG = lpItemBG;
		self.lpTargetBG = lpTargetBG;
		self.lpBoxBrush = lpBoxBrush;
		self.lpTxtBrush = lpTxtBrush;
		self.lpTxtFormat = lpTxtFormat;

		self.pScrollBar = NULL;
		self.SetScrollBar(pScrollBar);
	}
	VOID SetScrollBar(ScrollBar *pScrollBar){
		self.pScrollBar = new ScrollBar(*pScrollBar);
		self.pScrollBar->x = self.width - 16.0f;
		self.pScrollBar->y = 0.0f;
		self.pScrollBar->parent = this;
		self.pScrollBar->SetSize(16.0f, self.height);
		
		if(self.pPanel)
			self.pScrollBar->SetPanel(self.pPanel);
	}
	VOID SetSize(SPFP width, SPFP height){
		self.width = width;
		self.height = height;
		self.nItemFrame = (UINT)(height / self.rItemHeight);

		self.pScrollBar->x = width - self.pScrollBar->width;
		self.pScrollBar->SetSize(self.pScrollBar->width, height);
		self.pScrollBar->SetSectionFrame(self.nItemFrame);
	}
	VOID UpdateScrollBar(){
		self.pScrollBar->SetMaxSection(self.nItemCount);
		if(self.pScrollBar->nSecFrame < self.nItemFrame)
			self.pScrollBar->SetSectionFrame(self.nItemCount);
	}
	VOID ClearItems(){
		ListItem *pNode, *next;

		pNode = self.fstItem;
		while(pNode){
			next = pNode->next;
			delete pNode;
			pNode = next;
		}
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		if(self.pScrollBar->CursorIntersect(x, y)){
			self.pScrollBar->OnMouseDown(button, x, y);
			self.nItemStart = self.pScrollBar->curSection;
		} else{
			VECTOR2 cursor;
			self.GetLocalCursor(x, y, cursor);
			self.pSelect = self.GetItemAtPos(cursor.y);
			if(self.pSelect && self.bMultiple){
				self.pSelect->bSelected = !self.pSelect->bSelected;
				self.pSelect = NULL;
			}
		}
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){
		if(self.pScrollBar->CursorIntersect(x, y))
			self.pScrollBar->OnMouseUp(button, x, y);
	}
	VOID OnMouseMove(UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		if(cursor.x < (self.width - self.pScrollBar->width)){
			self.pPoint = self.GetItemAtPos(cursor.y);
			if(self.pScrollBar->bPoint){
				self.pScrollBar->OnMouseOver(FALSE);
				self.pPanel->pPoint = this;
			}
		} else{
			if(!self.pScrollBar->bPoint){
				self.pScrollBar->OnMouseOver(TRUE);
				self.pPanel->pPoint = this;
			}
			self.pScrollBar->OnMouseMove(x, y);
			self.nItemStart = self.pScrollBar->curSection;
			self.pPoint = NULL;
		}
	}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		self.pPoint = NULL;

		if(bOver)
			self.pPanel->pPoint = this;
		else{
			if(self.pPanel->pPoint == this)
				self.pPanel->pPoint = NULL;
			self.pScrollBar->OnMouseOver(FALSE);
		}
	}
	VOID OnRender(){
		D2DRectF rcFill;
		ListItem *pItem;
		ID2DDevCtx *lpD2DDC;

		self.GetFillRect(rcFill);
		lpD2DDC = self.GetD2DDC();
		lpD2DDC->FillRectangle(rcFill, self.lpBoxBrush);

		if(self.fstItem){
			if(self.nItemCount > self.nItemFrame){
				self.pScrollBar->OnRender();
				rcFill.right -= self.pScrollBar->width;
			}

			pItem = self.fstItem;
			for(UINT i = 0; i != self.nItemStart; ++i)
				pItem = pItem->next;

			rcFill.bottom = rcFill.top + self.rItemHeight;
			for(UINT i = 0; i < self.nItemFrame; ++i){
				if((pItem == self.pPoint) || (pItem == self.pSelect) || pItem->bSelected)
					lpD2DDC->DrawBitmap(self.lpTargetBG, rcFill);
				else if(self.lpItemBG)
					lpD2DDC->DrawBitmap(self.lpItemBG, rcFill);

				lpD2DDC->DrawText(pItem->caption, (UINT)StrLenW(pItem->caption),
					self.lpTxtFormat, rcFill, self.lpTxtBrush);

				if(pItem->next) pItem = pItem->next;
				else break;

				rcFill.top += self.rItemHeight;
				rcFill.bottom += self.rItemHeight;
			}
		}
	}
	ListItem* AddNewItem(WCHR *caption){
		ListItem *pItem = new ListItem(caption);

		if(!self.fstItem){
			self.fstItem = pItem;
		} else{
			ListItem *pNode = self.fstItem;
			while(pNode->next) pNode = pNode->next;
			pNode->next = pItem;
			pItem->prev = pNode;
		}

		self.nItemCount++;
		self.UpdateScrollBar();

		return pItem;
	}
	ListItem* GetItemByIndex(UINT index){
		ListItem *pItem = self.fstItem;
		for(UINT i = 0; i <= index; ++i){
			if(i == index) return pItem;
			if(pItem->next) pItem = pItem->next;
			else return NULL;
		}
	}
	ListItem* GetItemAtPos(SPFP location){
		ListItem *pItem;
		SPFP rItemTop, rItemBottom;

		pItem = self.fstItem;
		for(UINT i = 0; i != self.nItemStart; ++i)
			pItem = pItem->next;

		rItemTop = 0.0f;
		rItemBottom = self.rItemHeight;

		for(UINT i = 0; i < self.nItemFrame; ++i){
			if((rItemTop <= location) && (rItemBottom >= location)) return pItem;
			if(pItem->next) pItem = pItem->next;
			else break;
			rItemTop = rItemBottom;
			rItemBottom += self.rItemHeight;
		}

		return NULL;
	}
};

//下拉列表
struct DropDownList: public CtrlEx{
	BOOL bExpanded;
	Button *pButton;
	TxtBox *pTxtBox;
	ListBox *pListBox;

	~DropDownList(){
		DELETE_PTR(self.pButton);
		DELETE_PTR(self.pTxtBox);
		DELETE_PTR(self.pListBox);
	}
	DropDownList(){
		self.pButton = NULL;
		self.pTxtBox = NULL;
		self.pListBox = NULL;
	}
	DropDownList(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, TxtBox* pTxtBox, Button* pButton, ListBox* pListBox){
		self.Initalize(pPanel, x, y, width, height, pTxtBox, pButton, pListBox);
	}
	VOID Initalize(Panel* pPanel, SPFP x, SPFP y, SPFP width, SPFP height, TxtBox* pTxtBox, Button* pButton, ListBox* pListBox){
		self.x = x;
		self.y = y;
		self.width = width;
		self.height = height;
		self.pPanel = pPanel;
		self.pButton = pButton;
		self.pListBox = pListBox;
		self.bExpanded = FALSE;

		self.pTxtBox = new TxtBox(*pTxtBox);
		self.pTxtBox->x = 0.0f;
		self.pTxtBox->y = 0.0f;
		self.pTxtBox->width = width - height - 1;
		self.pTxtBox->height = height;
		self.pTxtBox->parent = this;
		self.pTxtBox->pPanel = self.pPanel;

		self.pButton = new Button(*pButton);
		self.pButton->x = self.pTxtBox->width + 1;
		self.pButton->y = 0.0f;
		self.pButton->width = height;
		self.pButton->height = height;
		self.pButton->parent = this;
		self.pButton->pPanel = self.pPanel;

		self.pListBox = new ListBox(*pListBox);
		self.pListBox->x = 0.0f;
		self.pListBox->y = height;
		self.pListBox->parent = this;
		self.pListBox->pPanel = self.pPanel;
		self.pListBox->SetSize(width, 80);
		self.pListBox->pScrollBar->SetPanel(self.pPanel);
	}
	VOID DropDown(BOOL enable){
		if(self.bExpanded == enable) return;
		if(self.bExpanded){
			self.child = NULL;
			self.height -= self.pListBox->height;
		} else{
			self.child = self.pListBox;
			self.height += self.pListBox->height;
		}
		self.bExpanded = enable;
	}
	VOID OnCharInput(WPARAM chr, LPARAM extra){}
	VOID OnKeyDown(WPARAM key, LPARAM extra){}
	VOID OnKeyUp(WPARAM key, LPARAM extra){}
	VOID OnMouseDown(DWRD button, UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		if(cursor.x < self.pTxtBox->width){
			self.pTxtBox->OnMouseDown(button, x, y);
			self.DropDown(FALSE);
		} else{
			self.pButton->OnMouseDown(button, x, y);
			self.DropDown(!self.bExpanded);
		}
	}
	VOID OnMouseUp(DWRD button, UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		if(cursor.x < self.pTxtBox->width)
			self.pTxtBox->OnMouseUp(button, x, y);
		else self.pButton->OnMouseUp(button, x, y);
	}
	VOID OnMouseMove(UINT x, UINT y){
		VECTOR2 cursor;
		self.GetLocalCursor(x, y, cursor);

		if(cursor.x > self.pTxtBox->width)
			self.pButton->bPoint = TRUE;
		else self.pButton->bPoint = FALSE;
	}
	VOID OnMouseOver(BOOL bOver){
		self.bPoint = bOver;
		if(bOver)
			self.pPanel->pPoint = this;
		else{
			if(self.pPanel->pPoint == this)
				self.pPanel->pPoint = NULL;
			self.pButton->bPoint = FALSE;
		}
	}
	VOID OnRender(){
		if(self.pListBox->pSelect){
			self.pTxtBox->SetText(self.pListBox->pSelect->caption);
			self.pListBox->pSelect = NULL;
		}

		self.pTxtBox->OnRender();
		self.pButton->OnRender();
	}
	STLStringW &GetText(){
		if(self.pListBox->pSelect){
			self.pTxtBox->SetText(self.pListBox->pSelect->caption);
			self.pListBox->pSelect = NULL;
		}

		return self.pTxtBox->content;
	}
};

//----------------------------------------//
