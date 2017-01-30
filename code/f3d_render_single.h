// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_SINGLE_H)

#define F3D_SINGLE_MAX 128*1024

typedef struct render_single_
{
    render_4ds *Render;
    b32 CheckFrustum;
    render_transform Transform;
    glm::mat4 TransformMatrix;
    aabb BBox;
    
    b32 Loaded;
    b32 Visible;
} render_single;

global_variable render_single GlobalSingle[F3D_SINGLE_MAX] = {0};

internal void
RenderSingleAdd(render_4ds *Render, render_transform Transform, b32 CheckFrustum)
{
    s32 Slot = -1;
    for(s32 Idx=0;
        Idx < F3D_SINGLE_MAX;
        ++Idx)
    {
        if(!GlobalSingle[Idx].Loaded)
        {
            Slot = Idx;
            break;
        }
    }
    
    Assert(Slot != -1);
    
    render_single *Node = GlobalSingle + Slot;
    
    Node->Render = Render;
    Node->Transform = Transform;
    Node->TransformMatrix = RenderTransformMatrix(Transform);
    Node->CheckFrustum = CheckFrustum;
    Node->Loaded = 1;
    Node->BBox = Render->BBox;
}

internal void
RenderSingleCull(s32 StartIdx, s32 NumIndices)
{
    for(s32 Idx = StartIdx;
        Idx < NumIndices;
        ++Idx)
    {
        render_single *N = (GlobalSingle + Idx);
        
        if(N->Loaded && N->CheckFrustum)
        {
            aabb tbbox = N->BBox;
            const glm::mat4 M = N->TransformMatrix;
            
            tbbox.Min = M * tbbox.Min;
            tbbox.Max = M * tbbox.Max;
            N->Visible = FrustumCheckAABB(tbbox);
        }
        else if(N->Loaded)
        {
            N->Visible = 1;
        }
    }
}

internal void
RenderSingleDraw(s32 StartIdx, s32 NumIndices, GLuint Program, camera *Camera, s32 RenderType)
{
    for(s32 Idx = StartIdx;
        Idx < NumIndices;
        ++Idx)
    {
         render_single *N = (GlobalSingle + Idx);
        
        if(N->Loaded && N->Visible)
        {
        Model4DSRender(N->Render, Program, Camera, N->TransformMatrix, RenderType, N->CheckFrustum); 
        }
    }
}

#define F3D_RENDER_SINGLE_H
#endif