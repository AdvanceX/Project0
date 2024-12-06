//******************** 外部依赖 ********************//

/// Win32
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
/// Bullet3
#define BT_USE_SSE_IN_API
#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include <d3d12.h>
#include <d2d1_3.h>
#include <dinput.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <xaudio2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <atlcomcli.h>
#include <d3d11on12.h>
#include <d3dcompiler.h>

/// Best Tiny C Collection
#include "../library/BTCC/cJSON.h"
#include "../library/BTCC/tinyxml2.h"
/// ImGui
#include "../library/ImGui/imgui.h"
#include "../library/ImGui/imgui_win32.h"
#include "../library/ImGui/imgui_dx11.h"
/// SQLite
#include "../library/SQLite3/sqlite3.h"
#include "../library/SQLite3/sqlite3ext.h"
/// Bullet3
#include "../library/Bullet3/btDynamicsComm.h"
#include "../library/Bullet3/btCollisionComm.h"
#include "../library/Bullet3/BtSoftBody/btSoftBody.h"
/// DirectXMath
#include "../library/DXMath/DirectXMath.h"
#include "../library/DXMath/DirectXCollision.h"
#include "../library/DXMath/DirectXPackedVector.h"
/// DirectXTK
#include "../library/DXTK12/d3dx12.h"
#include "../library/DXTK12/SimpleMath.h"
#include "../library/DXTK12/CommonStates.h"
#include "../library/DXTK12/WAVFileReader.h"
#include "../library/DXTK12/BufferHelpers.h"
#include "../library/DXTK12/LoaderHelpers.h"
#include "../library/DXTK12/DirectXHelpers.h"
#include "../library/DXTK12/DDSTextureLoader.h"
#include "../library/DXTK12/GeometricPrimitive.h"
#include "../library/DXTK12/ResourceUploadBatch.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")

#ifdef _WIN64
#pragma comment(lib, "library/BTCC/x64/cJSON.lib")
#pragma comment(lib, "library/BTCC/x64/tinyxml2.lib")
#pragma comment(lib, "library/ImGui/x64/imgui.lib")
#pragma comment(lib, "library/ImGui/x64/imgui_dx.lib")
#pragma comment(lib, "library/SQLite3/x64/sqlite3.lib")
#pragma comment(lib, "library/Bullet3/x64/Bullet3.lib")
#pragma comment(lib, "library/DXTK12/x64/DirectXTK12.lib")
#else
#pragma comment(lib, "library/BTCC/x86/cJSON.lib")
#pragma comment(lib, "library/BTCC/x86/tinyxml2.lib")
#pragma comment(lib, "library/ImGui/x86/imgui.lib")
#pragma comment(lib, "library/ImGui/x86/imgui_dx.lib")
#pragma comment(lib, "library/SQLite3/x86/sqlite3.lib")
#pragma comment(lib, "library/Bullet3/x86/Bullet3.lib")
#pragma comment(lib, "library/DXTK12/x86/DirectXTK12.lib")
#endif

//****************************************//


//******************** Game3D Engine ********************//

#pragma warning(disable:4200)
#pragma warning(disable:4996)

#include "ge-base.hpp"
#include "ge-util.hpp"
#include "ge-math.hpp"
#include "ge-cgres.hpp"
#include "ge-mmsys.hpp"
#include "ge-imgui.hpp"
#include "ge-sqlite.hpp"
#include "ge-physics.hpp"
#include "ge-context.hpp"
#include "ge-gpures.hpp"
#include "ge-render.hpp"
#include "ge-gui.hpp"

//****************************************//


/******************** 命名格式 ********************\

aaabbb            : 命名空间
AAABBB,XXAaaBbb   : 类型
AaaBbb,xxAaaBbb   : 变量,函数
AAA_BBB,AA_Bbbb   : 常量,算式宏
aaa_bbb           : 标签
_aaa_bbb,_AAA_BBB : 声明宏

\****************************************/


/******************** 变量名前缀 ********************\

v   : void(空类型)
b   : boolean(布尔值)
i   : index/ID(索引/标识)
s   : string(字符串/字节串)
h   : handle(句柄)
p   : pointer(指针)
lp  : long pointer(长指针)
hr  : handle of resource(资源句柄)
rg  : range(数组)
fn  : function(函数)
ap  : auto pointer(智能指针)
it  : iterator(迭代器)
rc  : rectangle(矩形)
cb  : count of bytes(字节数)
num : number(数量)
mat : matrix(矩阵)
g   : global(全局变量)
t   : static(静态变量)
k   : constant(常变量)
r   : reference(引用)

\****************************************/


/******************** 代码规范 ********************\

* 结构体型参数使用引用传递
* 结构体型返回值使用临时变量(RVO)
* 引用型参数(class 除外)考虑使用 const 修饰
* 引用型返回值(class 除外)考虑使用 const 修饰
* 显式默认构造函数使用 default 修饰
* 逻辑运算用括号括起
* 分支语句存在 return 时考虑省略 else、break
* 依场景使用 int 和 size_t
* 判断 bool、null、'\0'、compare() 时省略 ==、!=
* 使用 foreach 简化代码

\****************************************/