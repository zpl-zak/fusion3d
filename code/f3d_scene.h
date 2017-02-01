// (c) 2016 ZaKlaus; All Rights Reserved
// Contribution: ASM, DavoSK

#if !defined(F3D_SCENE_H)

#define F3D_SCENE_MAX 8
#define F3D_SCENE_RENDER_MAX 16036

#pragma pack(push, 1)

typedef struct
{
    u16 Type;
    u32 Size;
} scene_header;

typedef struct
{
    scene_header Header;
    u32 ModelNameLength;
    char *ModelName;
    glm::vec3 Pos;
    glm::vec4 Rot;
    glm::vec3 Scale;
    u32 _Unk0;
    glm::vec3 Scale2;
} scene_instance;

typedef struct
{
    scene_header Header;
    u32 ObjectNameLength;
    char *ObjectName;
     s8 Bounds[0x4C];
    mi InstanceCount;
    scene_instance *Instances;
} scene_object;

typedef struct
{
    u32 Version; // NOTE(zaklaus): Should always be 1.
    mi ObjectCount;
    scene_object *Objects;
} scene_chunk;

#pragma pack(pop)

typedef struct
{
    b32 Loaded;
    
    asset_file *Asset;
    scene_header Header;
    scene_chunk Chunk;
    
    ms RenderCount;
    render_4ds *Renders[F3D_SCENE_RENDER_MAX];
    scene_instance *Instances[F3D_SCENE_RENDER_MAX];
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
    sprintf(Temp, "%s%s/cache.bin", AssetTypeNames[Asset_Scene][0], FileName);
    asset_file *Asset = AssetRegister(Name, Temp, Asset_Scene);
    
    Map->Asset = Asset;
    
    return(Map);
}

internal scene *
SceneLoad(scene *Scene)
{
    if(Scene->Loaded)
    {
        return(Scene);
    }
    
    AssetLoadInternal(Scene->Asset);
    
    u8 *Data = Scene->Asset->Content;
    ms DataSize = Scene->Asset->FileSize;
    
    Scene->Header = *(scene_header *)Data;
    Data += sizeof(scene_header);
    
    Scene->Chunk.Version = *(u32 *)Data;
    Data += sizeof(u32);
    
    Scene->Chunk.Objects = (scene_object *)PlatformMemAlloc(Scene->Header.Size
                                                      - sizeof(u32));
    
    while(Data < Scene->Asset->Content+Scene->Header.Size
          - sizeof(u32))
    {
        scene_object *Object = Scene->Chunk.Objects + Scene->Chunk.ObjectCount++;
        scene_object Object_ = {};
        *Object = Object_;
        Object->Header = *(scene_header *)Data;
        Data += sizeof(scene_header);
        
        Object->ObjectNameLength = *(u32 *)Data;
        Data += sizeof(u32);
        
        Object->ObjectName = (char *)PlatformMemAlloc(Object->ObjectNameLength + 1);
        Copy(Object->ObjectNameLength, Data, Object->ObjectName);
        Object->ObjectName[Object->ObjectNameLength] = 0;
        Data += Object->ObjectNameLength;
        
        Copy(0x4C, Data, Object->Bounds);
        
        Data += 0x4C;
         u8* BaseData = Data;
          ms HeaderSize = sizeof(scene_header) + sizeof(u32) + Object->ObjectNameLength + 0x4C;
        
        Object->Instances = (scene_instance *)PlatformMemAlloc(Object->Header.Size - HeaderSize);
        
        while(Data < BaseData + Object->Header.Size - HeaderSize)
        {
            scene_instance *Instance = Object->Instances + Object->InstanceCount++;
            scene_instance Instance_ = {};
            *Instance = Instance_;
            Instance->Header = *(scene_header *)Data;
            Data += sizeof(scene_header);
            
            Instance->ModelNameLength = *(u32 *)Data;
            Data += sizeof(u32);
            
            Instance->ModelName = (char *)PlatformMemAlloc(Instance->ModelNameLength + 1);
            Copy(Instance->ModelNameLength-4, Data, Instance->ModelName);
            Instance->ModelName[Instance->ModelNameLength-4] = 0;
            Data += Instance->ModelNameLength;
            
            sprintf(Instance->ModelName, "%s.4ds", Instance->ModelName);
            Instance->ModelName[Instance->ModelNameLength] = 0;
            
            Instance->Pos = *(glm::vec3 *)Data;
            Instance->Pos.x *= -1.f;
            Data += sizeof(glm::vec3);
            
            r32 r[4];
            Copy(sizeof(r32)*4, Data, r);
            
            glm::vec4 v(r[1], r[2], r[3], r[0]);
            
            Instance->Rot = v;
            Data += sizeof(glm::quat);
            
            Instance->Scale = *(glm::vec3 *)Data;
            Data += sizeof(glm::vec3);
            
            Instance->_Unk0 = *(u32 *)Data;
            Data += sizeof(u32);
            
            Instance->Scale2 = *(glm::vec3 *)Data;
            Data += sizeof(glm::vec3);
            
            Scene->Renders[Scene->RenderCount] = Model4DSRegister(Instance->ModelName, Instance->ModelName);
            Model4DSLoad(Scene->Renders[Scene->RenderCount]);
            
            Scene->Instances[Scene->RenderCount] = Instance;
        
                Scene->RenderCount++;
            }
    }
    
    return(Scene);
}

#define F3D_SCENE_H
#endif
