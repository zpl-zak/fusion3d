// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_RENDER_OCTREE_H)

typedef struct render_octree_node_
{
    render_4ds *Render;
    render_transform Transform;
    aabb TBBox;
    
    b32 WasUsed;
    struct render_octree_node_ *Next;
} render_octree_node;

typedef struct render_octree_
{
    s32 NodeCount;
    render_octree_node *Node;
    
    aabb BBox, TBBox;
    glm::vec4 Center;
    r32 Radius;
    
    b32 IsLeaf;
    struct render_octree_ *Branches[8];
} render_octree;

global_variable render_octree GlobalWorld = {0};

internal void
RenderOctreeAdd(render_4ds *Render, render_transform Transform)
{
    render_octree_node *Node = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
    render_octree_node Node_ = {};
    *Node = Node_;
    
    Node->Transform = Transform;
    Node->Render = Render;
    Node->Next = 0;
    
    Transform.Rot = glm::vec4(0,0,0,0);
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
    return((a.Min.x <= b.Max.x) && (a.Min.y <= b.Max.y) && (a.Min.z <= b.Max.z) && (a.Max.x >= b.Min.x) && (a.Max.y >= b.Min.y) && (a.Max.z >= b.Min.z));
}

internal render_octree *
RenderOctreeGenerateSubtree(render_octree *Octree, glm::vec4 Center, r32 x, r32 y, r32 z)
{
    aabb Subtreebox;
    r32 ax = abs(x);
    r32 ay = abs(y);
    r32 az = abs(z);
    glm::vec4 Subtreecenter = Center + glm::vec4(x, y, z, 1);
    Subtreebox.Min = Subtreecenter - glm::vec4(ax, ay, az, 1);
    Subtreebox.Max = Subtreecenter + glm::vec4(ax, ay, az, 1);
    
    render_octree_node *Subtreelist = 0;
    
    render_octree *Subtree = (render_octree *)PlatformMemAlloc(sizeof(render_octree));
    render_octree o_ = {0};
    *Subtree = o_;
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        if(RenderTestAABB(Subtreebox, Node->TBBox) ||
           RenderTestAABB(Node->TBBox, Subtreebox))
        {
            ++Subtree->NodeCount;
            render_octree_node *NewNode = (render_octree_node *)PlatformMemAlloc(sizeof(render_octree_node));
            *NewNode = *Node;
            NewNode->Next = 0;
            NewNode->WasUsed = 1;
            
            if(!Subtreelist)
            {
                Subtreelist = NewNode;
            }
            else
            {
                for(render_octree_node *N = Subtreelist;
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
    
    Subtree->Node = Subtreelist;
    Subtree->BBox.Min = glm::vec4(-ax, -ay, -az, 1);
    Subtree->BBox.Max = glm::vec4(ax, ay, az, 1);
    Subtree->TBBox = Subtreebox;
    
    return(Subtree);
}

internal void
RenderOctreeGenerateInternal(render_octree *Octree)
{
    aabb bbox = Octree->TBBox;
    
    r32 wx = bbox.Max.x - bbox.Min.x;
    r32 wy = bbox.Max.y - bbox.Min.y;
    r32 wz = bbox.Max.z - bbox.Min.z;
    
    r32 hx = wx / 2.f;
    r32 hy = wy / 2.f;
    r32 hz = wz / 2.f;
    
    r32 qx = hx / 2.f;
    r32 qy = hy / 2.f;
    r32 qz = hz / 2.f;
    
    
    glm::vec4 Center = bbox.Max - glm::vec4(hx,hy,hz,1);
    
    real32 Radius = MathMAX(hx, MathMAX(hy, hz));
    
    Octree->Center = Center;
    Octree->Radius = Radius;
    
    if(Octree->NodeCount < 10 || Radius < 1.f)
    {
        return;
    }
    
    
    for(render_octree_node *Node = Octree->Node;
        Node;
        Node = Node->Next)
    {
        Node->WasUsed = 0;
    }
    
    render_octree o_ = {};
    
    ///
    /// Branch 1 -- ldf
    ///
    
    render_octree *o1 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    -qx,-qy,qz);
    
    Octree->Branches[0] = o1;
    
    ///
    /// Branch 2 -- rdf
    ///
    
    render_octree *o2 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    qx,-qy,qz);
    
    Octree->Branches[1] = o2;
    
    ///
    /// Branch 3 -- luf
    ///
    
    render_octree *o3 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    -qx,qy,qz);
    
    Octree->Branches[2] = o3;
    
    ///
    /// Branch 4 -- ruf
    ///
    
    render_octree *o4 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    qx,qy,qz);
    
    Octree->Branches[3] = o4;
    
    ///
    /// Branch 5 -- ldn
    ///
    render_octree *o5 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    -qx,-qy,-qz);
    
    Octree->Branches[4] = o5;
    
    ///
    /// Branch 6 -- rdn
    ///
    render_octree *o6 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    qx,-qy,-qz);
    
    Octree->Branches[5] = o6;
    
    ///
    /// Branch 7 -- run
    ///
    render_octree *o7 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    qx,qy,-qz);
    
    Octree->Branches[6] = o7;
    
    ///
    /// Branch 8 -- lun
    ///
    render_octree *o8 = RenderOctreeGenerateSubtree(Octree, Center,
                                                    -qx,qy,-qz);
    
    Octree->Branches[7] = o8;
    
    s8 NotEmpty = 0;
    
    for(s32 Idx = 0;
        Idx < 7;
        Idx++)
    {
        RenderOctreeGenerateInternal(Octree->Branches[Idx]);
        
        if(Octree->Branches[Idx]->NodeCount)
        {
            ++NotEmpty;
        }
    }
    
    Octree->IsLeaf = (NotEmpty == 0);
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
    GlobalWorld.TBBox = BBox;
    RenderOctreeGenerateInternal(&GlobalWorld);
}

internal void
DEBUGRenderOctreeViz(render_octree *Octree, GLuint Program, camera *Camera, b32 Recursive)
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
        
        min_x = Octree->TBBox.Min.x;
        min_y = Octree->TBBox.Min.y;
        min_z = Octree->TBBox.Min.z;
        max_x = Octree->TBBox.Max.x;
        max_y = Octree->TBBox.Max.y;
        max_z = Octree->TBBox.Max.z;
        
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
        
        if(Recursive)
            for(s32 Idx = 0;
                Idx < 7;
                Idx++)
        {
            if(Octree->Branches[Idx] && Octree->Branches[Idx]->NodeCount)
            {
                DEBUGRenderOctreeViz(Octree->Branches[Idx], Program, Camera, Recursive);
            }
        }
#endif
    }
}

internal void
RenderOctreeDrawInternal(render_octree *Octree, GLuint Program, camera *Camera, s32 RenderType)
{
    aabb bbox = Octree->BBox;
    glm::vec4 Center = Octree->Center;
    render_transform T = RenderTransform();
    T.Pos = glm::vec3(Center.x, Center.y, Center.z);
    glm::mat4 MVP = Camera->Projection * Camera->View * RenderTransformMatrix(T);
    FrustumExtract(MVP);
    
    b32 IsInFrustum = FrustumCheckAABB(bbox);
    
    if(IsInFrustum == 2 || Octree->IsLeaf)
    {
        DEBUGRenderOctreeViz(Octree, Program, Camera, 0);
        for(render_octree_node *Node = Octree->Node;
            Node;
            Node = Node->Next)
        {
            Model4DSRender(Node->Render, Program, Camera, Node->Transform, RenderType, 0);
        }
    }
    else if(IsInFrustum == 1)
    {
        s32 Subs = 0;
        for(s32 Idx = 0;
            Idx < 7;
            Idx++)
        {
            if(Octree->Branches[Idx])
            {
                ++Subs;
                RenderOctreeDrawInternal(Octree->Branches[Idx], Program, Camera, RenderType);
            }
        }
        
        if(!Subs)
        {
            for(render_octree_node *Node = Octree->Node;
                Node;
                Node = Node->Next)
            {
                Model4DSRender(Node->Render, Program, Camera, Node->Transform, RenderType, 0);
            }
        }
    }
}

internal void
RenderOctreeDraw(GLuint Program, camera *Camera, s32 RenderType)
{
    for(s32 Idx = 0;
        Idx < 7;
        Idx++)
    {
        if(GlobalWorld.Branches[Idx] && GlobalWorld.Branches[Idx]->NodeCount)
        {
            RenderOctreeDrawInternal(GlobalWorld.Branches[Idx], Program, Camera, RenderType);
        }
    }
}

#define F3D_RENDER_OCTREE_H
#endif