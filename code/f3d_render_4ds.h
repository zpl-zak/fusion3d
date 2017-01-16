// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_4DS_H)

#include "f3d_render.h"

#include "formats/hformat_4ds.h"

#define F3D_MODEL_4DS_MAX 128*1024

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
    GLuint NormalBuffer;
    GLuint UVBuffer;
    
    u8 FaceGroupCount;
    render_4ds_facegroup *FaceGroups;
} render_4ds_lod;

typedef struct
{
    u8 LODLevel;
    render_4ds_lod *LODs;
     s32 InstanceOf;
    s32 ParentID;
    
    glm::vec3 Pos;
    glm::vec4 Rot;
    glm::vec3 Scale;
    
    b32 Transformed;
    glm::mat4 Transform;
} render_4ds_mesh;

typedef struct
{
    render_material Material;
} render_4ds_material;

typedef struct
{
    b32 Loaded;
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

internal void
Model4DSLoadInternal(LPVOID Param)
{
    render_4ds *Render = (render_4ds *)Param;
    
    Assert(Render && Render->Asset);
    
    if(Render->Header)
    {
        return;
    }
    
    s32 FileHandle = IOFileOpenRead((s8 *)Render->Asset->FilePath, 0);
    
    Render->Header = HFormatLoad4DSModel(FileHandle);
    {
        Render->MaterialCount = Render->Header->MaterialCount;
        
        Render->Materials = (render_4ds_material *)PlatformMemAlloc(sizeof(render_4ds_material)*Render->MaterialCount);
        
        for(s32 Idx = 0;
            Idx < Render->MaterialCount;
            ++Idx)
        {
            hformat_4ds_material *HMaterial = Render->Header->Materials + Idx;
            render_4ds_material *Material = Render->Materials + Idx;
            render_4ds_material Material_ = {};
            *Material = Material_;
            
            Material->Material.Ambient = {.12,.12,.12};
            
            Material->Material.Diffuse = {0.5,.5,.5};
            
            if(HMaterial->DiffuseMapNameLength)
            {
            char Temp[255] = {0};
            sprintf(Temp, "%.*s", HMaterial->DiffuseMapNameLength, HMaterial->DiffuseMapName);
            
            render_texture *Texture = TextureRegister(Temp, Temp, 0);
            TextureLoad(Texture);
            Material->Material.DiffTexture = Texture;
            
            Material->Material.ColorKey = 0;
            if(HMaterial->Flags & HFormat4DSMaterialFlag_ColorKey)
            {
                Material->Material.ColorKey = 1;
            }
            
            Material->Material.DoubleSided = 0;
            if(HMaterial->Flags & HFormat4DSMaterialFlag_DoubleSidedMaterial)
            {
                Material->Material.DoubleSided = 1;
            }
        }
        }
        
        Render->MeshCount = Render->Header->MeshCount;
        
        Render->Meshes = (render_4ds_mesh *)PlatformMemAlloc(sizeof(render_4ds_mesh)*Render->MeshCount);
        
        for(s32 Idx = 0;
            Idx < Render->MeshCount;
            ++Idx)
        {
            hformat_4ds_mesh *HMesh = Render->Header->Meshes + Idx;
            render_4ds_mesh Mesh_ = {0};
            render_4ds_mesh *Mesh = Render->Meshes + Idx;
            hformat_4ds_standard *Standard = &HMesh->Standard;
            *Mesh = Mesh_;
            
            Mesh->ParentID = HMesh->ParentID;
            Mesh->Pos = {HMesh->Pos.X, HMesh->Pos.Y, HMesh->Pos.Z};
            Mesh->Rot = {HMesh->Rot.X, HMesh->Rot.Y, HMesh->Rot.Z, HMesh->Rot.W};
            Mesh->Scale = {HMesh->Scale.X, HMesh->Scale.Y, HMesh->Scale.Z};
            
            if(StringsAreEqualA(4, (char *)HMesh->MeshName, "wcol"))
               {
                   continue;
               }
            
            if(HMesh->MeshType == HFormat4DSMeshType_Standard &&
               HMesh->VisualMeshType == HFormat4DSVisualMeshType_Standard)
            {
                if(Standard->Instanced)
                {
                    Mesh->InstanceOf = Standard->Instanced;
                    continue;
                }
                
            Mesh->LODLevel = Standard->LODLevel;
                Mesh->LODs = (render_4ds_lod *)PlatformMemAlloc(sizeof(render_4ds_lod)*Mesh->LODLevel);
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
                
                v3 *Positions = (v3 *)PlatformMemAlloc(sizeof(v3)*LOD->VertexCount);
                
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
                
                v3 *Normals = (v3 *)PlatformMemAlloc(sizeof(v3)*LOD->VertexCount);
                
                for(s32 PosIdx = 0;
                    PosIdx < LOD->VertexCount;
                    ++PosIdx)
                {
                    Normals[PosIdx] = HLOD->Vertices[PosIdx].Normal;
                }
                
                glGenBuffers(1, &LOD->NormalBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, LOD->NormalBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(v3)*LOD->VertexCount, Normals, GL_STATIC_DRAW);
                
                PlatformMemFree(Normals);
                
                
                v2 *UV = (v2 *)PlatformMemAlloc(sizeof(v2)*LOD->VertexCount);
                
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
                LOD->FaceGroups = (render_4ds_facegroup *)PlatformMemAlloc(sizeof(render_4ds_facegroup)*LOD->FaceGroupCount);
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
                    
                    hformat_4ds_face *Faces = (hformat_4ds_face *)PlatformMemAlloc(sizeof(hformat_4ds_face)*FaceCount);
                    
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

Render->Loaded = 1;
}

internal void
Model4DSLoad(render_4ds *Render)
{
    Model4DSLoadInternal((LPVOID)Render);
}

internal void
AsyncModel4DSLoad(render_4ds *Render)
{
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Model4DSLoadInternal, (LPVOID)Render, 0, 0);
}

internal glm::mat4
Model4DSGetTransform(render_4ds_mesh *Mesh, render_4ds *Render)
{
    #if 1
    //v3 ScaleVec = Mesh->Scale;
     glm::mat4 Pos = glm::translate(Mesh->Pos);
    glm::quat Quat = glm::quat(Mesh->Rot.w, Mesh->Rot.x, Mesh->Rot.y, Mesh->Rot.z);
    glm::mat4 Rot = glm::toMat4(Quat);
     glm::mat4 Scale = glm::scale(Mesh->Scale);
    
    glm::mat4 Model = Pos * Rot * Scale;
    //Model = MathMultiplyMat4(Model, Pos);
    
    if(Mesh->ParentID)
    {
        Model = Model4DSGetTransform((Render->Meshes + Mesh->ParentID - 1), Render) * Model;
    }
    return(Model);
    #endif
}

global_variable GLuint gLastProgram = 0;

global_variable GLuint gMatrix = 0;
global_variable GLuint gMatrixM = 0;
global_variable GLuint gMatrixV = 0;
global_variable GLuint gMatrixP = 0;

internal render_4ds *
Model4DSRender(render_4ds *Render, GLuint Program, camera *Camera, render_transform Transform, s32 RenderType)
{
    if(!Render->Loaded)
    {
        return(Render);
    }
    
    glUseProgram(Program);
    
    switch(RenderType)
    {
        case ModelRenderType_Normal:
        {
            if(gLastProgram != Program)
            {
            gMatrix = glGetUniformLocation(Program, "mvp");
            gMatrixM = glGetUniformLocation(Program, "m");
            gMatrixV = glGetUniformLocation(Program, "v");
            gMatrixP = glGetUniformLocation(Program, "p");
                gLastProgram = Program;
            }
            
            glm::mat4 V = Camera->View;
            glm::mat4 P = Camera->Projection;
            glUniformMatrix4fv(gMatrixV, 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(gMatrixP, 1, GL_FALSE, &P[0][0]);
            
            for(s32 Idx = 0;
                Idx < Render->MeshCount;
                ++Idx)
            {
                render_4ds_mesh *Mesh = &Render->Meshes[Idx];
                render_4ds_lod *LOD = &Render->Meshes[Idx].LODs[0];
                
                s32 Cdx = Idx;
                while(!LOD && Render->Meshes[Cdx].InstanceOf)
                {
                    Cdx = Render->Meshes[Cdx].InstanceOf - 1;
                    LOD = &Render->Meshes[Cdx].LODs[0];
                }
                
                if(!LOD)
                {
                    continue;
                }
                
                
                glm::mat4 T;
                {
                    glm::mat4 Pos = glm::translate(Transform.Pos);
                    glm::quat Quat = glm::quat(Transform.Rot.w, Transform.Rot.x, Transform.Rot.y, Transform.Rot.z);
                    glm::mat4 Rot = glm::toMat4(Quat);
                    glm::mat4 Scale = glm::scale(Transform.Scale);
                    
                    T = Pos * Rot * Scale;
                    
                }
                
                glm::mat4 Model = Mesh->Transform;
                if(!Mesh->Transformed)
                    {
                        Model = Mesh->Transform = Model4DSGetTransform(Mesh, Render);
                        Mesh->Transformed = 1;
                    }
                    Model = T * Mesh->Transform;
                
                glm::mat4 MVP = Camera->Projection * Camera->View * Model;
                glm::mat4 M = Model;
                
                for(s32 FgIdx = 0;
                    FgIdx < LOD->FaceGroupCount;
                    ++FgIdx)
                {
                    glUniformMatrix4fv(gMatrix, 1, GL_FALSE, &MVP[0][0]);
                    glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &M[0][0]);
                    
                    s32 MatIdx = MathMAX(0,LOD->FaceGroups[FgIdx].MaterialIndex - 1);
                    
                    RenderApplyMaterial(&Render->Materials[MatIdx].Material, Program);
                    
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);
                    {
                        glBindBuffer(GL_ARRAY_BUFFER, LOD->PositionBuffer);
                        glVertexAttribPointer(
                            0,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0, (void *)0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, LOD->NormalBuffer);
                        glVertexAttribPointer(
                            1,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            0, (void *)0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, LOD->UVBuffer);
                        glVertexAttribPointer(
                            2,
                            2,
                            GL_FLOAT,
                            GL_FALSE,
                            0, (void *)0);
                        
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LOD->FaceGroups[FgIdx].ElementBuffer);
                        
                        glDrawElements(GL_TRIANGLES, LOD->FaceGroups[FgIdx].FaceCount*3, GL_UNSIGNED_SHORT, (void *)0);
                    }
                    glDisableVertexAttribArray(2);
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