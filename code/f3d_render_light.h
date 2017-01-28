// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_LIGHT_H)

typedef struct
{
    glm::vec3 Pos;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    
    real32 Constant;
    real32 Linear;
    real32 Quadratic;
    
    GLuint PosLoc;
    GLuint AmbLoc;
    GLuint DiffLoc;
    
    // NOTE(zaklaus): Attenuation
    GLuint CLoc;
    GLuint LLoc;
    GLuint QLoc;
} render_light_point;

typedef struct
{
    glm::vec3 Dir;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    
    GLuint DirLoc;
    GLuint AmbLoc;
    GLuint DiffLoc;
} render_light_dir;

internal void
RenderApplyLightDirectional(render_light_dir *Light, GLuint Program)
{
    glUseProgram(Program);
    
    if(!Light->AmbLoc)
    {
        Light->DirLoc = glGetUniformLocation(Program, "light.direction");
        Light->AmbLoc = glGetUniformLocation(Program, "light.ambient");Light->DiffLoc = glGetUniformLocation(Program, "light.diffuse");
    }
    
    glUniform3f(Light->DirLoc, Light->Dir.x, Light->Dir.y, Light->Dir.z);
    glUniform3f(Light->AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
    glUniform3f(Light->DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
    
}

internal void
RenderApplyLightPoint(s32 Index, render_light_point *Light, GLuint Program)
{
    glUseProgram(Program);
    
    if(!Light->AmbLoc)
    {
        char Temp[255] = {0};
        sprintf(Temp, "pointLight[%d].position", Index);
        Light->PosLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].ambient", Index);
        Light->AmbLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].diffuse", Index);Light->DiffLoc = glGetUniformLocation(Program, Temp);
        
        sprintf(Temp, "pointLight[%d].constant", Index);Light->CLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].linear", Index);Light->LLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].quadratic", Index);Light->QLoc = glGetUniformLocation(Program, Temp);
    }
    
    glUniform3f(Light->PosLoc, Light->Pos.x, Light->Pos.y, Light->Pos.z);
    glUniform3f(Light->AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
    glUniform3f(Light->DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
    
    glUniform1f(Light->CLoc, Light->Constant);
    glUniform1f(Light->LLoc, Light->Linear);
    glUniform1f(Light->QLoc, Light->Quadratic);
}

#define F3D_RENDER_LIGHT_H
#endif