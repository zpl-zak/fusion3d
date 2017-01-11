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
    
    u8 FaceGroupCount;
    render_4ds_facegroup *FaceGroups;
} render_4ds_lod;

typedef struct
{
    u8 LODLevel;
    render_4ds_lod *LODs;
} render_4ds_mesh;

typedef struct
{
    asset_file *Asset;
    
    u16 MeshCount;
    render_4ds_mesh *Meshes;
    
    hformat_4ds_header *Header;
} render_4ds;

global_variable render_4ds GlobalModel4DS[F3D_MODEL_4DS_MAX] = {0};

internal render_4ds *
F3DModel4DSRegister(char *Name, char *FileName)
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
    asset_file *Asset = F3DAssetRegister(Name, Temp, Asset_Model);
    
    Render->Asset = Asset;
    
    return(Render);
}

internal render_4ds *
F3DModel4DSLoad(render_4ds *Render)
{
    Assert(Render && Render->Asset);
    
    if(Render->Header)
    {
        return(Render);
    }
    
    s32 FileHandle = IOFileOpenRead(Render->Asset->FilePath, 0);
    
    Render->Header = HFormatLoad4DSModel(FileHandle);
    {
        Render->MeshCount = Render->Header->MeshCount;
        
        Render->Meshes = PlatformMemAlloc(sizeof(render_4ds_mesh)*Render->MeshCount);
        
        for(s32 Idx = 0;
            Idx < Render->MeshCount;
            ++Idx)
        {
            hformat_4ds_mesh *HMesh = Render->Header->Meshes + Idx;
            if(HMesh->VisualMeshType != HFormat4DSVisualMeshType_Standard)
            {
                continue;
            }
            
            render_4ds_mesh Mesh_ = {0};
            render_4ds_mesh *Mesh = Render->Meshes + Idx;
            hformat_4ds_standard *Standard = &HMesh->Standard;
            *Mesh = Mesh_;
            
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
                
                s32 HPosIdx = 0;
                for(s32 PosIdx = 0;
                    PosIdx < LOD->VertexCount;
                    ++PosIdx, ++HPosIdx)
                {
                    Positions[PosIdx] = HLOD->Vertices[HPosIdx].Pos;
                }
                
                glGenBuffers(1, &LOD->PositionBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, LOD->PositionBuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(v3)*LOD->VertexCount, Positions, GL_STATIC_DRAW);
                
                PlatformMemFree(Positions);
                
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
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hformat_4ds_face)*FaceCount*sizeof(u16), Faces, GL_STATIC_DRAW);
                    
                    PlatformMemFree(Faces);
                }
            }
        }
    }
    IOFileClose(FileHandle);
    
    return(Render);
}

internal render_4ds *
F3DModel4DSRender(render_4ds *Render, GLuint Program, s32 RenderType)
{
    glUseProgram(Program);
    
    switch(RenderType)
    {
        case ModelRenderType_Normal:
        {
            // NOTE(zaklaus): Test code
            m4 Projection = MathPerspective(45.f, 4.f / 3.f, .1f, 1000.f);
            
            v3 Pos = {6,6,-6};
            v3 Target = {0};
            v3 Up = {0, 1, 0};
            
            m4 View = MathLookAt(Pos, Target, Up);
            m4 Model = MathMat4d(1.f);
            
            mat4 MVP = MathMultiplyMat4(Projection, View);   
            MVP = MathMultiplyMat4(MVP, Model);
            
            GLuint Matrix = glGetUniformLocation(Program, "mvp");
            glUniformMatrix4fv(Matrix, 1, GL_FALSE, &MVP.Elements[0][0]);
            
            // Let's pretend the model has single mesh, single LOD and single face group.
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            {
                glBindBuffer(GL_ARRAY_BUFFER, Render->Meshes[0].LODs[0].PositionBuffer);
                glVertexAttribPointer(
                    0,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    0, (void *)0);
                glVertexAttribPointer(
                    1,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    0, (void *)0);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Render->Meshes[0].LODs[0].FaceGroups[0].ElementBuffer);
                
                glDrawElements(GL_TRIANGLES, Render->Meshes[0].LODs[0].FaceGroups[0].FaceCount*3, GL_UNSIGNED_SHORT, (void *)0);
            }
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(0);
        }break;
    }
    return(Render);
}

#define F3D_RENDER_4DS_H
#endif