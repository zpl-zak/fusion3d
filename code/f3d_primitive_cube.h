// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_PRIMITIVE_CUBE_H)

global_variable GLuint GlobalPrimitiveCubeBuffers[VBO_Count] = {0};

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
    glGenBuffers(1, &GlobalPrimitiveCubeBuffers[VBO_Position]);
    glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_Position]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlobalCubePositions), GlobalCubePositions, GL_STATIC_DRAW);

    glGenBuffers(1, &GlobalPrimitiveCubeBuffers[VBO_UV]);
    glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_UV]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlobalCubeTexCoords), GlobalCubeTexCoords, GL_STATIC_DRAW);

    // NOTE(ZaKlaus): Generate normals
    for(u32 Idx = 0;
        Idx < sizeof(GlobalCubeElements);
        Idx += 3)
    {
        u16 i0 = GlobalCubeElements[Idx];
        u16 i1 = GlobalCubeElements[Idx+1];
        u16 i2 = GlobalCubeElements[Idx+2];
        
        glm::vec3 v1 = 
            glm::vec3(GlobalCubePositions[i1],
                      GlobalCubePositions[i1+1],
                      GlobalCubePositions[i1+2]) -
            glm::vec3(GlobalCubePositions[i0],
                      GlobalCubePositions[i0+1],
                      GlobalCubePositions[i0+2]);
        
        glm::vec3 v2 = 
            glm::vec3(GlobalCubePositions[i2],
                      GlobalCubePositions[i2+1],
                      GlobalCubePositions[i2+2]) -
            glm::vec3(GlobalCubePositions[i0],
                      GlobalCubePositions[i0+1],
                      GlobalCubePositions[i0+2]);
        
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        
        // i0
        {
            GlobalCubeNormals[i0] = normal.x;
            GlobalCubeNormals[i0+1] = normal.y;
            GlobalCubeNormals[i0+2] = normal.z;
        }
        
        // i1
        {
            GlobalCubeNormals[i1] = normal.x;
            GlobalCubeNormals[i1+1] = normal.y;
            GlobalCubeNormals[i1+2] = normal.z;
        }
        
        // i2
        {
            GlobalCubeNormals[i2] = normal.x;
            GlobalCubeNormals[i2+1] = normal.y;
            GlobalCubeNormals[i2+2] = normal.z;
        }
    }
    
    glGenBuffers(1, &GlobalPrimitiveCubeBuffers[VBO_Normal]);
    glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_Normal]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GlobalCubeNormals), GlobalCubeNormals, GL_STATIC_DRAW);
    
    glGenBuffers(1, &GlobalPrimitiveCubeBuffers[3]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GlobalCubeElements), GlobalCubeElements, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

internal void
PrimitiveCubeRender(render_material *Mat,
                    GLuint Program,
                    glm::mat4 Transform,
                    s32 RenderType)
{
    if(!GlobalPrimitiveCubeBuffers[VBO_Position])
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
            
            for(s32 Idx = 0;
                Idx < VBO_Count;
                ++Idx)
            {
                glEnableVertexAttribArray(Idx);
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_Position]);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0, (void *)0);
            
            glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_Normal]);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                0, (void *)0);
            
            glBindBuffer(GL_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[VBO_UV]);
            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                0, (void *)0);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GlobalPrimitiveCubeBuffers[3]);
            
            glDrawElements(GL_QUADS, sizeof(GlobalCubeElements), GL_UNSIGNED_SHORT, (void *)0);
            
            for(s32 Idx = 0;
                Idx < VBO_Count;
                ++Idx)
            {
                glDisableVertexAttribArray(Idx);
            }
        }break;
    }
}

#define F3D_PRIMITIVE_CUBE_H
#endif