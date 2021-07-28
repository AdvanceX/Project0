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


//--------------------字符串散列--------------------//

UINT StringHash4A(CHAR* String){
	//!散列系数为31,131,1313,13131,131313等
	UINT Hash = 0;
	for(UINT i = 0; String[i]; ++i)
		Hash = (Hash * 131) + String[i];
	return Hash;
}
UINT StringHash4W(WCHR* String){
	//!散列系数为31,131,1313,13131,131313等

	CHAR MultiByte[128];
	UcsToMbs(MultiByte, String);

	UINT Hash = 0;
	for(UINT i = 0; MultiByte[i]; ++i)
		Hash = (Hash * 131) + MultiByte[i];

	return Hash;
}

ULNG StringHash8A(CHAR* String){
	//!散列系数为31,131,1313,13131,131313等
	ULNG Hash = 0;
	for(UINT i = 0; String[i]; ++i)
		Hash = (Hash * 13131) + String[i];
	return Hash;
}
ULNG StringHash8W(WCHR* String){
	//!散列系数为31,131,1313,13131,131313等

	CHAR MultiByte[128];
	UcsToMbs(MultiByte, String);

	UINT Hash = 0;
	for(UINT i = 0; MultiByte[i]; ++i)
		Hash = (Hash * 13131) + MultiByte[i];

	return Hash;
}

#ifdef UNICODE
	#define StringHash4 StringHash4W
	#define StringHash8 StringHash8W
#else
	#define StringHash4 StringHash4A
	#define StringHash8 StringHash8A
#endif

//----------------------------------------//


//--------------------随机数--------------------//

INT4 RandInt(INT4 Min, INT4 Max){
	if(Min >= Max){
		return Min;
	} else {
		INT4 Diff = Max - Min + 1;
		return Min + (Rand() % Diff);
	}
}
SPFP RandReal(SPFP Min, SPFP Max){
	if(Min >= Max){
		return Min;
	} else{
		SPFP Ratio = (SPFP)(Rand() % 10000) * 0.0001f;
		return (Ratio * (Max - Min)) + Min;
	}
}
VOID RandVec2(VECTOR2 &Output, VECTOR2 &Min, VECTOR2 &Max){
	Output.x = RandReal(Min.x, Max.x);
	Output.y = RandReal(Min.y, Max.y);
}
VOID RandVec3(VECTOR3 &Output, VECTOR3 &Min, VECTOR3 &Max){
	Output.x = RandReal(Min.x, Max.x);
	Output.y = RandReal(Min.y, Max.y);
	Output.z = RandReal(Min.z, Max.z);
}
VOID RandColor(COLOR128 &Output, COLOR128 &Min, COLOR128 &Max){
	Output.a = RandReal(Min.a, Max.a);
	Output.r = RandReal(Min.r, Max.r);
	Output.g = RandReal(Min.g, Max.g);
	Output.b = RandReal(Min.b, Max.b);
}

#define IntSample(rang) RandInt4(rang[0], rang[1])
#define RealSample(rang) RandReal(rang[0], rang[1])
#define Vec2Sample(output, rang) RandVec2(output, rang[0], rang[1])
#define Vec3Sample(output, rang) RandVec3(output, rang[0], rang[1])
#define ColorSample(output, rang) RandColor(output, rang[0], rang[1])

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
		MemZero(this, sizeof(*this));
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
	open VOID Push(VOID* pData){
		UINT Next = (self.Tail + 1) % self.Volume;
		if(Next == self.Head) return;
		UINT Dest = self.Tail * self.Stride;
		MemCopy(&self.Buffer[Dest], pData, self.Stride);
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
		MemZero(this, sizeof(*this));
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
	open VOID Push(VOID* pData){
		if(self.Top == self.Volume) return;
		UINT Dest = self.Top++ * self.Stride;
		MemCopy(&self.Buffer[Dest], pData, self.Stride);
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
		MemZero(this, sizeof(*this));
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
	open VOID Insert(VOID* pData){
		if(self.Size == self.Volume) self.Expand();
		UINT Dest = self.Size++ * self.Stride;
		MemCopy(&self.Buffer[Dest], pData, self.Stride);
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
		MemZero(this, sizeof(*this));
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
			MemZero(self.Locks, self.Volume);
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
	open VOID Insert(VOID* pData){
		if(self.Size >= self.Volume) return;

		for(UINT i = 0; i < self.Volume; ++i){
			UINT Dest = self.Cursor;
			self.Cursor = (self.Cursor + 1) % self.Volume;

			if(!self.Locks[Dest]){
				self.Size++;
				self.Locks[Dest] = TRUE;
				Dest *= self.Stride;
				MemCopy(&self.Buffer[Dest], pData, self.Stride);
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
	open VOID Copy(CLinkList &Other){
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
		return self.Tail->Prev;
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
		MemZero(this, sizeof(*this));
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
	open VOID Insert(CHAR* Key, LPTR Value, out ULNG* pHash = NULL){
		if(self.Size == self.Volume) self.Expand();
		self.bOrdered = FALSE;

		PAIR *Dest = &self.Items[self.Size++];
		Dest->Value = Value;
		Dest->Hash = StringHash8A(Key);
		Dest->Type |= STRING_KEY;

		if(pHash) *pHash = Dest->Hash;
	}
	open LPTR &Get(INT4 Key, out UINT* pIndex = NULL){
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
	open LPTR &Get(CHAR Key, out UINT* pIndex = NULL){
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
	open LPTR &Get(CHAR* Key, out UINT* pIndex = NULL){
		PAIR *List, Dest;
		UINT Low, Mid, High;

		if(pIndex) *pIndex = -1;
		if(!self.bOrdered) self.Sort();

		List = self.Items;
		Dest.Hash = StringHash8A(Key);
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
	open LPTR &Get(ULNG Hash, out UINT* pIndex = NULL){
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
		QueryPerformanceFrequency((BIGINTEGER*)&TickRate);
		self.TickPeriod = 1.0 / (DPFP)TickRate;
		self.Clear();
	}
	open VOID Update(){
		if(!self.bStopped){
			self.PrevTime = self.LastTime;
			QueryPerformanceCounter((BIGINTEGER*)&self.LastTime);
			self.LastSpan = self.LastTime - self.PrevTime;
		}
	}
	open VOID Stop(){
		if(!self.bStopped){
			self.LastSpan = 0;
			self.bStopped = TRUE;
			QueryPerformanceCounter((BIGINTEGER*)&self.StopTime);
		}
	}
	open VOID Start(){
		if(self.bStopped){
			QueryPerformanceCounter((BIGINTEGER*)&self.LastTime);
			self.StopSpan += self.LastTime - self.StopTime;
			self.bStopped = FALSE;
		}
	}
	open VOID Clear(){
		QueryPerformanceCounter((BIGINTEGER*)&self.BaseTime);

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