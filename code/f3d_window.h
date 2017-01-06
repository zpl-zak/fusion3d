// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_WINDOW_H)

global_variable window GlobalWindowArea;
global_variable HDC GlobalDeviceContext;
global_variable HWND GlobalWindow;

LRESULT CALLBACK
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
            if(WParam == VK_ESCAPE)
            {
                Running = 0;
            }
        }break;
    }
    return(DefWindowProc(Window, Message, WParam, LParam));
}

internal void
F3DWindowInitialize(HINSTANCE Instance)
{
    WindowCreateClass(Instance, "Handmade FTW", &WndProc);
    
    window_dim PosDim = {0};
    window_dim ResDim = {800, 600};
    
    WindowCreateWindowed("Handmade FTW", "Fusion3D", Instance, 0, 0, ResDim, PosDim, CW_USEDEFAULT, &GlobalWindow);
    
    window_dim Dim = WindowGetClientRect(GlobalWindow);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, GlobalWindowArea, 1);
    
    GlobalWindowArea = ResizeResult;
    
     GlobalDeviceContext = GetDC(GlobalWindow);
    b32 ModernContext = 1;
    Win32InitOpenGL(GlobalDeviceContext, &ModernContext);
    GUIInitialize(GlobalDeviceContext, GlobalWindow);
}

internal void
F3DWindowUpdate(void)
{
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
F3DWindowShutdown(void)
{
    ReleaseDC(GlobalWindow, GlobalDeviceContext);
}

#define F3D_WINDOW_H
#endif