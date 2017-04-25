 // (c) 2016 ZaKlaus; All Rights Reserved
 
#if !defined(F3D_TEXTURE_H)
 
#define F3D_TEXTURE_MAX 16*1024
 
#include "formats/hformat_bmp.h"
 
 typedef struct
 {
     GLuint TextureObject;
     asset_file *Asset;
     hformat_bmp *Bitmap;
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
 
 internal void
     TextureReleaseAll(void)
 {
     for(s32 Idx=0;
         Idx < F3D_TEXTURE_MAX;
         ++Idx)
     {
         render_texture *Texture = GlobalTextures + Idx;
         
         if(Texture->TextureObject)
         {
             glDeleteTextures(1, &Texture->TextureObject);
         }
     }
 }
 
 // NOTE(ZaKlaus): Call this after you bind the texture!!!
 internal void
     TextureModify(s32 Filtering = GL_LINEAR)
 {
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filtering);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filtering);
 }
 
 internal render_texture *
     TextureLoad(render_texture *Texture, s32 Filtering = GL_LINEAR)
 {
     Assert(Texture && Texture->Asset);
     
     if(Texture->TextureObject)
     {
         return(Texture);
     }
     
     AssetLoadInternal(Texture->Asset);
     
     // TODO(zaklaus): Make sure it loads from memory!
     s32 FileIdx = AssetOpenHandle(Texture->Asset, Texture->Asset->LoadFromPack);
     Texture->Bitmap = HFormatLoadBMPImage(FileIdx, 0);
     
     s32 x, y, cpp;
     
     u8 *Data = stbi_load_from_memory(Texture->Asset->Content, (s32)Texture->Asset->FileSize, &x, &y, &cpp, 0);
     u8 *TData = (u8 *)PlatformMemAlloc(x*y*4);
     
     // NOTE(zaklaus): Allocate memory for RGBA and handle colorkey for opaque textures.
     // Mafia uses the first color in 8-bit bitmap pallete as a colorkey,
     // or first pixel color in 24-bit bitmap.
     // We assume Mafia's opaque material uses only 8-bit bitmap for handing out the colorkey,
     // therefore there might be problems involved with 24-bit bitmaps.
     {
         glm::vec3 ColorKey;
         ColorKey.x = (Texture->Bitmap->Colors[0].rgbRed);
         ColorKey.y = (Texture->Bitmap->Colors[0].rgbGreen);
         ColorKey.z = (Texture->Bitmap->Colors[0].rgbBlue);
         for(s32 Idx = 0, Idx2 = 0;
             Idx < x*y*3;
             Idx += 3, Idx2 += 4)
         {
             *(TData+Idx2) = *(Data+Idx);
             *(TData+Idx2+1) = *(Data+Idx+1);
             *(TData+Idx2+2) = *(Data+Idx+2);
             *(TData+Idx2+3) = 255;
             
             if(ColorKey.x == *(TData+Idx2) && ColorKey.y == *(TData+Idx2+1) && ColorKey.z == *(TData+Idx2+2))
             {
                 *(TData+Idx2+3) = 0;
             }
         }
         
         glGenTextures(1, &Texture->TextureObject);
         glBindTexture(GL_TEXTURE_2D, Texture->TextureObject);
         
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, TData);
         
         PlatformMemFree(TData);
         
         
         TextureModify(Filtering);
     }
     
     IOFileClose(FileIdx);
     return(Texture);
 }
 
#define F3D_TEXTURE_H
#endif