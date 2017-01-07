// (c) 2016 ZaKlaus; All Rights Reserved

#define HANDMADE_SLOW

#include "hftw.h"
                 
global_variable b32 Running = 1;
                 
#include "f3d_asset.h"
#include "f3d_window.h"
#include "f3d_squirrel.h"
                 
int CALLBACK     
_WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int CmdShow)
{                
    F3DWindowInitialize(Instance);
    F3DAssetInitialize("test");
    
    F3DSQRegister("index");
    F3DSQLoadScript("index");
                 
    TimeInit();  
                 
    r64 OldTime = TimeGet();
                 
    while(Running)
    {             
        r64 NewTime = TimeGet();
        r64 DeltaTime = NewTime - OldTime;
        {        
            F3DWindowUpdate();
            {    
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
                SwapBuffers(GlobalDeviceContext);
                
            Sleep(10);
        }
    }
    F3DWindowShutdown();
    
    return(0);
}

FILE __iob[3];
FILE * __cdecl __iob_func(void) { return __iob; }

int
main(void)
{
    __iob[0] = *stdin;
    __iob[1] = *stdout;
    __iob[2] = *stderr;
    
    LRESULT Result = _WinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
    
    return(Result);
}