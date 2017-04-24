// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_MESH_H)

#define F3D_MESH_MAX 128*1024

typedef struct
{
    char* Name;
    b32 Used;
    
    GLuint ArrayBuffers[VBO_Count];
    GLuint ElemBuffer;
    
    struct _Vertices
    {
        ms PositionsSize;
        real32 *Positions;
        
        ms NormalsSize;
        real32 *Normals;
        
        ms TexCoordsSize;
        real32 *TexCoords;
    } Vertices;
    
    ms ElementsSize;
    u16 *Elements;
} render_mesh;

global_variable render_mesh  GlobalMeshes[F3D_MESH_MAX] = {};

internal render_mesh *
MeshRegister(char *Name)
{
    s32 Slot = -1;
    for(s32 Idx = 0;
        Idx < F3D_MESH_MAX;
        ++Idx)
    {
        if(!GlobalMeshes[Idx].Used && Slot == -1)
        {
            Slot = Idx;
        }
        
        if(StringsAreEqual(GlobalMeshes[Idx].Name, Name))
        {
            Slot = Idx;
            break;
        }
    }
    
    render_mesh *Mesh = GlobalMeshes + Slot;
    
    if(Mesh->Used)
    {
        return(Mesh);
    }
    
    render_mesh Mesh_ = {};
    *Mesh = Mesh_;
    
    Mesh->Used = 1;
    
    return(Mesh);
}

internal void
MeshBuild(render_mesh *Mesh, b32 GenerateNormals)
{
    glGenBuffers(1, &Mesh->ArrayBuffers[VBO_Position]);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_Position]);
    glBufferData(GL_ARRAY_BUFFER, Mesh->Vertices.PositionsSize, Mesh->Vertices.Positions, GL_STATIC_DRAW);

    glGenBuffers(1, &Mesh->ArrayBuffers[VBO_UV]);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_UV]);
    glBufferData(GL_ARRAY_BUFFER, Mesh->Vertices.TexCoordsSize, Mesh->Vertices.TexCoords, GL_STATIC_DRAW);

    // NOTE(ZaKlaus): Generate normals
    
    if(GenerateNormals)
    {
        for(u32 Idx = 0;
            Idx < Mesh->ElementsSize;
            Idx += 3)
        {
            u16 i0 = Mesh->Elements[Idx];
            u16 i1 = Mesh->Elements[Idx+1];
            u16 i2 = Mesh->Elements[Idx+2];
            
            glm::vec3 v1 = 
                glm::vec3(Mesh->Vertices.Positions[i1],
                          Mesh->Vertices.Positions[i1+1],
                          Mesh->Vertices.Positions[i1+2]) -
                glm::vec3(Mesh->Vertices.Positions[i0],
                          Mesh->Vertices.Positions[i0+1],
                          Mesh->Vertices.Positions[i0+2]);
            
            glm::vec3 v2 = 
                glm::vec3(Mesh->Vertices.Positions[i2],
                          Mesh->Vertices.Positions[i2+1],
                          Mesh->Vertices.Positions[i2+2]) -
                glm::vec3(Mesh->Vertices.Positions[i0],
                          Mesh->Vertices.Positions[i0+1],
                          Mesh->Vertices.Positions[i0+2]);
            
            glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
            
            // i0
            {
                Mesh->Vertices.Normals[i0] = normal.x;
                Mesh->Vertices.Normals[i0+1] = normal.y;
                Mesh->Vertices.Normals[i0+2] = normal.z;
            }
            
            // i1
            {
                Mesh->Vertices.Normals[i1] = normal.x;
                Mesh->Vertices.Normals[i1+1] = normal.y;
                Mesh->Vertices.Normals[i1+2] = normal.z;
            }
            
            // i2
            {
                Mesh->Vertices.Normals[i2] = normal.x;
                Mesh->Vertices.Normals[i2+1] = normal.y;
                Mesh->Vertices.Normals[i2+2] = normal.z;
            }
        }
    }
    
    glGenBuffers(1, &Mesh->ArrayBuffers[VBO_Normal]);
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_Normal]);
    glBufferData(GL_ARRAY_BUFFER, Mesh->Vertices.NormalsSize, Mesh->Vertices.Normals, GL_STATIC_DRAW);
    
    glGenBuffers(1, &Mesh->ElemBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->ElemBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh->ElementsSize, Mesh->Elements, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

internal void
MeshDraw(render_mesh *Mesh)
{
    for(s32 Idx = 0;
        Idx < VBO_Count;
        ++Idx)
    {
        glEnableVertexAttribArray(Idx);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_Position]);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0, (void *)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_Normal]);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        0, (void *)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, Mesh->ArrayBuffers[VBO_UV]);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        0, (void *)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh->ElemBuffer);
    
    glDrawElements(GL_QUADS, Mesh->ElementsSize, GL_UNSIGNED_SHORT, (void *)0);
    
    for(s32 Idx = 0;
        Idx < VBO_Count;
        ++Idx)
    {
        glDisableVertexAttribArray(Idx);
    }
}

#define F3D_RENDER_MESH_H
#endif