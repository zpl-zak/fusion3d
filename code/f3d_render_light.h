// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_LIGHT_H)

typedef struct
{
    glm::vec3 Pos;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    
    // NOTE(zaklaus): Attenuation
    real32 Constant;
    real32 Linear;
    real32 Quadratic;
} render_light_point;

typedef struct
{
    glm::vec3 Dir;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
} render_light_dir;

internal void
RenderApplyLightDirectional(render_light_dir *Light, GLuint Program)
{
    glUseProgram(Program);
    local_persist GLuint OldProgram = -1;
    
    if(OldProgram != Program)
    {
        local_persist GLuint DirLoc = glGetUniformLocation(Program, "light.direction");
        local_persist GLuint AmbLoc = glGetUniformLocation(Program, "light.ambient");
        local_persist GLuint DiffLoc = glGetUniformLocation(Program, "light.diffuse");
        
        dir_light_send:
        {
            glUniform3f(DirLoc, Light->Dir.x, Light->Dir.y, Light->Dir.z);
            glUniform3f(AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
            glUniform3f(DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
            
            goto dir_light_done;
        }
    }
    
    goto dir_light_send;
    
    dir_light_done:
    {
        OldProgram = Program;
    }
}

internal void
RenderApplyLightPoint(s32 Index, render_light_point *Light, GLuint Program)
{
    glUseProgram(Program);
    local_persist GLuint OldProgram = -1;
    
    if(OldProgram != Program)
    {
        char Temp[255] = {0};
        sprintf(Temp, "pointLight[%d].position", Index);
        local_persist GLuint PosLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].ambient", Index);
        local_persist GLuint AmbLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].diffuse", Index);local_persist GLuint DiffLoc = glGetUniformLocation(Program, Temp);
        
        sprintf(Temp, "pointLight[%d].constant", Index);local_persist GLuint CLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].linear", Index);local_persist GLuint LLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].quadratic", Index);local_persist GLuint QLoc = glGetUniformLocation(Program, Temp);
        
        point_light_send:
        {
            glUniform3f(PosLoc, Light->Pos.x, Light->Pos.y, Light->Pos.z);
            glUniform3f(AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
            glUniform3f(DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
            
            glUniform1f(CLoc, Light->Constant);
            glUniform1f(LLoc, Light->Linear);
            glUniform1f(QLoc, Light->Quadratic);
            
            goto point_light_done;
        }
    }
    
    goto point_light_send;
    
    point_light_done:
    {
        OldProgram = Program;
    }
}

#define F3D_RENDER_LIGHT_H
#endif