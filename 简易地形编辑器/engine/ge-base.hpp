//****************************************//

#pragma once

#include <map>
#include <set>
#include <list>
#include <deque>
#include <vector>
#include <string>
#include <future>
#include <memory>

#include <math.h>
#include <tchar.h>
#include <stdlib.h>
#include <immintrin.h>

//****************************************//


//-------------------- 基础数据类型 --------------------//

typedef bool BOOL8; // 8b Boolean
typedef int  IBOOL; // Integer for Boolean
typedef char    CHAR8; // 8b Character
typedef wchar_t WCHAR; // Wide Character
typedef signed char          INT08; // 8b Integer
typedef signed short int     INT16; // 16b Integer
typedef signed int           INT32; // 32b Integer
typedef signed long long int INT64; // 64b Integer
typedef unsigned char          UTINY; // Unsigned Tiny(Integer)
typedef unsigned short int     UHALF; // Unsigned Half(Integer)
typedef unsigned int           USINT; // Unsigned Standard Integer
typedef unsigned long long int ULEXT; // Unsigned Extra-Long(Integer)
typedef unsigned char      BYTET; // Byte
typedef unsigned short     WORDT; // Word
typedef unsigned long      DWORD; // Double Word
typedef unsigned long long QWORD; // Quadruple Word
typedef float  SPFPN; // Single-Precision Floating-Point Number
typedef double DPFPN; // Double-Precision Floating-Point Number

typedef intptr_t  SPINT; // Signed Pointer(Bit-Width) Integer
typedef uintptr_t UPINT; // Unsigned Pointer(Bit-Width) Integer

typedef __m64   MX064; // 64b MMX Vector
typedef __m128  MX128; // 128b MMX Vector
typedef __m128i MI128; // 128b MMX Integers
typedef __m128d MD128; // 128b MMX DPFPNs
typedef __m256  MX256; // 256b MMX Vector
typedef __m256i MI256; // 256b MMX Integers
typedef __m256d MD256; // 256b MMX DPFPNs

//----------------------------------------//


//-------------------- 变参列表 --------------------//

typedef va_list PVALIST;

//----------------------------------------//


//-------------------- div函数返回 --------------------//

typedef div_t   DIV;
typedef ldiv_t  DIV32;
typedef lldiv_t DIV64;

//----------------------------------------//


//-------------------- 字符串类 --------------------//

typedef std::string  AString;
typedef std::wstring WString;

//----------------------------------------//


//-------------------- 智能指针 --------------------//

template<typename TYPE>
using TWeakPtr = std::weak_ptr<TYPE>;

template<typename TYPE>
using TSharedPtr = std::shared_ptr<TYPE>;

template<typename TYPE>
using TUniquePtr = std::unique_ptr<TYPE>;

//----------------------------------------//


//-------------------- STL容器 --------------------//

template<typename KTYPE, typename VTYPE>
using STLMap = std::map<KTYPE, VTYPE>;

template<typename TYPE>
using STLSet = std::set<TYPE>;

template<typename TYPE>
using STLList = std::list<TYPE>;

template<typename TYPE>
using STLDeque = std::deque<TYPE>;

template<typename TYPE>
using STLVector = std::vector<TYPE>;

//----------------------------------------//


//-------------------- STL线程 --------------------//

template<typename TYPE>
using STLFuture = std::future<TYPE>;

template<typename TYPE>
using STLPromise = std::promise<TYPE>;

//----------------------------------------//


//-------------------- COM智能指针 --------------------//

#ifdef __ATLCOMCLI_H__
	template<typename TYPE>
	using TComPtr = CComPtr<TYPE>;

	template<typename TYPE>
	using TComQIPtr = CComQIPtr<TYPE>;
#endif

//----------------------------------------//


//-------------------- 重定义消除 --------------------//

#undef max
#undef min
#undef IN
#undef OUT
#undef RGB
#undef A2W
#undef W2A
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
#undef INFINITY
#undef SUCCEEDED
#undef GetRValue
#undef GetGValue
#undef GetBValue

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


//-------------------- 无穷&非数字 --------------------//

#define DPFP_INF (1e300 * 1e300)
#define DPFP_NAN (DPFP_INF * 0.0)
#define SPFP_INF ((float)DPFP_INF)
#define SPFP_NAN (SPFP_INF * 0.f)

//----------------------------------------//


//-------------------- 变参访问 --------------------//

#define VA_END __crt_va_end
#define VA_NEXT __crt_va_arg
#define VA_START __crt_va_start

//----------------------------------------//


//-------------------- 字符集兼容 --------------------//

#define StrLen _tcslen
#define StrSet _tcsset
#define StrCat _tcscat
#define StrCpy _tcscpy
#define StrCmp _tcscmp
#define StrIcmp _tcsicmp
#define StrNlen _tcsnlen
#define StrNset _tcsnset
#define StrNcat _tcsncat
#define StrNcpy _tcsncpy
#define StrNcmp _tcsncmp
#define StrNicmp _tcsnicmp
#define StrSpn _tcsspn
#define StrLwr _tcslwr
#define StrUpr _tcsupr
#define StrRev _tcsrev
#define StrDup _tcsdup
#define StrTok _tcstok
#define StrStr _tcsstr
#define StrChr _tcschr
#define StrRchr _tcsrchr
#define StrCspn _tcscspn
#define StrPbrk _tcspbrk
#define StrColl _tcscoll
#define StrXfrm _tcsxfrm

#define ItoS _itot
#define LtoS _ltot
#define UtoS _ultot
#define I64toS _i64tot
#define U64toS _ui64tot
#define StoD _ttof
#define StoI _ttoi
#define StoL _ttol
#define StoI64 _ttoll
#define StoDx _tcstod
#define StoFx _tcstof
#define StoLx _tcstol
#define StoUx _tcstoul
#define StoF64x _tcstold
#define StoI64x _tcstoll
#define StoU64x _tcstoull

#define Sscanf _stscanf
#define Svscanf _vstscanf
#define Sprintf _stprintf
#define Svprintf _vstprintf
#define Snprintf _sntprintf
#define Svnprintf _vsntprintf

//----------------------------------------//


//-------------------- 字符串操作 --------------------//

#define MbsLen strlen
#define MbsSet strset
#define MbsCat strcat
#define MbsCpy strcpy
#define MbsCmp strcmp
#define MbsIcmp stricmp
#define MbsNlen strnlen
#define MbsNset strnset
#define MbsNcat strncat
#define MbsNcpy strncpy
#define MbsNcmp strncmp
#define MbsNicmp strnicmp
#define MbsSpn strspn
#define MbsLwr strlwr
#define MbsUpr strupr
#define MbsRev strrev
#define MbsDup strdup
#define MbsTok strtok
#define MbsStr strstr
#define MbsChr strchr
#define MbsRchr strrchr
#define MbsCspn strcspn
#define MbsPbrk strpbrk
#define MbsColl strcoll
#define MbsXfrm strxfrm

#define WcsLen wcslen
#define WcsSet wcsset
#define WcsCat wcscat
#define WcsCpy wcscpy
#define WcsCmp wcscmp
#define WcsIcmp wcsicmp
#define WcsNlen wcsnlen
#define WcsNset wcsnset
#define WcsNcat wcsncat
#define WcsNcpy wcsncpy
#define WcsNcmp wcsncmp
#define WcsNicmp wcsnicmp
#define WcsSpn wcsspn
#define WcsLwr wcslwr
#define WcsUpr wcsupr
#define WcsRev wcsrev
#define WcsDup wcsdup
#define WcsTok wcstok
#define WcsStr wcsstr
#define WcsChr wcschr
#define WcsRchr wcsrchr
#define WcsCspn wcscspn
#define WcsPbrk wcspbrk
#define WcsColl wcscoll
#define WcsXfrm wcsxfrm

//----------------------------------------//


//-------------------- 字符串-数字转换 --------------------//

#define EtoA ecvt
#define FtoA fcvt
#define GtoA gcvt

#define ItoA itoa
#define LtoA ltoa
#define UtoA ultoa
#define I64toA _i64toa
#define U64toA _ui64toa
#define ItoW _itow
#define LtoW _ltow
#define UtoW _ultow
#define I64toW _i64tow
#define U64toW _ui64tow

#define AtoD atof
#define AtoI atoi
#define AtoL atol
#define AtoI64 atoll
#define WtoD _wtof
#define WtoI _wtoi
#define WtoL _wtol
#define WtoI64 _wtoll

#define AtoDx strtod
#define AtoFx strtof
#define AtoLx strtol
#define AtoUx strtoul
#define AtoF64x strtold
#define AtoI64x strtoll
#define AtoU64x strtoull
#define WtoDx wcstod
#define WtoFx wcstof
#define WtoLx wcstol
#define WtoUx wcstoul
#define WtoF64x wcstold
#define WtoI64x wcstoll
#define WtoU64x wcstoull

//----------------------------------------//


//-------------------- 字符串格式化 --------------------//

#define Ascanf sscanf
#define Wscanf swscanf
#define Avscanf vsscanf
#define Wvscanf vswscanf

#define Aprintf sprintf
#define Wprintf swprintf
#define Avprintf vsprintf
#define Wvprintf vswprintf

#define Anprintf snprintf
#define Wnprintf _snwprintf
#define Avnprintf vsnprintf
#define Wvnprintf _vsnwprintf

//----------------------------------------//


//-------------------- 字符检测 --------------------//

#define IsCntrl iscntrl  //是否控制码(0到30)
#define IsBlank isblank  //是否空格(' ','\t')
#define IsSpace isspace  //是否空字符(' ','\t','\r','\n','\v','\f')
#define IsPunct ispunct  //是否标点/特殊符号
#define IsPrint isprint  //是否可打印字符
#define IsGraph isgraph  //是否图形字符(除空字符外的可打印字符)
#define IsLover islower  //是否小写字母
#define IsUpper isupper  //是否大写字母
#define IsAlNum isalnum  //是否字母/数字
#define IsAlpha isalpha  //是否字母
#define IsDigit isdigit  //是否数字
#define IsHxNum isxdigit //是否16进制数字

//----------------------------------------//


//-------------------- 浮点数检测 --------------------//

#define IsNaN isnan       //是否非数(阶码全1,尾数不全0)
#define IsInf isinf       //是否无穷(阶码全1,尾数全0)
#define IsFinite isfinite //是否有限数(不是Inf/NaN)
#define IsNormal isnormal //是否规范数(不是Inf/NaN/0,也不是Subnormal(阶码全0))
#define FpClassif fpclassify //浮点数归类
#define SignBit signbit      //获取符号位

//----------------------------------------//


//-------------------- 数学计算 --------------------//

#define Abs abs   // |x|
#define Fabs fabs // |x|

#define Rint rint   // 四舍六入五取偶
#define Modf modf   // x - [x], y = [x]
#define Ceil ceil   // ⌈x⌉
#define Floor floor // ⌊x⌋
#define Trunc trunc // [x]
#define Round round // ⌊x + 0.5⌋

#define Div div       // x / y
#define Mod fmod      //求模(余数绝对值最小)
#define Rem remainder //求余(商最小)
#define RemQuo remquo //求余,商赋给z

#define Sqrt sqrt // ²√x
#define Cbrt cbrt // ³√x

#define Pow pow   // xʸ
#define Exp exp   // eˣ
#define Exp2 exp2 // 2ˣ

#define Log log     // logₑx
#define Log2 log2   // log₂x
#define Log10 log10 // log₁₀x

#define Sin sin // sin(x)
#define Cos cos // cos(x)
#define Tan tan // tan(x)

#define Asin asin   // arcsin(x)
#define Acos acos   // arccos(x)
#define Atan atan   // arctan(x)
#define Atan2 atan2 // arctan(y / x)

#define Sinh sinh //双曲正弦
#define Cosh cosh //双曲余弦
#define Tanh tanh //双曲正切

#define Asinh asinh //反双曲正弦
#define Acosh acosh //反双曲余弦
#define Atanh atanh //反双曲正切

#define Erf erf   //高斯误差
#define Erfc erfc //补余高斯误差

#define Tgamma tgamma // gamma(x)
#define Lgamma lgamma // ln(|gamma(x)|)

#define Fma fma     // x*y + z
#define Fdim fdim   // max(x - y, 0)
#define Hypot hypot // √(x² + y²)
#define Log1P log1p // logₑ(x + 1)
#define ExpM1 expm1 // eˣ − 1
#define LdExp ldexp // x * 2ʸ
#define FrExp frexp // x = return * 2ʸ

#define CopySign copysign // sign(y)|x|

//----------------------------------------//


//-------------------- 随机数生成 --------------------//

#define Rand rand
#define SeedRand srand

//----------------------------------------//


//-------------------- 数组查找与排序 --------------------//

#define QckSort qsort
#define LinFind lfind
#define LinSearch lsearch
#define BinSearch bsearch

//----------------------------------------//


//-------------------- 内存分配 --------------------//

#define Mfree free
#define Msize _msize
#define Malloc malloc
#define Mcalloc calloc
#define Malloca _alloca
#define Mexpand _expand
#define Mrealloc realloc
#define Mrecalloc _recalloc

#define AlignedMfree _aligned_free
#define AlignedMsize _aligned_msize
#define AlignedMalloc _aligned_malloc
#define AlignedMrealloc _aligned_realloc
#define AlignedMrecalloc _aligned_recalloc

//----------------------------------------//