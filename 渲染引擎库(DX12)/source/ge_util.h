//--------------------调试--------------------//

//弹出调试框(文本格式,文本参数...)
VOID PopDebug(CHAR* Format, ...){
	VALIST Args;
	CHAR Text[128];

	VA_START(Args, Format);
	VSPrint(Text, Format, Args);
	VA_END(Args);

	MessageBoxA(NULL, Text, NULL, MB_OK);
}
//弹出调试框(文本格式,文本参数...)
VOID PopDebugW(WCHR* Format, ...){
	VALIST Args;
	WCHR Text[128];

	VA_START(Args, Format);
	VWSPrint(Text, Format, Args);
	VA_END(Args);

	MessageBoxW(NULL, Text, NULL, MB_OK);
}

//----------------------------------------//


//--------------------二进制对齐--------------------//

inline BOOL IsMulOf2Pow(UINT Numeric, UINT Divisor){
	return !(Numeric & --Divisor);
}
inline UINT GetMulOf2Pow(UINT Floor, UINT Divisor){
	return (Floor + --Divisor) & ~Divisor;
}

inline BOOL IsPowOf2(UINT Numeric){
	return !(Numeric & (Numeric - 1));
}
inline UINT GetPowOf2(UINT Floor){
	UINT Result = 1;
	while(Result < Floor) Result <<= 1;
	return Result;
}

//----------------------------------------//


//--------------------字符串操作--------------------//

CHAR* StrCullA(CHAR* String, CHAR Match){
	CHAR *pSrcChar, *pDstChar;

	for(pSrcChar = pDstChar = String; *pSrcChar != '\0'; ++pSrcChar)
		if(*pSrcChar != Match) *pDstChar++ = *pSrcChar;
	*pDstChar = '\0';

	return String;
}
WCHR* StrCullW(WCHR* String, WCHR Match){
	WCHR *pSrcChar, *pDstChar;
	for(pSrcChar = pDstChar = String; *pSrcChar != '\0'; ++pSrcChar)
		if(*pSrcChar != Match) *pDstChar++ = *pSrcChar;
	*pDstChar = '\0';

	return String;
}
CHAR* StrCullA(CHAR* String, const CHAR* Match){
	CHAR **arrTarget;
	CHAR *pSrcChar, *pDstChar;
	UPTR StrSize, SubSize;
	UPTR CopySize, SubCount;

	//计算字符串长度

	StrSize = StrLenA(String);
	SubSize = StrLenA(Match);
	arrTarget = new CHAR*[StrSize / SubSize];

	//查找所有子串地址

	pSrcChar = String;
	for(SubCount = 0; TRUE; ++SubCount){
		arrTarget[SubCount] = StrStrA(pSrcChar, Match);
		if(!arrTarget[SubCount]) break;
		pSrcChar = arrTarget[SubCount] + SubSize;
	}

	if(SubCount == 0) goto done;
	else arrTarget[SubCount] = &String[StrSize];

	//覆盖第一个子串

	pDstChar = arrTarget[0];
	pSrcChar = pDstChar + SubSize;
	CopySize = arrTarget[1] - pSrcChar;
	CopyMemory(pDstChar, pSrcChar, CopySize);

	//覆盖其它子串

	for(UINT i = 1; i < SubCount; ++i){
		pDstChar += CopySize;
		pSrcChar = arrTarget[i] + SubSize;
		CopySize = arrTarget[i + 1] - pSrcChar;
		CopyMemory(pDstChar, pSrcChar, CopySize);
	}
	*(pDstChar + CopySize) = '\0';

	//返回字符串地址

done:
	delete[] arrTarget;
	return String;
};
WCHR* StrCullW(WCHR* String, const WCHR* Match){
	WCHR **arrTarget;
	WCHR *pDstChar, *pSrcChar;
	UPTR StrSize, SubSize;
	UPTR CopySize, SubCount;

	//计算字符串长度

	StrSize = StrLenW(String);
	SubSize = StrLenW(Match);
	arrTarget = new WCHR*[StrSize / SubSize];

	//查找所有子串地址

	pSrcChar = String;
	for(SubCount = 0; TRUE; ++SubCount){
		arrTarget[SubCount] = StrStrW(pSrcChar, Match);
		if(!arrTarget[SubCount]) break;
		pSrcChar = arrTarget[SubCount] + SubSize;
	}

	if(SubCount == 0) goto done;
	else arrTarget[SubCount] = &String[StrSize];

	//覆盖第一个子串

	pDstChar = arrTarget[0];
	pSrcChar = pDstChar + SubSize;
	CopySize = arrTarget[1] - pSrcChar;
	CopyMemory(pDstChar, pSrcChar, CopySize << 1);

	//覆盖其它子串

	for(UINT i = 1; i < SubCount; ++i){
		pDstChar += CopySize;
		pSrcChar = arrTarget[i] + SubSize;
		CopySize = arrTarget[i + 1] - pSrcChar;
		CopyMemory(pDstChar, pSrcChar, CopySize << 1);
	}
	*(pDstChar + CopySize) = L'\0';

done:
	delete[] arrTarget;
	return String;
}
CHAR* StrMergeA(CHAR* Output, const CHAR* String0, const CHAR* String1){
	UPTR Size0 = StrLenA(String0);
	UPTR Size1 = StrLenA(String1) + 1;

	if(!Output) Output = new CHAR[Size0 + Size1];
	CopyMemory(Output, String0, Size0);
	CopyMemory(&Output[Size0], String1, Size1);

	return Output;
}
WCHR* StrMergeW(WCHR* Output, const WCHR* String0, const WCHR* String1){
	UPTR Size0 = StrLenW(String0);
	UPTR Size1 = StrLenW(String1) + 1;

	if(!Output) Output = new WCHR[Size0 + Size1];
	CopyMemory(Output, String0, Size0 << 1);
	CopyMemory(&Output[Size0], String1, Size1 << 1);

	return Output;
}

WCHR* MbsToWcs(WCHR* Output, const CHAR* String, UINT* pSize = NULL){
	UPTR Size = StrLenA(String);
	if(pSize) *pSize = (UINT)Size;
	if(!Output) Output = new WCHR[++Size];
	mbstowcs(Output, String, Size);

	return Output;
}
WCHR* MbsToUcs(WCHR* Output, const CHAR* String, UINT* pSize = NULL){
	UINT Size = MultiByteToWideChar(CP_ACP, 0x0, String, -1, NULL, 0);
	if(pSize) *pSize = Size - 1;
	if(!Output) Output = new WCHR[Size];
	MultiByteToWideChar(CP_ACP, 0x0, String, -1, Output, Size);

	return Output;
}
CHAR* WcsToMbs(CHAR* Output, const WCHR* String, UINT* pSize = NULL){
	UPTR Size = StrLenW(String);
	if(pSize) *pSize = (UINT)Size;
	if(!Output) Output = new CHAR[++Size];
	wcstombs(Output, String, Size);

	return Output;
}
CHAR* UcsToMbs(CHAR* Output, const WCHR* String, UINT* pSize = NULL){
	UINT Size = WideCharToMultiByte(CP_ACP, 0x0, String, -1, NULL, 0, NULL, NULL);
	if(pSize) *pSize = Size - 1;
	if(!Output) Output = new CHAR[Size];
	WideCharToMultiByte(CP_ACP, 0x0, String, -1, Output, Size, NULL, NULL);

	return Output;
}

UINT StrHash4A(const CHAR* String){
	//!散列系数为31,131,1313,13131,131313等
	UINT Hash = 0;
	for(UINT i = 0; String[i]; ++i)
		Hash = (Hash * 131) + String[i];
	return Hash;
}
UINT StrHash4W(const WCHR* String){
	//!散列系数为31,131,1313,13131,131313等

	CHAR MultiByte[128];
	UcsToMbs(MultiByte, String);

	UINT Hash = 0;
	for(UINT i = 0; MultiByte[i]; ++i)
		Hash = (Hash * 131) + MultiByte[i];

	return Hash;
}
ULNG StrHash8A(const CHAR* String){
	//!散列系数为31,131,1313,13131,131313等
	ULNG Hash = 0;
	for(UINT i = 0; String[i]; ++i)
		Hash = (Hash * 13131) + String[i];
	return Hash;
}
ULNG StrHash8W(const WCHR* String){
	//!散列系数为31,131,1313,13131,131313等

	CHAR MultiByte[128];
	UcsToMbs(MultiByte, String);

	UINT Hash = 0;
	for(UINT i = 0; MultiByte[i]; ++i)
		Hash = (Hash * 13131) + MultiByte[i];

	return Hash;
}

#ifdef UNICODE
#define StrCull StrCullW
#define StrMerge StrMergeW
#define StrHash4 StrHash4W
#define StrHash8 StrHash8W
#else
#define StrCull StrCullA
#define StrMerge StrMergeA
#define StrHash4 StrHash4A
#define StrHash8 StrHash8A
#endif

//----------------------------------------//


//--------------------随机数--------------------//

INT4 IntSample(INT4 Min, INT4 Max){
	if(Min >= Max){
		return Min;
	} else {
		INT4 Diff = Max - Min + 1;
		return Min + (Rand() % Diff);
	}
}
SPFP RealSample(SPFP Min, SPFP Max){
	if(Min >= Max){
		return Min;
	} else{
		SPFP Ratio = (SPFP)(Rand() % 10000) * 0.0001f;
		return (Ratio * (Max - Min)) + Min;
	}
}
VOID Vec2Sample(VECTOR2 &Output, const VECTOR2 &Min, const VECTOR2 &Max){
	Output.x = RealSample(Min.x, Max.x);
	Output.y = RealSample(Min.y, Max.y);
}
VOID Vec3Sample(VECTOR3 &Output, const VECTOR3 &Min, const VECTOR3 &Max){
	Output.x = RealSample(Min.x, Max.x);
	Output.y = RealSample(Min.y, Max.y);
	Output.z = RealSample(Min.z, Max.z);
}
VOID ColorSample(COLOR128 &Output, const COLOR128 &Min, const COLOR128 &Max){
	Output.a = RealSample(Min.a, Max.a);
	Output.r = RealSample(Min.r, Max.r);
	Output.g = RealSample(Min.g, Max.g);
	Output.b = RealSample(Min.b, Max.b);
}

#define IntSample1(rang) IntSample(rang[0], rang[1])
#define RealSample1(rang) RealSample(rang[0], rang[1])
#define Vec2Sample1(out, rang) Vec2Sample(out, rang[0], rang[1])
#define Vec3Sample1(out, rang) Vec3Sample(out, rang[0], rang[1])
#define ColorSample1(out, rang) ColorSample(out, rang[0], rang[1])

//----------------------------------------//


//--------------------容器--------------------//

//循环队列
class CCycQueue{
	hide UINT Head;
	hide UINT Tail;
	hide UINT Stride;
	hide UINT Volume;
	hide BYTE *Buffer;

	open ~CCycQueue(){
		self.Clear(TRUE);
	}
	open CCycQueue(){
		ZeroMemory(this, sizeof(*this));
	}
	open CCycQueue(UINT Volume, UINT Stride){
		self.Init(Volume, Stride);
	}
	open VOID Init(UINT Volume, UINT Stride){
		self.Head = 0;
		self.Tail = 0;
		self.Stride = Stride;
		self.Volume = Volume + 1;
		self.Buffer = new BYTE[Volume * Stride];
	}
	open VOID Clear(BOOL bDestroy){
		if(!bDestroy) self.Head = self.Tail = 0;
		else RESET_ARR(self.Buffer);
	}
	open VOID Push(const VOID* pData){
		UINT Next = (self.Tail + 1) % self.Volume;
		if(Next == self.Head) return;
		UINT Dest = self.Tail * self.Stride;
		CopyMemory(&self.Buffer[Dest], pData, self.Stride);
		self.Tail = Next;
	}
	open VOID* Pop(){
		if(self.Head == self.Tail) return NULL;
		UINT Dest = self.Head * self.Stride;
		self.Head = (self.Head + 1) % self.Volume;
		return &self.Buffer[Dest];
	}
	open VOID* Get(){
		UINT Dest = self.Head * self.Stride;
		return &self.Buffer[Dest];
	}
	open UINT GetSize(){
		if(self.Tail < self.Head)
			return self.Volume - self.Head + self.Tail;
		else return self.Tail - self.Head;
	}
	open BOOL IsFull(){
		UINT Next = (self.Tail + 1) % self.Volume;
		return Next == self.Head;
	}
	open BOOL IsEmpty(){
		return self.Head == self.Tail;
	}
};
//顺序栈
class CSeqStack{
	hide UINT Top;
	hide UINT Stride;
	hide UINT Volume;
	hide BYTE *Buffer;

	open ~CSeqStack(){
		self.Clear(TRUE);
	}
	open CSeqStack(){
		ZeroMemory(this, sizeof(*this));
	}
	open CSeqStack(UINT Volume, UINT Stride){
		self.Init(Volume, Stride);
	}
	open VOID Init(UINT Volume, UINT Stride){
		self.Top = 0;
		self.Stride = Stride;
		self.Volume = Volume;
		self.Buffer = new BYTE[Volume * Stride];
	}
	open VOID Clear(BOOL bDestroy){
		if(!bDestroy) self.Top = 0;
		else RESET_ARR(self.Buffer);
	}
	open VOID Push(const VOID* pData){
		if(self.Top == self.Volume) return;
		UINT Dest = self.Top++ * self.Stride;
		CopyMemory(&self.Buffer[Dest], pData, self.Stride);
	}
	open VOID* Pop(){
		if(self.Top == 0) return NULL;;
		UINT Dest = --self.Top * self.Stride;
		return &self.Buffer[Dest];
	}
	open VOID* Get(){
		UINT Dest = (self.Top - 1) * self.Stride;
		return &self.Buffer[Dest];
	}
	open UINT GetSize(){
		return self.Top;
	}
	open BOOL IsFull(){
		return self.Top == self.Volume;
	}
	open BOOL IsEmpty(){
		return self.Top == 0;
	}
};
//顺序表
class CSeqList{
	open UINT Size;
	hide UINT Stride;
	hide UINT Volume;
	hide BYTE *Buffer;

	open ~CSeqList(){
		self.Clear(TRUE);
	}
	open CSeqList(){
		ZeroMemory(this, sizeof(*this));
	}
	open CSeqList(UINT Volume, UINT Stride){
		self.Init(Volume, Stride);
	}
	open VOID Init(UINT Volume, UINT Stride){
		self.Size = 0;
		self.Stride = Stride;
		self.Volume = Volume;
		self.Buffer = (BYTE*)MAlloc(Volume * Stride);
	}
	open VOID Clear(BOOL bDestroy){
		if(!bDestroy){
			self.Size = 0;
		} else if(self.Buffer){
			MFree(self.Buffer);
			self.Buffer = NULL;
		}
	}
	open VOID SetVolume(UINT Volume){
		if(Volume <= self.Volume) return;
		self.Volume = Volume;
		self.Buffer = (BYTE*)MRealloc(
			self.Buffer, self.Volume * self.Stride);
	}
	open VOID Insert(const VOID* pData){
		if(self.Size == self.Volume) self.Expand();
		UINT Dest = self.Size++ * self.Stride;
		CopyMemory(&self.Buffer[Dest], pData, self.Stride);
	}
	open VOID Expand(){
		self.Volume <<= 1;
		self.Buffer = (BYTE*)MRealloc(
			self.Buffer, self.Volume * self.Stride);
	}
	open VOID Shrink(){
		self.Volume = self.Size;
		self.Buffer = (BYTE*)MRealloc(
			self.Buffer, self.Volume * self.Stride);
	}
	open VOID* New(){
		if(self.Size == self.Volume) self.Expand();
		UINT Dest = self.Size++ * self.Stride;
		return &self.Buffer[Dest];
	}
	open VOID* Pop(){
		if(self.Size == 0) return NULL;
		UINT Dest = --self.Size * self.Stride;
		return &self.Buffer[Dest];
	}
	open VOID* GetFirst(){
		return self.Buffer;
	}
	open VOID* GetLast(){
		UINT Dest = (self.Size - 1) * self.Stride;
		return &self.Buffer[Dest];
	}
	open VOID* Get(UINT Index){
		if(Index >= self.Size) return NULL;
		UINT Dest = Index * self.Stride;
		return &self.Buffer[Dest];
	}
};
//循环表
class CCycList{
	open UINT Size;
	hide UINT Stride;
	hide UINT Volume;
	hide UINT Cursor;
	hide BYTE *Buffer;
	hide bool *Locks;

	open ~CCycList(){
		self.Clear(TRUE);
	}
	open CCycList(){
		ZeroMemory(this, sizeof(*this));
	}
	open CCycList(UINT Volume, UINT Stride){
		self.Init(Volume, Stride);
	}
	open VOID Init(UINT Volume, UINT Stride){
		self.Size = 0;
		self.Cursor = 0;
		self.Stride = Stride;
		self.Volume = Volume;
		self.Locks = new bool[Volume]{ 0 };
		self.Buffer = new BYTE[Volume * Stride];
	}
	open VOID Clear(BOOL bDestroy){
		if(!bDestroy){
			self.Size = self.Cursor = 0;
			ZeroMemory(self.Locks, self.Volume);
		} else{
			RESET_ARR(self.Locks);
			RESET_ARR(self.Buffer);
		}
	}
	open VOID Delete(UINT Index){
		if((Index >= self.Volume) || !self.Locks[Index]) return;
		self.Size--;
		self.Locks[Index] = FALSE;
	}
	open VOID Insert(const VOID* pData){
		if(self.Size >= self.Volume) return;

		for(UINT i = 0; i < self.Volume; ++i){
			UINT Dest = self.Cursor;
			self.Cursor = (self.Cursor + 1) % self.Volume;

			if(!self.Locks[Dest]){
				self.Size++;
				self.Locks[Dest] = TRUE;
				Dest *= self.Stride;
				CopyMemory(&self.Buffer[Dest], pData, self.Stride);
				return;
			}
		}
	}
	open VOID* New(){
		if(self.Size >= self.Volume) return NULL;

		for(UINT i = 0; i < self.Volume; ++i){
			UINT Dest = self.Cursor;
			self.Cursor = (self.Cursor + 1) % self.Volume;

			if(!self.Locks[Dest]){
				self.Size++;
				self.Locks[Dest] = TRUE;
				Dest *= self.Stride;
				return &self.Buffer[Dest];
			}
		}

		return NULL;
	}
	open VOID* Get(UINT Index){
		UINT Dest = Index * self.Stride;
		return &self.Buffer[Dest];
	}
	open BOOL IsValid(UINT Index){
		return self.Locks[Index];
	}
};
//链表
class CLinkList{
	open struct NODE{
		LPTR Data;
		NODE *Prev;
		NODE *Next;
	};

	open UINT Size;
	hide NODE *Head;
	hide NODE *Tail;

	open ~CLinkList(){
		self.Clear(TRUE);
	}
	open CLinkList(){
		self.Init();
	}
	hide VOID Init(){
		self.Size = 0;
		self.Head = new NODE;
		self.Tail = new NODE;
		self.Head->Prev = NULL;
		self.Tail->Next = NULL;
		self.Head->Next = self.Tail;
		self.Tail->Prev = self.Head;
	}
	open VOID Clear(BOOL bDestroy){
		if(self.Head){
			NODE *pNode, *Next;

			Next = self.Head->Next;
			while(Next != self.Tail){
				pNode = Next;
				Next = pNode->Next;
				delete pNode;
			}

			if(bDestroy){
				RESET_PTR(self.Head);
				RESET_PTR(self.Tail);
			} else {
				self.Head->Next = self.Tail;
				self.Tail->Prev = self.Head;
			}

			self.Size = 0;
		}
	}
	open VOID Sort(INT4(*fnCompare)(LPTR, LPTR)){
		if(self.Size > 1){
			LPTR Temp;
			UINT Cursor0, Cursor1;
			NODE *pNode0, *pNode1;

			Cursor0 = self.Size - 1;
			for(UINT i = 0; i < (self.Size - 1); ++i){
				Cursor1 = 0;

				pNode0 = self.Head->Next;
				pNode1 = pNode0->Next;
				for(UINT j = 0; j < Cursor0; ++j){
					if(fnCompare(pNode0->Data, pNode1->Data) > 0){
						SWAP(pNode0->Data, pNode1->Data, Temp);
						Cursor1 = j;
					}
					pNode0 = pNode0->Next;
					pNode1 = pNode1->Next;
				}

				if(Cursor1 == 0) return;
				Cursor0 = Cursor1;
			}
		}
	}
	open VOID Copy(const CLinkList &Other){
		NODE *pNode, *Temp;
		pNode = Other.Head->Next;

		while(pNode != Other.Tail){
			Temp = new NODE;
			self.Tail->Data = pNode->Data;
			self.Tail->Next = Temp;
			Temp->Prev = self.Tail;
			self.Tail = Temp;
			self.Size++;
		}
	}
	open VOID Insert(LPTR Data){
		NODE *Temp = new NODE;
		self.Tail->Data = Data;
		self.Tail->Next = Temp;
		Temp->Prev = self.Tail;
		Temp->Next = NULL;
		self.Tail = Temp;
		self.Size++;
	}
	open VOID Delete(UINT Index){
		if(Index < self.Size){
			NODE *pNode, *Temp;

			if(Index <= (self.Size >> 1)){
				pNode = self.Head->Next;
				for(UINT i = 0; i < Index; ++i)
					pNode = pNode->Next;
			} else{
				pNode = self.Tail->Prev;
				for(UINT i = self.Size - 1; i > Index; --i)
					pNode = pNode->Prev;
			}

			Temp = pNode->Prev;
			Temp->Next = pNode->Next;
			pNode->Next->Prev = Temp;
			delete pNode;
			self.Size--;
		}
	}
	open LPTR Get(UINT Index){
		if(Index >= self.Size){
			return 0;
		} else{
			NODE *pNode;
			if(Index <= (self.Size >> 1)){
				pNode = self.Head->Next;
				for(UINT i = 0; i < Index; ++i)
					pNode = pNode->Next;
			} else{
				pNode = self.Tail->Prev;
				for(UINT i = self.Size - 1; i > Index; --i)
					pNode = pNode->Prev;
			}
			return pNode->Data;
		}
	}
	open NODE* GetFirst(){
		return self.Head->Next;
	}
	open NODE* GetLast(){
		return self.Tail;
	}
};
//映射表
class CMapList{
	enum KEYTYPE{
		INT_KEY = 0x1,
		CHAR_KEY = 0x2,
		STRING_KEY = 0x80000000
	};
	struct PAIR{
		union{
			ULNG Hash;
			struct{
				DWRD Key;
				DWRD Type;
			};
		};
		LPTR Value;
	};

	open UINT Size;
	hide UINT Volume;
	hide BOOL bOrdered;
	hide PAIR *Items;

	open ~CMapList(){
		self.Clear(TRUE);
	}
	open CMapList(){
		ZeroMemory(this, sizeof(*this));
	}
	open CMapList(UINT Volume){
		self.Init(Volume);
	}
	open VOID Init(UINT Volume){
		self.Size = 0;
		self.Volume = Volume;
		self.bOrdered = FALSE;
		self.Items = (PAIR*)MAlloc(sizeof(PAIR) * Volume);
	}
	open VOID Clear(BOOL bDestroy){
		if(!bDestroy){
			self.Size = 0;
		} else if(self.Items){
			MFree(self.Items);
			self.Items = NULL;
		}
	}
	open VOID Expand(){
		self.Volume <<= 1;
		self.Items = (PAIR*)MRealloc(
			self.Items, sizeof(PAIR) * self.Volume);
	}
	open VOID Shrink(){
		self.Volume = self.Size;
		self.Items = (PAIR*)MRealloc(
			self.Items, sizeof(PAIR) * self.Volume);
	}
	open VOID Sort(){
		QSort(self.Items, self.Size, sizeof(PAIR), CMapList::Compare);
		self.bOrdered = TRUE;
	}
	open VOID Insert(INT4 Key, LPTR Value){
		if(self.Size == self.Volume) self.Expand();
		self.bOrdered = FALSE;

		PAIR *Dest = &self.Items[self.Size++];
		Dest->Type = INT_KEY;
		Dest->Key = Key;
		Dest->Value = Value;
	}
	open VOID Insert(CHAR Key, LPTR Value){
		if(self.Size == self.Volume) self.Expand();
		self.bOrdered = FALSE;

		PAIR *Dest = &self.Items[self.Size++];
		Dest->Type = CHAR_KEY;
		Dest->Key = Key;
		Dest->Value = Value;
	}
	open VOID Insert(const CHAR* Key, LPTR Value, ULNG* pHash = NULL){
		if(self.Size == self.Volume) self.Expand();
		self.bOrdered = FALSE;

		PAIR *Dest = &self.Items[self.Size++];
		Dest->Value = Value;
		Dest->Hash = StrHash8A(Key);
		Dest->Type |= STRING_KEY;

		if(pHash) *pHash = Dest->Hash;
	}
	open LPTR &Get(INT4 Key, UINT* pIndex = NULL){
		PAIR *List, Dest;
		UINT Low, Mid, High;

		if(pIndex) *pIndex = -1;
		if(!self.bOrdered) self.Sort();

		List = self.Items;
		Dest.Key = Key;
		Dest.Type = INT_KEY;

		Low = 0;
		High = self.Size - 1;
		while(Low <= High){
			Mid = (Low + High) / 2;
			if(Dest.Hash < List[Mid].Hash){
				High = Mid - 1;
			} else if(Dest.Hash > List[Mid].Hash){
				Low = Mid + 1;
			} else{
				if(pIndex) *pIndex = Mid;
				break;
			}
		}

		return List[Mid].Value;
	}
	open LPTR &Get(CHAR Key, UINT* pIndex = NULL){
		PAIR *List, Dest;
		UINT Low, Mid, High;

		if(pIndex) *pIndex = -1;
		if(!self.bOrdered) self.Sort();

		List = self.Items;
		Dest.Key = Key;
		Dest.Type = CHAR_KEY;

		Low = 0;
		High = self.Size - 1;
		while(Low <= High){
			Mid = (Low + High) / 2;
			if(Dest.Hash < List[Mid].Hash){
				High = Mid - 1;
			} else if(Dest.Hash > List[Mid].Hash){
				Low = Mid + 1;
			} else{
				if(pIndex) *pIndex = Mid;
				break;
			}
		}

		return List[Mid].Value;
	}
	open LPTR &Get(ULNG Hash, UINT* pIndex = NULL){
		PAIR *List = self.Items;;
		UINT Low, Mid, High;

		if(pIndex) *pIndex = -1;
		if(!self.bOrdered) self.Sort();

		Low = 0;
		High = self.Size - 1;
		while(Low <= High){
			Mid = (Low + High) / 2;
			if(Hash < List[Mid].Hash){
				High = Mid - 1;
			} else if(Hash > List[Mid].Hash){
				Low = Mid + 1;
			} else{
				if(pIndex) *pIndex = Mid;
				break;
			}
		}

		return List[Mid].Value;
	}
	open LPTR &Get(const CHAR* Key, UINT* pIndex = NULL){
		PAIR *List, Dest;
		UINT Low, Mid, High;

		if(pIndex) *pIndex = -1;
		if(!self.bOrdered) self.Sort();

		List = self.Items;
		Dest.Hash = StrHash8A(Key);
		Dest.Type |= STRING_KEY;

		Low = 0;
		High = self.Size - 1;
		while(Low <= High){
			Mid = (Low + High) / 2;
			if(Dest.Hash < List[Mid].Hash){
				High = Mid - 1;
			} else if(Dest.Hash > List[Mid].Hash){
				Low = Mid + 1;
			} else{
				if(pIndex) *pIndex = Mid;
				break;
			}
		}

		return List[Mid].Value;
	}
	open LPTR &operator[](INT4 Key){
		return self.Get(Key);
	}
	open LPTR &operator[](CHAR Key){
		return self.Get(Key);
	}
	open LPTR &operator[](CHAR* Key){
		return self.Get(Key);
	}

	static INT4 Compare(const VOID* pData0, const VOID* pData1){
		PAIR *pPair0 = (PAIR*)pData0;
		PAIR *pPair1 = (PAIR*)pData1;

		if(pPair0->Hash < pPair1->Hash) return -1;
		else if(pPair0->Hash == pPair1->Hash) return 0;
		else return 1;
	}
};

//----------------------------------------//


//--------------------计时器--------------------//

//计时器
class CTimer{
	hide ULNG LastTime;
	hide ULNG PrevTime;
	hide ULNG BaseTime;
	hide ULNG StopTime;
	hide ULNG LastSpan;
	hide ULNG StopSpan;
	hide DPFP TickPeriod;
	hide BOOL bStopped;

	open CTimer(){
		self.Initialize();
	}
	hide VOID Initialize(){
		ULNG TickRate; //!单位:计数/秒
		QueryPerformanceFrequency((BIGINT*)&TickRate);
		self.TickPeriod = 1.0 / (DPFP)TickRate;
		self.Clear();
	}
	open VOID Update(){
		if(!self.bStopped){
			self.PrevTime = self.LastTime;
			QueryPerformanceCounter((BIGINT*)&self.LastTime);
			self.LastSpan = self.LastTime - self.PrevTime;
		}
	}
	open VOID Stop(){
		if(!self.bStopped){
			self.LastSpan = 0;
			self.bStopped = TRUE;
			QueryPerformanceCounter((BIGINT*)&self.StopTime);
		}
	}
	open VOID Start(){
		if(self.bStopped){
			QueryPerformanceCounter((BIGINT*)&self.LastTime);
			self.StopSpan += self.LastTime - self.StopTime;
			self.bStopped = FALSE;
		}
	}
	open VOID Clear(){
		QueryPerformanceCounter((BIGINT*)&self.BaseTime);

		self.LastSpan = 0;
		self.StopSpan = 0;
		self.StopTime = 0;
		self.LastTime = self.BaseTime;
		self.PrevTime = self.BaseTime;
		self.bStopped = FALSE;
	}
	open DPFP LastInterval(){
		return (DPFP)self.LastSpan * self.TickPeriod;
	}
	open DPFP TotalDuration(){
		ULNG LastTime = self.bStopped ? self.StopTime : self.LastTime;
		ULNG TotalSpan = LastTime - self.BaseTime - self.StopSpan;
		return (DPFP)TotalSpan * self.TickPeriod;
	}
};

//----------------------------------------//