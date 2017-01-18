#if !defined(F3D_RENDER_4DS_PROTO)

#include "formats/hformat_4ds.h"

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
    glm::vec3 Position;
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
    
    b32 BBoxSet;
    aabb BBox;
    
    hformat_4ds_header *Header;
} render_4ds;

typedef struct
{
    s32 Min, Max;
} render_mesh_volume;

internal render_4ds *
Model4DSRender(render_4ds *Render, GLuint Program, camera *Camera, render_transform Transform, s32 RenderType, b32 CheckFrustum);

#define F3D_RENDER_4DS_PROTO
#endif