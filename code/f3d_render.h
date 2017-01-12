// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_H)

enum
{
    ModelRenderType_Normal,
};

typedef struct
{
    v3 Pos;
     q4 Rot;
    v3 Scale;
} render_transform;

internal render_transform
RenderTransform(void)
{
    render_transform Result = {0};
    Result.Scale.X = 1.f;
    Result.Scale.Y = 1.f;
    Result.Scale.Z = 1.f;
    
    return(Result);
}

#define F3D_RENDER_H
#endif