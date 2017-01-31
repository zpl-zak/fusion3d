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

// TODO(zaklaus): Probably shouldn't be here.
internal void
RenderApplyCamera(GLuint Program, camera *Camera)
{
    local_persist GLuint OldProgram = -1;
    
    if(OldProgram != Program)
    {
    local_persist GLuint ProjLoc = glGetUniformLocation(Program, "camera.projection");
    local_persist GLuint ViewLoc = glGetUniformLocation(Program, "camera.view");
    local_persist GLuint PosLoc = glGetUniformLocation(Program, "camera.position");
    local_persist GLuint AngleLoc = glGetUniformLocation(Program, "camera.angle");
    
    local_persist GLuint FOVLoc = glGetUniformLocation(Program, "camera.fieldOfView");
    local_persist GLuint ARLoc = glGetUniformLocation(Program, "camera.aspectRatio");
    local_persist GLuint NearLoc = glGetUniformLocation(Program, "camera.nearPlane");
    local_persist GLuint FarLoc = glGetUniformLocation(Program, "camera.farPlane");
        
        camera_send:
        {
            glUniformMatrix4fv(ProjLoc, 1, GL_FALSE, &Camera->Projection[0][0]);
            glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, &Camera->View[0][0]);
            glUniform3fv(PosLoc, 1, &Camera->Position[0]);
            glUniform2fv(AngleLoc, 1, &Camera->Angle[0]);
            
            glUniform1f(FOVLoc, Camera->FieldOfView);
            glUniform1f(ARLoc, Camera->AspectRatio);
            glUniform1f(NearLoc, Camera->NearPlane);
            glUniform1f(FarLoc, Camera->FarPlane);
            
            goto camera_done;
        }
    }
    
    goto camera_send;
    
    camera_done:
    {
    OldProgram = Program;
    }
}

#define F3D_CAMERA_H
#endif