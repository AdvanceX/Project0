//-------------------- ImGui命名空间 --------------------//

namespace imgui = ImGui;

//----------------------------------------//


//-------------------- ImGui数据类型 --------------------//

typedef ImVec2 IM1VEC2;
typedef ImVec4 IM1VEC4;
typedef ImFont IM1FONT;
typedef ImGuiIO      IMGUI1IO;
typedef ImGuiStyle   IMGUI1STYLE;
typedef ImGuiContext IMGUI1CONTEXT;

//----------------------------------------//


//-------------------- ImGui操作 --------------------//

#define ImguiWinInit ImGui_ImplWin32_Init
#define ImguiWinShutdown ImGui_ImplWin32_Shutdown
#define ImguiWinNewFrame ImGui_ImplWin32_NewFrame
#define ImguiWinMsgProc ImGui_ImplWin32_WndProcHandler

#define ImguiD3dInit ImGui_ImplDX11_Init
#define ImguiD3dShutdown ImGui_ImplDX11_Shutdown
#define ImguiD3dNewFrame ImGui_ImplDX11_NewFrame
#define ImguiD3dRendering ImGui_ImplDX11_RenderDrawData

//----------------------------------------//