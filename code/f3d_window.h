// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_WINDOW_H)

global_variable window GlobalWindowArea;
global_variable HDC GlobalDeviceContext;
global_variable HWND GlobalWindow;

global_variable b32 GlobalKeyDown[0xFE] = {0};
global_variable b32 GlobalKeyUp[0xFE] = {0};
global_variable b32 GlobalKeyPress[0xFE] = {0};

global_variable s32 GlobalMouseX = 0;
global_variable s32 GlobalMouseY = 0;

extern b32 Running;

LRESULT
WndProc(HWND Window,
        UINT Message,
        WPARAM WParam,
        LPARAM LParam)
{
    if(GUIProcessFrame(Window, Message, WParam, LParam))
    {
        return(0);
    }
    
    switch(Message)
    {
        case WM_ACTIVATEAPP:
        case WM_SIZE:
        {
        }break;
        
        case WM_CLOSE:
        case WM_DESTROY:
        {
            Running = 0;
        }break;
        
        case WM_KEYDOWN:
        {
            GlobalKeyDown[WParam] = 1;
            GlobalKeyPress[WParam] = 1;
            GlobalKeyUp[WParam] = 0;
        }break;
        
        case WM_KEYUP:
        {
            GlobalKeyDown[WParam] = 0;
            GlobalKeyUp[WParam] = 1;
        }break;
    }
    return(DefWindowProc(Window, Message, WParam, LParam));
}

internal void
WindowInitialize(HINSTANCE Instance)
{
    WindowCreateClass(Instance, "Handmade FTW", &WndProc);
    
    window_dim PosDim = {0};
    window_dim ResDim = {1280, 800};
    
    WindowCreateWindowed("Handmade FTW", "Fusion3D", Instance, 0, 0, ResDim, PosDim, CW_USEDEFAULT, &GlobalWindow);
    
    window_dim Dim = WindowGetClientRect(GlobalWindow);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, GlobalWindowArea, 1);
    
    GlobalWindowArea = ResizeResult;
    
    GlobalDeviceContext = GetDC(GlobalWindow);
    b32 ModernContext = 1;
    Win32InitOpenGL(GlobalDeviceContext, &ModernContext);
    GUIInitialize(GlobalDeviceContext, GlobalWindow);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    
    wglSwapIntervalEXT(0);
}

internal void
WindowSetMousePos(s32 x, s32 y)
{
    POINT MousePos = {x,y};
    ClientToScreen(GlobalWindow, &MousePos);
    
    SetCursorPos(MousePos.x, MousePos.y);
    
    POINT MousePos2 = {0};
    GetCursorPos(&MousePos2);
    ScreenToClient(GlobalWindow, &MousePos2);
    
    GlobalMouseX = MousePos2.x;
    GlobalMouseY = MousePos2.y;
}

internal void
MainWindowUpdate(void)
{
    wglMakeCurrent(GlobalDeviceContext, OpenGLRC);
    
    for(s32 Idx = 0;
        Idx < 0xFE;
        ++Idx)
    {
        GlobalKeyPress[Idx] = 0;
    }
    
    POINT MousePos = {0};
    GetCursorPos(&MousePos);
    ScreenToClient(GlobalWindow, &MousePos);
    
    GlobalMouseX = MousePos.x;
    GlobalMouseY = MousePos.y;
    
    WindowUpdate();
    window_dim Dim = {0};
    s32 GlobalWindowWidth = GlobalWindowArea.Width;
    s32 GlobalWindowHeight = GlobalWindowArea.Height;
    
    Dim = WindowGetClientRect(GlobalWindow);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, GlobalWindowArea, 1);
    glViewport(0, 0, GlobalWindowArea.Width, GlobalWindowArea.Height);
    GlobalWindowArea = ResizeResult;
}

internal void
WindowShutdown(void)
{
    ReleaseDC(GlobalWindow, GlobalDeviceContext);
}

#define F3D_WINDOW_H
#endif