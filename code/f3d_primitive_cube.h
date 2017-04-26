// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_PRIMITIVE_CUBE_H)

global_variable render_mesh *GlobalCube = 0;

global_variable real32 GlobalCubePositions[] =
{
    // front
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // top
    -1.0,  1.0,  1.0,
     1.0,  1.0,  1.0,
     1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0,
    // back
     1.0, -1.0, -1.0,
    -1.0, -1.0, -1.0,
    -1.0,  1.0, -1.0,
     1.0,  1.0, -1.0,
    // bottom
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0, -1.0,  1.0,
    -1.0, -1.0,  1.0,
    // left
    -1.0, -1.0, -1.0,
    -1.0, -1.0,  1.0,
    -1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0,
    // right
     1.0, -1.0,  1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
     1.0,  1.0,  1.0,
};

global_variable real32 GlobalCubeTexCoords[2*4*6] =
{
    // front
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
};

global_variable real32 GlobalCubeNormals[sizeof(GlobalCubePositions)] = {0};

global_variable u16 GlobalCubeElements[] =
{
    // front
    0,  1,  2,
    2,  3,  0,
    // top
    4,  5,  6,
    6,  7,  4,
    // back
    8,  9, 10,
    10, 11,  8,
    // bottom
    12, 13, 14,
    14, 15, 12,
    // left
    16, 17, 18,
    18, 19, 16,
    // right
    20, 21, 22,
    22, 23, 20,
};

internal void
PrimitiveCubeBuild(void)
{
    GlobalCube = MeshRegister("_Default_Cube");

    GlobalCube->Vertices.PositionsSize = sizeof(GlobalCubePositions);
    GlobalCube->Vertices.Positions = GlobalCubePositions;
    
    GlobalCube->Vertices.NormalsSize = sizeof(GlobalCubePositions);
    GlobalCube->Vertices.Normals = GlobalCubeNormals;
    
    for(s32 Idx = 1;
        Idx < 6;
        ++Idx)
    {
        Copy(2*4*sizeof(real32), &GlobalCubeTexCoords[0], &GlobalCubeTexCoords[Idx*4*2]);
    }
    
    GlobalCube->Vertices.TexCoordsSize = sizeof(GlobalCubeTexCoords);
    GlobalCube->Vertices.TexCoords = GlobalCubeTexCoords;
    
    GlobalCube->ElementsSize = sizeof(GlobalCubeElements);
    GlobalCube->Elements = GlobalCubeElements;
    
    //TRAP();
    
    MeshBuild(GlobalCube, 1);
}

internal void
PrimitiveCubeDraw(render_material *Mat,
                  GLuint Program,
                  glm::mat4 Transform)
{
    if(!GlobalCube)
    {
        PrimitiveCubeBuild();
    }
    
    
    
    /*
    RenderCheckUniforms(Program);
    
    glUniformMatrix4fv(gMatrixM, 1, GL_FALSE, &Transform[0][0]);
    
    RenderApplyMaterial(Mat, Program);
    
    MeshDraw(GlobalCube);
    
    */
    
    RenderAddQuery(RenderPass_Color, GlobalCube, Mat, Transform, Program);
}

#define F3D_PRIMITIVE_CUBE_H
#endif