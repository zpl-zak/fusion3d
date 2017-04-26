// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_PRIMITIVE_PLANE_H)

global_variable render_mesh *GlobalPlane = 0;

global_variable real32 GlobalPlanePositions[] =
{
    // bottom
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0, -1.0,  1.0,
    -1.0, -1.0,  1.0
};

global_variable real32 GlobalPlaneTexCoords[] =
{
    // bottom
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
};

global_variable real32 GlobalPlaneNormals[sizeof(GlobalPlanePositions)] = {0};

global_variable u16 GlobalPlaneElements[] =
{    
    0, 3, 2,
    2, 1, 0
};

internal void
PrimitivePlaneBuild(void)
{
    GlobalPlane = MeshRegister("_Default_Plane");

    GlobalPlane->Vertices.PositionsSize = sizeof(GlobalPlanePositions);
    GlobalPlane->Vertices.Positions = GlobalPlanePositions;
    
    GlobalPlane->Vertices.NormalsSize = sizeof(GlobalPlanePositions);
    GlobalPlane->Vertices.Normals = GlobalPlaneNormals;
    
    GlobalPlane->Vertices.TexCoordsSize = sizeof(GlobalPlaneTexCoords);
    GlobalPlane->Vertices.TexCoords = GlobalPlaneTexCoords;
    
    GlobalPlane->ElementsSize = sizeof(GlobalPlaneElements);
    GlobalPlane->Elements = GlobalPlaneElements;
    
    MeshBuild(GlobalPlane, 1);
}

internal void
PrimitivePlaneDraw(render_material *Mat,
                  GLuint Program,
                  glm::mat4 Transform)
{
    if(!GlobalPlane)
    {
        PrimitivePlaneBuild();
    }    
    RenderAddQuery(RenderPass_Color, GlobalPlane, Mat, Transform, Program);
}

#define F3D_PRIMITIVE_PLANE_H
#endif