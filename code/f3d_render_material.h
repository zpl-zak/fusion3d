// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_MATERIAL_H)

typedef struct
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    r32 Opacity; // 0 - solid, 1 - invisible
    render_texture *DiffTexture;
    b32 ColorKey;
    b32 DoubleSided;
    b32 Fullbright;
} render_material;

global_variable render_texture *GlobalDefaultTexture = 0;

internal void
RenderApplyMaterial(render_material *Mat, GLuint Program)
{
    local_persist GLuint OldProgram = -1;
    
    if(OldProgram != Program)
    {
        local_persist GLuint AmbLoc = glGetUniformLocation(Program, "material.ambient");
        local_persist GLuint DiffLoc = glGetUniformLocation(Program, "material.diffuse");
        local_persist GLuint DiffTex = glGetUniformLocation(Program, "material.difftex");
        local_persist GLuint ColLoc = glGetUniformLocation(Program, "material.colorkey");
        local_persist GLuint OpLoc  = glGetUniformLocation(Program, "material.opacity");
        local_persist GLuint FbLoc  = glGetUniformLocation(Program, "material.fullbright");
        
        local_persist b32 CallOnce = 0;
        
        if(!CallOnce)
        {
            CallOnce = 1;
            
            GlobalDefaultTexture = TextureRegister("DefaultTexture", "white.bmp", 0);
            TextureLoad(GlobalDefaultTexture, GL_NEAREST);
        }
        
        material_send:
        {
            glUniform3f(AmbLoc, Mat->Ambient.x, Mat->Ambient.y, Mat->Ambient.z);
            glUniform3f(DiffLoc, Mat->Diffuse.x, Mat->Diffuse.y, Mat->Diffuse.z);

            glActiveTexture(GL_TEXTURE3);
            if(Mat->DiffTexture && Mat->DiffTexture->TextureObject)
            {
                glBindTexture(GL_TEXTURE_2D, Mat->DiffTexture->TextureObject);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, GlobalDefaultTexture->TextureObject);
            }
            
            glUniform1i(DiffTex, 3);
            glUniform1f(OpLoc, Mat->Opacity);
            glUniform1i(ColLoc, Mat->ColorKey);
            glUniform1i(FbLoc, Mat->Fullbright);
            
            goto material_done;
        }
    }
    
    goto material_send;
    
    material_done:
    {
        if(Mat->DoubleSided)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
        }
        
        OldProgram = Program;
    }
}

#define F3D_RENDER_MATERIAL_H
#endif