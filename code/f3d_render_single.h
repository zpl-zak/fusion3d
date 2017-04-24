// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_SINGLE_H)

#define F3D_SINGLE_MAX 128*1024

typedef struct render_single_
{
    render_4ds *Render;
    b32 CheckFrustum;
    render_transform Transform;
    glm::mat4 TransformMatrix;
glm::mat4 LocalTransformMatrix;
    aabb BBox;
    
    b32 Loaded;
    b32 Visible;
} render_single;

global_variable render_single GlobalSingle[F3D_SINGLE_MAX] = {0};

internal void
RenderSingleAdd(render_4ds *Render, render_transform Transform, b32 CheckFrustum)
{
    s32 Slot = -1;
    for(s32 Idx=0;
        Idx < F3D_SINGLE_MAX;
        ++Idx)
    {
        if(!GlobalSingle[Idx].Loaded)
        {
            Slot = Idx;
            break;
        }
    }
    
    Assert(Slot != -1);
    
    render_single *Node = GlobalSingle + Slot;
    
    Node->Render = Render;
    Node->Transform = Transform;
    Node->TransformMatrix = RenderTransformMatrix(Transform);
    Node->CheckFrustum = CheckFrustum;
    Node->Loaded = 1;
    Node->BBox = Render->BBox;
    
    render_transform_result Tr = Model4DSGetTransform(Render->Meshes, Render);
        
    Node->LocalTransformMatrix = Tr.Transform;
}

internal void
RenderSingleCull(s32 StartIdx, s32 NumIndices)
{
    for(s32 Idx = StartIdx;
        Idx < NumIndices;
        ++Idx)
    {
        render_single *N = (GlobalSingle + Idx);
        
        if(N->Loaded && N->CheckFrustum)
        {
            aabb tbbox = N->BBox;
            const glm::mat4 M = N->TransformMatrix;
            const glm::mat4 LM = N->LocalTransformMatrix;
            
            tbbox.Min = M * tbbox.Min;
            tbbox.Max = M * tbbox.Max;
            N->Visible = FrustumCheckAABB(tbbox);
        }
        else if(N->Loaded)
        {
            N->Visible = 1;
        }
    }
}


internal void
DEBUGRenderSingleViz(aabb BBox, GLuint Program, glm::mat4 TransformMatrix, glm::mat4 LocalTransformMatrix)
{
    {
#if 1
        glLineWidth(2);
        GLfloat vertices[] = {
            -0.5, -0.5, -0.5, 1.0,
            0.5, -0.5, -0.5, 1.0,
            0.5,  0.5, -0.5, 1.0,
            -0.5,  0.5, -0.5, 1.0,
            -0.5, -0.5,  0.5, 1.0,
            0.5, -0.5,  0.5, 1.0,
            0.5,  0.5,  0.5, 1.0,
            -0.5,  0.5,  0.5, 1.0,
        };
        
        GLuint vbo_vertices;
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        GLushort elements[] = {
            0, 1, 2, 3,
            4, 5, 6, 7,
            0, 4, 1, 5, 2, 6, 3, 7
        };
        GLuint ibo_elements;
        glGenBuffers(1, &ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        
        GLfloat
            min_x, max_x,
        min_y, max_y,
        min_z, max_z;
        
        min_x = BBox.Min.x;
        min_y = BBox.Min.y;
        min_z = BBox.Min.z;
        max_x = BBox.Max.x;
        max_y = BBox.Max.y;
        max_z = BBox.Max.z;
        
        glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
        glm::vec3 center = glm::vec3((min_x+max_x)/2.f, (min_y+max_y)/2.f, (min_z+max_z)/2.f);
        glm::mat4 transform = glm::scale(glm::translate(TransformMatrix*LocalTransformMatrix, center), size);
        
//        transform = transform * LocalTransformMatrix;
        
        /* Apply object's transformation matrix */
        glm::mat4 m = transform;
        GLuint gMatrix = glGetUniformLocation(Program, "m");
        glUniformMatrix4fv(gMatrix, 1, GL_FALSE, glm::value_ptr(m));
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,  // attribute
            4,                  // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
            );
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vbo_vertices);
        glDeleteBuffers(1, &ibo_elements);
#endif
    }
}


internal void
RenderSingleDraw(s32 StartIdx, s32 NumIndices, GLuint Program, s32 RenderType)
{
    for(s32 Idx = StartIdx;
        Idx < NumIndices;
        ++Idx)
    {
         render_single *N = (GlobalSingle + Idx);
        
        if(N->Loaded && N->Visible)
        {
            Model4DSRender(N->Render, Program, N->TransformMatrix, RenderType, N->CheckFrustum); 
            //DEBUGRenderSingleViz(N->Render->BBox, Program, N->TransformMatrix, N->LocalTransformMatrix);
        }
    }
}

#define F3D_RENDER_SINGLE_H
#endif