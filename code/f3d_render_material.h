// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_MATERIAL_H)

typedef struct
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    render_texture *DiffTexture;
    b32 ColorKey;
    b32 DoubleSided;
    
    GLuint AmbLoc;
    GLuint DiffLoc;
    GLuint DiffTex;
    GLuint ColLoc;
} render_material;

internal void
RenderApplyMaterial(render_material *Mat, GLuint Program)
{
    if(!Mat->AmbLoc)
    {
        Mat->AmbLoc = glGetUniformLocation(Program, "material.ambient");Mat->DiffLoc = glGetUniformLocation(Program, "material.diffuse");
        Mat->DiffTex = glGetUniformLocation(Program, "material.difftex");
        Mat->ColLoc = glGetUniformLocation(Program, "material.colorkey");
    }
    
    glUniform3f(Mat->AmbLoc, Mat->Ambient.x, Mat->Ambient.y, Mat->Ambient.z);
    glUniform3f(Mat->DiffLoc, Mat->Diffuse.x, Mat->Diffuse.y, Mat->Diffuse.z);
    
    if(Mat->DiffTexture && Mat->DiffTexture->TextureObject)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Mat->DiffTexture->TextureObject);
        glUniform1i(Mat->DiffTex, 0);
        glUniform1i(Mat->ColLoc, Mat->ColorKey);
    }
    
    if(Mat->DoubleSided)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }
}

#define F3D_RENDER_MATERIAL_H
#endif