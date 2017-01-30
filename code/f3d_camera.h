// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_CAMERA_H)

typedef struct
{
     glm::mat4 Projection;
     glm::mat4 View;
     glm::vec3 Position;
     glm::vec2 Angle;
    
    real32 FieldOfView;
    real32 AspectRatio;
    real32 NearPlane;
    real32 FarPlane;
} camera;

internal glm::vec3
CameraGetDirection(camera *Camera)
{
     glm::vec3 Result = glm::vec3(cosf(Camera->Angle.y) * sinf(Camera->Angle.x),
                      sinf(Camera->Angle.y),
                      cosf(Camera->Angle.y) * cosf(Camera->Angle.x));
    return(Result);
}

internal glm::vec3
CameraGetRight(camera *Camera)
{
    glm::vec3 Result = glm::vec3(sinf(Camera->Angle.x - 3.14f / 2.f),
                         0,
                         cosf(Camera->Angle.x - 3.14f / 2.f));
    return(Result);
}

internal glm::vec3
CameraGetUp(camera *Camera)
{
    glm::vec3 Result = glm::cross(glm::normalize(CameraGetRight(Camera)), glm::normalize(CameraGetDirection(Camera)));
    return(Result);
}

internal void
CameraFixAngles(camera *Camera)
{
    Camera->Angle.x = MathToRadians(Camera->Angle.x);Camera->Angle.y = MathToRadians(Camera->Angle.y);
}

internal void
FrustumExtract(glm::mat4 Matrix);

internal camera *
CameraUpdate(camera *Camera, window_dim WindowDimension, r32 FieldOfView, r32 NearPlane, r32 FarPlane)
{
    Assert(Camera);
    
    Camera->Projection = glm::perspective(glm::radians(FieldOfView), WindowDimension.X / (r32)WindowDimension.Y, NearPlane, FarPlane);
    
    Camera->FieldOfView = glm::radians(FieldOfView);
    Camera->AspectRatio = WindowDimension.X / (r32)WindowDimension.Y;
    Camera->NearPlane = NearPlane;
    Camera->FarPlane = FarPlane;
    
     glm::vec3 Dir = glm::normalize(CameraGetDirection(Camera));
     glm::vec3 Up = glm::normalize(CameraGetUp(Camera));
    
    glm::vec3 Pos = {Camera->Position.x, Camera->Position.y, Camera->Position.z};
    
    Camera->View = glm::lookAt(
        Pos,
        Pos+Dir,
        Up);
    
    FrustumExtract(Camera->Projection * Camera->View);
    
    return(Camera);
}

#define F3D_CAMERA_H
#endif