// (c) 2016 ZaKlaus; All Rights Reserved
// Contribution: ASM, DavoSK

#if !defined(F3D_SCENE_H)

#define F3D_SCENE_MAX 8

// Credits to ASM
enum nodeTypes {
    // top nodes
    MISSION = 0x4c53,
    META = 0x0001,
    UNK_FILE = 0xAFFF,
    UNK_FILE2 = 0x3200,
    FOV = 0x3010,
    VIEW_DISTANCE = 0x3011,
    CLIPPING_PLANES = 0x3211,
    WORLD = 0x4000,
    ENTITIES = 0xAE20,
    INIT = 0xAE50,
    // WORLD subnode
    OBJECT = 0x4010
};

enum objectTypes {
    TYPE = 0x4011,
    POSITION = 0x0020,
    ROTATION = 0x0022,
    POSITION_2 = 0x002C,
    SCALE = 0x002D,
    PARENT = 0x4020,
    NAME = 0x0010, 
    MODEL = 0x2012
};
#pragma pack(push, 1)
typedef struct
{
    uint16_t type;
    uint32_t size;
    unsigned char data[1];
} scene_node;
#pragma pack(pop)

typedef struct
{
    glm::vec3 Pos;
    glm::vec3 Rot;
    glm::vec3 Rot2;
    glm::vec3 Scale;
    
    s8 *Name;
    s8 *ModelName;
    
    scene_node *Node;
} scene_object;

typedef struct
{
    scene_object *Object;
    scene_node *Node;
} scene_world;

typedef struct
{
    b32 Loaded;
    
    asset_file *Asset;
    scene_world *World;
} scene;

global_variable scene GlobalScenes[F3D_SCENE_MAX] = {0};

internal scene *
SceneRegister(char *Name, char *FileName)
{
    s32 Slot = -1;
    for(s32 Idx = 0;
        Idx < F3D_SCENE_MAX;
        ++Idx)
    {
        if(!GlobalScenes[Idx].Asset && Slot == -1)
        {
            Slot = Idx;
        }
        
        if(GlobalScenes[Idx].Asset && StringsAreEqual(GlobalScenes[Idx].Asset->Name, Name))
        {
            Slot = Idx;
            break;
        }
    }
    
     scene *Map = GlobalScenes + Slot;
    
    if(Map->Asset)
    {
        return(Map);
    }
    
     scene Map_ = {0};
    *Map = Map_;
    
    char Temp[256];
    sprintf(Temp, "%s%s/scene2.obj", AssetTypeNames[Asset_Scene][0], FileName);
    asset_file *Asset = AssetRegister(Name, Temp, Asset_Scene);
    
    Map->Asset = Asset;
    
    return(Map);
}

internal void 
readObjectFunction(scene2_node* readObject)
{
    switch(readObject->type)
    {
        case NAME:
        {
        
        }break;
        
        case MODEL:
        {
            
        }break;
        
        case TYPE:
        {
            
        }break;
        
        case POSITION:
        {
        }break;
        
        case ROTATION:
        {
            
        }break;
        
        case ROTATION_2:
        {
            
        }break;
        
        case SCALE:
        {
            
        }break;
        
        case PARENT:
        {
        }break;
        
        default:
        {
            
        }break;
    }
}

internal void 
SceneNodeRead(scene2_node* readNode)
{
    switch(readNode->type)
    {
        case WORLD:
        {
            u32 position = 0;
            while(position + 6 < node->size)
            {
                scene2_node* nextNode = (scene2_node*)(readNode->data + position);
                readNodeFunction(nextNode);
                position += nextNode->size;
            }
            
        }break;
        
        case OBJECT:
        {
            u32 position = 0;
            while(position + 6 < readNode->size)
            {
                scene2_node* nextNode = (scene2_node*)(readNode->data + position);
                readObjectFunction(nextNode);
                position += nextNode->size;
            }
        }break;
        
        default:
        {
            
        }break;
        }
    }
    
internal void 
SceneRead(scene *Scene) 
{
    s8 *SceneData = Scene->Asset->Content;
    scene_node *MainNode = (scene_node *)SceneData;
    u32 Position = 6;
    u32 ReadCount = 0;
    
    while(Position + 6 < MainNode->size)
    {
        scene_node *currentNode = (scene2_node*)(SceneData + Position);
        readNode(currentNode);
        position += currentNode->size;
        readCount++;
}
}

internal scene *
SceneLoad(scene *Scene)
{
    if(Scene->Loaded)
    {
        return(Scene);
    }
    
    AssetLoadInternal(Scene->Asset);
    
    SceneRead(Scene);
    
    return(Scene);
}

#define F3D_SCENE_H
#endif
