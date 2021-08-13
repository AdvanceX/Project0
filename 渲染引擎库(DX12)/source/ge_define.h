//--------------------基础类型--------------------//

typedef int BOOL; //! Boolean
typedef char    CHAR; //! Character
typedef wchar_t WCHR; //! Wide Character
typedef signed __int8  CINT; //! Character Integer
typedef signed __int16 SINT; //! Short Integer
typedef signed __int32 INT4; //! 4B Integer
typedef signed __int64 LINT; //! Long Integer
typedef unsigned __int8  UCHR; //! Unsigned Character(Integer)
typedef unsigned __int16 USRT; //! Unsigned Short(Integer)
typedef unsigned __int32 UINT; //! Unsigned Integer
typedef unsigned __int64 ULNG; //! Unsigned Long(Integer)
typedef unsigned char      BYTE; //! Byte
typedef unsigned short     WORD; //! Word
typedef unsigned long      DWRD; //! Double Word
typedef unsigned long long QWRD; //! Quadruple Word
typedef float  SPFP; //! Single-Precision Floating-Point(Number)
typedef double DPFP; //! Double-Precision Floating-Point(Number)

typedef TCHAR TCHR; //! (Generic)Text Character
typedef INT_PTR   IPTR; //! Integer Pointer
typedef UINT_PTR  UPTR; //! Unsigned(Integer) Pointer
typedef LONG_PTR  LPTR; //! Long(Integer) Pointer
typedef ULONG_PTR DPTR; //! Double(Word) Pointer

typedef __m64   M08X; //! 8B Multiple Extension
typedef __m128  M16X; //! 16B Multiple Extension
typedef __m128i M16I; //! 16B Multiple Integer
typedef __m128d M16D; //! 16B Multiple DPFP
typedef __m256  M32X; //! 32B Multiple Extension
typedef __m256i M32I; //! 32B Multiple Integer
typedef __m256d M32D; //! 32B Multiple DPFP

//----------------------------------------//


//--------------------系统类型--------------------//

typedef COLORREF ARGB;
typedef SYSTEMTIME SYSTIME;
typedef LARGE_INTEGER BIGINT;
typedef WIN32_FIND_DATA FILEINFO;
typedef CRITICAL_SECTION CRITSECTION;

//----------------------------------------//


//--------------------媒体类型--------------------//

typedef DirectX::Mouse DXTKMouse;
typedef DirectX::Keyboard DXTKKeyboard;
typedef DirectX::Mouse::State DXTKMouseState;
typedef DirectX::Keyboard::State DXTKKeysState;
typedef DirectX::GeometricPrimitive DXTKGeomPrim;
typedef DirectX::ResourceUploadBatch DXTKUploader;
typedef XAUDIO2_BUFFER XABuffer;
typedef DWRITE_TEXT_METRICS DWTxtMetrics;
typedef DXGI_FORMAT          DXGIFormat;
typedef DXGI_MODE_DESC       DXGIModeDesc;
typedef DXGI_SAMPLE_DESC     DXGISampleDesc;
typedef DXGI_OUTPUT_DESC     DXGIOutputDesc;
typedef DXGI_ADAPTER_DESC1   DXGIAdapterDesc;
typedef DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc;
typedef D2D_SIZE_F              D2DSizeF;
typedef D2D_SIZE_U              D2DSizeU;
typedef D2D_RECT_F              D2DRectF;
typedef D2D_RECT_U              D2DRectU;
typedef D2D_POINT_2F            D2DPointF;
typedef D2D_POINT_2U            D2DPointU;
typedef D2D1_MAPPED_RECT        D2DMappedMat;
typedef D2D1_BITMAP_PROPERTIES1 D2DBmpProps;
typedef D3D_SHADER_MACRO       D3DShaderMacro;
typedef D3D_FEATURE_LEVEL      D3DFeatureLevel;
typedef D3D_PRIMITIVE_TOPOLOGY D3DPrimTopo;
typedef D3D11_RESOURCE_FLAGS D3D11ResFlags;
typedef D3D12_GPU_VIRTUAL_ADDRESS HGPUBUFF;
typedef D3D12_RECT                 D3DRect;
typedef D3D12_BLEND                D3DBlendMode;
typedef D3D12_BLEND_OP             D3DBlendOP;
typedef D3D12_LOGIC_OP             D3DLogicOP;
typedef D3D12_STENCIL_OP           D3DStencilOP;
typedef D3D12_RESOURCE_FLAGS       D3DResFlag;
typedef D3D12_COMPARISON_FUNC      D3DComparisonFunc;
typedef D3D12_TEXTURE_ADDRESS_MODE D3DTexAddrMode;
typedef D3D12_INDEX_BUFFER_VIEW            D3DIdxBuffView;
typedef D3D12_VERTEX_BUFFER_VIEW           D3DVertBuffView;
typedef D3D12_DEPTH_STENCIL_VIEW_DESC      D3DDsvDesc;
typedef D3D12_RENDER_TARGET_VIEW_DESC      D3DRtvDesc;
typedef D3D12_CONSTANT_BUFFER_VIEW_DESC    D3DCbvDesc;
typedef D3D12_SHADER_RESOURCE_VIEW_DESC    D3DSrvDesc;
typedef D3D12_UNORDERED_ACCESS_VIEW_DESC   D3DUavDesc;
typedef D3D12_INPUT_LAYOUT_DESC            D3DInputLayoutDesc;
typedef D3D12_INPUT_ELEMENT_DESC           D3DInputElemDesc;
typedef D3D12_SAMPLER_DESC                 D3DSamplerDesc;
typedef D3D12_COMMAND_QUEUE_DESC           D3DCmdQueueDesc;
typedef D3D12_DESCRIPTOR_HEAP_DESC         D3DViewHeapDesc;
typedef D3D12_RENDER_TARGET_BLEND_DESC     D3DBuffBlendDesc;
typedef D3D12_COMPUTE_PIPELINE_STATE_DESC  D3DComputePipeStateDesc;
typedef D3D12_GRAPHICS_PIPELINE_STATE_DESC D3DGfxPipeStateDesc;
typedef D3D12_SUBRESOURCE_DATA             D3DResData;
typedef D3D12_PLACED_SUBRESOURCE_FOOTPRINT D3DPlacedResLayout;
typedef D3D12_FEATURE_DATA_FEATURE_LEVELS             D3DLevelFeature;
typedef D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS D3DSampleFeature;
typedef CD3DX12_RANGE                 D3DRange;
typedef CD3DX12_VIEWPORT              D3DViewport;
typedef CD3DX12_CLEAR_VALUE           D3DFillValue;
typedef CD3DX12_SHADER_BYTECODE       D3DShaderBytecode;
typedef CD3DX12_HEAP_PROPERTIES       D3DHeapProps;
typedef CD3DX12_DESCRIPTOR_RANGE      D3DViewRange;
typedef CD3DX12_RESOURCE_BARRIER      D3DResBarrier;
typedef CD3DX12_SUBRESOURCE_FOOTPRINT D3DResLayout;
typedef CD3DX12_TEXTURE_COPY_LOCATION D3DTexCopyLocation;
typedef CD3DX12_BLEND_DESC            D3DBlendDesc;
typedef CD3DX12_RESOURCE_DESC         D3DResDesc;
typedef CD3DX12_RASTERIZER_DESC       D3DRasterizerDesc;
typedef CD3DX12_DEPTH_STENCIL_DESC    D3DDepthStencilDesc;
typedef CD3DX12_STATIC_SAMPLER_DESC   D3DStaticSamplerDesc;
typedef CD3DX12_ROOT_PARAMETER        D3DRootParam;
typedef CD3DX12_ROOT_CONSTANTS        D3DRootConst;
typedef CD3DX12_ROOT_DESCRIPTOR       D3DRootView;
typedef CD3DX12_ROOT_SIGNATURE_DESC   D3DRootSigDesc;
typedef CD3DX12_ROOT_DESCRIPTOR_TABLE D3DRootViewTable;
typedef CD3DX12_CPU_DESCRIPTOR_HANDLE HD3DVIEW;
typedef CD3DX12_GPU_DESCRIPTOR_HANDLE HGPUVIEW;
typedef IDXGIFactory4   IDXGIFactory4;
typedef IDXGIAdapter4   IDXGIAdapter4;
typedef IDXGISwapChain4 IDXGISwapChain4;
typedef ID3D12Fence               ID3DFence;
typedef ID3D12Device4             ID3DDevice;
typedef ID3D12Resource            ID3DResource;
typedef ID3D12RootSignature       ID3DRootSig;
typedef ID3D12PipelineState       ID3DPipeState;
typedef ID3D12DescriptorHeap      ID3DViewHeap;
typedef ID3D12CommandList         ID3DCmdList;
typedef ID3D12CommandQueue        ID3DCmdQueue;
typedef ID3D12CommandAllocator    ID3DCmdAlloc;
typedef ID3D12GraphicsCommandList ID3DGfxCmdList;
typedef ID3D11On12Device    ID3D11On12Dev;
typedef ID3D11DeviceContext ID3D11DevCtx;
typedef ID2D1Bitmap1             ID2DBitmap;
typedef ID2D1Device3             ID2DDevice;
typedef ID2D1Factory4            ID2DFactory;
typedef ID2D1RenderTarget        ID2DRenderTarget;
typedef ID2D1DeviceContext       ID2DDevCtx;
typedef ID2D1Brush               ID2DBrush;
typedef ID2D1ImageBrush          ID2DImgBrush;
typedef ID2D1BitmapBrush1        ID2DBmpBrush;
typedef ID2D1SolidColorBrush     ID2DSolidColorBrush;
typedef ID2D1LinearGradientBrush ID2DLinearGradientBrush;
typedef ID2D1RadialGradientBrush ID2DRadialGradientBrush;
typedef IDWriteFactory    IDWFactory;
typedef IDWriteTextFormat IDWTxtFormat;
typedef IDWriteTextLayout IDWTxtLayout;
typedef IXAudio2               IXAFactory;
typedef IXAudio2SourceVoice    IXASrcVoice;
typedef IXAudio2MasteringVoice IXADestVoice;
typedef IMFSample               IMFMediaSample;
typedef IMFAttributes           IMFMediaAttr;
typedef IMFMediaType            IMFMediaType;
typedef IMFMediaEvent           IMFMediaEvent;
typedef IMFMediaBuffer          IMFMediaBuff;
typedef IMFSourceReader         IMFSourceReader;
typedef IMFSourceReaderCallback IMFReaderCallback;
typedef IWICBitmapDecoder     IWICBmpDecode;
typedef IWICImagingFactory    IWICFactory;
typedef IWICFormatConverter   IWICFormatConvert;
typedef IWICBitmapFrameDecode IWICBmpFrameDecode;

//----------------------------------------//


//--------------------其它类型--------------------//

typedef va_list VALIST;
typedef sqlite3 SQLite;
typedef std::string STLString;
typedef std::wstring STLStringW;

//----------------------------------------//


//--------------------模板类型--------------------//

template<typename Type>
using STLList = std::list<Type>;
template<typename Type>
using STLVector = std::vector<Type>;

template<typename Type>
using ComPtr = Microsoft::WRL::ComPtr<Type>;
template<typename Type>
using UniquePtr = std::unique_ptr<Type>;
template<typename Type>
using SharedPtr = std::shared_ptr<Type>;
template<typename Type>
using WeakPtr = std::weak_ptr<Type>;

//----------------------------------------//


//--------------------命名空间--------------------//

namespace d2d = D2D1;
namespace dx = DirectX;
namespace dx_pv = DirectX::PackedVector;
namespace dx_tri = DirectX::TriangleTests;

//----------------------------------------//


//--------------------无穷值--------------------//

#define FLT_INFINITE 0x7F800000
#define FLT_NEG_INF  0xFF800000

#define DBL_INFINITE 0x7FF0000000000000
#define DBL_NEG_INF  0xFFF0000000000000

//----------------------------------------//


//--------------------数学常量--------------------//

#define PI            3.141592654f  //! Pi
#define PI_MUL_2      6.283185307f  //! Pi*2
#define PI_DIV_BY_2   1.570796326f  //! Pi/2
#define PI_DIV_BY_4   0.785398163f  //! Pi/4
#define PI_DIV_180    57.295779513f //! 180/Pi
#define PI_DIV_BY_180 0.017453292f  //! Pi/180

//----------------------------------------//


//--------------------通用宏--------------------//

#define TXT __T

#define SUCC 0L
#define FAIL -1L

#define TRUE true
#define FALSE false
#define NULL nullptr

#define DW(var) *(DWRD*)&(var)
#define I4(var) *(INT4*)&(var)
#define SF(var) *(SPFP*)&(var)

#define OPEN_BIT(flag, key) (flag |= (key))
#define SHUT_BIT(flag, key) (flag &= ~(key))

#define RETURN_FAIL(Result) if(Result < 0) return FAIL
#define RETURN_ERROR(Result) if(Result < 0) return Result

#define DELETE_PTR(ptr) if(ptr) delete ptr
#define RESET_PTR(ptr) if(ptr){ delete ptr; ptr = NULL; }
#define DELETE_ARR(arr) if(arr) delete[] arr
#define RESET_ARR(arr) if(arr){ delete[] arr; arr = NULL; }
#define RELEASE_REF(ref) if(ref) ref->Release()
#define RESET_REF(ref) if(ref){ ref->Release(); ref = NULL; }

#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define COM_ARGS(obj) __uuidof(**obj), (VOID**)obj
#define FORCE_CVT(a, b) a = (decltype(a))b
#define NEAR_EQUAL(a, b, e) (fabsf((a) - (b)) < (e))

//----------------------------------------//


//--------------------数学宏--------------------//

#define RAD_TO_DEG(rad) ((rad) * PI_DIV_180)
#define DEG_TO_RAD(ang) ((ang) * PI_DIV_BY_180)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define XCHG(a, b){ a += b; b = a - b; a -= b; }
#define SWAP(a, b, t){ t = a; a = b; b = t; }

#define LERP(a, b, t) ((a) + ((b) - (a))*(t))
#define CLAMP(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))

//----------------------------------------//


//--------------------变参传递--------------------//

#define VA_START __crt_va_start
#define VA_NEXT __crt_va_arg
#define VA_END __crt_va_end

//----------------------------------------//


//--------------------内存分配--------------------//

#define MFree free
#define MAlloc malloc
#define MRealloc realloc
#define AMFree _aligned_free
#define AMAlloc _aligned_malloc
#define AMRealloc _aligned_realloc

//----------------------------------------//


//--------------------格式化读写--------------------//

#define SScan sscanf
#define SPrint sprintf
#define SNPrint snprintf
#define VSScan vsscanf
#define VSPrint vsprintf
#define VSNprint vsnprintf

#define WSScan swscanf
#define WSPrint swprintf
#define VWSScan vswscanf
#define VWSPrint vswprintf

#define FScan fscanf
#define FPrint fprintf
#define VFscan vfscanf
#define VFprint vfprintf

//----------------------------------------//


//--------------------数据库操作--------------------//

#define SQLiteOpen sqlite3_open
#define SQLiteExec sqlite3_exec
#define SQLiteFree sqlite3_free
#define SQLiteClose sqlite3_close
#define SQLiteErrMsg sqlite3_errmsg

//----------------------------------------//


//--------------------随机数生成--------------------//

#define Rand rand
#define SRand srand

//----------------------------------------//


//--------------------查找/排序--------------------//

#define QSort qsort
#define BSearch bsearch

//----------------------------------------//


//--------------------字符串转换--------------------//

#define StoI atoi
#define StoF atof
#define ItoS itoa
#define FtoS gcvt

//----------------------------------------//


//--------------------字符串操作--------------------//

#define StrNLen _tcsnlen
#define StrNSet _tcsnset
#define StrLen _tcslen
#define StrSet _tcsset
#define StrLwr _tcslwr
#define StrUpr _tcsupr
#define StrRev _tcsrev
#define StrTok _tcstok

#define StrNLenA strnlen
#define StrNSetA strnset
#define StrLenA strlen
#define StrSetA strset
#define StrLwrA strlwr
#define StrUprA strupr
#define StrRevA strrev
#define StrTokA strtok

#define StrNLenW wcsnlen
#define StrNSetW wcsnset
#define StrLenW wcslen
#define StrSetW wcsset
#define StrLwrW wcslwr
#define StrUprW wcsupr
#define StrRevW wcsrev
#define WcsTokW wcstok

//----------------------------------------//


//--------------------字符鉴别--------------------//

#define IsCntrl iscntrl   //!是否控制码(0到30)
#define IsSpace isspace   //!是否空字符(' ','\t','\r','\n','\v','\f')
#define IsPunct ispunct   //!是否标点/特殊符号
#define IsPrint isprint   //!是否可打印字符
#define IsGraph isgraph   //!是否图形字符(除空字符外的可打印字符)
#define IsAlNum isalnum   //!是否字母/数字
#define IsAlpha isalpha   //!是否字母
#define IsDigit isdigit   //!是否数字
#define IsLover islower   //!是否小写字母
#define IsUpper isupper   //!是否大写字母
#define IsXDigit isxdigit //!是否16进制数字

//----------------------------------------//


//--------------------浮点数鉴别--------------------//

#define IsNaN isnan       //!是否非数(阶码全1,尾数不全0)
#define IsInf isinf       //!是否无穷(阶码全1,尾数全0)
#define IsFinite isfinite //!是否有限数(不是Inf/NaN)
#define IsNormal isnormal //!是否规范数(不是Inf/NaN/0,也不是Subnormal(阶码全0))
#define SignBit signbit       //!获取符号位
#define FPClassify fpclassify //!判断浮点数类型

//----------------------------------------//


//--------------------数学函数--------------------//

#define Abs abs    //! |x|
#define FAbs fabsf //! |x|

#define Mod fmodf      //!求模,让余数绝对值最小
#define Rem remainderf //!求余,让商最小
#define RemQuo remquof //!z为商,返回余数

#define Sin sinf //! sin(x)
#define Cos cosf //! cos(x)
#define Tan tanf //! tan(x)

#define Asin asinf   //! asin(x)
#define Acos acosf   //! acos(x)
#define Atan atanf   //! atan(x)
#define Atan2 atan2f //! atan(y/x)

#define Sinh sinhf //!双曲正弦
#define Cosh coshf //!双曲余弦
#define Tanh tanhf //!双曲正切

#define Asinh asinhf //!反双曲正弦
#define Acosh acoshf //!反双曲余弦
#define Atanh atanhf //!反双曲正切

#define Pow powf   //! x^y
#define Exp expf   //! e^x
#define Exp2 exp2f //! 2^x

#define Log logf     //! log(e)x
#define Log2 log2f   //! log(2)x
#define Log10 log10f //! log(10)x

#define Sqrt sqrtf //! 2√x
#define Cbrt cbrtf //! 3√x

#define LGamma lgamma //! gamma(x)
#define TGamma tgamma //! ln(|gamma(x)|)

#define Erf erf   //!误差函数
#define Erfc erfc //!互补误差函数

#define ModF modf   //!整数部分给y,返回小数部分
#define Ceil ceil   //!向上取整
#define Floor floor //!向下取整
#define Round round //!四舍五入
#define Trunc trunc //!截断小数

#define Fma fma     //! x*y + z
#define FDim fdim   //! |x-y|
#define ExpM1 expm1 //! e^x - 1
#define Log1P log1p //! log(e)(1+x)
#define Hypot hypot //! √(x^2 + y^2)
#define LdExp ldexp //! x * 2^y
#define FrExp frexp //! x = (return) * 2^y

#define CopySign copysign //!返回取x绝对值和y的符号的数

//----------------------------------------//