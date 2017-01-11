                                                                   // (c) 2016 ZaKlaus; All Rights Reserved

#define HANDMADE_SLOW

#include "hftw.h"

global_variable b32 Running = 1;
                 
#include "f3d_async.h"
#include "f3d_asset.h"
#include "f3d_window.h"
                                                                   #include "f3d_shader.h"
                                                                   #include "f3d_render_4ds.h"

int CALLBACK     
_WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CmdLine,
        int CmdShow)
{                
    F3DWindowInitialize(Instance);
    F3DAssetInitialize("test");
    
    asset_file *VShader = F3DShaderLoad("color_vs", "color.vert");
    asset_file *FShader = F3DShaderLoad("color_fs", "color.frag");
    
    GLuint ColorProgram = F3DShaderProgramInit();
    {
        F3DShaderLink(ColorProgram, VShader, GL_VERTEX_SHADER);
F3DShaderLink(ColorProgram, FShader, GL_FRAGMENT_SHADER);
    }
    F3DShaderProgramLink(ColorProgram);
    
    render_4ds *BalikSena = F3DModel4DSRegister("baliksena", "baliksena.4ds");
    F3DModel4DSLoad(BalikSena);
    
    TimeInit();  
    r64 OldTime = TimeGet();
    
    m4 Projection = MathPerspective(45.f, 4.f / 3.f, .1f, 100.f);
        
            v3 Pos = {4, 3, 7};
        v3 Target = {0};
        v3 Up = {0, 1, 0};
        
            m4 View = MathLookAt(Pos, Target, Up);
        m4 Model = MathMat4d(1.f);
        
            mat4 MVP = MathMultiplyMat4(Projection, View);   
        MVP = MathMultiplyMat4(MVP, Model);
        
            GLuint MatrixID = glGetUniformLocation(ColorProgram, "mvp");
        
         const GLfloat g_vertex_buffer_data[] = {
                    -1.0f, -1.0f, 0.0f,
                    1.0f, -1.0f, 0.0f,
                    0.0f,  1.0f, 0.0f,
                };
                
                GLuint vertexbuffer;
                    
                        glGenBuffers(1, &vertexbuffer);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glUseProgram(ColorProgram);
                 
    while(Running) 
    {             
        r64 NewTime = TimeGet();
        r64 DeltaTime = NewTime - OldTime;
        {        
            F3DWindowUpdate();
            {    
                glClearColor(0.44, 0.44, 0.56, 0.f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                  
                glUseProgram(ColorProgram);
                F3DModel4DSRender(BalikSena, ColorProgram, ModelRenderType_Normal);
                
                #if 0
                {
                    local_persist b32 MainWindowVisible = 1;
                    GUIBeginWindow("Handmade FTW", MathVec2(20, 20), MathVec2(220, 480), MathVec3(0.12, 0.12, 0.12), &MainWindowVisible);
                    {
                        
                    }
                    GUIEndWindow();
                }
                GUIDrawFrame();
                #endif
                }
                SwapBuffers(GlobalDeviceContext);
                
            Sleep(10);
        }
    }
    F3DWindowShutdown();
    
    return(0);
}

int
main(void)
{
    LRESULT Result = _WinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
    
    return((int)Result);
}