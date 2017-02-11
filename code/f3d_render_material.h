// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_MATERIAL_H)

typedef struct
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    render_texture *DiffTexture;
    b32 ColorKey;
    b32 DoubleSided;
} render_material;

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
        
        material_send:
        {
            glUniform3f(AmbLoc, Mat->Ambient.x, Mat->Ambient.y, Mat->Ambient.z);
            glUniform3f(DiffLoc, Mat->Diffuse.x, Mat->Diffuse.y, Mat->Diffuse.z);
            
            if(Mat->DiffTexture && Mat->DiffTexture->TextureObject)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Mat->DiffTexture->TextureObject);
                glUniform1i(DiffTex, 0);
                glUniform1i(ColLoc, Mat->ColorKey);
            }
            
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