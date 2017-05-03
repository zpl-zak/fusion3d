// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_H)

enum
{
    ModelRenderType_Normal,
};

enum
{
    VBO_Position,
    VBO_Normal,
    VBO_UV,
    VBO_Count,
};

typedef struct
{
    glm::vec3 Pos;
    glm::vec4 Rot;
    glm::vec3 Scale;
} render_transform;

typedef struct
{
    glm::vec4 Min,Max;
} aabb;

#include "f3d_frustum.h"

internal render_transform
RenderTransform(void)
{
    render_transform Result;
    Result.Scale.x = 1.f;
    Result.Scale.y = 1.f;
    Result.Scale.z = 1.f;
    
    return(Result);
}

internal glm::mat4
RenderTransformMatrix(render_transform Transform)
{
    glm::mat4 T;
    {
        glm::mat4 Pos = glm::translate(Transform.Pos);
        glm::quat Quat = glm::quat(Transform.Rot.w, Transform.Rot.x, Transform.Rot.y, Transform.Rot.z);
        glm::mat4 Rot = glm::toMat4(Quat);
        glm::mat4 Scale = glm::scale(Transform.Scale);
        
        T = Pos * Rot * Scale;
    }
    return(T);
}

global_variable GLuint gLastProgram = 0;

global_variable GLuint gMatrix  = 0;
global_variable GLuint gLight   = 0;
global_variable GLuint gMatrixM = 0;
global_variable GLuint gMatrixV = 0;
global_variable GLuint gMatrixP = 0;
global_variable GLuint gCameraP = 0;
global_variable GLuint gHasShadow = 0;

internal void
RenderCheckUniforms(GLuint Program)
{
    if(gLastProgram != Program)
    {
        gMatrix = glGetUniformLocation(Program, "mvp");
        gMatrixM = glGetUniformLocation(Program, "m");
        gMatrixV = glGetUniformLocation(Program, "v");
        gMatrixP = glGetUniformLocation(Program, "p");
        gLight   = glGetUniformLocation(Program, "lm");
        gCameraP = glGetUniformLocation(Program, "camP");
        gHasShadow = glGetUniformLocation(Program, "hasShadow");
        gLastProgram = Program;
    }
}

enum
{
    RenderPass_Depth,
    RenderPass_Stencil,
    RenderPass_Color,
    RenderPass_PostProcess,
    Num_Of_RenderPasses
};

#include "f3d_render_material.h"
#include "f3d_render_mesh.h"
#include "f3d_render_4ds_proto.h"
#include "f3d_render_light.h"
#include "f3d_render_octree.h"
#include "f3d_render_single.h"

internal void
RenderAddQuery(u8 RenderPass, 
               render_mesh* Mesh, 
               render_material *Material, 
               glm::mat4 RenderTransform,
               GLuint Shader);

#include "f3d_primitive_cube.h"
#include "f3d_primitive_plane.h"

typedef struct
{
    render_mesh *Mesh;
    render_material *Material;
    glm::mat4 RenderTransform;
    GLuint Shader;
} render_query;

typedef struct
{
    render_query *Begin;
    render_query *Ptr;
    u32 Size;
    u32 Used;
} render_queue;

global_variable render_queue GlobalRenderPool[Num_Of_RenderPasses] = {};

internal void
RenderAddQueryInternal(render_queue *Queue,
                       render_mesh* Mesh, 
                       render_material *Material, 
                       glm::mat4 RenderTransform,
                       GLuint Shader)
{    
    if(Queue->Size)
    {
        if((Queue->Size - Queue->Used) < 5)
        {
            Queue->Size += 100;
            Queue->Begin = (render_query *)PlatformMemRealloc(Queue->Begin, sizeof(render_query)*Queue->Size);
        }
        
        add_query:
        
        render_query *Query = &Queue->Begin[Queue->Used];
        render_query Query_ = {};
        *Query = Query_;
        
        Query->Mesh = Mesh;
        Query->Material = Material;
        Query->RenderTransform = RenderTransform;
        Query->Shader = Shader;
        
        Queue->Used++;
    }
    else
    {
        Queue->Begin = (render_query *)PlatformMemAlloc(sizeof(render_query)*1000);
        Queue->Size = 1000;
        Queue->Used = 0;
        
        goto add_query;
    }
}


internal void
RenderAddQuery(u8 RenderPass, 
               render_mesh* Mesh, 
               render_material *Material, 
               glm::mat4 RenderTransform,
               GLuint Shader)
{
    Assert(RenderPass > -1 && RenderPass < Num_Of_RenderPasses);
    render_queue *Queue = GlobalRenderPool + RenderPass;
    
    RenderAddQueryInternal(Queue, Mesh, Material, RenderTransform, Shader);
}

typedef struct
{
    glm::mat4 ShadowMatrix;
    render_queue Meshes;
    b32 Used;
} shadow_generator;

#define F3D_MAX_SHADOWS 256

global_variable shadow_generator GlobalShadowGeneratorPool[F3D_MAX_SHADOWS] = {};

internal void
RenderDraw(u8 RenderPass)
{
    Assert(RenderPass > -1 && RenderPass < Num_Of_RenderPasses);
    
    render_queue *Queue = GlobalRenderPool + RenderPass;
    
    /**/ if(RenderPass == RenderPass_Color)
    {
        glViewport(0, 0, GlobalWindowArea.Width, GlobalWindowArea.Height);
        glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderBuffers[Framebuffer_Color]);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, GlobalFrameTextures[Framebuffer_Depth]);
        glUniform1i(depthT, 1);
        glUniform1i(renderType, 0);
        
        if(Queue)
        {
            mi Idx = 0;
            shadow_generator *Shadow = GlobalShadowGeneratorPool;
            
            if(Shadow->Used)
            {
                glUniformMatrix4fv(gLight, 1, GL_FALSE, &Shadow->ShadowMatrix[0][0]);
                glUniform1i(gHasShadow, 1);
            }
            else
            {
                glUniform1i(gHasShadow, 0);
            }
            
            do
            {
                for(u32 Idx = 0;
                    Idx < Queue->Used;
                    Idx++)
                {
                    render_query *Query = &Queue->Begin[Idx];
                    
                    RenderCheckUniforms(Query->Shader);
                    
                    RenderApplyMaterial(Query->Material, Query->Shader);
                    
                    glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &Query->RenderTransform[0][0]);
                    
                    MeshDraw(Query->Mesh);
                }
                
                Shadow = GlobalShadowGeneratorPool + ++Idx;
            }
            while(Idx < F3D_MAX_SHADOWS && Shadow->Used);
            
            Queue->Used = 0;
            Queue->Ptr = Queue->Begin;
            
            for(ms Idx = 0;
                Idx < F3D_MAX_SHADOWS;
                Idx++)
            {
                shadow_generator *Shadow = GlobalShadowGeneratorPool + Idx;
                
                if(!Shadow->Used) break;
                Shadow->Used = 0;
            }
        }
    }
    else if(RenderPass == RenderPass_Depth)
    {
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, GlobalRenderBuffers[Framebuffer_Depth]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUniform1i(renderType, 2);
    }
    else
    {
        InvalidCodePath;   
    }
}

internal void
ShadowAddQuery(shadow_generator *Shadow,
               render_mesh* Mesh, 
               glm::mat4 RenderTransform,
               GLuint Shader)
{
    RenderAddQueryInternal(&Shadow->Meshes, Mesh, 0, RenderTransform, Shader);
}

internal void
ShadowGenerate(shadow_generator *Shadow, GLuint Program)
{
    glUseProgram(Program);
    
    glUniformMatrix4fv(gLight, 1, GL_FALSE, &Shadow->ShadowMatrix[0][0]);
    
    if(Shadow)
    {
        render_queue *Queue = &Shadow->Meshes;
        for(u32 Idx = 0;
            Idx < Queue->Used;
            Idx++)
        {
            render_query *Query = &Queue->Begin[Idx];
            
            RenderCheckUniforms(Query->Shader);
            
            glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &Query->RenderTransform[0][0]);
            
            MeshDraw(Query->Mesh);
        }
        
        Queue->Used = 0;
        Queue->Ptr = Queue->Begin;
    }
}

internal void
ShadowDraw(shadow_generator ShadowGens)
{
    for(mi Idx=0;
        Idx < F3D_MAX_SHADOWS;
        ++Idx)
    {
        shadow_generator *Shadow = GlobalShadowGeneratorPool + Idx;
        if(!Shadow->Used)
        {
            *Shadow = ShadowGens;
            Shadow->Used = 1;
            return;
        }
    }
}

#define F3D_RENDER_H
#endif