                                                                   // (c) 2016 ZaKlaus; All Rights Reserved
                                                                   
#define HANDMADE_SLOW
                                                                   
#include "hftw.h"
                                                                   
#include "common.hpp"
#include "vec3.hpp"
#include "mat4x4.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"
#include "gtc/matrix_transform.hpp"
                                                                   #include "gtc/matrix_access.hpp"
#include "gtx/transform.hpp"
                                                                   #include "gtc/type_ptr.hpp"
                                                                   
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
                                                                   
                                                                   global_variable b32 Running = 1;
                                                                   
#include "f3d_camera.h"
#include "f3d_async.h"
#include "f3d_asset.h"
#include "f3d_window.h"
#include "f3d_texture.h"
                                                                   #include "f3d_render.h"
#include "f3d_shader.h"
#include "f3d_render_4ds.h"
#include "f3d_scene.h"
                                                                   
                                                                   void HandleInput(camera *Camera, r32 DeltaTime)
                                                                   {
                                                                       local_persist s32 x,y;
                                                                       local_persist b32 FirstPos = 1;
                                                                       local_persist b32 UseMouse = 1;
                                                                       
                                                                       if(UseMouse)
                                                                       {
                                                                           if(FirstPos)
                                                                           {
                                                                               FirstPos = 0;
                                                                               x = GlobalMouseX;
                                                                               y = GlobalMouseY;
                                                                           }
                                                                           
                                                                           Camera->Angle.x +=  0.007f * 5.f * DeltaTime * (x - GlobalMouseX);
                                                                           Camera->Angle.y +=  0.007f * 5.f * DeltaTime * (y - GlobalMouseY);
                                                                           
                                                                           window_dim Res = WindowGetClientRect(GlobalWindow);
                                                                           WindowSetMousePos((s32)(Res.X / 2.f), (s32)(Res.Y / 2.f));
                                                                           
                                                                           x = GlobalMouseX;
                                                                           y = GlobalMouseY;
                                                                       }
                                                                       
                                                                       if(GlobalKeyPress[VK_CONTROL])
                                                                       {
                                                                           UseMouse = !UseMouse;
                                                                       }
                                                                       
                                                                       if(GlobalKeyDown[VK_ESCAPE])
                                                                       {
                                                                           Running = 0;
                                                                       }
                                                                       
                                                                       f32 Speed = 0.3f;
                                                                       
                                                                       if(GlobalKeyDown[VK_SHIFT])
                                                                       {
                                                                           Speed = 2.1f;
                                                                       }
                                                                       
                                                                       glm::vec3 Dir = glm::normalize(CameraGetDirection(Camera)) * Speed;
                                                                       glm::vec3 Right = glm::normalize(CameraGetRight(Camera)) *  Speed;
                                                                       
                                                                       if(GlobalKeyDown[0x57])
                                                                       {
                                                                           Camera->Position += Dir;
                                                                       }
                                                                       
                                                                       if(GlobalKeyDown[0x53])
                                                                       {
                                                                           Camera->Position -= Dir;
                                                                       }
                                                                       
                                                                       if(GlobalKeyDown[0x41])
                                                                       {
                                                                           Camera->Position -= Right;
                                                                       }
                                                                       
                                                                       if(GlobalKeyDown[0x44])
                                                                       {
                                                                           Camera->Position += Right;
                                                                       }
                                                                       
                                                                   }
                                                                   
                                                                   int CALLBACK     
                                                                       _WinMain(HINSTANCE Instance,
                                                                                HINSTANCE PrevInstance,
                                                                                LPSTR CmdLine,
                                                                                int CmdShow)
                                                                   {                
                                                                       WindowInitialize(Instance);
                                                                       AssetInitialize("D:\\Games\\Mafia");
                                                                       
                                                                       asset_file *VShader = ShaderLoad("ambient_vs", "ambient.vert");
                                                                       asset_file *FShader = ShaderLoad("ambient_fs", "ambient.frag");
                                                                       
                                                                       GLuint AmbientProgram = ShaderProgramInit();
                                                                       {
                                                                           ShaderLink(AmbientProgram, VShader, GL_VERTEX_SHADER);
                                                                           ShaderLink(AmbientProgram, FShader, GL_FRAGMENT_SHADER);
                                                                       }
                                                                       ShaderProgramLink(AmbientProgram);
                                                                       
                                                                       asset_file *CVShader = ShaderLoad("color_vs", "color.vert");
                                                                       asset_file *CFShader = ShaderLoad("color_fs", "color.frag");
                                                                       
                                                                       GLuint ColorProgram = ShaderProgramInit();
                                                                       {
                                                                           ShaderLink(ColorProgram, CVShader, GL_VERTEX_SHADER);
                                                                           ShaderLink(ColorProgram, CFShader, GL_FRAGMENT_SHADER);
                                                                       }
                                                                       ShaderProgramLink(ColorProgram);
                                                                       
                                                                       
                                                                       
#if 1
                                                                       scene *CityScene = SceneRegister("city", "freeride");
                                                                       SceneLoad(CityScene);
                                                                       
                                                                       for(s32 Idx = 0;
                                                                           Idx < CityScene->RenderCount;
                                                                           ++Idx)
                                                                       {
                                                                           scene_instance *Instance = *(CityScene->Instances + Idx);
                                                                           render_transform Transform = RenderTransform();
                                                                           {
                                                                               Transform.Pos = Instance->Pos;
                                                                               Transform.Rot = Instance->Rot;
                                                                               Transform.Scale = Instance->Scale;
                                                                           }
                                                                           //Model4DSRender(CityScene->Renders[Idx], AmbientProgram, &MainCamera, Transform, ModelRenderType_Normal, 1);
                                                                           
                                                                           RenderSingleAdd(CityScene->Renders[Idx], Transform, 1);
                                                                           RenderOctreeAdd(CityScene->Renders[Idx], Transform);
                                                                       }
#endif
                                                                       
                                                                       RenderOctreeGenerate();
                                                                       
                                                                       render_4ds *BalikSena = Model4DSRegister("krajina", "..\\missions\\freekrajina\\scene.4ds");
                                                                       Model4DSLoad(BalikSena);
                                                                       
                                                                       render_4ds *Mesto = Model4DSRegister("mesto", "..\\missions\\freeride\\scene.4ds");
                                                                       Model4DSLoad(Mesto);
                                                                       
                                                                       render_light_dir Sun = {};
                                                                       Sun.Ambient = {0.34, 0.43, .54};
                                                                       Sun.Diffuse = {0.98,0.76,0.23};//{.1,.1,.1};
                                                                       Sun.Dir = {-.12,-1.,-.3};
                                                                       
                                                                       camera MainCamera;
                                                                       MainCamera.Position.x = 0;
                                                                       MainCamera.Position.y = 0;
                                                                       MainCamera.Position.z =-6;
                                                                       
                                                                       render_light_point CameraLight = {};
                                                                       
                                                                       CameraLight.Ambient = {.3,.3,.3};
                                                                       CameraLight.Diffuse = {.6,.6,.6};
                                                                       CameraLight.Constant = 1.f;
                                                                       CameraLight.Linear = .01f;
                                                                       CameraLight.Quadratic = .045f;
                                                                       
                                                                       TimeInit();  
                                                                       r64 OldTime = TimeGet();
                                                                       
                                                                       RenderApplyLightDirectional(&Sun, AmbientProgram);
                                                                       
                                                                       while(Running) 
                                                                       {             
                                                                           r64 NewTime = TimeGet();
                                                                           r64 DeltaTime = NewTime - OldTime;
                                                                           printf("Delta: %f (%f FPS)\n", (r32)DeltaTime * 1000.f, 1.f / (r32)DeltaTime);
                                                                           {        
                                                                               MainWindowUpdate();
                                                                               {
                                                                                   HandleInput(&MainCamera, (r32)DeltaTime);
                                                                               }
                                                                               CameraUpdate(&MainCamera, 
                                                                                            WindowGetClientRect(GlobalWindow),
                                                                                            75.f,
                                                                                            0.1f,
                                                                                            800.f);
                                                                               {    
                                                                                   glClearColor(Sun.Ambient.x, Sun.Ambient.y, Sun.Ambient.z, 0.f);
                                                                                   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                                                                                   
                                                                                   local_persist r32 offset = 0.f;
                                                                                   
                                                                                   CameraLight.Pos = MainCamera.Position;
                                                                                   
                                                                                   RenderApplyLightPoint(0, &CameraLight, AmbientProgram);
                                                                                   
                                                                                   for(s32 Idx = 0;
                                                                                       Idx < 1;
                                                                                       ++Idx)
                                                                                   {
                                                                                       offset = sinf((r32)NewTime);
                                                                                       glm::vec3 BalikPos = glm::vec3(0, 0, 0);
                                                                                       
                                                                                       render_transform Transform = RenderTransform();
                                                                                       Transform.Pos = BalikPos;
                                                                                       Model4DSRender(BalikSena, AmbientProgram, &MainCamera, Transform, ModelRenderType_Normal, 0);
                                                                                       
                                                                                       Model4DSRender(Mesto, AmbientProgram, &MainCamera, Transform, ModelRenderType_Normal, 0);
                                                                                   }
                                                                                   RenderOctreeDraw(AmbientProgram, &MainCamera, ModelRenderType_Normal);
                                                                                   //DEBUGRenderOctreeViz(&GlobalWorld, AmbientProgram, &MainCamera);
                                                                               }
                                                                               SwapBuffers(GlobalDeviceContext);
                                                                               
                                                                               OldTime = NewTime;
                                                                           }
                                                                       }
                                                                       WindowShutdown();
                                                                       
                                                                       return(0);
                                                                   }
                                                                   
                                                                   int
                                                                       main(void)
                                                                   {
                                                                       LRESULT Result = _WinMain(GetModuleHandle(0), 0, GetCommandLine(), SW_SHOW);
                                                                       
                                                                       return((int)Result);
}