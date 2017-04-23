// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_4DS_H)

#include "formats/hformat_4ds.h"

#include "f3d_render.h"

#define F3D_MODEL_4DS_MAX 128*1024

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
    
    AssetLoadInternal(Render->Asset);
    s32 FileHandle = AssetOpenHandle(Render->Asset, 1);//IOFileOpenRead((s8 *)Render->Asset->FilePath, 0);
    
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
                TextureLoad(Texture, GL_NEAREST);
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
                    
                    if(!Render->BBoxSet)
                    {
                        Render->BBoxSet = 1;
                        
                        aabb BBox = {};
                        
                        glm::vec4 Min, Max;
                        Min.w = Max.w = 1;
                        for(s32 PosIdx = 0;
                            PosIdx < LOD->VertexCount;
                            ++PosIdx)
                        {
                            if(HLOD->Vertices[PosIdx].Pos.X < Min.x)
                            {
                                Min.x = HLOD->Vertices[PosIdx].Pos.X;
                            }
                            
                            if(HLOD->Vertices[PosIdx].Pos.Y < Min.y)
                            {
                                Min.y = HLOD->Vertices[PosIdx].Pos.Y;
                            }
                            
                            if(HLOD->Vertices[PosIdx].Pos.Z < Min.z)
                            {
                                Min.z = HLOD->Vertices[PosIdx].Pos.Z;
                            }
                            
                            if(HLOD->Vertices[PosIdx].Pos.X > Max.x)
                            {
                                Max.x = HLOD->Vertices[PosIdx].Pos.X;
                            }
                            
                            if(HLOD->Vertices[PosIdx].Pos.Y > Max.y)
                            {
                                Max.y = HLOD->Vertices[PosIdx].Pos.Y;
                            }
                            
                            if(HLOD->Vertices[PosIdx].Pos.Z > Max.z)
                            {
                                Max.z = HLOD->Vertices[PosIdx].Pos.Z;
                            }
                        }
                        
                        BBox.Min = Min;
                        BBox.Max = Max;
                        
                        if((Render->BBox.Min.x > BBox.Min.x ||
                            Render->BBox.Min.y > BBox.Min.y ||
                            Render->BBox.Min.z > BBox.Min.z))
                        {
                            Render->BBox.Min = BBox.Min;
                        }
                        
                        if(Render->BBox.Max.x < BBox.Max.x ||
                           Render->BBox.Max.y < BBox.Max.y ||
                           Render->BBox.Max.z < BBox.Max.z)
                        {
                            Render->BBox.Max = BBox.Max;
                        }
                    }
                    
                    glGenBuffers(1, &LOD->Buffers[VBO_Position]);
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_Position]);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(v3)*LOD->VertexCount, Positions, GL_STATIC_DRAW);
                    
                    PlatformMemFree(Positions);
                    
                    v3 *Normals = (v3 *)PlatformMemAlloc(sizeof(v3)*LOD->VertexCount);
                    
                    for(s32 PosIdx = 0;
                        PosIdx < LOD->VertexCount;
                        ++PosIdx)
                    {
                        Normals[PosIdx] = HLOD->Vertices[PosIdx].Normal;
                    }
                    
                    glGenBuffers(1, &LOD->Buffers[VBO_Normal]);
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_Normal]);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(v3)*LOD->VertexCount, Normals, GL_STATIC_DRAW);
                    
                    PlatformMemFree(Normals);
                    
                    
                    v2 *UV = (v2 *)PlatformMemAlloc(sizeof(v2)*LOD->VertexCount);
                    
                    for(s32 PosIdx = 0;
                        PosIdx < LOD->VertexCount;
                        ++PosIdx)
                    {
                        UV[PosIdx] = HLOD->Vertices[PosIdx].UV;
                    }
                    
                    glGenBuffers(1, &LOD->Buffers[VBO_UV]);
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_UV]);
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

internal render_transform_result
Model4DSGetTransform(render_4ds_mesh *Mesh, render_4ds *Render)
{
#if 1
    render_transform_result Result;
    Result.Position = Mesh->Pos;
    
    glm::mat4 Pos = glm::translate(Mesh->Pos);
    glm::quat Quat = glm::quat(Mesh->Rot.w, Mesh->Rot.x, Mesh->Rot.y, Mesh->Rot.z);
    glm::mat4 Rot = glm::toMat4(Quat);
    glm::mat4 Scale = glm::scale(Mesh->Scale);
    
    glm::mat4 Model = Pos * Rot * Scale;
    Result.Transform = Model;
    
    if(Mesh->ParentID)
    {
        render_transform_result Parent = Model4DSGetTransform((Render->Meshes + Mesh->ParentID - 1), Render);
        Result.Transform = Parent.Transform * Result.Transform;
        Result.Position = Parent.Position * Result.Position;
    }
    return(Result);
#endif
}

internal render_4ds *
Model4DSRender(render_4ds *Render, GLuint Program, glm::mat4 Transform, s32 RenderType, b32 CheckFrustum)
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
            RenderCheckUniforms(Program);
            
            for(s32 Idx = 0;
                Idx < Render->MeshCount;
                ++Idx)
            {
                render_4ds_mesh *Mesh = &Render->Meshes[Idx];
                render_4ds_lod *LOD = &Render->Meshes[Idx].LODs[0];
                
                if(!LOD)
                {
                    continue;
                }
                
                s32 Cdx = Idx;
                while(!LOD && Render->Meshes[Cdx].InstanceOf)
                {
                    Cdx = Render->Meshes[Cdx].InstanceOf - 1;
                    LOD = &Render->Meshes[Cdx].LODs[Mesh->LODLevel-1];
                }
                
                glm::mat4 T = Transform;
                
                glm::mat4 Model = Mesh->Transform;
                if(!Mesh->Transformed)
                {
                    render_transform_result Tr = Model4DSGetTransform(Mesh, Render);
                    local_persist glm::mat4 FlipZY = glm::scale(glm::vec3(-1,1,1));
                    Model = Mesh->Transform = FlipZY * Tr.Transform;
                    Mesh->Position = Tr.Position;
                    Mesh->Transformed = 1;
                }
                
                
                Model = T * Mesh->Transform;
                
                glm::mat4 M = Model;
                
                /*
                s32 LODAttempts = 0;
                while(LOD->RelativeDistance > sqrt(pow(Mesh->Position.x - Camera->Position.x,2.f)
                                                   +pow(Mesh->Position.y - Camera->Position.y,2.f)
                                                   +pow(Mesh->Position.z - Camera->Position.z,2.f)) && LODAttempts < Mesh->LODLevel)
                {
                    LOD = &Mesh->LODs[++LODAttempts];
                }*/
                
                glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &M[0][0]);
                
                for(s32 FgIdx = 0;
                    FgIdx < LOD->FaceGroupCount;
                    ++FgIdx)
                {
                    s32 MatIdx = MathMAX(0,LOD->FaceGroups[FgIdx].MaterialIndex - 1);
                    
                    RenderApplyMaterial(&Render->Materials[MatIdx].Material, Program);
                    
                    for(s32 Idx = 0;
                        Idx < VBO_Count;
                        ++Idx)
                    {
                        glEnableVertexAttribArray(Idx);
                    }
                    
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_Position]);
                    glVertexAttribPointer(
                        0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0, (void *)0);
                    
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_Normal]);
                    glVertexAttribPointer(
                        1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0, (void *)0);
                    
                    glBindBuffer(GL_ARRAY_BUFFER, LOD->Buffers[VBO_UV]);
                    glVertexAttribPointer(
                        2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        0, (void *)0);
                    
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LOD->FaceGroups[FgIdx].ElementBuffer);
                    
                    glDrawElements(GL_TRIANGLES, LOD->FaceGroups[FgIdx].FaceCount*3, GL_UNSIGNED_SHORT, (void *)0);
                    
                    for(s32 Idx = 0;
                        Idx < VBO_Count;
                        ++Idx)
                    {
                        glDisableVertexAttribArray(Idx);
                    }
                }
            }
            
        }break;
    }
    return(Render);
}

#define F3D_RENDER_4DS_H
#endif