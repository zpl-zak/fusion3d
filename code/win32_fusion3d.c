// (c) 2016 ZaKlaus; All Rights Reserved

#define HANDMADE_SLOW

#include "hftw.h"

#include "formats/hformat_4ds.h"
#include "formats/hformat_klz.h"
#include "formats/hformat_bmp.h"

#define COLOR_PURPLE 0x00ff00ff

global_variable b32 Running = 1;
global_variable window WindowArea;

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

int CALLBACK
_WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int CmdShow)
{
    WindowCreateClass(Instance, "Handmade FTW", &WndProc);
    
    window_dim PosDim = {0};
    window_dim ResDim = {800, 600};
    
    HWND Window;
    WindowCreateWindowed("Handmade FTW", "Win32 Test", Instance, 0, 0, ResDim, PosDim, CW_USEDEFAULT, &Window);
    
    window_dim Dim = WindowGetClientRect(Window);
    window_resize_result ResizeResult = WindowResize(Dim.X, Dim.Y, WindowArea, 1);
    
    WindowArea = ResizeResult;
    
    WindowUpdate();
    TimeInit();
    
    r64 OldTime = TimeGet();
    
    HDC DeviceContext = GetDC(Window);
    
    b32 ModernContext = 1;
    
    Win32InitOpenGL(DeviceContext, &ModernContext);
    
    GUIInitialize(DeviceContext, Window);
    
    while(Running)
    {             
        r64 NewTime = TimeGet();
        r64 DeltaTime = NewTime - OldTime;
        {
            WindowUpdate();
            {     
                s32 WindowWidth = WindowArea.Width;
                s32 WindowHeight = WindowArea.Height;
                  
                { 
                     Dim = WindowGetClientRect(Window);
                    ResizeResult = WindowResize(Dim.X, Dim.Y, WindowArea, 1);
                    glViewport(0, 0, WindowArea.Width, WindowArea.Height);
                    WindowArea = ResizeResult;
                } 
                  
                glClearColor(0.44, 0.44, 0.56, 0.f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                  {
                    local_persist b32 MainWindowVisible = 1;
                    GUIBeginWindow("Handmade FTW", MathVec2(20, 20), MathVec2(220, 480), MathVec3(0.12, 0.12, 0.12), &MainWindowVisible);
                    {
                        
                    }
                    GUIEndWindow();
                }
                GUIDrawFrame();
                }
                SwapBuffers(DeviceContext);
                
            Sleep(10);
        }
    }
    
    ReleaseDC(Window, DeviceContext);
    
    return(0);
}

int
main(void)
{
     LRESULT Result = _WinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
    
    return(Result);
}