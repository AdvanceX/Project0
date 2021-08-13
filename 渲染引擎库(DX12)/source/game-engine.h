#define WIN32_LEAN_AND_MEAN

#include <list>
#include <wrl.h>
#include <tchar.h>
#include <mfapi.h>
#include <mfidl.h>
#include <d2d1_3.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <xaudio2.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <dwrite_3.h>
#include <d3d11on12.h>
#include <d3dcompiler.h>
#include <mfreadwrite.h>
#include <propvarutil.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include "DirectXTK12/Src/d3dx12.h"
#include "DirectXTK12/Inc/Mouse.h"
#include "DirectXTK12/Inc/Keyboard.h"
#include "DirectXTK12/Inc/CommonStates.h"
#include "DirectXTK12/Inc/BufferHelpers.h"
#include "DirectXTK12/Inc/DDSTextureLoader.h"
#include "DirectXTK12/Inc/GeometricPrimitive.h"
#include "DirectXTK12/Inc/ResourceUploadBatch.h"
#include "TinyXML/tinyxml.h"
#include "SQLite/sqlite3.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "source/DirectXTK12/DirectXTK12.lib")
#pragma comment(lib, "source/TinyXML/tinyxml.lib")
#pragma comment(lib, "source/SQLite/sqlite3.lib")

#undef max
#undef min
#undef LPTR
#undef LHND
#undef NULL
#undef TRUE
#undef FALSE


//************************************************//


#define self (*this)
#define open public:
#define hide private:
#define prot protected:
#define iidof _uuidof
#define clsidof __uuidof
#define defcall _cdecl
#define stdcall _stdcall
#define veccall _vectorcall
#define fastcall _fastcall
#define aligndecl(size) _declspec(align(size))
#define allocdecl(seg) _declspec(allocate(seg))

#include "ge_define.h"
#include "ge_vector.h"
#include "ge_util.h"
#include "ge_base.h"
#include "ge_helper.h"
#include "ge_gui.h"
#include "ge_gfx.h"
#include "ge_asset.h"
#include "ge_render.h"

/* 命名格式 *
USERIDENT   类型
UserIdent   变量,函数
USER_IDENT  常量,宏
user_ident  命名空间,标签
CUserIdent  类
IUserIdent  接口
DoUserIdent 虚函数
OnUserIdent 回调函数
**/

/* 变量前缀 *
  # 系统型前缀
v  | VOID | void;空类型
b  | BOOL,bool | boolean;布尔值
c  | CHAR,WCHR | character;字符
d  | SPFP,DPFP | decimal;浮点数
n  | CINT,SINT,INT4,LINT | integer;整数
n  | UCHR,USRT,UINT,ULNG | unsigned integer;无符号整数
f  | BYTE,WORD,DWRD,QWRD | flag;标记
e  | enum | enumeration;枚举
un | union | union;共用体
t  | struct | structure;结构体
i  | class | instance;对象实例
var| typedef | variate;变量
arr| [] | array;数组
p  | * | pointer;指针
h  | HANDLE | handle;句柄
lp | IUnknown* | long pointer;长指针
fn | (*Func)() | function;函数指针
sz | CHAR[],WCHR[] | zero ended string;字符数组
s  | String | string | 字符串
it | Iterator | iterator;迭代器
cr | ARGB | color reference;色彩参考
tm | TEXTMETRIC | text metric;文本度量
pt | POINT | point;坐标点
rc | RECT | rectangle;矩形
vec| VECTOR | vector;向量
mat| MATRIX | matrix;矩阵
g | :: | global;全局变量
k | const | constant;常变量

  # 应用型前缀
cb  | count of bytes;字节数
num | number;数量
ix  | index;索引
rw  | row;行号
cl  | column;列号
**/