//-------------------- 简单表达式 --------------------//

#define DW_CNT(arg) (sizeof(arg) / 4)
#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define AS_INT32(var) *(int*)&(var)
#define AS_SPFPN(var) *(float*)&(var)
#define AS_DWORD(var) *(unsigned long*)&(var)

#define IID_AND_PPV(ptr) __uuidof(*ptr), (void**)&ptr

#define SAFE_FREE(ptr) if(ptr){ free(ptr); ptr = nullptr; };
#define SAFE_FREEA(ptr) if(ptr){ _aligned_free(ptr); ptr = nullptr; };
#define SAFE_CLOSE(ptr) if(ptr){ CloseHandle(ptr); ptr = nullptr; };
#define SAFE_RELEASE(ptr) if(ptr){ ptr->Release(); ptr = nullptr; };
#define SAFE_DELETEA(ptr) if(ptr){ delete[] ptr; ptr = nullptr; };
#define SAFE_DELETE(ptr) if(ptr){ delete ptr; ptr = nullptr; };

#define BYTE_OFFSET(ptr, cb) ((unsigned char*)(ptr) + (cb))

#define SWAP_(v0, v1) { auto t = v0; v0 = v1; v1 = t; }
#define CAST_(dst, src) { dst = decltype(dst)(src); }
#define ASSIGN_(dst, src) { dst = *(decltype(&dst))&(src); }
#define DETACH_(dst, src) { dst = src; src = nullptr; }

//----------------------------------------//


//-------------------- 简单计算 --------------------//

#define MATH_E 2.71828182f
#define MATH_PI 3.14159265f

#define TO_RAD(deg) ((deg) * MATH_PI / 180.f)
#define TO_DEG(rad) ((rad) * 180.f / MATH_PI)

#define LO_BYTE(val) ((val) & 0xFF)
#define LO_WORD(val) ((val) & 0xFFFF)
#define HI_BYTE(val) (((val) >> 8) & 0xFF)
#define HI_WORD(val) (((val) >> 16) & 0xFFFF)

#define MAKE_WORD(low, high) (((low) & 0xFF) | (((high) & 0xFF) << 8))
#define MAKE_DWORD(low, high) (((low) & 0xFFFF) | (((high) & 0xFFFF) << 16))

#define B_CHANNEL(argb) ((argb) & 0xFF)
#define G_CHANNEL(argb) (((argb) >> 8) & 0xFF)
#define R_CHANNEL(argb) (((argb) >> 16) & 0xFF)
#define A_CHANNEL(argb) (((argb) >> 24) & 0xFF)

#define MAKE_XRGB(r, g, b) ((b) | ((g) << 8) | ((r) << 16))
#define MAKE_ARGB(r, g, b, a) ((b) | ((g) << 8) | ((r) << 16) | ((a) << 24))

#define XRGB_TO_ARGB(xrgb) ((xrgb) | 0xFF000000)
#define ARGB_TO_ABGR(argb) (((argb) & 0xFF00FF00) | (((argb) & 0xFF) << 16) | (((argb) >> 16) & 0xFF))

#define RECT_WIDTH(rc) (rc.right - rc.left)
#define RECT_HEIGHT(rc) (rc.bottom - rc.top)

#define RECT_SIZE(rc) { (rc.right - rc.left), (rc.bottom - rc.top) }
#define RECT_CENTER(rc) { (rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2 }

#define MAKE_RECT(x, y, w, h) { (x), (y), (x) + (w), (y) + (h) }
#define MAKE_SQUARE(c, hw, hh) { (c).x - (hw), (c).y - (hh), (c).x + (hw), (c).y + (hh) }

#define SAME_SIGN(v0, v1) (signbit(v0) == signbit(v1))
#define NEAR_EQUAL(v0, v1, e) (fabs((v0) - (v1)) < (e))

#define CEIL_DIV(v0, v1) (((v0) - 1)/(v1) + 1)

#define ABS_(val) (((val) < 0) ? -(val) : (val))
#define MAX_(v0, v1) (((v0) > (v1)) ? (v0) : (v1))
#define MIN_(v0, v1) (((v0) < (v1)) ? (v0) : (v1))
#define LERP_(v0, v1, t) ((v0) + ((v1) - (v0))*(t))
#define CLAMP_(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

//----------------------------------------//


//-------------------- 返回码检查 --------------------//

#define FAILED_(val) (HRESULT(val) < 0)
#define WORKED_(val) (HRESULT(val) >= 0)
#define INVALID_(val) (HANDLE(val) == INVALID_HANDLE_VALUE)

//----------------------------------------//


//-------------------- 内存接口简化 --------------------//

#define Var_Zero(dst) __stosb((BYTET*)(dst), 0, sizeof(*dst))

#define BStr_Zero(dst, cnt) __stosb((BYTET*)(dst), 0, cnt)
#define WStr_Zero(dst, cnt) __stosw((WORDT*)(dst), 0, cnt)
#define DStr_Zero(dst, cnt) __stosd((DWORD*)(dst), 0, cnt)
#define QStr_Zero(dst, cnt) __stosq((QWORD*)(dst), 0, cnt)

#define BStr_Store(dst, val, cnt) __stosb((BYTET*)(dst), val, cnt)
#define WStr_Store(dst, val, cnt) __stosw((WORDT*)(dst), val, cnt)
#define DStr_Store(dst, val, cnt) __stosd((DWORD*)(dst), val, cnt)
#define QStr_Store(dst, val, cnt) __stosq((QWORD*)(dst), val, cnt)

#define BStr_Move(dst, src, cnt) __movsb((BYTET*)(dst), (BYTET*)(src), cnt)
#define WStr_Move(dst, src, cnt) __movsw((WORDT*)(dst), (WORDT*)(src), cnt)
#define DStr_Move(dst, src, cnt) __movsd((DWORD*)(dst), (DWORD*)(src), cnt)
#define QStr_Move(dst, src, cnt) __movsq((QWORD*)(dst), (QWORD*)(src), cnt)

#define BStr_Compare(buf0, buf1, cnt) memcmp(buf0, buf1, cnt)
#define WStr_Compare(buf0, buf1, cnt) wmemcmp((WCHAR*)(buf0), (WCHAR*)(buf1), cnt)

#define BStr_Swap(buf0, buf1, cnt) swab((CHAR8*)(buf0), (CHAR8*)(buf1), cnt)

//----------------------------------------//


//-------------------- 随机数接口简化 --------------------//

#define RandVec2_3(out, min, max) RandVec2((SPFPN*)(out), (SPFPN*)(min), (SPFPN*)(max))
#define RandVec3_3(out, min, max) RandVec3((SPFPN*)(out), (SPFPN*)(min), (SPFPN*)(max))
#define RandVec4_3(out, min, max) RandVec4((SPFPN*)(out), (SPFPN*)(min), (SPFPN*)(max))

#define RandVec2_2(out, rng) RandVec2((SPFPN*)(out), (SPFPN*)(rng), (SPFPN*)((rng) + 1))
#define RandVec3_2(out, rng) RandVec3((SPFPN*)(out), (SPFPN*)(rng), (SPFPN*)((rng) + 1))
#define RandVec4_2(out, rng) RandVec4((SPFPN*)(out), (SPFPN*)(rng), (SPFPN*)((rng) + 1))

//----------------------------------------//


//-------------------- 字符集兼容1 --------------------//

#ifdef _UNICODE
#define StrCover WcsCover
#define StrRemov WcsErase
#else
#define StrCover MbsCover
#define StrRemov MbsErase
#endif

//----------------------------------------//


//-------------------- 字符集兼容2 --------------------//

#ifdef _UNICODE
#define A_TO_W(str) Utf8ToUtf16(str)
#define W_TO_A(str) Utf16ToUtf8(str)
#define A_TO_T(str) Utf8ToUtf16(str)
#define T_TO_A(str) Utf16ToUtf8(str)
#define W_TO_T(str) str
#define T_TO_W(str) str
#else
#define A_TO_W(str) Utf8ToUtf16(str)
#define W_TO_A(str) Utf16ToUtf8(str)
#define A_TO_T(str) str
#define T_TO_A(str) str
#define W_TO_T(str) Utf16ToUtf8(str)
#define T_TO_W(str) Utf8ToUtf16(str)
#endif

//----------------------------------------//


//-------------------- 常用回调类型 --------------------//

// For Each
typedef $VOID(*PFNFOREACH)(PVOID, PVOID);
// Compare
typedef INT32(*PFNCOMPARE)(_in $VOID*, _in $VOID*);

//----------------------------------------//


//-------------------- 数据比较 --------------------//

//数据比较
template <typename TYPE>
INT32 tlCompare(_in $VOID *pData0, _in $VOID *pData1){
	TYPE Data0 = *(TYPE*)pData0;
	TYPE Data1 = *(TYPE*)pData1;

	if(Data0 < Data1) return -1;
	if(Data0 > Data1) return 1;
	return 0;
}

//数据比较(逆序)
template <typename TYPE>
INT32 tlCompareR(_in $VOID *pData0, _in $VOID *pData1){
	TYPE Data0 = *(TYPE*)pData0;
	TYPE Data1 = *(TYPE*)pData1;

	if(Data0 > Data1) return -1;
	if(Data0 < Data1) return 1;
	return 0;
}

//----------------------------------------//


//-------------------- 二分查找 --------------------//

// 二分查找
template <typename TYPE>
PVOID tlBinSearch(PVOID pArray, _in TYPE &Key, USINT Count, USINT cbElement){
	TYPE *pElem;
	INT32 Mid, Low = 0, High = Count - 1;

	while(Low <= High){
		Mid = (Low + High) / 2;
		pElem = (TYPE*)BYTE_OFFSET(pArray, cbElement*Mid);

		if(*pElem > Key)
			High = Mid - 1;
		else if(*pElem < Key)
			Low = Mid + 1;
		else return pElem;
	}

	return P_Null;
}

//----------------------------------------//


//-------------------- 对齐计算 --------------------//

//是否2的幂
inline IBOOL IsPowOf2(UPINT Value){
	return !(Value & (Value - 1));
}

//升为2的幂
inline UPINT ToPowOf2(UPINT Value){
	UPINT Temp = Value - 1;
	Temp |= Temp >> 1;
	Temp |= Temp >> 2;
	Temp |= Temp >> 4;
	Temp |= Temp >> 8;
	Temp |= Temp >> 16;
	return Temp + 1;
}

//是否2的幂的倍数
inline IBOOL IsPowOf2Mul(UPINT Value, UPINT PowOf2){
	return !(Value & (PowOf2 - 1));
}

//升为2的幂的倍数
inline UPINT ToPowOf2Mul(UPINT Value, UPINT PowOf2){
	UPINT Factor = PowOf2 - 1;
	return (Value + Factor) & ~Factor;
}

//----------------------------------------//


//-------------------- 随机数生成 --------------------//

//随机整数
inline INT32 RandInt(INT32 Min, INT32 Max){
	if(Min >= Max) return Min;
	INT32 Diff = Max - Min + 1;
	return Min + (Rand() % Diff);
}

//随机实数
inline SPFPN RandReal(SPFPN Min, SPFPN Max){
	if(Min >= Max) return Min;
	SPFPN Weight = (Rand() % 10000) * 0.0001f;
	return LERP_(Min, Max, Weight);
}

//----------------------------------------//


//-------------------- 随机向量生成 --------------------//

//随机2D向量
inline $VOID RandVec2(_out SPFPN psResult[2], _in SPFPN psMin[2], _in SPFPN psMax[2]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
}

//随机3D向量
inline $VOID RandVec3(_out SPFPN psResult[3], _in SPFPN psMin[3], _in SPFPN psMax[3]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
	psResult[2] = RandReal(psMin[2], psMax[2]);
}

//随机4D向量
inline $VOID RandVec4(_out SPFPN psResult[4], _in SPFPN psMin[4], _in SPFPN psMax[4]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
	psResult[2] = RandReal(psMin[2], psMax[2]);
	psResult[3] = RandReal(psMin[3], psMax[3]);
}

//----------------------------------------//


//-------------------- 字符编码转换 --------------------//

//UTF16转为UTF8
inline USINT Utf16ToUtf8(_out CHAR8 *psDest, _in WCHAR *psSource, USINT BufSize){
	return WideCharToMultiByte(CP_UTF8, 0L, psSource, -1, psDest, BufSize, P_Null, P_Null);
}

//UTF8转为UTF16
inline USINT Utf8ToUtf16(_out WCHAR *psDest, _in CHAR8 *psSource, USINT BufSize){
	return MultiByteToWideChar(CP_UTF8, 0L, psSource, -1, psDest, BufSize);
}

//UTF16转为UTF8(静态缓存区)
inline CHAR8 *Utf16ToUtf8(_in WCHAR *psSource, USINT BufSize = 0){
	static USINT tBufSize = 0;
	static CHAR8 *tBuffer = P_Null;
	static IBOOL tIsRefBuff = B_False;

	if(BufSize != 0){
		if(tBuffer && !tIsRefBuff)
			delete tBuffer;

		if(BufSize == UINT_MAX){
			tBufSize = 0;
			tBuffer = P_Null;
			tIsRefBuff = B_False;
		} else{
			tBuffer = (CHAR8*)psSource;
			tBufSize = BufSize;
			tIsRefBuff = B_True;
		}

		return P_Null;
	} else{
		if(!tIsRefBuff){
			USINT Demand = WideCharToMultiByte(CP_UTF8, 0L, psSource, -1, P_Null, 0, P_Null, P_Null);
			if(Demand > tBufSize){
				delete tBuffer;
				tBuffer = new CHAR8[Demand];
				tBufSize = Demand;
			}
		}

		WideCharToMultiByte(CP_UTF8, 0L, psSource, -1, tBuffer, tBufSize, P_Null, P_Null);
		return tBuffer;
	}
}

//UTF8转为UTF16(静态缓存区)
inline WCHAR *Utf8ToUtf16(_in CHAR8 *psSource, USINT BufSize = 0){
	static USINT tBufSize = 0;
	static WCHAR *tBuffer = P_Null;
	static IBOOL tIsRefBuff = B_False;

	if(BufSize != 0){
		if(tBuffer && !tIsRefBuff)
			delete tBuffer;

		if(BufSize == UINT_MAX){
			tBufSize = 0;
			tBuffer = P_Null;
			tIsRefBuff = B_False;
		} else{
			tBuffer = (WCHAR*)psSource;
			tBufSize = BufSize;
			tIsRefBuff = B_True;
		}

		return P_Null;
	} else{
		if(!tIsRefBuff){
			USINT Demand = MultiByteToWideChar(CP_UTF8, 0L, psSource, -1, P_Null, 0);
			if(Demand > tBufSize){
				delete tBuffer;
				tBuffer = new WCHAR[Demand];
				tBufSize = Demand;
			}
		}

		MultiByteToWideChar(CP_UTF8, 0L, psSource, -1, tBuffer, tBufSize);
		return tBuffer;
	}
}

//----------------------------------------//


//-------------------- 字符串散列 --------------------//

//字符串散列(32b值)
USINT StrHash32(_in CHAR8 *psValue){
	USINT Hash = 2166136261;
	const CHAR8 *pChar = psValue;

	while(*pChar){
		Hash *= 16777619;
		Hash ^= *pChar++;
	}

	return Hash;
}

//字符串散列(64b值)
ULEXT StrHash64(_in CHAR8 *psValue){
	ULEXT Hash = 14695981039346656037;
	const CHAR8 *pChar = psValue;

	while(*pChar){
		Hash *= 1099511628211;
		Hash ^= *pChar++;
	}

	return Hash;
}

//----------------------------------------//


//-------------------- 字符串操作 --------------------//

//字符串擦除(字符)
CHAR8 *MbsErase(_io CHAR8 *psHost, CHAR8 Key){
	CHAR8 *pDest = MbsChr(psHost, Key);
	CHAR8 *pSource = pDest ? (pDest + 1) : P_Null;

	if(pSource){
		while(*pSource){
			if(*pSource == Key) pSource++;
			else *pDest++ = *pSource++;
		}
		*pDest = '\0';
	}

	return psHost;
}

//宽字符串擦除(字符)
WCHAR *WcsErase(_io WCHAR *psHost, WCHAR Key){
	WCHAR *pDest = WcsChr(psHost, Key);
	WCHAR *pSource = pDest ? (pDest + 1) : P_Null;

	if(pSource){
		while(*pSource){
			if(*pSource == Key) pSource++;
			else *pDest++ = *pSource++;
		}
		*pDest = L'\0';
	}

	return psHost;
}

//字符串擦除(子串)
CHAR8 *MbsErase(_io CHAR8 *psHost, _in CHAR8 *psKey){
	CHAR8 *pDest = MbsStr(psHost, psKey);
	if(!pDest) return psHost;
	UPINT Stride = MbsLen(psKey);
	CHAR8 *pSource = pDest + Stride;
	CHAR8 *pNext = MbsStr(pSource, psKey);
	CHAR8 *pEnd = psHost + MbsLen(psHost);
	UPINT Span = (pNext ? pNext : pEnd) - pSource;
	MbsNcpy(pDest, pSource, Span);

	while(pNext){
		pDest += Span;
		pSource = pNext + Stride;
		pNext = MbsStr(pSource, psKey);
		Span = (pNext ? pNext : pEnd) - pSource;
		MbsNcpy(pDest, pSource, Span);
	}
	*(pDest + Span) = '\0';

	return psHost;
}

//宽字符串擦除(子串)
WCHAR *WcsErase(_io WCHAR *psHost, _in WCHAR *psKey){
	WCHAR *pDest = WcsStr(psHost, psKey);
	if(!pDest) return psHost;
	UPINT Stride = WcsLen(psKey);
	WCHAR *pSource = pDest + Stride;
	WCHAR *pNext = WcsStr(pSource, psKey);
	WCHAR *pEnd = psHost + WcsLen(psHost);
	UPINT Span = (pNext ? pNext : pEnd) - pSource;
	WcsNcpy(pDest, pSource, Span);

	while(pNext){
		pDest += Span;
		pSource = pNext + Stride;
		pNext = WcsStr(pSource, psKey);
		Span = (pNext ? pNext : pEnd) - pSource;
		WcsNcpy(pDest, pSource, Span);
	}
	*(pDest + Span) = L'\0';

	return psHost;
}

//字符串覆盖
CHAR8 *MbsCover(_io CHAR8 *psDest, _in CHAR8 *psSource, UPINT Offset){
	CHAR8 *pDest = psDest + Offset;
	const CHAR8 *pSource = psSource;

	while(*pSource)
		*pDest++ = *pSource++;
	*pDest = '\0';

	return psDest;
}

//宽字符串覆盖
WCHAR *WcsCover(_io WCHAR *psDest, _in WCHAR *psSource, UPINT Offset){
	WCHAR *pDest = psDest + Offset;
	const WCHAR *pSource = psSource;

	while(*pSource)
		*pDest++ = *pSource++;
	*pDest = L'\0';

	return psDest;
}

//字符串覆盖(兼容WCHAR)
CHAR8 *MbsCover(_io CHAR8 *psDest, _in WCHAR *psSource, UPINT Offset){
	CHAR8 *pDest = psDest + Offset;
	const WCHAR *pSource = psSource;

	while(*pSource)
		*pDest++ = (CHAR8)*pSource++;
	*pDest = '\0';

	return psDest;
}

//宽字符串覆盖(兼容CHAR8)
WCHAR *WcsCover(_io WCHAR *psDest, _in CHAR8 *psSource, UPINT Offset){
	WCHAR *pDest = psDest + Offset;
	const CHAR8 *pSource = psSource;

	while(*pSource)
		*pDest++ = *pSource++;
	*pDest = L'\0';

	return psDest;
}

//----------------------------------------//


//-------------------- 节点操作 --------------------//

//统计节点
USINT CountNodes(PVOID pHead, UPINT cbHeader){
	USINT Count = 0;
	BYTET *psNode = (BYTET*)pHead;
	BYTET *psNodePtr;

	while(psNode){
		psNodePtr = psNode + cbHeader;
		psNode = *(BYTET**)psNodePtr;
		Count++;
	}

	return Count;
}

//移除表头
PVOID PopFront(_io PVOID *ppHead, UPINT cbHeader){
	BYTET *psNode = (BYTET*)*ppHead;
	*ppHead = *($VOID**)(psNode + cbHeader);
	return psNode;
}

//移除节点
PVOID RemoveNode(PVOID pPrev, UPINT cbHeader){
	BYTET **ppNext = (BYTET**)BYTE_OFFSET(pPrev, cbHeader);
	BYTET *pTarget = *ppNext;
	*ppNext = *(BYTET**)(pTarget + cbHeader);
	return pTarget;
}

//插入表头
$VOID PushFront(_io PVOID *ppHead, PVOID pTarget, UPINT cbHeader){
	BYTET *pNodePtr = (BYTET*)pTarget + cbHeader;
	*($VOID**)pNodePtr = *ppHead;
	*ppHead = pTarget;
}

//插入节点
$VOID InsertNode(PVOID pPrev, PVOID pTarget, UPINT cbHeader){
	PVOID *ppNext0 = (PVOID*)BYTE_OFFSET(pPrev, cbHeader);
	PVOID *ppNext1 = (PVOID*)BYTE_OFFSET(pTarget, cbHeader);
	*ppNext1 = *ppNext0;
	*ppNext0 = pTarget;
}

//----------------------------------------//


//-------------------- 节点 --------------------//

// Singly Linked List Node
struct SLLNODE{
	SLLNODE *pNext;
	BYTET Data[0];

	PVOID operator new(UPINT cbHeader, UPINT cbData, PVOID pData = P_Null, _in SLLNODE *pNext = P_Null){
		SLLNODE *pNode = (SLLNODE*)Malloc(cbHeader + cbData);
		IBOOL bSelfRef = ((UPINT)pNext == 0x10);
		pNode->pNext = bSelfRef ? pNode : (SLLNODE*)pNext;
		if(pData) BStr_Move(pNode->Data, pData, cbData);
		return pNode;
	}
	$VOID operator delete(PVOID pTarget){
		Mfree(pTarget);
	}

	///单向链表
	static $VOID Delete(_io SLLNODE *pPrev){
		SLLNODE *pNode = pPrev->pNext;
		pPrev->pNext = pNode->pNext;
		delete pNode;
	}
	static $VOID PopBack(_io SLLNODE **ppHead){
		if(!(*ppHead)->pNext){
			delete *ppHead;
			*ppHead = P_Null;
		}

		SLLNODE *pPrev = *ppHead;
		SLLNODE *pNode = pPrev->pNext;

		while(pNode->pNext){
			pPrev = pNode;
			pNode = pNode->pNext;
		}

		pPrev->pNext = P_Null;
		delete pNode;
	}
	static $VOID PopFront(_io SLLNODE **ppHead){
		SLLNODE *pNext = (*ppHead)->pNext;
		delete *ppHead;
		*ppHead = pNext;
	}
	static $VOID DeleteAll(_in SLLNODE *pHead){
		const SLLNODE *pNext;
		const SLLNODE *pNode = pHead;

		while(pNode){
			pNext = pNode->pNext;
			delete pNode;
			pNode = pNext;
		}
	}
	static $VOID Insert(_io SLLNODE *pPrev, PVOID pData, UPINT cbData){
		pPrev->pNext = new(cbData, pData, pPrev->pNext)SLLNODE;
	}
	static $VOID PushBack(_io SLLNODE **ppHead, PVOID pData, UPINT cbData){
		if(!*ppHead){
			*ppHead = new(cbData, pData)SLLNODE;
		} else{
			SLLNODE *pNode = *ppHead;
			while(pNode->pNext) pNode = pNode->pNext;
			pNode->pNext = new(cbData, pData)SLLNODE;
		}
	}
	static $VOID PushFront(_io SLLNODE **ppHead, PVOID pData, UPINT cbData){
		*ppHead = new(cbData, pData, *ppHead)SLLNODE;
	}
	static $VOID Sort(_io SLLNODE **ppHead, PFNCOMPARE pfnCompare){
		USINT End0, End1, Count;
		SLLNODE *pNode0, *pNode1, *pNode2;

		Count = SLLNODE::Count(*ppHead);
		if(Count <= 1) return;

		End0 = Count - 1;
		for(USINT iExe0 = 0; iExe0 < (Count - 1); ++iExe0){
			End1 = 0;

			pNode0 = P_Null;
			pNode1 = *ppHead;
			pNode2 = pNode1->pNext;

			for(USINT iExe1 = 0; iExe1 < End0; ++iExe1){
				if(pfnCompare(pNode1->Data, pNode2->Data) > 0){
					if(!pNode0) *ppHead = pNode2;
					else pNode0 = pNode0->pNext = pNode2;
					pNode1->pNext = pNode2->pNext;
					pNode2->pNext = pNode1;
					pNode2 = pNode1->pNext;
					End1 = iExe1;
				} else{
					pNode0 = pNode1;
					pNode1 = pNode2;
					pNode2 = pNode2->pNext;
				}
			}

			if(End1 == 0) return;
			else End0 = End1;
		}
	}
	static USINT Count(_in SLLNODE *pHead){
		USINT Count = 0;
		const SLLNODE *pNode = pHead;

		while(pNode){
			pNode = pNode->pNext;
			Count++;
		}

		return Count;
	}
	static IBOOL Override(_io SLLNODE **ppHead, PVOID pKey, UPINT cbKey, PFNCOMPARE pfnCompare){
		SLLNODE *pTarget = SLLNODE::Find(*ppHead, pKey, pfnCompare);
		if(pTarget){
			BStr_Move(pTarget->Data, pKey, cbKey);
			return B_True;
		} else{
			SLLNODE::PushBack(ppHead, pKey, cbKey);
			return B_False;
		}
	}
	static SLLNODE *Find(_in SLLNODE *pHead, PVOID pKey, PFNCOMPARE pfnCompare){
		INT32 Diff;
		const SLLNODE *pNode = pHead;

		for(; pNode; pNode = pNode->pNext){
			Diff = pfnCompare(pKey, pNode->Data);
			if(!Diff) return (SLLNODE*)pNode;
		}

		return P_Null;
	}
	static SLLNODE *GetAt(_in SLLNODE *pHead, USINT Index){
		USINT CurIdx = 0;
		const SLLNODE *pNode = pHead;

		while((CurIdx < Index) && pNode->pNext){
			pNode = pNode->pNext;
			CurIdx++;
		}

		if(CurIdx != Index) return P_Null;
		return (SLLNODE*)pNode;
	}
	///循环链表
	static $VOID DeleteAllC(_in SLLNODE *pHead){
		if(pHead){
			const SLLNODE *pNext;
			const SLLNODE *pNode = pHead;

			do{
				pNext = pNode->pNext;
				delete pNode;
				pNode = pNext;
			} while(pNode != pHead);
		}
	}
	static USINT CountC(_in SLLNODE *pHead){
		USINT Count = pHead ? 1 : 0;
		SLLNODE *pNode = pHead ? pHead->pNext : P_Null;

		while(pNode != pHead){
			pNode = pNode->pNext;
			Count++;
		}

		return Count;
	}
	static IBOOL OverrideC(_io SLLNODE **ppHead, PVOID pKey, UPINT cbKey, PFNCOMPARE pfnCompare){
		SLLNODE *pTarget = SLLNODE::FindC(*ppHead, pKey, pfnCompare);
		if(pTarget){
			BStr_Move(pTarget->Data, pKey, cbKey);
			return B_True;
		} else{
			if(*ppHead) SLLNODE::Insert(*ppHead, pKey, cbKey);
			else *ppHead = new(cbKey, pKey, (SLLNODE*)0x10)SLLNODE;
			return B_False;
		}
	}
	static SLLNODE *FindC(_in SLLNODE *pHead, PVOID pKey, PFNCOMPARE pfnCompare){
		INT32 Diff;
		const SLLNODE *pNode = pHead;

		if(!pNode) return P_Null;

		do{
			Diff = pfnCompare(pKey, pNode->Data);
			if(!Diff) return (SLLNODE*)pNode;
			pNode = pNode->pNext;
		} while(pNode != pHead);

		return P_Null;
	}
};

// Balanced Binary Tree Node
struct BBTNODE{
	using CLS = BBTNODE;

	BBTNODE *pLeft;
	BBTNODE *pRight;
	USINT cbData;
	USINT Depth;
	ULEXT Ident;
	BYTET Data[0];

	PVOID operator new(UPINT cbHeader, UPINT cbData, ULEXT Ident, PVOID pData){
		BBTNODE *pNode = (BBTNODE*)Malloc(cbHeader + cbData);

		pNode->Depth = 0;
		pNode->Ident = Ident;
		pNode->cbData = (USINT)cbData;
		pNode->pRight = P_Null;
		pNode->pLeft = P_Null;

		if(pData) BStr_Move(pNode->Data, pData, cbData);
		else BStr_Zero(pNode->Data, cbData);

		return pNode;
	}
	$VOID operator delete(PVOID pTarget){
		Mfree(pTarget);
	}

	static $VOID Dispatch(_io BBTNODE *pIter, PFNFOREACH pfnForEach){
		if(pIter){
			CLS::Dispatch(pIter->pLeft, pfnForEach);
			pfnForEach(pIter->Data, P_Null);
			CLS::Dispatch(pIter->pRight, pfnForEach);
		}
	}
	static $VOID DeleteAll(_in BBTNODE *pIter){
		if(pIter){
			CLS::DeleteAll(pIter->pLeft);
			CLS::DeleteAll(pIter->pRight);
			delete pIter;
		}
	}
	static $VOID UpdateDepth(_io BBTNODE *pNode){
		USINT DepthL = CLS::GetDepth(pNode->pLeft);
		USINT DepthR = CLS::GetDepth(pNode->pRight);
		pNode->Depth = MAX_(DepthL, DepthR) + 1;
	}
	static USINT GetSize(_in BBTNODE *pNode){
		if(!pNode) return 0;
		USINT SizeL = CLS::GetSize(pNode->pLeft);
		USINT SizeR = CLS::GetSize(pNode->pRight);
		return SizeL + SizeR + 1;
	}
	static USINT GetDepth(_in BBTNODE *pNode){
		if(!pNode) return 0;
		return pNode->Depth;
	}
	static INT32 GetBalance(_in BBTNODE *pNode){
		if(!pNode) return 0;
		INT32 DepthL = CLS::GetDepth(pNode->pLeft);
		INT32 DepthR = CLS::GetDepth(pNode->pRight);
		return DepthL - DepthR;
	}
	static BBTNODE *FindMax(_in BBTNODE *pRoot){
		const BBTNODE *pNode = pRoot;
		while(pNode->pRight) pNode = pNode->pRight;
		return (BBTNODE*)pNode;
	}
	static BBTNODE *FindMin(_in BBTNODE *pRoot){
		const BBTNODE *pNode = pRoot;
		while(pNode->pLeft) pNode = pNode->pLeft;
		return (BBTNODE*)pNode;
	}
	static BBTNODE *Rebalance(_io BBTNODE *pNode){
		INT32 Balance = CLS::GetBalance(pNode);

		if(Balance > 1){
			if(CLS::GetBalance(pNode->pLeft) > 0){
				return CLS::RotateRight(pNode);
			} else{
				pNode->pLeft = CLS::RotateLeft(pNode->pLeft);
				return CLS::RotateRight(pNode);
			}
		}
		if(Balance < -1){
			if(CLS::GetBalance(pNode->pRight) < 0){
				return CLS::RotateLeft(pNode);
			} else{
				pNode->pRight = CLS::RotateRight(pNode->pRight);
				return CLS::RotateLeft(pNode);
			}
		}

		return pNode;
	}
	static BBTNODE *RotateLeft(_io BBTNODE *pNode){
		BBTNODE *pRight = pNode->pRight;

		pNode->pRight = pRight->pLeft;
		pRight->pLeft = pNode;

		CLS::UpdateDepth(pNode);
		CLS::UpdateDepth(pRight);

		return pRight;
	}
	static BBTNODE *RotateRight(_io BBTNODE *pNode){
		BBTNODE *pLeft = pNode->pLeft;

		pNode->pLeft = pLeft->pRight;
		pLeft->pRight = pNode;

		CLS::UpdateDepth(pNode);
		CLS::UpdateDepth(pLeft);

		return pLeft;
	}
	static BBTNODE *Find(_in BBTNODE *pIter, ULEXT Ident){
		if(!pIter) return P_Null;

		if(Ident < pIter->Ident)
			return CLS::Find(pIter->pLeft, Ident);
		if(Ident > pIter->Ident)
			return CLS::Find(pIter->pRight, Ident);

		return (BBTNODE*)pIter;
	}
	static BBTNODE *Delete(_io BBTNODE *pIter, ULEXT Ident){
		if(!pIter) return P_Null;

		if(Ident < pIter->Ident){
			pIter->pLeft = CLS::Delete(pIter->pLeft, Ident);
		} else if(Ident > pIter->Ident){
			pIter->pRight = CLS::Delete(pIter->pRight, Ident);
		} else{
			if(pIter->pLeft && pIter->pRight){
				if(CLS::GetDepth(pIter->pLeft) > CLS::GetDepth(pIter->pRight)){
					BBTNODE *pMaxNode = CLS::FindMax(pIter->pLeft);
					BStr_Move(pIter->Data, pMaxNode->Data, pIter->cbData);
					pIter->Ident = pMaxNode->Ident;
					pIter->pLeft = CLS::Delete(pIter->pLeft, pMaxNode->Ident);
				} else{
					BBTNODE *pMinNode = CLS::FindMin(pIter->pRight);
					BStr_Move(pIter->Data, pMinNode->Data, pIter->cbData);
					pIter->Ident = pMinNode->Ident;
					pIter->pRight = CLS::Delete(pIter->pRight, pMinNode->Ident);
				}
			} else if(pIter->pLeft || pIter->pRight){
				BBTNODE *pTarget = pIter;
				if(pIter->pLeft) pIter = pIter->pLeft;
				else pIter = pIter->pRight;
				delete pTarget;
			} else{
				delete pIter;
				return P_Null;
			}
		}

		CLS::UpdateDepth(pIter);
		return pIter;
	}
	static BBTNODE *Insert(_io BBTNODE *pIter, _io BBTNODE *pTarget){
		if(!pIter) return pTarget;

		if(pTarget->Ident < pIter->Ident){
			pIter->pLeft = CLS::Insert(pIter->pLeft, pTarget);
		} else if(pTarget->Ident > pIter->Ident){
			pIter->pRight = CLS::Insert(pIter->pRight, pTarget);
		} else{
			pTarget->pLeft = pIter->pLeft;
			pTarget->pRight = pIter->pRight;
			delete pIter;
			return pTarget;
		}

		return CLS::Rebalance(pIter);
	}
	static BBTNODE *Insert(_io BBTNODE *pRoot, UPINT cbData, ULEXT Ident, PVOID pData){
		BBTNODE *pTarget = new(cbData, Ident, pData)BBTNODE;
		return CLS::Insert(pRoot, pTarget);
	}
};

//----------------------------------------//


//-------------------- 容器 --------------------//

// Sequence List
class CSeqList{
	_rest USINT Size;
	_rest USINT Capacity;
	_rest USINT cbElement;
	_rest BYTET *lpsData;

	_open ~CSeqList(){
		$m.Clear(B_True);
	}
	_open CSeqList(){
		Var_Zero(this);
	}
	_open CSeqList(USINT Capacity, USINT cbElement){
		$m.Init(Capacity, cbElement);
	}
	//////
	_open $VOID Init(USINT Capacity, USINT cbElement){
		$m.Size = 0;
		$m.Capacity = Capacity;
		$m.cbElement = cbElement;
		$m.lpsData = (BYTET*)Malloc(cbElement * Capacity);
	}
	_open $VOID Sort(PFNCOMPARE pfnCompare){
		QckSort($m.lpsData, $m.Size, $m.cbElement, pfnCompare);
	}
	_open $VOID Clear(IBOOL bDestroy){
		if($m.lpsData){
			if(!bDestroy){
				$m.Size = 0;
			} else{
				Mfree($m.lpsData);
				$m.lpsData = P_Null;
			}
		}
	}
	_open $VOID Reserve(USINT Capacity){
		$m.Capacity = Capacity;
		$m.Size = MIN_($m.Size, Capacity);
		$m.lpsData = (BYTET*)Mrealloc($m.lpsData, $m.cbElement * $m.Capacity);
	}
	_open $VOID Delete(USINT Index){
		if(--$m.Size != Index){
			BYTET *psDest = $m.lpsData + ($m.cbElement * Index);
			BYTET *psSource = $m.lpsData + ($m.cbElement * $m.Size);
			BStr_Move(psDest, psSource, $m.cbElement);
		}
	}
	_open $VOID PushBack(PVOID pData, USINT Count = 1){
		BStr_Move($m.New(Count), pData, Count * $m.cbElement);
	}
	_open PVOID PopBack(USINT Count = 1){
		$m.Size -= Count;
		return $m.lpsData + ($m.Size * $m.cbElement);
	}
	_open PVOID Detach(){
		PVOID lpsData;
		$m.Reserve($m.Size);
		DETACH_(lpsData, $m.lpsData);
		return lpsData;
	}
	_open PVOID GetBack(){
		USINT Offset = ($m.Size - 1) * $m.cbElement;
		return $m.lpsData + Offset;
	}
	_open PVOID GetFront(){
		return $m.lpsData;
	}
	_open PVOID Get(USINT Index){
		USINT Offset = Index * $m.cbElement;
		return $m.lpsData + Offset;
	}
	_open PVOID New(USINT Count = 1){
		USINT Offset = $m.Size * $m.cbElement;
		$m.Size += Count;
		if($m.Size > $m.Capacity) $m.Reserve((USINT)ToPowOf2($m.Size));
		return $m.lpsData + Offset;
	}
	_open PVOID Find(PVOID pKey, PFNCOMPARE pfnCompare){
		return LinFind(pKey, $m.lpsData, &$m.Size, $m.cbElement, pfnCompare);
	}
	_open IBOOL Override(PVOID pKey, PFNCOMPARE pfnCompare){
		PVOID pTarget = $m.Find(pKey, pfnCompare);
		if(pTarget){
			BStr_Move(pTarget, pKey, $m.cbElement);
			return B_True;
		} else{
			$m.PushBack(pKey);
			return B_False;
		}
	}
	_open USINT GetCapacity(){
		return $m.Capacity;
	}
	_open USINT GetSize(){
		return $m.Size;
	}
};

// Linked List
class CLnkList{
	_open struct NODE{
		NODE *pPrev;
		NODE *pNext;
		BYTET Data[0];

		PVOID operator new(UPINT cbHeader, UPINT cbData){
			NODE *pNode = (NODE*)Malloc(cbHeader + cbData);
			pNode->pPrev = P_Null;
			pNode->pNext = P_Null;
			return pNode;
		}
		$VOID operator delete(PVOID pTarget){
			Mfree(pTarget);
		}
	};

	_rest USINT Size;
	_rest USINT cbElement;
	_rest NODE *lpHead;
	_rest NODE *lpTail;

	_open ~CLnkList(){
		$m.Clear(B_True);
	}
	_open CLnkList(){
		Var_Zero(this);
	}
	_open CLnkList(USINT cbElement){
		$m.Init(cbElement);
	}
	//////
	_open $VOID Init(USINT cbElement){
		$m.Size = 0;
		$m.cbElement = cbElement;
		$m.lpHead = new(0)NODE;
		$m.lpTail = new(0)NODE;
		$m.lpHead->pNext = $m.lpTail;
		$m.lpTail->pPrev = $m.lpHead;
	}
	_open $VOID Clear(IBOOL bDestroy){
		if($m.lpHead){
			//数据删除

			NODE *pNode = $m.lpHead->pNext;
			NODE *pNext = pNode->pNext;

			while(pNode != $m.lpTail){
				delete pNode;
				pNode = pNext;
				pNext = pNode->pNext;
			}

			//头尾删除

			if(bDestroy){
				SAFE_DELETE($m.lpHead);
				SAFE_DELETE($m.lpTail);
			} else{
				$m.lpHead->pNext = $m.lpTail;
				$m.lpTail->pPrev = $m.lpHead;
			}

			//归零尺寸

			$m.Size = 0;
		}
	}
	_open $VOID Sort(PFNCOMPARE pfnCompare){
		if($m.Size > 1){
			USINT End0, End1;
			NODE *pNode0, *pNode1;

			End0 = $m.Size - 1;
			for(USINT iExe0 = 0; iExe0 < ($m.Size - 1); ++iExe0){
				End1 = 0;

				pNode0 = $m.lpHead->pNext;
				pNode1 = pNode0->pNext;

				for(USINT iExe1 = 0; iExe1 < End0; ++iExe1){
					if(pfnCompare(pNode0->Data, pNode1->Data) > 0){
						BStr_Swap(pNode0->Data, pNode1->Data, $m.cbElement);
						End1 = iExe1;
					}
					pNode0 = pNode1;
					pNode1 = pNode1->pNext;
				}

				if(End1 == 0) return;
				else End0 = End1;
			}
		}
	}
	_open $VOID Copy(_in NODE *pHead, _in NODE *pEnd = P_Null){
		NODE *pDest = $m.lpTail;
		const NODE *pSource = pHead;

		while(pSource != pEnd){
			BStr_Move(pDest->Data, pSource->Data, $m.cbElement);
			$m.Size++;
			$m.lpTail = new($m.cbElement)NODE;
			$m.lpTail->pPrev = pDest;
			pSource = pSource->pNext;
			pDest->pNext = $m.lpTail;
			pDest = $m.lpTail;
		}
	}
	_open $VOID Insert(_io NODE *pPrev, PVOID pData){
		BStr_Move($m.New(pPrev), pData, $m.cbElement);
	}
	_open $VOID Delete(_in NODE *pNode){
		NODE *pPrev = pNode->pPrev;
		NODE *pNext = pNode->pNext;

		pPrev->pNext = pNext;
		pNext->pPrev = pPrev;

		$m.Size -= 1;
		delete pNode;
	}
	_open $VOID PushFront(PVOID pData){
		$m.Insert($m.lpHead, pData);
	}
	_open $VOID PushBack(PVOID pData){
		$m.Insert($m.lpTail->pPrev, pData);
	}
	_open $VOID PopFront(){
		$m.Delete($m.lpHead->pNext);
	}
	_open $VOID PopBack(){
		$m.Delete($m.lpTail->pPrev);
	}
	_open PVOID GetFront(){
		return $m.lpHead->pNext->Data;
	}
	_open PVOID GetBack(){
		return $m.lpTail->pPrev->Data;
	}
	_open PVOID Get(USINT Index){
		return $m.GetNode(Index)->Data;
	}
	_open PVOID New(_io NODE *pPrev = P_Null){
		if(pPrev == P_Null) pPrev = $m.lpTail->pPrev;

		NODE *pNode = new($m.cbElement)NODE;
		NODE *pNext = pPrev->pNext;

		pNode->pPrev = pPrev;
		pNode->pNext = pNext;
		pNext->pPrev = pNode;
		pPrev->pNext = pNode;

		$m.Size += 1;
		return pNode->Data;
	}
	_open NODE *GetStart(){
		return $m.lpHead->pNext;
	}
	_open NODE *GetEnd(){
		return $m.lpTail;
	}
	_open NODE *GetStartR(){
		return $m.lpTail->pPrev;
	}
	_open NODE *GetEndR(){
		return $m.lpHead;
	}
	_open NODE *GetNode(USINT Index){
		NODE *pNode;

		if(Index <= ($m.Size / 2)){
			pNode = $m.lpHead->pNext;
			for(USINT iNode = 0; iNode < Index; ++iNode)
				pNode = pNode->pNext;
		} else{
			pNode = $m.lpTail;
			for(USINT iNode = $m.Size; iNode > Index; --iNode)
				pNode = pNode->pPrev;
		}

		return pNode;
	}
	_open NODE *Find(PVOID pKey, PFNCOMPARE pfnCompare){
		INT32 Diff;
		NODE *pNode = $m.lpHead->pNext;

		while(pNode != $m.lpTail){
			Diff = pfnCompare(pKey, pNode->Data);
			if(!Diff) return pNode;
			pNode = pNode->pNext;
		}

		return P_Null;
	}
	_open IBOOL Override(PVOID pKey, PFNCOMPARE pfnCompare){
		NODE *pTarget = $m.Find(pKey, pfnCompare);
		if(pTarget){
			BStr_Move(pTarget->Data, pKey, $m.cbElement);
			return B_True;
		} else{
			$m.PushBack(pKey);
			return B_False;
		}
	}
	_open USINT GetSize(){
		return $m.Size;
	}
};

// Sequence Dictionary
class CSeqDict: protected CSeqList{
	_open struct PAIR{
		ULEXT Key;
		BYTET Value[0];
	};

	_rest IBOOL bSorted;
	_rest USINT cbValue;

	_open ~CSeqDict(){}
	_open CSeqDict(): CSeqList(){}
	_open CSeqDict(USINT Capacity, USINT cbValue) : CSeqList(Capacity, cbValue + sizeof(PAIR)){
		$m.bSorted = B_False;
		$m.cbValue = cbValue;
	}
	//////
	_open $VOID Init(USINT Capacity, USINT cbValue){
		$m.bSorted = B_False;
		$m.cbValue = cbValue;
		$sup Init(Capacity, cbValue + sizeof(PAIR));
	}
	_open $VOID Clear(IBOOL bDestroy){
		$sup Clear(bDestroy);
	}
	_open $VOID Reserve(USINT Capacity){
		$sup Reserve(Capacity);
	}
	_open $VOID Insert(_in CHAR8 *psKey, PVOID pValue){
		$m.Insert(StrHash64(psKey), pValue);
	}
	_open $VOID Insert(ULEXT Key, PVOID pValue){
		BStr_Move($m.New(Key), pValue, $m.cbValue);
	}
	_open PVOID New(_in CHAR8 *psKey){
		return $m.New(StrHash64(psKey));
	}
	_open PVOID New(ULEXT Key){
		PAIR *pPair = (PAIR*)$sup New();
		pPair->Key = Key;
		$m.bSorted = B_False;
		return pPair->Value;
	}
	_open PVOID Get(_in CHAR8 *psKey){
		return $m.Get(StrHash64(psKey));
	}
	_open PVOID Get(ULEXT Key){
		if(!$m.bSorted){
			$m.bSorted = B_True;
			QckSort($m.lpsData, $m.Size, $m.cbElement, tlCompare<ULEXT>);
		}

		PVOID pPair = tlBinSearch($m.lpsData, Key, $m.Size, $m.cbElement);
		if(!pPair) return P_Null;
		return ((PAIR*)pPair)->Value;
	}
	_open PVOID operator[](_in CHAR8 *psKey){
		return $m.operator[](StrHash64(psKey));
	}
	_open PVOID operator[](ULEXT Key){
		PVOID pValue = $m.Get(Key);
		if(pValue) return pValue;
		pValue = $m.New(Key);
		BStr_Zero(pValue, $m.cbValue);
		return pValue;
	}
	_open PVOID GetAt(USINT Index){
		BYTET *psTarget = $m.lpsData + ($m.cbElement * Index);
		return ((PAIR*)psTarget)->Value;
	}
	_open PAIR *GetPair(USINT Index){
		BYTET *psTarget = $m.lpsData + ($m.cbElement * Index);
		return (PAIR*)psTarget;
	}
	_open USINT GetSize(){
		return $m.Size;
	}
};

// BBT Dictionary
class CBbtDict{
	_rest USINT cbValue;
	_rest BBTNODE *lpRoot;

	_open ~CBbtDict(){}
	_open CBbtDict(){
		Var_Zero(this);
	}
	_open CBbtDict(USINT cbValue){
		$m.Init(cbValue);
	}
	//////
	_open $VOID Clear(){
		BBTNODE::DeleteAll($m.lpRoot);
		$m.lpRoot = P_Null;
	}
	_open $VOID Init(USINT cbValue){
		$m.lpRoot = P_Null;
		$m.cbValue = cbValue;
	}
	_open $VOID Dispatch(PFNFOREACH pfnForEach){
		BBTNODE::Dispatch($m.lpRoot, pfnForEach);
	}
	_open $VOID Insert(_in CHAR8 *psKey, PVOID pValue){
		$m.Insert(StrHash64(psKey), pValue);
	}
	_open $VOID Insert(ULEXT Key, PVOID pValue){
		$m.lpRoot = BBTNODE::Insert($m.lpRoot, $m.cbValue, Key, pValue);
	}
	_open $VOID Delete(_in CHAR8 *psKey){
		$m.Delete(StrHash64(psKey));
	}
	_open $VOID Delete(ULEXT Key){
		$m.lpRoot = BBTNODE::Delete($m.lpRoot, Key);
	}
	_open PVOID Get(_in CHAR8 *psKey){
		return $m.Get(StrHash64(psKey));
	}
	_open PVOID Get(ULEXT Key){
		BBTNODE *pNode = BBTNODE::Find($m.lpRoot, Key);
		if(!pNode) return P_Null;
		return pNode->Data;
	}
	_open PVOID operator[](_in CHAR8 *psKey){
		return $m.operator[](StrHash64(psKey));
	}
	_open PVOID operator[](ULEXT Key){
		BBTNODE *pNode = BBTNODE::Find($m.lpRoot, Key);
		if(pNode) return pNode->Data;
		$m.lpRoot = BBTNODE::Insert($m.lpRoot, $m.cbValue, Key, P_Null);
		return BBTNODE::Find($m.lpRoot, Key)->Data;
	}
	_open USINT GetSize(){
		return BBTNODE::GetSize($m.lpRoot);
	}
	_open USINT GetDepth(){
		return BBTNODE::GetDepth($m.lpRoot);
	}
	_open BBTNODE *GetRoot(){
		return $m.lpRoot;
	}
};

// Circular Queue
class CCirQueue{
	_rest USINT iHead;
	_rest USINT iTail;
	_rest USINT Capacity;
	_rest USINT cbElement;
	_rest BYTET *lpsData;

	_open ~CCirQueue(){
		$m.Clear(B_True);
	}
	_open CCirQueue(){
		Var_Zero(this);
	}
	_open CCirQueue(USINT Capacity, USINT cbElement){
		$m.Init(Capacity, cbElement);
	}
	//////
	_open $VOID Init(USINT Capacity, USINT cbElement){
		$m.iHead = 0;
		$m.iTail = 0;
		$m.cbElement = cbElement;
		$m.Capacity = Capacity + 1;
		$m.lpsData = new BYTET[$m.cbElement * $m.Capacity];
	}
	_open $VOID Clear(IBOOL bDestroy){
		if($m.lpsData){
			if(bDestroy){
				delete[] $m.lpsData;
				$m.lpsData = P_Null;
			} else{
				$m.iHead = 0;
				$m.iTail = 0;
			}
		}
	}
	_open $VOID PushBack(PVOID pData){
		USINT Offset = $m.iTail * $m.cbElement;
		$m.iTail += 1;
		$m.iTail %= $m.Capacity;
		BStr_Move($m.lpsData + Offset, pData, $m.cbElement);
	}
	_open PVOID PopFront(){
		USINT Offset = $m.iHead * $m.cbElement;
		$m.iHead += 1;
		$m.iHead %= $m.Capacity;
		return $m.lpsData + Offset;
	}
	_open PVOID GetFront(){
		USINT Offset = $m.iHead * $m.cbElement;
		return $m.lpsData + Offset;
	}
	_open USINT GetSize(){
		return ($m.iTail >= $m.iHead) ?
			($m.iTail - $m.iHead) :
			($m.Capacity - $m.iHead + $m.iTail);
	}
	_open IBOOL IsEmpty(){
		return ($m.iHead == $m.iTail);
	}
	_open IBOOL IsFull(){
		USINT NextPos = ($m.iTail + 1) % $m.Capacity;
		return (NextPos == $m.iHead);
	}
};

// Linked Queue
class ClnkQueue: protected CLnkList{
	_open class CIter{
		_secr INT32 IntranodeIdx;
		_secr INT32 ElemIndex;
		_secr BYTET *psElement;
		_secr NODE *pCurNode;
		_secr ClnkQueue *pOwner;

		_open CIter(ClnkQueue *pOwner, NODE *pCurNode, INT32 ElemIndex, INT32 IntranodeIdx){
			$m.pOwner = pOwner;
			$m.pCurNode = pCurNode;
			$m.psElement = pCurNode->Data;
			$m.psElement += pOwner->cbElementS * IntranodeIdx;
			$m.ElemIndex = ElemIndex;
			$m.IntranodeIdx = IntranodeIdx;
		}
		//////
		_open PVOID operator*(){
			return $m.psElement;
		}
		_open CIter &operator++(){
			$m.ElemIndex += 1;
			$m.IntranodeIdx += 1;

			if($m.IntranodeIdx == pOwner->NodeSubdiv){
				$m.IntranodeIdx = 0;
				$m.pCurNode = $m.pCurNode->pNext;
			}

			$m.psElement = $m.pCurNode->Data;
			$m.psElement += $m.IntranodeIdx * $m.pOwner->cbElementS;
		}
		_open CIter &operator--(){
			$m.ElemIndex -= 1;
			$m.IntranodeIdx -= 1;

			if($m.IntranodeIdx < 0){
				$m.IntranodeIdx = pOwner->NodeSubdiv - 1;
				$m.pCurNode = $m.pCurNode->pPrev;
			}

			$m.psElement = $m.pCurNode->Data;
			$m.psElement += $m.IntranodeIdx * $m.pOwner->cbElementS;
		}

		_open friend BOOL8 operator==(_in CIter &Data0, _in CIter &Data1){
			if(Data0.ElemIndex != Data1.ElemIndex) return B_False;
			if(Data0.pOwner != Data1.pOwner) return B_False;
			return B_True;
		}
		_open friend BOOL8 operator!=(_in CIter &Data0, _in CIter &Data1){
			if(Data0.ElemIndex != Data1.ElemIndex) return B_True;
			if(Data0.pOwner != Data1.pOwner) return B_True;
			return B_False;
		}
	};

	_rest USINT cbElementS;
	_rest USINT ElemCountS;
	_rest USINT NodeSubdiv;
	_rest USINT IntranodeEnd;
	_rest USINT IntranodeStart;

	_open ~ClnkQueue(){}
	_open ClnkQueue(): CLnkList(){}
	_open ClnkQueue(USINT ChunkSize, USINT cbElement) : CLnkList(ChunkSize * cbElement){
		$m.ElemCountS = 0;
		$m.cbElementS = cbElement;
		$m.NodeSubdiv = ChunkSize;
		$m.IntranodeEnd = ChunkSize;
		$m.IntranodeStart = 0;
	}
	//////
	_open $VOID Init(USINT ChunkSize, USINT cbElement){
		$m.ElemCountS = 0;
		$m.cbElementS = cbElement;
		$m.NodeSubdiv = ChunkSize;
		$m.IntranodeEnd = ChunkSize;
		$m.IntranodeStart = -1;
		$sup Init(ChunkSize * cbElement);
	}
	_open $VOID Clear(IBOOL bDestroy){
		$m.ElemCountS = 0;
		$m.IntranodeStart = -1;
		$m.IntranodeEnd = $m.NodeSubdiv;
		$sup Clear(bDestroy);
	}
	_open $VOID PushFront(PVOID pData){
		BYTET *psTarget;

		if($m.IntranodeStart == -1){
			$m.IntranodeStart = $m.NodeSubdiv - 1;
			psTarget = (BYTET*)$sup New($m.lpHead);
		} else{
			$m.IntranodeStart -= 1;
			psTarget = (BYTET*)$sup GetFront();
		}

		$m.ElemCountS += 1;
		psTarget += $m.IntranodeStart * $m.cbElementS;
		BStr_Move(psTarget, pData, $m.cbElementS);
	}
	_open $VOID PushBack(PVOID pData){
		BYTET *psTarget;

		if($m.IntranodeEnd == $m.NodeSubdiv){
			$m.IntranodeEnd = 0;
			psTarget = (BYTET*)$sup New($m.lpTail->pPrev);
		} else{
			$m.IntranodeEnd += 1;
			psTarget = (BYTET*)$sup GetBack();
		}

		$m.ElemCountS += 1;
		psTarget += $m.IntranodeEnd * $m.cbElementS;
		BStr_Move(psTarget, pData, $m.cbElementS);
	}
	_open $VOID PopFront(){
		$m.ElemCountS -= 1;
		$m.IntranodeStart += 1;

		if($m.ElemCountS == 0){
			$m.Clear(B_False);
			return;
		}

		if($m.IntranodeStart == $m.NodeSubdiv){
			$m.IntranodeStart = -1;
			$sup Delete($m.lpHead->pNext);
		}
	}
	_open $VOID PopBack(){
		$m.ElemCountS -= 1;
		$m.IntranodeEnd -= 1;

		if($m.ElemCountS == 0){
			$m.Clear(B_False);
			return;
		}

		if($m.IntranodeEnd == -1){
			$m.IntranodeEnd = $m.NodeSubdiv;
			$sup Delete($m.lpTail->pPrev);
		}
	}
	_open PVOID GetFront(){
		BYTET *psElement = (BYTET*)$sup GetFront();
		return psElement + ($m.IntranodeStart * $m.cbElementS);
	}
	_open PVOID GetBack(){
		BYTET *psElement = (BYTET*)$sup GetBack();
		return psElement + ($m.IntranodeEnd * $m.cbElementS);
	}
	_open USINT GetSize(){
		return $m.ElemCountS;
	}
	_open IBOOL IsEmpty(){
		return ($m.ElemCountS == 0);
	}
	_open CIter GetStart(){
		return CIter(this, $m.lpHead->pNext, 0, $m.IntranodeStart);
	}
	_open CIter GetEnd(){
		return CIter(this, P_Null, $m.ElemCountS, 0);
	}
	_open CIter GetStartR(){
		return CIter(this, $m.lpTail->pPrev, $m.ElemCountS - 1, $m.IntranodeEnd);
	}
	_open CIter GetEndR(){
		return CIter(this, P_Null, -1, 0);
	}
};

//----------------------------------------//


//-------------------- 字符串库 --------------------//

// String Repository
class CStrRepos{
	_secr USINT Count;
	_secr USINT *lprgAddr;
	_secr CHAR8 *lpsBuffer;

	_open ~CStrRepos(){
		$m.Finalize();
	}
	_open CStrRepos(){
		Var_Zero(this);
	}
	_open CStrRepos(_in WCHAR *psFilePath, _in CHAR8 *psDelim){
		$m.Initialize(psFilePath, psDelim);
	}
	//////
	_open $VOID Finalize(){
		SAFE_DELETEA($m.lprgAddr);
		SAFE_DELETEA($m.lpsBuffer);
	}
	_open $VOID Initialize(_in WCHAR *psFilePath, _in CHAR8 *psDelim){
		///加载文件

		HANDLE hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		USINT FileSize = GetFileSize(hrFile, P_Null);

		$m.lpsBuffer = new CHAR8[FileSize + 1];
		ReadFile(hrFile, $m.lpsBuffer, FileSize, P_Null, P_Null);
		$m.lpsBuffer[FileSize] = '\0';
		CloseHandle(hrFile);

		///划分字符串

		CHAR8 *psItem = $m.lpsBuffer;
		UPINT Offset = 0, Span = MbsLen(psDelim);
		CSeqList AddrList(64, sizeof(USINT));

		AddrList.PushBack(&Offset);
		while(psItem = MbsStr(psItem, psDelim)){
			BStr_Zero(psItem, Span);
			psItem += Span;
			Offset = psItem - $m.lpsBuffer;
			AddrList.PushBack(&Offset);
		}

		$m.Count = AddrList.GetSize();
		$m.lprgAddr = (USINT*)AddrList.Detach();
	}
	_open $VOID Replace(CHAR8 Old, CHAR8 New){
		for(USINT iStr = 0; iStr < $m.Count; ++iStr){
			CHAR8 *pChar = $m.lpsBuffer + $m.lprgAddr[iStr];
			for(; *pChar; ++pChar)
				if(*pChar == Old) *pChar = New;
		}
	}
	_open USINT GetCount(){
		return $m.Count;
	}
	_open const CHAR8 *Get(USINT Index){
		return $m.lpsBuffer + $m.lprgAddr[Index];
	}
};

//----------------------------------------//


//-------------------- XML&JSON --------------------//

#ifdef cJSON__h
// JSON Node
class CJsonNode: protected cJSON{
	_secr ~CJsonNode(){}
	_secr CJsonNode(){}
	//////
	_open $VOID Release(){
		cJSON_Delete(this);
	}
	_open $VOID SaveToFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_WRITE, 0L, CREATE_ALWAYS, P_Null);
		CHAR8 *lpsCode = cJSON_Print(this);

		WriteFile(hrFile, lpsCode, (USINT)MbsLen(lpsCode), P_Null, P_Null);
		CloseHandle(hrFile);

		delete[] lpsCode;
	}
	//////
	_open $VOID InsertChild(CJsonNode *pNode, USINT Index){
		cJSON_InsertItemInArray(this, Index, pNode);
	}
	_open $VOID AppendChild(CJsonNode *pNode, _in CHAR8 *psKey){
		cJSON_AddItemToObject(this, psKey, pNode);
	}
	_open $VOID AppendChild(CJsonNode *pNode){
		cJSON_AddItemToArray(this, pNode);
	}
	//////
	_open $VOID DeleteChild(USINT Index){
		cJSON_Delete(cJSON_DetachItemFromArray(this, Index));
	}
	_open $VOID DeleteChild(_in CHAR8 *psName){
		cJSON_Delete(cJSON_DetachItemFromObjectCaseSensitive(this, psName));
	}
	//////
	_open $VOID GetArrVal8(_out BYTET *prgResult){
		BYTET *pDest = prgResult;
		cJSON *pSource = $m.child;

		while(pSource){
			*pDest = (BYTET)pSource->valueint;
			pSource = pSource->next;
			pDest++;
		}
	}
	_open $VOID GetArrVal16(_out WORDT *prgResult){
		WORDT *pDest = prgResult;
		cJSON *pSource = $m.child;

		while(pSource){
			*pDest = (WORDT)pSource->valueint;
			pSource = pSource->next;
			pDest++;
		}
	}
	_open $VOID GetArrVal32(_out DWORD *prgResult){
		DWORD *pDest = prgResult;
		cJSON *pSource = $m.child;

		while(pSource){
			*pDest = (DWORD)pSource->valueint;
			pSource = pSource->next;
			pDest++;
		}
	}
	_open $VOID GetArrValF32(_out SPFPN *prgResult){
		SPFPN *pDest = prgResult;
		cJSON *pSource = $m.child;

		while(pSource){
			*pDest = (SPFPN)pSource->valuedouble;
			pSource = pSource->next;
			pDest++;
		}
	}
	_open $VOID GetArrValF64(_out DPFPN *prgResult){
		DPFPN *pDest = prgResult;
		cJSON *pSource = $m.child;

		while(pSource){
			*pDest = pSource->valuedouble;
			pSource = pSource->next;
			pDest++;
		}
	}
	//////
	_open BOOL8 GetValB(){
		return (BOOL8)$m.valueint;
	}
	_open INT32 GetValI(){
		return $m.valueint;
	}
	_open SPFPN GetValF32(){
		return (SPFPN)$m.valuedouble;
	}
	_open DPFPN GetValF64(){
		return $m.valuedouble;
	}
	//////
	_open BOOL8 GetElemB(USINT Index){
		return (BOOL8)cJSON_GetArrayItem(this, Index)->valueint;
	}
	_open INT32 GetElemI(USINT Index){
		return cJSON_GetArrayItem(this, Index)->valueint;
	}
	_open SPFPN GetElemF32(USINT Index){
		return (SPFPN)cJSON_GetArrayItem(this, Index)->valuedouble;
	}
	_open DPFPN GetElemF64(USINT Index){
		return cJSON_GetArrayItem(this, Index)->valuedouble;
	}
	//////
	_open BOOL8 GetMemB(_in CHAR8 *psName){
		return (BOOL8)cJSON_GetObjectItemCaseSensitive(this, psName)->valueint;
	}
	_open INT32 GetMemI(_in CHAR8 *psName){
		return cJSON_GetObjectItemCaseSensitive(this, psName)->valueint;
	}
	_open SPFPN GetMemF32(_in CHAR8 *psName){
		return (SPFPN)cJSON_GetObjectItemCaseSensitive(this, psName)->valuedouble;
	}
	_open DPFPN GetMemF64(_in CHAR8 *psName){
		return cJSON_GetObjectItemCaseSensitive(this, psName)->valuedouble;
	}
	//////
	_open IBOOL IsRaw(CJsonNode *pNode){
		return cJSON_IsRaw(pNode);
	}
	_open IBOOL IsNull(CJsonNode *pNode){
		return cJSON_IsNull(pNode);
	}
	_open IBOOL IsBool(CJsonNode *pNode){
		return cJSON_IsBool(pNode);
	}
	_open IBOOL IsNumber(CJsonNode *pNode){
		return cJSON_IsNumber(pNode);
	}
	_open IBOOL IsString(CJsonNode *pNode){
		return cJSON_IsString(pNode);
	}
	_open IBOOL IsArray(CJsonNode *pNode){
		return cJSON_IsArray(pNode);
	}
	_open IBOOL IsObject(CJsonNode *pNode){
		return cJSON_IsObject(pNode);
	}
	_open IBOOL IsInvalid(CJsonNode *pNode){
		return cJSON_IsInvalid(pNode);
	}
	//////
	_open USINT GetSize(){
		return CountNodes($m.child, FIELD_OFFSET(cJSON, next));
	}
	_open CHAR8 *CreateCode(){
		return cJSON_Print(this);
	}
	//////
	_open CJsonNode *GetNext(){
		return (CJsonNode*)$m.next;
	}
	_open CJsonNode *GetChild(){
		return (CJsonNode*)$m.child;
	}
	_open CJsonNode *GetChild(USINT Index){
		return (CJsonNode*)cJSON_GetArrayItem(this, Index);
	}
	_open CJsonNode *GetChild(_in CHAR8 *psName){
		return (CJsonNode*)cJSON_GetObjectItemCaseSensitive(this, psName);
	}
	_open CJsonNode *DetachChild(USINT Index){
		return (CJsonNode*)cJSON_DetachItemFromArray(this, Index);
	}
	_open CJsonNode *DetachChild(_in CHAR8 *psName){
		return (CJsonNode*)cJSON_DetachItemFromObjectCaseSensitive(this, psName);
	}
	//////
	_open const CHAR8 *GetKey(){
		return $m.string;
	}
	_open const CHAR8 *GetValS(){
		return $m.valuestring;
	}
	_open const CHAR8 *GetElemS(USINT Index){
		return cJSON_GetArrayItem(this, Index)->valuestring;
	}
	_open const CHAR8 *GetMemS(_in CHAR8 *psName){
		return cJSON_GetObjectItemCaseSensitive(this, psName)->valuestring;
	}

	_open static CJsonNode *CreateNull(){
		return (CJsonNode*)cJSON_CreateNull();
	}
	_open static CJsonNode *CreateArray(){
		return (CJsonNode*)cJSON_CreateArray();
	}
	_open static CJsonNode *CreateObject(){
		return (CJsonNode*)cJSON_CreateObject();
	}
	_open static CJsonNode *CreateBool(IBOOL Value){
		return (CJsonNode*)cJSON_CreateBool(Value);
	}
	_open static CJsonNode *CreateNumber(DPFPN Value){
		return (CJsonNode*)cJSON_CreateNumber(Value);
	}
	_open static CJsonNode *CreateRaw(_in CHAR8 *psValue){
		return (CJsonNode*)cJSON_CreateRaw(psValue);
	}
	_open static CJsonNode *CreateString(_in CHAR8 *psValue){
		return (CJsonNode*)cJSON_CreateString(psValue);
	}
	//////
	_open static CJsonNode *CreateArrayS(_in CHAR8 **prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateStringArray(prgValue, Count);
	}
	_open static CJsonNode *CreateArrayI(_in INT32 *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateIntArray(prgValue, Count);
	}
	_open static CJsonNode *CreateArrayF32(_in SPFPN *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateFloatArray(prgValue, Count);
	}
	_open static CJsonNode *CreateArrayF64(_in DPFPN *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateDoubleArray(prgValue, Count);
	}
	//////
	_open static CJsonNode *CreateByCode(_in CHAR8 *psCode){
		return (CJsonNode*)cJSON_Parse(psCode);
	}
	_open static CJsonNode *CreateByFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		USINT FileSize = GetFileSize(hrFile, P_Null);
		CHAR8 *lpsCode = new CHAR8[FileSize + 1];
		CJsonNode *lpRoot;

		ReadFile(hrFile, lpsCode, FileSize, P_Null, P_Null);
		CloseHandle(hrFile);

		lpsCode[FileSize] = '\0';
		lpRoot = (CJsonNode*)cJSON_Parse(lpsCode);

		delete[] lpsCode;
		return lpRoot;
	}
};
#endif

#ifdef TINYXML2_INCLUDED
// XML Node
class CXmlNode: protected tinyxml2::XMLNode{
	_secr ~CXmlNode(){}
	_secr CXmlNode(): tinyxml2::XMLNode(P_Null){}
	//////
	_open $VOID SaveToFile(_in WCHAR *psPath){
		tinyxml2::XMLPrinter Printer;
		$m._document->Print(&Printer);

		HANDLE hrFile = CreateFile2(psPath, GENERIC_WRITE, 0L, CREATE_ALWAYS, P_Null);
		WriteFile(hrFile, Printer.CStr(), (USINT)Printer.CStrSize(), P_Null, P_Null);
		CloseHandle(hrFile);
	}
	//////
	_open $VOID Release(){
		if(!$m._parent->ToDocument())
			$m._parent->DeleteChild(this);
		else delete $m._document;
	}
	_open $VOID DeleteCont(){
		tinyxml2::XMLNode *pChild = $m._firstChild;

		while(pChild){
			if(!pChild->ToComment()) break;
			else pChild = pChild->NextSibling();
		}

		if(pChild && pChild->ToText())
			$sup DeleteChild(pChild);
	}
	_open $VOID DeleteAttr(_in CHAR8 *psName){
		$sup ToElement()->DeleteAttribute(psName);
	}
	_open $VOID DeleteChild(_in CHAR8 *psName){
		tinyxml2::XMLNode *pNode = $sup FirstChildElement(psName);
		$sup DeleteChild(pNode);
	}
	//////
	_open $VOID SetName(_in CHAR8 *psName){
		$sup ToElement()->SetName(psName);
	}
	_open $VOID SetCont(_in CHAR8 *psText){
		$sup ToElement()->SetText(psText);
	}
	_open $VOID SetAttr(_in CHAR8 *psName, _in CHAR8 *psValue){
		$sup ToElement()->SetAttribute(psName, psValue);
	}
	//////
	_open BOOL8 GetContB(){
		return $sup ToElement()->BoolText();
	}
	_open INT32 GetContI32(){
		return $sup ToElement()->IntText();
	}
	_open INT64 GetContI64(){
		return $sup ToElement()->Int64Text();
	}
	_open SPFPN GetContF32(){
		return $sup ToElement()->FloatText();
	}
	_open DPFPN GetContF64(){
		return $sup ToElement()->DoubleText();
	}
	//////
	_open BOOL8 GetAttrB(_in CHAR8 *psName){
		return $sup ToElement()->BoolAttribute(psName);
	}
	_open INT32 GetAttrI32(_in CHAR8 *psName){
		return $sup ToElement()->IntAttribute(psName);
	}
	_open INT64 GetAttrI64(_in CHAR8 *psName){
		return $sup ToElement()->Int64Attribute(psName);
	}
	_open SPFPN GetAttrF32(_in CHAR8 *psName){
		return $sup ToElement()->FloatAttribute(psName);
	}
	_open DPFPN GetAttrF64(_in CHAR8 *psName){
		return $sup ToElement()->DoubleAttribute(psName);
	}
	//////
	_open USINT GetSize(){
		return $sup ChildElementCount();
	}
	_open CHAR8 *CreateCode(){
		tinyxml2::XMLPrinter Printer;
		$m._document->Print(&Printer);
		
		CHAR8 *lpsCode = new CHAR8[Printer.CStrSize()];
		MbsNcpy(lpsCode, Printer.CStr(), Printer.CStrSize());

		return lpsCode;
	}
	//////
	_open CXmlNode *GetNext(){
		return (CXmlNode*)$m._next;
	}
	_open CXmlNode *GetChild(){
		return (CXmlNode*)$m._firstChild;
	}
	_open CXmlNode *GetChild(_in CHAR8 *psName){
		return (CXmlNode*)$sup FirstChildElement(psName);
	}
	_open CXmlNode *NewChild(_in CHAR8 *psName, CXmlNode *pPrev = P_Null){
		tinyxml2::XMLElement *pNode = $m._document->NewElement(psName);
		if(!pPrev) $sup InsertEndChild(pNode);
		else $sup InsertAfterChild(pPrev, pNode);
	}
	//////
	_open const CHAR8 *GetName(){
		return $m._value.GetStr();
	}
	_open const CHAR8 *GetContS(){
		return $sup ToElement()->GetText();
	}
	_open const CHAR8 *GetAttrS(_in CHAR8 *psName){
		return $sup ToElement()->Attribute(psName);
	}

	_open static CXmlNode *CreateRoot(_in CHAR8 *psName){
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument;
		tinyxml2::XMLElement *pNode = pDoc->NewElement(psName);
		pDoc->InsertFirstChild(pNode);
		return (CXmlNode*)pNode;
	}
	_open static CXmlNode *CreateByCode(_in CHAR8 *psCode){
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument;
		pDoc->Parse(psCode);
		return (CXmlNode*)pDoc->RootElement();
	}
	_open static CXmlNode *CreateByFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		USINT FileSize = GetFileSize(hrFile, P_Null);
		CHAR8 *lpsCode = new CHAR8[FileSize + 1];
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument;

		ReadFile(hrFile, lpsCode, FileSize, P_Null, P_Null);
		CloseHandle(hrFile);

		lpsCode[FileSize] = '\0';
		pDoc->Parse(lpsCode);

		delete[] lpsCode;
		return (CXmlNode*)pDoc->RootElement();
	}
};
#endif

//----------------------------------------//