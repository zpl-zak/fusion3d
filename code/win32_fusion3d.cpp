// (c) 2016 ZaKlaus; All Rights Reserved
                                               
#define HANDMADE_SLOW
                                               
#include "f3d_includes.h"

global_variable b32 Running = 1;

#define CITY_STUFF 0

// NOTE(ZaKlaus): This is an example on how to manipulate our player's camera.
// to provide a simple free fly mechanism.
void DEBUGHandleInput(camera *Camera, r32 DeltaTime)
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
   
   if(GlobalKeyPress[VK_F5])
   {
       AssetSyncPack();
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
   
   s32 Filtering = GL_LINEAR;
   
   if(GlobalKeyPress[VK_F6])
   {
       Filtering = GL_LINEAR;
   }
   
   if(GlobalKeyPress[VK_F7])
   {
       Filtering = GL_NEAREST;
   }
   
   if(GlobalKeyPress[VK_F6] || GlobalKeyPress[VK_F7])
   {
       for(s32 Idx = 0;
           Idx < F3D_TEXTURE_MAX;
           ++Idx)
       {
           render_texture *Texture = GlobalTextures + Idx;
           
           if(Texture->TextureObject)
           {
               glBindTexture(GL_TEXTURE_2D, Texture->TextureObject);
               
               TextureModify(Filtering);
           }
       }
       
       glBindTexture(GL_TEXTURE_2D, 0);
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

global_variable s32 AmbientProgramID = -1;
global_variable asset_file *VShader = 0;
global_variable asset_file *FShader = 0;
global_variable GLuint VShaderID = 0;
global_variable GLuint FShaderID = 0;
global_variable b32 ProgramGotReloaded = 0;

// NOTE(ZaKlaus): This is an example of how hot-reloading works in
// the current architecture. Assets get reloaded and optionally processed
// by an assigned callback, which in this case, reloads/re-creates 
// a shader program and gives our game a new one, or
// discards any changes that would result to failure, such as faulty shaders
// broken by syntax error. In such case, game will rollback to 
// its last shader program.
internal void
   DEBUGReloadShaderSource(asset_file *Asset)
{
   local_persist b32 VChanged = 0;
   local_persist b32 FChanged = 0;
   
   if(StringsAreEqual(Asset->Name, "ambient_vs"))
   {
       VChanged = 1;
   }
   
   if(StringsAreEqual(Asset->Name, "ambient_fs"))
   {
       FChanged = 1;
   }
   
   if(VChanged || FChanged)
   {
       VChanged = FChanged = 0;
       
       GLuint NewAmbientProgramID = ShaderProgramInit();
       {
           
           GLuint s1 = 0;
           b32 f1 = ShaderLink(NewAmbientProgramID, VShader, GL_VERTEX_SHADER, &s1);
           GLuint s2 = 0;
           b32 f2 = ShaderLink(NewAmbientProgramID, FShader, GL_FRAGMENT_SHADER, &s2);
           
           if(f1 && f2)
           {
               ShaderUnload(AmbientProgramID, VShaderID);
               ShaderUnload(AmbientProgramID, FShaderID);
               glDeleteProgram(ShaderProgramGet(AmbientProgramID));
               VShaderID = s1;
               FShaderID = s2;
               AmbientProgramID = NewAmbientProgramID;
               ShaderProgramLink(NewAmbientProgramID);
           }
           else
           {
               glDeleteProgram(ShaderProgramGet(NewAmbientProgramID));
               return;
           }
       }
       
       ProgramGotReloaded = 1;
       printf("Shader program reloaded!\n");
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
   
   VShader = ShaderLoad("ambient_vs", "ambient.vert");
   AssetAssignReloadCallbackInternal(VShader, DEBUGReloadShaderSource);
   FShader = ShaderLoad("ambient_fs", "ambient.frag");
   AssetAssignReloadCallbackInternal(FShader, DEBUGReloadShaderSource);
   
   AmbientProgramID = ShaderProgramInit();
   {
       GLuint s1 = 0;
       b32 f1 = ShaderLink(AmbientProgramID, VShader, GL_VERTEX_SHADER, &s1);
       GLuint s2 = 0;
       b32 f2 = ShaderLink(AmbientProgramID, FShader, GL_FRAGMENT_SHADER, &s2);
       
       Assert(f1 && f2);
       
       VShaderID = s1;
       FShaderID = s2;
   }
   ShaderProgramLink(AmbientProgramID);
   
   
   render_4ds *BalikSena = 0;
   render_4ds *Mesto = 0;
   
#if CITY_STUFF
   scene *CityScene = SceneRegister("city", "freeride");
   SceneLoad(CityScene);
   
   for(mi Idx = 0;
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
       //RenderOctreeAdd(CityScene->Renders[Idx], Transform);
   }

   
   //RenderOctreeGenerate();
   
   
   BalikSena = Model4DSRegister("krajina", "..\\missions\\freekrajina\\scene.4ds");
   Model4DSLoad(BalikSena);
   
   Mesto = Model4DSRegister("mesto", "..\\missions\\freeride\\scene.4ds");
   Model4DSLoad(Mesto);
#endif   
   
   r32 changex = 1.f;
   
   render_light_dir Sun = {};
   Sun.Ambient = {0.};
   Sun.Diffuse = {188/255.f, 152/255.f, 45/255.f};//{0.98,0.76,0.23};//{.1,.1,.1};
   Sun.Dir = {-.12+changex,-1.,-.3};
   
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
   
   GLuint AmbientProgram = ShaderProgramGet(AmbientProgramID);
   
   RenderApplyLightDirectional(&Sun, AmbientProgram);
   
   r64 LastStatsTime = 0;
   
   MainCamera.AmbColor = {212.f/255.f, 232.f/255.f, 211.f/255.f};
   
   render_material CubeTest = {};
   CubeTest.Diffuse = {0.2f, 1.2f, 0.34f};
   CubeTest.Ambient = {0.2f, 1.2f, 0.34f};
   CubeTest.DoubleSided = 0;
   CubeTest.Fullbright = 1;
   CubeTest.Opacity = 0.; // Udava priehladnost, dajme 20% viditelnost
   
   while(Running) 
   {             
       r64 NewTime = TimeGet();
       r64 DeltaTime = NewTime - OldTime;
       
       if(NewTime - LastStatsTime > 5)
       {
           LastStatsTime = NewTime;
           AssetSyncPack();
           printf("Delta: %f (%f FPS)\n", (r32)DeltaTime * 1000.f, 1.f / (r32)DeltaTime);
       }
       
       AmbientProgram = ShaderProgramGet(AmbientProgramID);
       
       if(ProgramGotReloaded)
       {
           ProgramGotReloaded = 0;
           RenderApplyLightDirectional(&Sun, AmbientProgram);
       }
       
       // NOTE(zaklaus): Render pass
       {
           MainWindowUpdate();
           {
               DEBUGHandleInput(&MainCamera, (r32)DeltaTime);
           }
           CameraUpdate(&MainCamera, 
                        WindowGetClientRect(GlobalWindow),
                        75.f,
                        0.1f,
                        500.f);
           RenderApplyCamera(AmbientProgram, &MainCamera);
           {    
               glClearColor(MainCamera.AmbColor.x, MainCamera.AmbColor.y, MainCamera.AmbColor.z, 1.f);
               glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
               
               local_persist r32 offset = 0.f;
               
               CameraLight.Pos = MainCamera.Position;
               
               RenderApplyLightPoint(0, &CameraLight, AmbientProgram);
               
               #if CITY_STUFF
               offset = sinf((r32)NewTime);
               glm::vec3 BalikPos = glm::vec3(0, 0, 0);
                   
               local_persist render_transform Transform = RenderTransform();
               local_persist glm::mat4 TransformMatrix = RenderTransformMatrix(Transform);
               Transform.Pos = BalikPos;
               Model4DSRender(BalikSena, AmbientProgram, TransformMatrix, ModelRenderType_Normal, 0);
                   
               Model4DSRender(Mesto, AmbientProgram, TransformMatrix, ModelRenderType_Normal, 0);
                              
               RenderSingleCull(0, F3D_SINGLE_MAX);
               
               RenderSingleDraw(0, F3D_SINGLE_MAX, AmbientProgram, ModelRenderType_Normal);
               #endif
               //DEBUGRenderOctreeViz(&GlobalWorld, AmbientProgram, 1);
               
               // NOTE(ZaKlaus): Small cube test.
               {
                   constexpr s32 CUBE_SIZE = 4;
                   constexpr s32 CUBE_MULT = 4;
                   
                   for(int i = -CUBE_SIZE; i <= CUBE_SIZE; i++)
                   {
                       for(int j = -CUBE_SIZE; j <= CUBE_SIZE; j++)
                       {
                           for(int k = -CUBE_SIZE; k <= CUBE_SIZE; k++)
                           {
                               if(i == -CUBE_SIZE || i == CUBE_SIZE ||
                                  j == -CUBE_SIZE || j == CUBE_SIZE ||
                                  k == -CUBE_SIZE || k == CUBE_SIZE)
                               {
                                   render_transform TransformOrig = RenderTransform();
                                   TransformOrig.Pos = glm::vec3(i*CUBE_MULT, j*CUBE_MULT, k*CUBE_MULT);
                                   glm::mat4 Mat = RenderTransformMatrix(TransformOrig);
                                   PrimitiveCubeDraw(&CubeTest, AmbientProgram, Mat);
                               }
                           }
                       }
                   }
               }
               
               RenderDraw(RenderPass_Color);
               
               changex += sinf((r32)TimeGet())*30.f;
               RenderApplyLightDirectional(&Sun, AmbientProgram);
           }
           SwapBuffers(GlobalDeviceContext);
           
           OldTime = NewTime;
           
           AmbientProgram = ShaderProgramGet(AmbientProgramID);
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

// NOTE(zaklaus): Tell the OS to prefer dedicated video card.
DWORD NvOptimusEnablement = 0x00000001; // NVIDIA
int AmdPowerXpressRequestHighPerformance = 1; // ATI/AMD