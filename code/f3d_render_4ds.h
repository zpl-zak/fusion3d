// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_4DS_H)

#include "f3d_render.h"

#include "formats/hformat_4ds.h"

#define F3D_MODEL_4DS_MAX 16

typedef struct
{
    GLuint ElementBuffer;
    u16 FaceCount;
     u16 MaterialIndex;
} render_4ds_facegroup;

typedef struct
{
    r32 RelativeDistance;
    u16 VertexCount;
    
    GLuint PositionBuffer;
    GLuint UVBuffer;
    
    u8 FaceGroupCount;
    render_4ds_facegroup *FaceGroups;
} render_4ds_lod;

typedef struct
{
    u8 LODLevel;
    render_4ds_lod *LODs;
     s32 InstanceOf;
    
    v3 Pos;
    v4 Rot;
    v3 Scale;
} render_4ds_mesh;

typedef struct
{
    render_texture *Diffuse;
} render_4ds_material;

typedef struct
{
    asset_file *Asset;
    
    u16 MeshCount;
    render_4ds_mesh *Meshes;
    
    u16 MaterialCount;
    render_4ds_material *Materials;
    
    hformat_4ds_header *Header;
} render_4ds;

typedef struct
{
    s32 Min, Max;
} render_mesh_volume;

global_variable render_4ds GlobalModel4DS[F3D_MODEL_4DS_MAX] = {0};

internal render_4ds *
Model4DSRegister(char *Name, char *FileName)
{
    s32 Slot = -1;
    for(s32 Idx = 0;
        Idx < F3D_MODEL_4DS_MAX;
        ++Idx)
    {
        if(!GlobalModel4DS[Idx].Asset && Slot == -1)
        {
            Slot = Idx;
        }
        
        if(GlobalModel4DS[Idx].Asset && StringsAreEqual(GlobalModel4DS[Idx].Asset->Name, Name))
        {
            Slot = Idx;
            break;
        }
    }
    
    render_4ds *Render = GlobalModel4DS + Slot;
    
    if(Render->Asset)
    {
        return(Render);
    }
    
    render_4ds Render_ = {0};
    *Render = Render_;
    
    char Temp[256];
    sprintf(Temp, "%s%s", AssetTypeNames[Asset_Model][0], FileName);
    asset_file *Asset = AssetRegister(Name, Temp, Asset_Model);
    
    Render->Asset = Asset;
    
    return(Render);
}

internal render_4ds *
Model4DSLoad(render_4ds *Render)
{
    Assert(Render && Render->Asset);
    
    if(Render->Header)
    {
        return(Render);
    }
    
    s32 FileHandle = IOFileOpenRead(Render->Asset->FilePath, 0);
    
    Render->Header = HFormatLoad4DSModel(FileHandle);
    {
        Render->MaterialCount = Render->Header->MaterialCount;
        
        Render->Materials = PlatformMemAlloc(sizeof(render_4ds_material)*Render->MaterialCount);
        
        for(s32 Idx = 0;
            Idx < Render->MaterialCount;
            ++Idx)
        {
            hformat_4ds_material *HMaterial = Render->Header->Materials + Idx;
            render_4ds_material *Material = Render->Materials + Idx;
            render_4ds_material Material_ = {0};
            *Material = Material_;
            
            if(HMaterial->DiffuseMapNameLength)
            {
            char Temp[255] = {0};
            sprintf(Temp, "%.*s", HMaterial->DiffuseMapNameLength, HMaterial->DiffuseMapName);
            
            render_texture *Texture = TextureRegister(Temp, Temp, 0);
            TextureLoad(Texture);
            Material->Diffuse = Texture;
        }
        }
        
        Render->MeshCount = Render->Header->MeshCount;
        
        Render->Meshes = PlatformMemAlloc(sizeof(render_4ds_mesh)*Render->MeshCount);
        
        for(s32 Idx = 0;
            Idx < Render->MeshCount;
            ++Idx)
        {
            hformat_4ds_mesh *HMesh = Render->Header->Meshes + Idx;
            render_4ds_mesh Mesh_ = {0};
            render_4ds_mesh *Mesh = Render->Meshes + Idx;
            hformat_4ds_standard *Standard = &HMesh->Standard;
            *Mesh = Mesh_;
            if(HMesh->MeshType == HFormat4DSMeshType_Standard &&
               HMesh->VisualMeshType == HFormat4DSVisualMeshType_Standard)
            {
            if(Standard->Instanced)
            {
                Mesh->InstanceOf = Standard->Instanced;
                continue;
            }
            
            Mesh->Pos = HMesh->Pos;
            Mesh->Rot = HMesh->Rot;
            Mesh->Scale = HMesh->Scale;
            
            Mesh->LODLevel = Standard->LODLevel;
            Mesh->LODs = PlatformMemAlloc(sizeof(render_4ds_lod)*Mesh->LODLevel);
            for(s32 Idx2 = 0;
                Idx2 < Mesh->LODLevel;
                ++Idx2)
            {
                hformat_4ds_lod *HLOD = Standard->LODs + Idx2;
                render_4ds_lod LOD_ = {0};
                render_4ds_lod *LOD = Mesh->LODs + Idx2;
                *LOD = LOD_;
                
                LOD->RelativeDistance = HLOD->RelativeDistance;
                LOD->VertexCount = HLOD->VertexCount;
                
                 v3 *Positions = PlatformMemAlloc(sizeof(v3)*LOD->VertexCount);
                
                for(s32 PosIdx = 0;
                    PosIdx < LOD->VertexCount;
                    ++PosIdx)
                {
                    Positions[PosIdx] = HLOD->Vertices[PosIdx].Pos;
                }
                
                glGenBuffers(1, &LOD->PositionBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, LOD->PositionBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(v3)*LOD->VertexCount, Positions, GL_STATIC_DRAW);
                
                PlatformMemFree(Positions);
                
                v2 *UV = PlatformMemAlloc(sizeof(v2)*LOD->VertexCount);
                
                for(s32 PosIdx = 0;
                    PosIdx < LOD->VertexCount;
                    ++PosIdx)
                {
                    UV[PosIdx] = HLOD->Vertices[PosIdx].UV;
                }
                
                glGenBuffers(1, &LOD->UVBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, LOD->UVBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(v2)*LOD->VertexCount, UV, GL_STATIC_DRAW);
                
                PlatformMemFree(UV);
                
                
                LOD->FaceGroupCount = HLOD->FaceGroupCount;
                LOD->FaceGroups = PlatformMemAlloc(sizeof(render_4ds_facegroup)*LOD->FaceGroupCount);
                for(s32 GroupIdx = 0;
                    GroupIdx < LOD->FaceGroupCount;
                    ++GroupIdx)
                {
                    render_4ds_facegroup Group_ = {0};
                    render_4ds_facegroup *Group = LOD->FaceGroups + GroupIdx;
                    hformat_4ds_facegroup *HGroup = HLOD->FaceGroups + GroupIdx;
                    
                    *Group = Group_;
                    
                    Group->MaterialIndex = HGroup->MaterialID;
                    u16 FaceCount = HGroup->FaceCount;
                    Group->FaceCount = FaceCount;
                    hformat_4ds_face *HFaces = HGroup->Faces;
                    
                    if(!FaceCount)
                    {
                        continue;
                    }
                    
                     hformat_4ds_face *Faces = PlatformMemAlloc(sizeof(hformat_4ds_face)*FaceCount);
                    
                    s32 HFaceIdx = 0;
                    for(s32 FaceIdx = 0;
                        FaceIdx < FaceCount;
                        ++FaceIdx, ++HFaceIdx)
                    {
                        Faces[FaceIdx] = HFaces[HFaceIdx];
                    }
                    
                    glGenBuffers(1, &Group->ElementBuffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Group->ElementBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hformat_4ds_face)*FaceCount, Faces, GL_STATIC_DRAW);
                    
                    PlatformMemFree(Faces);
                }
            }
        }
    }
}
    IOFileClose(FileHandle);
    
    return(Render);
}

internal render_4ds *
Model4DSRender(render_4ds *Render, GLuint Program, camera *Camera, render_transform Transform, s32 RenderType)
{
    glUseProgram(Program);
    
    switch(RenderType)
    {
        case ModelRenderType_Normal:
        {
            
            
            GLuint Matrix = glGetUniformLocation(Program, "mvp");
            GLuint Texture = glGetUniformLocation(Program, "texSampler");
            
            for(s32 Idx = 0;
                Idx < Render->MeshCount;
                ++Idx)
            {
                render_4ds_mesh *Mesh = &Render->Meshes[Idx];
                render_4ds_lod *LOD = &Render->Meshes[Idx].LODs[0];
                
                s32 Cdx = Idx;
                if(!LOD)
                {
                    continue;
                    Cdx = Render->Meshes[Cdx].InstanceOf + 1;
                    LOD = &Render->Meshes[Cdx].LODs[0];
                }
                
                mat4 Model = MathMultiplyMat4(MathTranslate(Mesh->Pos), MathRotate(Mesh->Rot.W, Mesh->Rot.XYZ));
                Model = MathMultiplyMat4(Model, MathScale(Mesh->Scale));
                
                mat4 MVP = MathMultiplyMat4(Camera->Projection, 
                                            Camera->View);   
                MVP = MathMultiplyMat4(MVP, Model);
                
                
                for(s32 FgIdx = 0;
                    FgIdx < LOD->FaceGroupCount;
                    ++FgIdx)
                {
                    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &MVP.Elements[0][0]);
                    
                    s32 MatIdx = MathMAX(0,LOD->FaceGroups[FgIdx].MaterialIndex - 1);
                    if(Render->Materials[MatIdx].Diffuse && Render->Materials[MatIdx].Diffuse->TextureObject)
                    {
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, Render->Materials[MatIdx].Diffuse->TextureObject);
                        glUniform1i(Texture, 0);
                    }
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, LOD->PositionBuffer);
                        glVertexAttribPointer(
                            0,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0, (void *)0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, LOD->UVBuffer);
                        glVertexAttribPointer(
                            1,
                            2,
                            GL_FLOAT,
                            GL_FALSE,
                            0, (void *)0);
                        
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LOD->FaceGroups[FgIdx].ElementBuffer);
                        
                        glDrawElements(GL_TRIANGLES, LOD->FaceGroups[FgIdx].FaceCount*3, GL_UNSIGNED_SHORT, (void *)0);
                    }
                    glDisableVertexAttribArray(1);
                    glDisableVertexAttribArray(0);
                }
            }
            
        }break;
    }
    return(Render);
}

#define F3D_RENDER_4DS_H
#endif