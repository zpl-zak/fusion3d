// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_CAMERA_H)

typedef struct
{
    m4 Projection;
    m4 View;
    v3 Position;
    v2 Angle;
} camera;

internal v3
CameraGetDirection(camera *Camera)
{
    v3 Result = MathVec3(cosf(Camera->Angle.Y) * sinf(Camera->Angle.X),
                      sinf(Camera->Angle.Y),
                      cosf(Camera->Angle.Y) * cosf(Camera->Angle.X));
    return(Result);
}

internal v3
CameraGetRight(camera *Camera)
{
    v3 Result = MathVec3(sinf(Camera->Angle.X - 3.14f / 2.f),
                         0,
                         cosf(Camera->Angle.X - 3.14f / 2.f));
    return(Result);
}

internal v3
CameraGetUp(camera *Camera)
{
    v3 Result = MathCross(MathNormalize(CameraGetRight(Camera)), MathNormalize(CameraGetDirection(Camera)));
    return(Result);
}

internal void
CameraFixAngles(camera *Camera)
{
    Camera->Angle.X = MathToRadians(Camera->Angle.X);Camera->Angle.Y = MathToRadians(Camera->Angle.Y);
}

internal camera *
CameraUpdate(camera *Camera, window_dim WindowDimension, r32 FieldOfView, r32 NearPlane, r32 FarPlane)
{
    Assert(Camera);
    
    Camera->Projection = MathPerspective(FieldOfView, WindowDimension.X / (r32)WindowDimension.Y, NearPlane, FarPlane);
    
    v3 Dir = MathNormalize(CameraGetDirection(Camera));
    v3 Up = MathNormalize(CameraGetUp(Camera));
    
    Camera->View = MathLookAt(
        Camera->Position,
        MathAddVec3(Camera->Position, Dir),
        Up);
    
    return(Camera);
}

#define F3D_CAMERA_H
#endif