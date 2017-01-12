// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_TEXTURE_H)

#include "f3d_render.h"

#define F3D_TEXTURE_MAX 4096

typedef struct
{
    GLuint TextureObject;
    asset_file *Asset;
} render_texture;

global_variable render_texture GlobalTextures[F3D_TEXTURE_MAX] = {0};

internal render_texture *
TextureRegister(char *Name, char *FileName, u8 Type)
{
    s32 Slot = -1;
    for(s32 Idx = 0;
        Idx < F3D_TEXTURE_MAX;
        ++Idx)
    {
        if(!GlobalTextures[Idx].Asset && Slot == -1)
        {
            Slot = Idx;
        }
        
        if(GlobalTextures[Idx].Asset && 
           StringsAreEqual(Name, GlobalTextures[Idx].Asset->Name))
        {
            Slot = Idx;
            break;
        }
    }
    
    Assert(Slot != -1);
    
    render_texture *Texture = GlobalTextures + Slot;
    render_texture Texture_ = {0};
    *Texture = Texture_;
    
    char Temp[256];
    sprintf(Temp, "%s%s", AssetTypeNames[Asset_Texture][0], FileName);
    asset_file *Asset = AssetRegister(Name, Temp, Asset_Texture);
    
    Texture->Asset = Asset;
    
    return(Texture);
}

internal render_texture *
TextureLoad(render_texture *Texture)
{
    Assert(Texture && Texture->Asset);
    
    if(Texture->TextureObject)
    {
        return(Texture);
    }
    
    s32 x, y, bpp;
    
    u8 *Data = stbi_load(Texture->Asset->FilePath, &x, &y, &bpp, 0);
    {
        glGenTextures(1, &Texture->TextureObject);
        glBindTexture(GL_TEXTURE_2D, Texture->TextureObject);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    stbi_image_free(Data);
    
    return(Texture);
}

#define F3D_TEXTURE_H
#endif