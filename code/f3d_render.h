// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_H)

typedef void light_pass_func(GLuint program);

enum
{
    ModelRenderType_Normal,
};

typedef struct
{
    glm::vec3 Pos;
     glm::vec4 Rot;
    glm::vec3 Scale;
} render_transform;

typedef struct
{
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    render_texture *DiffTexture;
    b32 ColorKey;
    b32 DoubleSided;
    
    GLuint AmbLoc;
    GLuint DiffLoc;
    GLuint DiffTex;
    GLuint ColLoc;
} render_material;

typedef struct
{
    glm::vec3 Pos;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    
    real32 Constant;
    real32 Linear;
    real32 Quadratic;
    
    GLuint PosLoc;
    GLuint AmbLoc;
    GLuint DiffLoc;
    
    // NOTE(zaklaus): Attenuation
    GLuint CLoc;
    GLuint LLoc;
    GLuint QLoc;
} render_light_point;

typedef struct
{
    glm::vec3 Dir;
    glm::vec3 Ambient;
    glm::vec3 Diffuse;
    
    GLuint DirLoc;
    GLuint AmbLoc;
    GLuint DiffLoc;
} render_light_dir;

typedef struct
{
    glm::vec4 Min,Max;
} aabb;

#include "f3d_render_4ds_proto.h"

typedef struct render_octree_node_
{
    render_4ds *Render;
    render_transform Transform;
    aabb TBBox;
    struct render_octree_node_ *Next;
} render_octree_node;

typedef struct render_octree_
{
    s32 NodeCount;
    render_octree_node *Node;
    
    aabb BBox;
    glm::vec4 Center;
    r32 Radius;
    
    struct render_octree_ *Branches[8];
} render_octree;

typedef struct render_single_
{
    render_4ds *Render;
    b32 CheckFrustum;
    render_transform Transform;
    
    struct render_single_ *Next;
} render_single;

#include "f3d_frustum.h"

global_variable render_octree GlobalWorld = {0};
global_variable render_single *GlobalSingle = 0;

internal render_transform
RenderTransform(void)
{
    render_transform Result;
    Result.Scale.x = 1.f;
    Result.Scale.y = 1.f;
    Result.Scale.z = 1.f;
    
    return(Result);
}

internal glm::mat4
RenderTransformMatrix(render_transform Transform)
{
    glm::mat4 T;
    {
        glm::mat4 Pos = glm::translate(Transform.Pos);
        glm::quat Quat = glm::quat(Transform.Rot.w, Transform.Rot.x, Transform.Rot.y, Transform.Rot.z);
        glm::mat4 Rot = glm::toMat4(Quat);
        glm::mat4 Scale = glm::scale(Transform.Scale);
        
        T = Pos * Rot * Scale;
    }
    return(T);
}

internal void
RenderApplyMaterial(render_material *Mat, GLuint Program)
{
    if(!Mat->AmbLoc)
    {
    Mat->AmbLoc = glGetUniformLocation(Program, "material.ambient");Mat->DiffLoc = glGetUniformLocation(Program, "material.diffuse");
        Mat->DiffTex = glGetUniformLocation(Program, "material.difftex");
        Mat->ColLoc = glGetUniformLocation(Program, "material.colorkey");
    }
    
    glUniform3f(Mat->AmbLoc, Mat->Ambient.x, Mat->Ambient.y, Mat->Ambient.z);
    glUniform3f(Mat->DiffLoc, Mat->Diffuse.x, Mat->Diffuse.y, Mat->Diffuse.z);
    
    if(Mat->DiffTexture && Mat->DiffTexture->TextureObject)
    {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Mat->DiffTexture->TextureObject);
    glUniform1i(Mat->DiffTex, 0);
        glUniform1i(Mat->ColLoc, Mat->ColorKey);
    }
    
    if(Mat->DoubleSided)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }
}

internal void
RenderApplyLightDirectional(render_light_dir *Light, GLuint Program)
{
    glUseProgram(Program);
    
    if(!Light->AmbLoc)
    {
        Light->DirLoc = glGetUniformLocation(Program, "light.direction");
        Light->AmbLoc = glGetUniformLocation(Program, "light.ambient");Light->DiffLoc = glGetUniformLocation(Program, "light.diffuse");
    }
    
    glUniform3f(Light->DirLoc, Light->Dir.x, Light->Dir.y, Light->Dir.z);
    glUniform3f(Light->AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
    glUniform3f(Light->DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
    
}

internal void
RenderApplyLightPoint(s32 Index, render_light_point *Light, GLuint Program)
{
    glUseProgram(Program);
    
    if(!Light->AmbLoc)
    {
        char Temp[255] = {0};
        sprintf(Temp, "pointLight[%d].position", Index);
        Light->PosLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].ambient", Index);
        Light->AmbLoc = glGetUniformLocation(Program, Temp);
            sprintf(Temp, "pointLight[%d].diffuse", Index);Light->DiffLoc = glGetUniformLocation(Program, Temp);
        
        sprintf(Temp, "pointLight[%d].constant", Index);Light->CLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].linear", Index);Light->LLoc = glGetUniformLocation(Program, Temp);
        sprintf(Temp, "pointLight[%d].quadratic", Index);Light->QLoc = glGetUniformLocation(Program, Temp);
    }
    
    glUniform3f(Light->PosLoc, Light->Pos.x, Light->Pos.y, Light->Pos.z);
    glUniform3f(Light->AmbLoc, Light->Ambient.x, Light->Ambient.y, Light->Ambient.z);
    glUniform3f(Light->DiffLoc, Light->Diffuse.x, Light->Diffuse.y, Light->Diffuse.z);
    
    glUniform1f(Light->CLoc, Light->Constant);
    glUniform1f(Light->LLoc, Light->Linear);
    glUniform1f(Light->QLoc, Light->Quadratic);
}

internal void
RenderSingleAdd(render_4ds *Render, render_transform Transform, b32 CheckFrustum)
{
    render_single *Node = (render_single *)PlatformMemAlloc(sizeof(render_single));
    render_single Node_ = {0};
    *Node = Node_;
    
    Node->Render = Render;
    Node->Transform = Transform;
    Node->CheckFrustum = CheckFrustum;
    Node->Next = 0;
    
    if(!GlobalSingle)
    {
        GlobalSingle = Node;
    }
    else
    {
        for(render_single *N = GlobalSingle;
            N;
            N = N->Next)
        {
            if(!N->Next)
            {
            N->Next = Node;
                break;
            }
        }
    }
}

internal void
RenderSingleDraw(GLuint Program, camera *Camera, s32 RenderType)
{
    for(render_single *N = GlobalSingle;
        N;
        N = N->Next)
    {
        Model4DSRender(N->Render, Program, Camera, N->Transform, RenderType, N->CheckFrustum); 
    }
}

internal void
RenderOctreeAdd(render_4ds *Render, render_transform Transform)
{
    render_octree_node *Node = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
    render_octree_node Node_ = {};
    *Node = Node_;
    
    Node->Transform = Transform;
    Node->Render = Render;
    Node->Next = 0;
    
    glm::mat4 T = RenderTransformMatrix(Transform);
    
    aabb bbox = Render->BBox;
    
    bbox.Min = T * bbox.Min;
    bbox.Max = T * bbox.Max;
    
    Node->TBBox = bbox;
    
    if(!GlobalWorld.Node)
    {
        GlobalWorld.Node = Node;
    }
    else
    {
        for(render_octree_node *N = GlobalWorld.Node;
            N;
            N = N->Next)
        {
            if(!N->Next)
            {
                N->Next = Node;
                break;
            }
        }
    }
    
    ++GlobalWorld.NodeCount;
}

internal b32
RenderTestAABB(aabb a, aabb b)
{
    b32 x = (a.Min.x < b.Min.x) && (a.Max.x > b.Max.x);
    b32 y = (a.Min.y < b.Min.y) && (a.Max.y > b.Max.y);
    b32 z = (a.Min.z < b.Min.z) && (a.Max.z > b.Max.z);
    
    return(x && y && z);
}

internal void
RenderOctreeGenerateInternal(render_octree *Octree)
{
    aabb bbox = Octree->BBox;
    
    r32 wx = bbox.Max.x - bbox.Min.x;
    r32 wy = bbox.Max.y - bbox.Min.y;
    r32 wz = bbox.Max.z - bbox.Min.z;
    
    r32 hx = wx / 2.f;
    r32 hy = wy / 2.f;
    r32 hz = wz / 2.f;
    
    glm::vec4 Center = bbox.Max - glm::vec4(wx,wy,wz,1);
    
    real32 Radius = MathMAX(hx, MathMAX(hy, hz));
    
    Octree->Center = Center;
    Octree->Radius = Radius;
    
    if(Octree->NodeCount < 10)
    {
        return;
    }
    
    render_octree o_ = {};
    
    ///
    /// Branch 1 -- luf
    ///
    
    aabb o1box;
    o1box.Min = bbox.Min;
    o1box.Max = bbox.Max - glm::vec4(hx,hy,hz,1);
    
    render_octree_node *o1list = 0;
    
    render_octree *o1 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o1 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o1box, Node->TBBox))
        {
            ++o1->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o1list)
            {
                o1list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o1list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o1->Node = o1list;
    o1->BBox = o1box;
    
    Octree->Branches[0] = o1;
    
    ///
    /// Branch 2 -- ruf
    ///
    
    aabb o2box;
    o2box.Min = bbox.Min + glm::vec4(hx,0,0,1);
    o2box.Max = bbox.Max - glm::vec4(0,hy,hz,1);
    
    render_octree_node *o2list = 0;
    
    render_octree *o2 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o2 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o2box, Node->TBBox))
        {
            ++o2->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o2list)
            {
                o2list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o2list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o2->Node = o2list;
    o2->BBox = o2box;
    
    Octree->Branches[1] = o2;
    
    ///
    /// Branch 3 -- rdf
    ///
    
    aabb o3box;
    o3box.Min = bbox.Min + glm::vec4(hx,hy,0,1);
    o3box.Max = bbox.Max - glm::vec4(0,0,hz,1);
    
    render_octree_node *o3list = 0;
    
    render_octree *o3 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o3 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o3box, Node->TBBox))
        {
            ++o3->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o3list)
            {
                o3list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o3list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o3->Node = o3list;
    o3->BBox = o3box;
    
    Octree->Branches[2] = o3;
    
    ///
    /// Branch 4 -- ldf
    ///
    
    aabb o4box;
    o4box.Min = bbox.Min + glm::vec4(0,hy,0,1);
    o4box.Max = bbox.Max - glm::vec4(hx, 0, hz,1);
    
    render_octree_node *o4list = 0;
    
    render_octree *o4 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o4 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o4box, Node->TBBox))
        {
            ++o4->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o4list)
            {
                o4list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o4list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o4->Node = o4list;
    o4->BBox = o4box;
    
    Octree->Branches[3] = o4;
    
    ///
    /// Branch 5 -- lun
    ///
    
    aabb o5box;
    o5box.Min = bbox.Min + glm::vec4(0,0,hz,1);
    o5box.Max = bbox.Max - glm::vec4(hx,hz,0,1);
    
    render_octree_node *o5list = 0;
    
    render_octree *o5 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o5 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o5box, Node->TBBox))
        {
            ++o5->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o5list)
            {
                o5list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o5list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o5->Node = o5list;
    o5->BBox = o5box;
    
    Octree->Branches[4] = o5;
    
    ///
    /// Branch 6 -- run
    ///
    
    aabb o6box;
    o6box.Min = bbox.Min + glm::vec4(hx,0,hz,1);
    o6box.Max = bbox.Max - glm::vec4(0,hy,0,1);
    
    render_octree_node *o6list = 0;
    
    render_octree *o6 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o6 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o6box, Node->TBBox))
        {
            ++o6->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o6list)
            {
                o6list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o6list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o6->Node = o6list;
    o6->BBox = o6box;
    
    Octree->Branches[5] = o6;
    
    ///
    /// Branch 7 -- rdn
    ///
    
    aabb o7box;
    o7box.Min = bbox.Min + glm::vec4(hx,hy,hz,1);
    o7box.Max = bbox.Max;
    
    render_octree_node *o7list = 0;
    
    render_octree *o7 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o7 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o7box, Node->TBBox))
        {
            ++o7->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o7list)
            {
                o7list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o7list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o7->Node = o7list;
    o7->BBox = o7box;
    
    Octree->Branches[6] = o7;
    
    ///
    /// Branch 8 -- ldn
    ///
    
    aabb o8box;
    o8box.Min = bbox.Min + glm::vec4(0,hy,hz,1);
    o8box.Max = bbox.Max - glm::vec4(hx,0,0,1);
    
    render_octree_node *o8list = 0;
    
    render_octree *o8 = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    *o8 = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(o8box, Node->TBBox))
        {
            ++o8->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            
            if(!o8list)
            {
                o8list = NewNode;
            }
            else
            {
                for(render_octree_node *N = o8list;
                    N;
                    N = N->Next)
                {
                    if(!N->Next)
                    {
                        N->Next = NewNode;
                        break;
                    }
                }
            }
        }
    }
    
    o8->Node = o8list;
    o8->BBox = o8box;
    
    Octree->Branches[7] = o8;
    
    for(s32 Idx = 0;
        Idx < 7;
        Idx++)
    {
        RenderOctreeGenerateInternal(Octree->Branches[Idx]);
    }
}

internal void
RenderOctreeGenerate(void)
{
    aabb BBox = {};
    
    for(render_octree_node *Node = GlobalWorld.Node;
        Node;
        Node = Node->Next)
    {
        if(Node->TBBox.Min.x < BBox.Min.x)
        {
            BBox.Min.x = Node->TBBox.Min.x;
        }
        
        if(Node->TBBox.Min.y < BBox.Min.y)
        {
            BBox.Min.y = Node->TBBox.Min.y;
        }
        
        if(Node->TBBox.Min.z < BBox.Min.z)
        {
            BBox.Min.z = Node->TBBox.Min.z;
        }
        
        if(Node->TBBox.Max.x > BBox.Max.x)
        {
            BBox.Max.x = Node->TBBox.Max.x;
        }
        
        if(Node->TBBox.Max.y > BBox.Max.y)
        {
            BBox.Max.y = Node->TBBox.Max.y;
        }
        
        if(Node->TBBox.Max.z > BBox.Max.z)
        {
            BBox.Max.z = Node->TBBox.Max.z;
        }
    }
    
     GlobalWorld.BBox = BBox;
    RenderOctreeGenerateInternal(&GlobalWorld);
}

internal void
DEBUGRenderOctreeViz(render_octree *Octree, GLuint Program, camera *Camera)
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
        
        min_x = Octree->BBox.Min.x;
        min_y = Octree->BBox.Min.y;
        min_z = Octree->BBox.Min.z;
        max_x = Octree->BBox.Max.x;
        max_y = Octree->BBox.Max.y;
        max_z = Octree->BBox.Max.z;
        
        glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
        glm::vec3 center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
        glm::mat4 transform =  glm::translate(glm::mat4(1), center)* glm::scale(glm::mat4(1), size);
        
        /* Apply object's transformation matrix */
        glm::mat4 m = Camera->Projection * Camera->View * transform;
        GLuint gMatrix = glGetUniformLocation(Program, "mvp");
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
        
        for(s32 Idx = 0;
            Idx < 7;
            Idx++)
        {
            if(Octree->Branches[Idx] && Octree->Branches[Idx]->NodeCount)
            {
                DEBUGRenderOctreeViz(Octree->Branches[Idx], Program, Camera);
            }
        }
#endif
    }
}

internal void
RenderOctreeDrawInternal(render_octree *Octree, GLuint Program, camera *Camera, s32 RenderType)
{
    
    aabb bbox = Octree->BBox;
    glm::vec4 Center = (bbox.Max - bbox.Min)/2.f;
    render_transform T = RenderTransform();
    T.Pos = glm::vec3(Center.x, Center.y, Center.z);
    glm::mat4 MVP = Camera->Projection * Camera->View * RenderTransformMatrix(T);
    FrustumExtract(MVP);
    
    b32 IsInFrustum = FrustumCheckAABB(bbox);
    
    if(IsInFrustum == 2)
    {
        for(render_octree_node *Node = Octree->Node;
            Node;
            Node = Node->Next)
        {
            Model4DSRender(Node->Render, Program, Camera, Node->Transform, RenderType, 0);
        }
    }
    else if(IsInFrustum == 1)
    {
        for(s32 Idx = 0;
            Idx < 7;
            Idx++)
        {
            if(Octree->Branches[Idx] && Octree->Branches[Idx]->NodeCount)
            {
            RenderOctreeDrawInternal(Octree->Branches[Idx], Program, Camera, RenderType);
            }
        }
    }
}

internal void
RenderOctreeDraw(GLuint Program, camera *Camera, s32 RenderType)
{
    RenderOctreeDrawInternal(&GlobalWorld, Program, Camera, RenderType);
}

#define F3D_RENDER_H
#endif