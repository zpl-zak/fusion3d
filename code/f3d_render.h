// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_H)

typedef void light_pass_func(GLuint program);

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

typedef struct
{
    
} render_pass;

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

#include "f3d_render_material.h"
#include "f3d_render_4ds_proto.h"
#include "f3d_render_light.h"
#include "f3d_render_octree.h"
#include "f3d_render_single.h"

#define F3D_RENDER_H
#endif