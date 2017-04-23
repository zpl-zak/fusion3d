// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_PRIMITIVE_CUBE_H)

global_variable render_mesh *GlobalCube = 0;

global_variable real32 GlobalCubePositions[] =
{
    -1.f, -1.f,-1.f,
    1.f, -1.f, -1.f,
    1.f, 1.f,  -1.f,
    -1.f, 1.f, -1.f,
    
    -1.f, -1.f, 1.f,
    1.f, -1.f,  1.f,
    1.f, 1.f,   1.f,
    -1.f, 1.f,  1.f
};

global_variable real32 GlobalCubeTexCoords[] =
{
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f,
    1.f, 0.f,
    0.f, 1.f,
    0.f, 0.f,
    1.f, 1.f,
    1.f, 1.f,
    1.f, 1.f,
    1.f, 1.f,
    1.f, 1.f
};

global_variable real32 GlobalCubeNormals[sizeof(GlobalCubePositions)];

global_variable u16 GlobalCubeElements[] =
{
    // back
    0, 1, 2, 3,
    
    // front
    4, 5, 6, 7,
    
    // right
    0, 4, 7, 3,
    
    // left
    1, 5, 6, 2,
    
    // top
    2, 6, 7, 3,
    
    // bottom
    0, 4, 5, 1
};

internal void
PrimitiveCubeBuild(void)
{
    GlobalCube = MeshRegister("_Default_Cube");

    GlobalCube->Vertices.PositionsSize = sizeof(GlobalCubePositions);
    GlobalCube->Vertices.Positions = GlobalCubePositions;
    
    GlobalCube->Vertices.NormalsSize = sizeof(GlobalCubeNormals);
    GlobalCube->Vertices.Normals = GlobalCubeNormals;
    
    GlobalCube->Vertices.TexCoordsSize = sizeof(GlobalCubeTexCoords);
    GlobalCube->Vertices.TexCoords = GlobalCubeTexCoords;
    
    GlobalCube->ElementsSize = sizeof(GlobalCubeElements);
    GlobalCube->Elements = GlobalCubeElements;
    
    MeshBuild(GlobalCube);
}

internal void
PrimitiveCubeDraw(render_material *Mat,
                    GLuint Program,
                    glm::mat4 Transform,
                    s32 RenderType)
{
    if(!GlobalCube)
    {
        PrimitiveCubeBuild();
    }
    
    RenderCheckUniforms(Program);
    
    switch(RenderType)
    {
        case ModelRenderType_Normal:
        {
            glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &Transform[0][0]);
            
            RenderApplyMaterial(Mat, Program);
            
            MeshDraw(GlobalCube);
        }break;
    }
}

#define F3D_PRIMITIVE_CUBE_H
#endif