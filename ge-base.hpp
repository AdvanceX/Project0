//----------------------------------------//

#pragma once

//----------------------------------------//


//-------------------- 基础数据类型 --------------------//

typedef bool BOOL8; // 8b Boolean
typedef int  IBOOL; // Integer for Boolean
typedef char    CHAR8; // 8b Character
typedef wchar_t WCHAR; // Wide Character
typedef signed char          INT08; // 8b Integer
typedef signed short int     INT16; // 16b Integer
typedef signed int           INT32; // 32b Integer
typedef signed long long int INT64; // 64b Integer
typedef unsigned char          UNS08; // 8b Unsigned Integer
typedef unsigned short int     UNS16; // 16b Unsigned Integer
typedef unsigned int           UNS32; // 32b Unsigned Integer
typedef unsigned long long int UNS64; // 64b Unsigned Integer
typedef unsigned char      BYTET; // Byte
typedef unsigned short     WORDT; // Word
typedef unsigned long      DWORD; // Double Word
typedef unsigned long long QWORD; // Quadruple Word
typedef float  SPFPN; // Single-Precision Floating-Point Number
typedef double DPFPN; // Double-Precision Floating-Point Number

typedef intptr_t  SIPTR; // Signed Integer for Pointer
typedef uintptr_t UIPTR; // Unsigned Integer for Pointer

//----------------------------------------//


//-------------------- 常用回调类型 --------------------//

typedef void(*PFNFOREACH)(void*, void*); // For Each
typedef int(*PFNCOMPARE)(const void*, const void*); // Compare

//----------------------------------------//


//-------------------- 字符串类 --------------------//

#ifdef _STRING_
typedef std::string  CString;
typedef std::wstring CWString;
#endif

//----------------------------------------//


//-------------------- 泛型容器 --------------------//

#ifdef _ARRAY_
template<typename TYPE>
using TArray = std::array<TYPE>;
#endif

#ifdef _VECTOR_
template<typename TYPE>
using TSeqList = std::vector<TYPE>;
#endif

#ifdef _LIST_
template<typename TYPE>
using TLinkedList = std::list<TYPE>;
#endif

#ifdef _FORWARD_LIST_
template<typename TYPE>
using TSinglyList = std::forward_list<TYPE>;
#endif

#ifdef _DEQUE_
template<typename TYPE>
using TDEQueue = std::deque<TYPE>;
#endif

#ifdef _QUEUE_
template<typename TYPE>
using TPriorityQueue = std::priority_queue<TYPE>;
#endif

#ifdef _SET_
template<typename TYPE>
using TTreeSet = std::set<TYPE>;
#endif

#ifdef _UNORDERED_SET_
template<typename TYPE>
using THashSet = std::unordered_set<TYPE>;
#endif

#ifdef _MAP_
template<typename KTYPE, typename VTYPE>
using TTreeMap = std::map<KTYPE, VTYPE>;
#endif

#ifdef _UNORDERED_MAP_
template<typename KTYPE, typename VTYPE>
using THashMap = std::unordered_map<KTYPE, VTYPE>;
#endif

//----------------------------------------//


//-------------------- STL线程 --------------------//

#ifdef _FUTURE_
template<typename TYPE>
using TStlFuture = std::future<TYPE>;

template<typename TYPE>
using TStlPromise = std::promise<TYPE>;

template<typename TYPE>
using TSharedFuture = std::shared_future<TYPE>;

template<typename RETTYPE, typename... ARGTYPES>
using TPackagedTask = std::packaged_task<RETTYPE(ARGTYPES...)>;
#endif

//----------------------------------------//


//-------------------- 智能指针 --------------------//

#ifdef _MEMORY_
template<typename TYPE>
using TWeakPtr = std::weak_ptr<TYPE>;

template<typename TYPE>
using TSharedPtr = std::shared_ptr<TYPE>;

template<typename TYPE>
using TUniquePtr = std::unique_ptr<TYPE>;
#endif

//----------------------------------------//


//-------------------- 重定义消除 --------------------//

#undef max
#undef min
#undef IN
#undef OUT
#undef RGB
#undef VOID
#undef NULL
#undef TRUE
#undef FALSE
#undef HIBYTE
#undef HIWORD
#undef LOBYTE
#undef LOWORD
#undef FAILED
#undef MAKELONG
#undef MAKEWORD
#undef INFINITE
#undef SUCCEEDED
#undef GetRValue
#undef GetGValue
#undef GetBValue
#undef CopyMemory
#undef FillMemory
#undef MoveMemory
#undef ZeroMemory

//----------------------------------------//


//-------------------- 声明修饰 --------------------//

#define _io
#define _out
#define _in const

#define _SYSC _stdcall
#define _CBACK _stdcall

#define _open public:
#define _secr private:
#define _rest protected:

#define _DLLIMPORT _declspec(dllimport)
#define _DLLEXPORT extern"C" _declspec(dllexport)

//----------------------------------------//


//-------------------- 空类型 --------------------//

#define $VOID void

//----------------------------------------//


//-------------------- 当前对象 --------------------//

#define $m (*this)
#define $sup __super::

//----------------------------------------//


//-------------------- 标准常量 --------------------//

#define P_Null nullptr
#define B_True true
#define B_False false

//----------------------------------------//


//-------------------- 无穷大&NaN --------------------//

#define FLT_NAN ((float)NAN)
#define DBL_NAN ((double)NAN)

#define FLT_INFINITY ((float)INFINITY)
#define DBL_INFINITY ((double)INFINITY)

//----------------------------------------//