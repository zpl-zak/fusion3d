// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_SINGLE_H)

typedef struct render_single_
{
    render_4ds *Render;
    b32 CheckFrustum;
    render_transform Transform;
    glm::mat4 TransformMatrix;
    
    struct render_single_ *Next;
} render_single;

global_variable render_single *GlobalSingle = 0;

internal void
RenderSingleAdd(render_4ds *Render, render_transform Transform, b32 CheckFrustum)
{
    render_single *Node = (render_single *)PlatformMemAlloc(sizeof(render_single));
    render_single Node_ = {0};
    *Node = Node_;
    
    Node->Render = Render;
    Node->Transform = Transform;
    Node->TransformMatrix = RenderTransformMatrix(Transform);
    Node->CheckFrustum = CheckFrustum;
    Node->Next = 0;
    
    if(!GlobalSingle)
    {
        GlobalSingle = Node;
    }
    else
    {
        for(render_single *N = GlobalSingle;
            N;
            N = N->Next)
        {
            if(!N->Next)
            {
                N->Next = Node;
                break;
            }
        }
    }
}

internal void
RenderSingleDraw(GLuint Program, camera *Camera, s32 RenderType)
{
    for(render_single *N = GlobalSingle;
        N;
        N = N->Next)
    {
        Model4DSRender(N->Render, Program, Camera, N->TransformMatrix, RenderType, N->CheckFrustum); 
    }
}

#define F3D_RENDER_SINGLE_H
#endif