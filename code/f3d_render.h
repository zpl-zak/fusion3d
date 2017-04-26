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

global_variable GLuint gMatrix = 0;
global_variable GLuint gMatrixM = 0;
global_variable GLuint gMatrixV = 0;
global_variable GLuint gMatrixP = 0;
global_variable GLuint gCameraP = 0;

internal void
RenderCheckUniforms(GLuint Program)
{
    if(gLastProgram != Program)
    {
        gMatrix = glGetUniformLocation(Program, "mvp");
        gMatrixM = glGetUniformLocation(Program, "m");
        gMatrixV = glGetUniformLocation(Program, "v");
        gMatrixP = glGetUniformLocation(Program, "p");
        gCameraP = glGetUniformLocation(Program, "camP");
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
RenderAddQuery(u8 RenderPass, 
               render_mesh* Mesh, 
               render_material *Material, 
               glm::mat4 RenderTransform,
               GLuint Shader)
{
    Assert(RenderPass > -1 && RenderPass < Num_Of_RenderPasses);
    render_queue *Queue = GlobalRenderPool + RenderPass;
    
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
RenderDraw(u8 RenderPass)
{
    Assert(RenderPass > -1 && RenderPass < Num_Of_RenderPasses);
    
    render_queue *Queue = GlobalRenderPool + RenderPass;
    
    if(Queue)
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
        
        Queue->Used = 0;
        Queue->Ptr = Queue->Begin;
    }
}

#define F3D_RENDER_H
#endif