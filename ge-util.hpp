//-------------------- 简单表达式 --------------------//

#define DW_CNT(toe) (sizeof(toe) / 4)
#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define AS_INT32(var) *(int*)&(var)
#define AS_SPFPN(var) *(float*)&(var)
#define AS_DWORD(var) *(unsigned long*)&(var)

#define SAFE_FREE(ptr) if(ptr){ free(ptr); ptr = nullptr; };
#define SAFE_FREEA(ptr) if(ptr){ _aligned_free(ptr); ptr = nullptr; };
#define SAFE_CLOSE(ptr) if(ptr){ CloseHandle(ptr); ptr = nullptr; };
#define SAFE_RELEASE(ptr) if(ptr){ ptr->Release(); ptr = nullptr; };
#define SAFE_DELETEA(ptr) if(ptr){ delete[] ptr; ptr = nullptr; };
#define SAFE_DELETE(ptr) if(ptr){ delete ptr; ptr = nullptr; };

#define SWAP_(v0, v1) { auto t = v0; v0 = v1; v1 = t; }
#define CAST_(dst, src) { dst = decltype(dst)(src); }
#define ASSIGN_(dst, src) { dst = *(decltype(&dst))&(src); }

#define OFFSET_PTR(ptr, cb) ((unsigned char*)(ptr) + (cb))

#define DYNARR_ALLOC(ptr, len) { ptr = (decltype(ptr))malloc(sizeof(*ptr) * (len)); }
#define DYNARR_REALLOC(ptr, len) { ptr = (decltype(ptr))realloc(ptr, sizeof(*ptr) * (len)); }

//----------------------------------------//


//-------------------- 简单计算 --------------------//

#define MATH_E 2.71828182845904523536
#define MATH_PI 3.14159265358979323846

#define TO_RAD(deg) ((deg) * MATH_PI / 180.0)
#define TO_DEG(rad) ((rad) * 180.0 / MATH_PI)

#define LO_BYTE(val) ((val) & 0xFF)
#define LO_WORD(val) ((val) & 0xFFFF)
#define HI_BYTE(val) (((val) >> 8) & 0xFF)
#define HI_WORD(val) (((val) >> 16) & 0xFFFF)

#define MAKE_WORD(low, high) (((low) & 0xFF) | (((high) & 0xFF) << 8))
#define MAKE_DWORD(low, high) (((low) & 0xFFFF) | (((high) & 0xFFFF) << 16))

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


//-------------------- 内存数据使用简化 --------------------//

#define Var_Zero(dst) memset((dst), 0, sizeof(*dst))
#define Mem_Zero(dst, size) memset((dst), 0, size)
#define Mem_Copy(dst, src, size) memcpy((dst), (src), size)
#define Mem_Move(dst, src, size) memmove((dst), (src), size)
#define Mem_Swap(mem0, mem1, size) swab((CHAR8*)(mem0), (CHAR8*)(mem1), size)
#define Mem_Fill(dst, val, size) __stosb((BYTET*)(dst), val, size)
#define WMem_Fill(dst, val, size) __stosw((WORDT*)(dst), val, size)
#define DMem_Fill(dst, val, size) __stosd((DWORD*)(dst), val, size)
#define QMem_Fill(dst, val, size) __stosq((QWORD*)(dst), val, size)
#define Mem_Compare(mem0, mem1, size) memcmp(mem0, mem1, size)
#define WMem_Compare(mem0, mem1, size) wmemcmp((WCHAR*)(mem0), (WCHAR*)(mem1), size)

//----------------------------------------//


//-------------------- 字符集兼容 --------------------//

#ifdef _UNICODE
#define A_TO_W(str) MultiByteToWideChar(str)
#define W_TO_A(str) WideCharToMultiByte(str)
#define A_TO_T(str) MultiByteToWideChar(str)
#define T_TO_A(str) WideCharToMultiByte(str)
#define W_TO_T(str) str
#define T_TO_W(str) str
#else
#define A_TO_W(str) MultiByteToWideChar(str)
#define W_TO_A(str) WideCharToMultiByte(str)
#define A_TO_T(str) str
#define T_TO_A(str) str
#define W_TO_T(str) WideCharToMultiByte(str)
#define T_TO_W(str) MultiByteToWideChar(str)
#endif

//----------------------------------------//


//-------------------- 内部实现 --------------------//

namespace impl{
	namespace perlin{
		const UNS32 gPermSize = 256;
		INT32 gPerm[gPermSize * 2]; // Permutation

		inline SPFPN Fade(SPFPN T){
			return T * T * T * (T*(T*6.f - 15.f) + 10.f);
		}
		inline SPFPN Gradient(INT32 Hash, SPFPN X, SPFPN Y){
			INT32 H = Hash & 0xF;      // 取最低4位
			SPFPN U = 1.f + (H & 0x7); // 生成1到8之间的随机数
			if(H & 8) U = -U;          // 随机决定梯度的正负
			return U*X + U*Y;          // 梯度值与输入值相乘
		}
	}

	namespace cec{
		UNS32 gBufSize = 0;
		CHAR8 *gMbsPtr = P_Null;
		WCHAR *gWcsPtr = P_Null;

		inline $VOID ResetBuffer(){
			delete gMbsPtr;
			UNS32 gBufSize = 0;
			CHAR8 *gMbsPtr = P_Null;
			WCHAR *gWcsPtr = P_Null;
		}
		inline $VOID SetBuffer(UNS32 Size){
			delete gMbsPtr;
			gBufSize = Size;
			gMbsPtr = new CHAR8[Size];
			gWcsPtr = (WCHAR*)gMbsPtr;
		}
	}
}

//----------------------------------------//


//-------------------- 对齐计算 --------------------//

// 是否2的幂
inline IBOOL IsPowOf2(UIPTR Value){
	return !(Value & (Value - 1));
}

// 升为2的幂
inline UIPTR ToPowOf2(UIPTR Value){
	UIPTR Mask = Value - 1;
	Mask |= Mask >> 1;
	Mask |= Mask >> 2;
	Mask |= Mask >> 4;
	Mask |= Mask >> 8;
	Mask |= Mask >> 16;
	return Mask + 1;
}

// 是否2的幂的倍数
inline IBOOL IsPowOf2Mul(UIPTR Value, UIPTR PowOf2){
	return !(Value & (PowOf2 - 1));
}

// 升为2的幂的倍数
inline UIPTR ToPowOf2Mul(UIPTR Value, UIPTR PowOf2){
	UIPTR Factor = PowOf2 - 1;
	return (Value + Factor) & ~Factor;
}

//----------------------------------------//


//-------------------- 随机数生成 --------------------//

// 随机整数
inline INT32 RandInt(INT32 Min, INT32 Max){
	INT32 Diff = Max - Min + 1;
	return Min + (rand() % Diff);
}

// 随机实数(FLOAT)
inline SPFPN RandReal(SPFPN Min, SPFPN Max){
	static const SPFPN tInvRandMax = 1.f / RAND_MAX;
	SPFPN Weight = (SPFPN)rand() * tInvRandMax;
	return LERP_(Min, Max, Weight);
}

// 随机实数(DOUBLE)
inline DPFPN RandReal(DPFPN Min, DPFPN Max){
	static const DPFPN tInvRandMax = 1.0 / RAND_MAX;
	DPFPN Weight = (DPFPN)rand() * tInvRandMax;
	return LERP_(Min, Max, Weight);
}

//----------------------------------------//


//-------------------- 随机向量生成 --------------------//

// 随机2D向量
inline $VOID RandVec2(_out SPFPN psResult[2], _in SPFPN psMin[2], _in SPFPN psMax[2]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
}

// 随机3D向量
inline $VOID RandVec3(_out SPFPN psResult[3], _in SPFPN psMin[3], _in SPFPN psMax[3]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
	psResult[2] = RandReal(psMin[2], psMax[2]);
}

// 随机4D向量
inline $VOID RandVec4(_out SPFPN psResult[4], _in SPFPN psMin[4], _in SPFPN psMax[4]){
	psResult[0] = RandReal(psMin[0], psMax[0]);
	psResult[1] = RandReal(psMin[1], psMax[1]);
	psResult[2] = RandReal(psMin[2], psMax[2]);
	psResult[3] = RandReal(psMin[3], psMax[3]);
}

//----------------------------------------//


//-------------------- 字符编码转换 --------------------//

// WCS转为MBS
inline UNS32 WideCharToMultiByte(_out CHAR8 *psDest, _in WCHAR *psSource, UNS32 BufSize, UNS32 CodePage = CP_UTF8){
	return WideCharToMultiByte(CodePage, 0L, psSource, -1, psDest, BufSize, P_Null, P_Null);
}

// MBS转为WCS
inline UNS32 MultiByteToWideChar(_out WCHAR *psDest, _in CHAR8 *psSource, UNS32 BufSize, UNS32 CodePage = CP_UTF8){
	return MultiByteToWideChar(CodePage, 0L, psSource, -1, psDest, BufSize);
}

//----------------------------------------//


//-------------------- 字符编码转换(静态缓存区) --------------------//

// WCS转为MBS
CHAR8* WideCharToMultiByte(_in WCHAR *psSource, UNS32 CodePage = CP_UTF8){
	using namespace impl::cec;
	UNS32 MinSize = WideCharToMultiByte(CodePage, 0L, psSource, -1, P_Null, 0, P_Null, P_Null);
	if(MinSize > gBufSize) SetBuffer(MinSize);
	WideCharToMultiByte(CodePage, 0L, psSource, -1, gMbsPtr, gBufSize, P_Null, P_Null);
	return gMbsPtr;
}

// MBS转为WCS
WCHAR* MultiByteToWideChar(_in CHAR8 *psSource, UNS32 CodePage = CP_UTF8){
	using namespace impl::cec;
	UNS32 MinSize = MultiByteToWideChar(CodePage, 0L, psSource, -1, P_Null, 0) * 2;
	if(MinSize > gBufSize) SetBuffer(MinSize);
	MultiByteToWideChar(CodePage, 0L, psSource, -1, gWcsPtr, gBufSize / 2);
	return gWcsPtr;
}

//----------------------------------------//


//-------------------- 字符串修改 --------------------//

// 字符串移除字符
CHAR8* StrRemoveChr(_io CHAR8 *psInput, CHAR8 Target){
	CHAR8 *pDest = strchr(psInput, Target);
	CHAR8 *pSrc = pDest ? (pDest + 1) : P_Null;

	if(pSrc){
		while(*pSrc){
			if(*pSrc == Target) pSrc++;
			else *pDest++ = *pSrc++;
		}
		*pDest = '\0';
	}

	return psInput;
}

// 宽字符串移除字符
WCHAR* StrRemoveChr(_io WCHAR *psInput, WCHAR Target){
	WCHAR *pDest = wcschr(psInput, Target);
	WCHAR *pSrc = pDest ? (pDest + 1) : P_Null;

	if(pSrc){
		while(*pSrc){
			if(*pSrc == Target) pSrc++;
			else *pDest++ = *pSrc++;
		}
		*pDest = L'\0';
	}

	return psInput;
}

// 字符串移除子串
CHAR8* StrRemoveSub(_io CHAR8 *psInput, _in CHAR8 *psTarget){
	if(!psInput || !psTarget || *psTarget == '\0')
		return psInput;

	UIPTR SkipLen;
	UIPTR TargetLen = strlen(psTarget);
	CHAR8 *pWrite = psInput;
	CHAR8 *pRead = psInput;
	CHAR8 *pNext;

	/// 寻找第一个匹配项

	while((pNext = strstr(pRead, psTarget)) != P_Null){
		SkipLen = pNext - pRead;
		if(pWrite != pRead)
			Mem_Move(pWrite, pRead, SkipLen);
		pWrite += SkipLen;
		pRead = pNext + TargetLen;
	}

	/// 处理所有匹配项之后的剩余部分

	if(*pRead != '\0'){
		SkipLen = strlen(pRead);
		Mem_Move(pWrite, pRead, SkipLen);
		pWrite += SkipLen;
	}

	 /// 放置结束符

	*pWrite = '\0';
	return psInput;
}

// 宽字符串移除子串
WCHAR* StrRemoveSub(_io WCHAR *psInput, _in WCHAR *psTarget){
	if(!psInput || !psTarget || *psTarget == L'\0')
		return psInput;

	UIPTR SkipLen;
	UIPTR TargetLen = wcslen(psTarget);
	WCHAR *pWrite = psInput;
	WCHAR *pRead = psInput;
	WCHAR *pNext;

	/// 寻找第一个匹配项

	while((pNext = wcsstr(pRead, psTarget)) != P_Null){
		SkipLen = pNext - pRead;
		if(pWrite != pRead)
			Mem_Move(pWrite, pRead, SkipLen * 2);
		pWrite += SkipLen;
		pRead = pNext + TargetLen;
	}
	
	/// 处理所有匹配项之后的剩余部分

	if(*pRead != L'\0'){
		SkipLen = wcslen(pRead);
		Mem_Move(pWrite, pRead, SkipLen * 2);
		pWrite += SkipLen;
	}
	
	/// 放置结束符

	*pWrite = L'\0';
	return psInput;
}

// 字符串追加
CHAR8* StrAppendAt(_io CHAR8 *psDest, _in CHAR8 *psSource, UIPTR DstOffset){
	CHAR8 *pDest = psDest + DstOffset;
	const CHAR8 *pSrc = psSource;

	while(*pSrc)
		*pDest++ = *pSrc++;
	*pDest = '\0';

	return psDest;
}

// 宽字符串追加
WCHAR* StrAppendAt(_io WCHAR *psDest, _in WCHAR *psSource, UIPTR DstOffset){
	WCHAR *pDest = psDest + DstOffset;
	const WCHAR *pSrc = psSource;

	while(*pSrc)
		*pDest++ = *pSrc++;
	*pDest = L'\0';

	return psDest;
}

// 字符串追加
CHAR8* StrAppendAt(_io CHAR8 *psDest, _in WCHAR *psSource, UIPTR DstOffset){
	CHAR8 *pDest = psDest + DstOffset;
	const WCHAR *pSrc = psSource;

	while(*pSrc)
		*pDest++ = (CHAR8)*pSrc++;
	*pDest = '\0';

	return psDest;
}

// 宽字符串追加
WCHAR* StrAppendAt(_io WCHAR *psDest, _in CHAR8 *psSource, UIPTR DstOffset){
	WCHAR *pDest = psDest + DstOffset;
	const CHAR8 *pSrc = psSource;

	while(*pSrc)
		*pDest++ = *pSrc++;
	*pDest = L'\0';

	return psDest;
}

//----------------------------------------//


//-------------------- 字符串散列 --------------------//

// 字符串散列(32b值)
UNS32 StrHash32(_in CHAR8 *psValue){
	UNS32 Hash = 2166136261;
	const CHAR8 *pChar = psValue;

	for(; *pChar; ++pChar){
		Hash *= 16777619;
		Hash ^= *pChar;
	}

	return Hash;
}

// 字符串散列(64b值)
UNS64 StrHash64(_in CHAR8 *psValue){
	UNS64 Hash = 14695981039346656037;
	const CHAR8 *pChar = psValue;

	for(; *pChar; ++pChar){
		Hash *= 1099511628211;
		Hash ^= *pChar;
	}

	return Hash;
}

//----------------------------------------//


//-------------------- 噪声值生成 --------------------//

// Perlin梯度
$VOID PerlinPerm(){
	using namespace impl::perlin;

	for(UNS32 N = 0; N < gPermSize; ++N)
		gPerm[N] = N;

	for(UNS32 N = 0; N < gPermSize; ++N){
		UNS32 M = rand() % gPermSize;
		SWAP_(gPerm[N], gPerm[M]);
	}

	for(UNS32 N = 0; N < gPermSize; ++N)
		gPerm[gPermSize + N] = gPerm[N];
}

// Perlin噪声
SPFPN PerlinNoise(SPFPN X, SPFPN Y){
	using namespace impl::perlin;

	UNS32 ModX = (INT32)floor(X) & (gPermSize - 1);
	UNS32 ModY = (INT32)floor(Y) & (gPermSize - 1);

	SPFPN DecX = X - floor(X);
	SPFPN DecY = Y - floor(Y);

	SPFPN U = Fade(DecX);
	SPFPN V = Fade(DecY);

	INT32 A0 = gPerm[gPerm[ModX] + ModY];
	INT32 A1 = gPerm[gPerm[ModX] + ModY + 1];
	INT32 B0 = gPerm[gPerm[ModX + 1] + ModY];
	INT32 B1 = gPerm[gPerm[ModX + 1] + ModY + 1];

	SPFPN G00 = Gradient(A0, DecX, DecY);
	SPFPN G01 = Gradient(B0, DecX - 1.f, DecY);
	SPFPN G10 = Gradient(A1, DecX, DecY - 1.f);
	SPFPN G11 = Gradient(B1, DecX - 1.f, DecY - 1.f);

	SPFPN X0 = LERP_(G00, G01, U);
	SPFPN X1 = LERP_(G10, G11, U);

	return LERP_(X0, X1, V);
}

//----------------------------------------//


//-------------------- 数据比较 --------------------//

// 升序比较
template<typename TYPE>
INT32 tlCompareAsc(_in $VOID *pvData0, _in $VOID *pvData1){
	TYPE Data0 = *(TYPE*)pvData0;
	TYPE Data1 = *(TYPE*)pvData1;

	if(Data0 < Data1) return -1;
	if(Data0 > Data1) return 1;
	return 0;
}

// 逆序比较
template<typename TYPE>
INT32 tlCompareDesc(_in $VOID *pvData0, _in $VOID *pvData1){
	TYPE Data0 = *(TYPE*)pvData0;
	TYPE Data1 = *(TYPE*)pvData1;

	if(Data0 > Data1) return -1;
	if(Data0 < Data1) return 1;
	return 0;
}

//----------------------------------------//


//-------------------- 侵入式单链表操作 --------------------//

// 统计链表节点
template<typename TYPE>
UNS32 LnkListSize(_in TYPE *pHead, TYPE* TYPE::*pNext){
	UNS32 Count = 0;
	const TYPE *pNode = pHead;

	while(pNode){
		pNode = pNode->*pNext;
		Count++;
	}

	return Count;
}

// 移除链表表头
template<typename TYPE>
TYPE* LnkListPopHead(_io TYPE *&rHead, TYPE* TYPE::*pNext){
	TYPE *pNode = rHead;
	rHead = rHead->*pNext;
	return pNode;
}

// 移除链表节点
template<typename TYPE>
TYPE* LnkListRemoveAfter(_io TYPE *&rPrev, TYPE* TYPE::*pNext){
	TYPE *pTarget = rPrev->*pNext;
	rPrev->*pNext = pTarget->*pNext;
	return pTarget;
}

// 插入链表表头
template<typename TYPE>
$VOID LnkListPushHead(_io TYPE *&rHead, _io TYPE *pTarget, TYPE* TYPE::*pNext){
	pTarget->*pNext = rHead;
	rHead = pTarget;
}

// 插入链表节点
template<typename TYPE>
$VOID LnkListInsertAfter(_io TYPE *&rPrev, _io TYPE *pTarget, TYPE* TYPE::*pNext){
	pTarget->*pNext = rPrev->*pNext;
	rPrev->*pNext = pTarget;
}

//----------------------------------------//


//-------------------- 动态分配数组操作 --------------------//

// 追加数组元素
template<typename TYPE>
$VOID DynArrAppend(_io TYPE *&rArrAddr, _io UNS32 &rCurSize, _io UNS32 &rCapacity, _in TYPE &NewElem){
	if(rCurSize == rCapacity){
		rCapacity = (UNS32)ToPowOf2(rCapacity * 2);
		rArrAddr = (TYPE*)realloc(rArrAddr, rCapacity * sizeof(TYPE));
	}

	rArrAddr[rCurSize] = NewElem;
	rCurSize += 1;
}

// 追加数组元素
template<typename TYPE>
$VOID DynArrAppend(_io TYPE *&rArrAddr, _io UNS32 &rCurSize, _in TYPE &NewElem){
	UIPTR rCapacity = _msize(rArrAddr) / sizeof(TYPE);

	if(rCurSize == rCapacity){
		rCapacity = ToPowOf2(rCapacity * 2);
		rArrAddr = (TYPE*)realloc(rArrAddr, rCapacity * sizeof(TYPE));
	}

	rArrAddr[rCurSize] = NewElem;
	rCurSize += 1;
}

// 预留数组空间
template<typename TYPE>
TYPE* DynArrReserve(_io TYPE *&rArrAddr, _io UNS32 &rCurSize, _io UNS32 &rCapacity, UNS32 AddCnt){
	UNS32 NewSize = rCurSize + AddCnt;

	if(NewSize > rCapacity){
		rCapacity = (UNS32)ToPowOf2(NewSize);
		rArrAddr = (TYPE*)realloc(rArrAddr, rCapacity * sizeof(TYPE));
	}

	TYPE *pDest = rArrAddr + rCurSize;
	rCurSize = NewSize;
	return pDest;
}

// 预留数组空间
template<typename TYPE>
TYPE* DynArrReserve(_io TYPE *&rArrAddr, _io UNS32 &rCurSize, UNS32 AddCnt){
	UIPTR rCapacity = _msize(rArrAddr) / sizeof(TYPE);
	UNS32 NewSize = rCurSize + AddCnt;

	if(NewSize > rCapacity){
		rCapacity = (UNS32)ToPowOf2(NewSize);
		rArrAddr = (TYPE*)realloc(rArrAddr, rCapacity * sizeof(TYPE));
	}

	TYPE *pDest = rArrAddr + rCurSize;
	rCurSize = NewSize;
	return pDest;
}

//----------------------------------------//


//-------------------- 容器 --------------------//

// Sequence Map
template<typename VTYPE>
class TSeqMap{
	_open struct NPair{
		UNS64 Key;
		VTYPE Value;
	};

	_rest UNS32 Size;
	_rest UNS32 Capacity;
	_rest IBOOL bSorted;
	_rest NPair *lprgPair;

	_open ~TSeqMap(){
		$m.Clear(B_True);
	}
	_open TSeqMap(){
		$m.lprgPair = P_Null;
	}
	_open TSeqMap(UNS32 Capacity){
		$m.Init(Capacity);
	}
	//////
	_open $VOID Init(UNS32 Capacity){
		$m.Size = 0;
		$m.bSorted = B_False;
		$m.Capacity = Capacity;
		DYNARR_ALLOC($m.lprgPair, Capacity);
	}
	_open $VOID Clear(IBOOL bDestroy){
		if($m.lprgPair){
			for(UNS32 N = 0; N < $m.Size; ++N)
				$m.lprgPair[N].Value.~VTYPE();

			if(bDestroy) {
				free($m.lprgPair);
				$m.lprgPair = P_Null;
				$m.Capacity = 0;
			}
		}

		$m.Size = 0;
		$m.bSorted = B_False;
	}
	_open $VOID Reserve(UNS32 Capacity){
		$m.Capacity = Capacity;
		DYNARR_REALLOC($m.lprgPair, Capacity);
	}
	_open $VOID Insert(_in CHAR8 *psKey, _in VTYPE &Value){
		$m.Insert(StrHash64(psKey), Value);
	}
	_open $VOID Insert(UNS64 Key, _in VTYPE &Value){
		*$m.New(Key) = Value;
	}
	_open VTYPE& operator[](_in CHAR8 *psKey){
		return $m.operator[](StrHash64(psKey));
	}
	_open VTYPE& operator[](UNS64 Key){
		VTYPE *pValue = $m.Get(Key);
		if(pValue) return *pValue;
		pValue = $m.New(Key);
		new(pValue) VTYPE();;
		return *pValue;
	}
	_open VTYPE* New(_in CHAR8 *psKey){
		return $m.New(StrHash64(psKey));
	}
	_open VTYPE* New(UNS64 Key){
		if($m.Size == $m.Capacity)
			$m.Reserve((UNS32)ToPowOf2($m.Capacity * 2));

		NPair *pPair = $m.lprgPair + $m.Size;
		pPair->Key = Key;

		$m.bSorted = B_False;
		$m.Size += 1;

		return &pPair->Value;
	}
	_open VTYPE* Get(_in CHAR8 *psKey){
		return $m.Get(StrHash64(psKey));
	}
	_open VTYPE* Get(UNS64 Key){
		if(!$m.bSorted){
			$m.bSorted = B_True;
			qsort($m.lprgPair, $m.Size, sizeof(NPair), tlCompareAsc<UNS64>);
		}

		NPair *pPair = $m.Search(Key);
		if(!pPair) return P_Null;
		else return &pPair->Value;
	}
	_open VTYPE* GetAt(UNS32 Index){
		return &$m.lprgPair[Index].Value;
	}
	_secr NPair* Search(UNS64 Key){
		NPair *pElem;
		INT32 Mid, Low = 0, High = $m.Size - 1;

		while(Low <= High){
			Mid = (Low + High) / 2;
			pElem = $m.lprgPair + Mid;

			if(pElem->Key > Key)
				High = Mid - 1;
			else if(pElem->Key < Key)
				Low = Mid + 1;
			else return pElem;
		}

		return P_Null;
	}
	_open UNS32 GetSize(){
		return $m.Size;
	}
};

// Circular Queue
template<typename VTYPE>
class TCirQueue{
	_rest UNS32 iHead;
	_rest UNS32 iTail;
	_rest UNS32 Capacity;
	_rest VTYPE *lprgElem;

	_open ~TCirQueue(){
		SAFE_FREE($m.lprgElem);
	}
	_open TCirQueue(){
		$m.lprgElem = P_Null;
	}
	_open TCirQueue(UNS32 Capacity){
		$m.Init(Capacity);
	}
	//////
	_open $VOID Init(UNS32 Capacity){
		$m.iHead = 0;
		$m.iTail = 0;
		$m.Capacity = Capacity + 1;
		DYNARR_ALLOC($m.lprgElem, $m.Capacity);
	}
	_open $VOID Clear(IBOOL bDestroy){
		if(bDestroy){
			SAFE_FREE($m.lprgElem);
			$m.Capacity = 0;
		}

		$m.iHead = 0;
		$m.iTail = 0;
	}
	_open $VOID PushBack(_in VTYPE &Value){
		UNS32 Index = $m.iTail;
		$m.iTail += 1;
		$m.iTail %= $m.Capacity;
		$m.lprgElem[Index] = Value;
	}
	_open VTYPE* PopFront(){
		UNS32 Index = $m.iHead;
		$m.iHead += 1;
		$m.iHead %= $m.Capacity;
		return $m.lprgElem + Index;
	}
	_open VTYPE* GetFront(){
		return $m.lprgElem + $m.iHead;
	}
	_open UNS32 GetSize(){
		return ($m.iTail >= $m.iHead) ?
			($m.iTail - $m.iHead) :
			($m.Capacity - $m.iHead + $m.iTail);
	}
	_open IBOOL IsEmpty(){
		return ($m.iHead == $m.iTail);
	}
	_open IBOOL IsFull(){
		UNS32 NextPos = ($m.iTail + 1) % $m.Capacity;
		return (NextPos == $m.iHead);
	}
};

//----------------------------------------//


//-------------------- 字符串库 --------------------//

// String Repository
class CStrRepos{
	_secr UNS32 Count;
	_secr UNS32 *lprgAddr;
	_secr CHAR8 *lpsBuffer;

	_open ~CStrRepos(){
		$m.Release();
	}
	_open CStrRepos(){
		Var_Zero(this);
	}
	_open CStrRepos(_in WCHAR *psFilePath, _in CHAR8 *psDelim){
		$m.Initialize(psFilePath, psDelim);
	}
	//////
	_open $VOID Release(){
		SAFE_DELETEA($m.lprgAddr);
		SAFE_DELETEA($m.lpsBuffer);
	}
	_open $VOID Initialize(_in WCHAR *psFilePath, _in CHAR8 *psDelim){
		/// 加载文件

		HANDLE hrFile = CreateFile2(psFilePath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		UNS32 FileSize = GetFileSize(hrFile, P_Null);

		$m.lpsBuffer = new CHAR8[FileSize + 1];
		ReadFile(hrFile, $m.lpsBuffer, FileSize, P_Null, P_Null);
		$m.lpsBuffer[FileSize] = '\0';
		CloseHandle(hrFile);

		/// 划分字符串

		CHAR8 *psMatch = $m.lpsBuffer;
		UIPTR DelimLen = strlen(psDelim);
		UNS32 Capacity = 64;
		UNS32 Offset = 0;

		DYNARR_ALLOC($m.lprgAddr, Capacity);
		$m.lprgAddr[0] = 0;
		$m.Count = 1;

		while(psMatch = strstr(psMatch, psDelim)){
			Mem_Zero(psMatch, DelimLen);
			psMatch += DelimLen;
			Offset = UNS32(psMatch - $m.lpsBuffer);
			DynArrAppend($m.lprgAddr, $m.Count, Capacity, Offset);
		}

		DYNARR_REALLOC($m.lprgAddr, $m.Count);
	}
	_open $VOID Replace(CHAR8 Old, CHAR8 New){
		for(UNS32 N = 0; N < $m.Count; ++N){
			CHAR8 *pChar = $m.lpsBuffer + $m.lprgAddr[N];
			for(; *pChar; ++pChar)
				if(*pChar == Old) *pChar = New;
		}
	}
	_open UNS32 GetCount(){
		return $m.Count;
	}
	_open const CHAR8* Get(UNS32 Index){
		return $m.lpsBuffer + $m.lprgAddr[Index];
	}
};

//----------------------------------------//


//-------------------- JSON --------------------//

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

		WriteFile(hrFile, lpsCode, (UNS32)strlen(lpsCode), P_Null, P_Null);
		CloseHandle(hrFile);

		cJSON_free(lpsCode);
	}
	//////
	_open $VOID InsertChild(CJsonNode *pNode, UNS32 Index){
		cJSON_InsertItemInArray(this, Index, pNode);
	}
	_open $VOID AppendChild(CJsonNode *pNode, _in CHAR8 *psKey){
		cJSON_AddItemToObject(this, psKey, pNode);
	}
	_open $VOID AppendChild(CJsonNode *pNode){
		cJSON_AddItemToArray(this, pNode);
	}
	//////
	_open $VOID DeleteChild(UNS32 Index){
		cJSON_Delete(cJSON_DetachItemFromArray(this, Index));
	}
	_open $VOID DeleteChild(_in CHAR8 *psName){
		cJSON_Delete(cJSON_DetachItemFromObjectCaseSensitive(this, psName));
	}
	//////
	_open $VOID GetArrVal8(_out BYTET *prgResult){
		BYTET *pDest = prgResult;
		cJSON *pSrc = $m.child;

		while(pSrc){
			*pDest = (BYTET)pSrc->valueint;
			pSrc = pSrc->next;
			pDest++;
		}
	}
	_open $VOID GetArrVal16(_out WORDT *prgResult){
		WORDT *pDest = prgResult;
		cJSON *pSrc = $m.child;

		while(pSrc){
			*pDest = (WORDT)pSrc->valueint;
			pSrc = pSrc->next;
			pDest++;
		}
	}
	_open $VOID GetArrVal32(_out DWORD *prgResult){
		DWORD *pDest = prgResult;
		cJSON *pSrc = $m.child;

		while(pSrc){
			*pDest = (DWORD)pSrc->valueint;
			pSrc = pSrc->next;
			pDest++;
		}
	}
	_open $VOID GetArrValF32(_out SPFPN *prgResult){
		SPFPN *pDest = prgResult;
		cJSON *pSrc = $m.child;

		while(pSrc){
			*pDest = (SPFPN)pSrc->valuedouble;
			pSrc = pSrc->next;
			pDest++;
		}
	}
	_open $VOID GetArrValF64(_out DPFPN *prgResult){
		DPFPN *pDest = prgResult;
		cJSON *pSrc = $m.child;

		while(pSrc){
			*pDest = pSrc->valuedouble;
			pSrc = pSrc->next;
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
	_open BOOL8 GetElemB(UNS32 Index){
		return (BOOL8)cJSON_GetArrayItem(this, Index)->valueint;
	}
	_open INT32 GetElemI(UNS32 Index){
		return cJSON_GetArrayItem(this, Index)->valueint;
	}
	_open SPFPN GetElemF32(UNS32 Index){
		return (SPFPN)cJSON_GetArrayItem(this, Index)->valuedouble;
	}
	_open DPFPN GetElemF64(UNS32 Index){
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
	_open UNS32 GetSize(){
		return cJSON_GetArraySize(this);
	}
	_open CHAR8* CreateCode(){
		return cJSON_Print(this);
	}
	//////
	_open CJsonNode* GetNext(){
		return (CJsonNode*)$m.next;
	}
	_open CJsonNode* GetChild(){
		return (CJsonNode*)$m.child;
	}
	_open CJsonNode* GetChild(UNS32 Index){
		return (CJsonNode*)cJSON_GetArrayItem(this, Index);
	}
	_open CJsonNode* GetChild(_in CHAR8 *psName){
		return (CJsonNode*)cJSON_GetObjectItemCaseSensitive(this, psName);
	}
	_open CJsonNode* DetachChild(UNS32 Index){
		return (CJsonNode*)cJSON_DetachItemFromArray(this, Index);
	}
	_open CJsonNode* DetachChild(_in CHAR8 *psName){
		return (CJsonNode*)cJSON_DetachItemFromObjectCaseSensitive(this, psName);
	}
	//////
	_open const CHAR8* GetKey(){
		return $m.string;
	}
	_open const CHAR8* GetValS(){
		return $m.valuestring;
	}
	_open const CHAR8* GetElemS(UNS32 Index){
		return cJSON_GetArrayItem(this, Index)->valuestring;
	}
	_open const CHAR8* GetMemS(_in CHAR8 *psName){
		return cJSON_GetObjectItemCaseSensitive(this, psName)->valuestring;
	}

	_open static CJsonNode* CreateNull(){
		return (CJsonNode*)cJSON_CreateNull();
	}
	_open static CJsonNode* CreateArray(){
		return (CJsonNode*)cJSON_CreateArray();
	}
	_open static CJsonNode* CreateObject(){
		return (CJsonNode*)cJSON_CreateObject();
	}
	_open static CJsonNode* CreateBool(IBOOL Value){
		return (CJsonNode*)cJSON_CreateBool(Value);
	}
	_open static CJsonNode* CreateNumber(DPFPN Value){
		return (CJsonNode*)cJSON_CreateNumber(Value);
	}
	_open static CJsonNode* CreateRaw(_in CHAR8 *psValue){
		return (CJsonNode*)cJSON_CreateRaw(psValue);
	}
	_open static CJsonNode* CreateString(_in CHAR8 *psValue){
		return (CJsonNode*)cJSON_CreateString(psValue);
	}
	//////
	_open static CJsonNode* CreateArrayS(_in CHAR8 **prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateStringArray(prgValue, Count);
	}
	_open static CJsonNode* CreateArrayI(_in INT32 *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateIntArray(prgValue, Count);
	}
	_open static CJsonNode* CreateArrayF32(_in SPFPN *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateFloatArray(prgValue, Count);
	}
	_open static CJsonNode* CreateArrayF64(_in DPFPN *prgValue, INT32 Count){
		return (CJsonNode*)cJSON_CreateDoubleArray(prgValue, Count);
	}
	//////
	_open static CJsonNode* CreateByCode(_in CHAR8 *psCode){
		return (CJsonNode*)cJSON_Parse(psCode);
	}
	_open static CJsonNode* CreateByFile(_in WCHAR *psPath){
		HANDLE hrFile = CreateFile2(psPath, GENERIC_READ, 0L, OPEN_EXISTING, P_Null);
		UNS32 FileSize = GetFileSize(hrFile, P_Null);
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

//----------------------------------------//


//-------------------- XML --------------------//

#ifdef TINYXML2_INCLUDED
/// XML类
typedef tinyxml2::XMLNode CXmlNode;
typedef tinyxml2::XMLText CXmlText;
typedef tinyxml2::XMLHandle CXmlHandle;
typedef tinyxml2::XMLComment CXmlComment;
typedef tinyxml2::XMLElement CXmlElement;
typedef tinyxml2::XMLUnknown CXmlUnknown;
typedef tinyxml2::XMLPrinter CXmlPrinter;
typedef tinyxml2::XMLVisitor CXmlVisitor;
typedef tinyxml2::XMLDocument CXmlDocument;
typedef tinyxml2::XMLAttribute CXmlAttribute;
typedef tinyxml2::XMLDeclaration CXmlDeclaration;
typedef tinyxml2::XMLConstHandle CXmlConstHandle;
/// XML枚举
typedef tinyxml2::XMLError XMLERROR;
typedef tinyxml2::Whitespace XMLWHITESPACE;
#endif

//----------------------------------------//