// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_ASSET_H)

#define ASSET_MAX 1024

typedef struct
{
    char *Name;
    char *FilePath;
    u32 Type;
    
    b32 Loaded;
    u8 *Content;
    ms FileSize;
} asset_file;

enum
{
    Asset_None,
    Asset_Script,
    Asset_Texture,
    Num_of_Asset_Types,
};

global_variable char *AssetTypeNames[][2] = {
    {"", "Asset_None"},
    {"scripts/", "Asset_Script"},
        {"textures/", "Asset_Texture"}
};

global_variable char *GlobalGamePath = "game/default";
global_variable asset_file GlobalAssets[ASSET_MAX];

internal void
F3DAssetInitialize(char *GamePath)
{
    char Temp[256];
    sprintf(Temp, "game/%s", GamePath);
    GlobalGamePath = PlatformMemAlloc(strlen(Temp)+1);
    Copy(strlen(Temp)+1, Temp, GlobalGamePath);
}

internal asset_file *
F3DAssetRegister(char *Name, char *FilePath, u32 AssetType)
{
    asset_file *File = 0;
    for(s32 Idx = 0;
        Idx < ASSET_MAX;
        ++Idx)
    {
        asset_file *Asset = GlobalAssets + Idx;
        if(!Asset->Name && !File)
        {
            File = Asset;
        }
        
        if(Asset->Name && StringsAreEqual(Asset->Name, Name))
        {
            return(Asset);
        }
    }
    
    if(!File)
    {
        Assert(!"Max capacity has been reached!");
        return(0);
    }
    
    char Temp[256];
    sprintf(Temp, "%s/%s", GlobalGamePath, FilePath);
    
     ms FileSize;
    s32 FileIndex = IOFileOpenRead(Temp, &FileSize);
    
    if(FileIndex == -1)
    {
        Assert(0);
    }
    
    IOFileClose(FileIndex);
    
    asset_file Asset = {0};
    Asset.Name = PlatformMemAlloc(strlen(Name)+1);
    Copy(strlen(Name)+1, Name, Asset.Name);
    Asset.FilePath = PlatformMemAlloc(strlen(Temp)+1);
    Copy(strlen(Temp)+1, Temp, Asset.FilePath);
    Asset.Type = AssetType;
    Asset.FileSize = FileSize;
    *File = Asset;
    
    return(File);
}

internal asset_file *
F3DAssetFind(char *Name)
{
    for(s32 Idx = 0;
        Idx < ASSET_MAX;
        ++Idx)
    {
        asset_file *Asset = GlobalAssets + Idx;
        if(Asset->Name && StringsAreEqual(Name, Asset->Name))
        {
            return(Asset);
        }
    }
    return(0);
}

internal asset_file *
F3DAssetLoadInternal(asset_file *Asset)
{
    if(Asset->Loaded)
    {
        return(Asset);
    }
    
    s32 File = IOFileOpenRead(Asset->FilePath, 0);
    Asset->Content = PlatformMemAlloc(Asset->FileSize);
    IOFileRead(File, Asset->Content, Asset->FileSize);
    Asset->Loaded = 1;
    IOFileClose(File);
    
    return(Asset);
}

internal asset_file *
F3DAssetLoad(char *Name, b32 Async)
{
    asset_file *Asset = F3DAssetFind(Name);
    F3DAssetLoadInternal(Asset);
    
    return(Asset);
}

//
// NOTE(zaklaus): Natives
//

internal SQInteger 
SQNATAssetRegister(HSQUIRRELVM VM)
{
    SQChar *Name, *FilePath;
    SQInteger AssetType;
    
    sq_getstring(VM, -3, &Name);
    sq_getstring(VM, -2, &FilePath);
    sq_getinteger(VM, -1, &AssetType);
    
    char Temp[256];
    sprintf(Temp, "%s%s", AssetTypeNames[AssetType][0], FilePath);
    
    void *Asset = F3DAssetRegister(Name, Temp, (u32)AssetType);
    sq_pushuserpointer(VM, Asset);
    
    return(1);
}

internal SQInteger 
SQNATAssetLoadByName(HSQUIRRELVM VM)
{
    SQChar *Name;
    sq_getstring(VM, -3, &Name);
    
    void *Asset = F3DAssetLoad(Name, 0);
    sq_pushuserpointer(VM, Asset);
    
    return(1);
}

internal SQInteger 
SQNATAssetLoad(HSQUIRRELVM VM)
{
    SQUserPointer Ptr;
    sq_getuserpointer(VM, -1, &Ptr);
    
    void *Asset = F3DAssetLoadInternal((asset_file *)Ptr);
    sq_pushuserpointer(VM, Asset);
    
    return(1);
}

internal SQInteger
SQNATAssetGetName(HSQUIRRELVM VM)
{
    SQUserPointer Ptr;
    sq_getuserpointer(VM, -1, &Ptr);
    asset_file *Asset = (asset_file *)Ptr;
    
    sq_pushstring(VM, Asset->Name, strlen(Asset->Name));
    
    return(1);
}

internal SQInteger
SQNATAssetGetContent(HSQUIRRELVM VM)
{
    SQUserPointer Ptr;
    sq_getuserpointer(VM, -1, &Ptr);
    asset_file *Asset = (asset_file *)Ptr;
    
    if(!Asset->Loaded)
    {
        sq_pushbool(VM, 0);
        return(1);
    }
    
    void *Data = sq_newuserdata(VM, Asset->FileSize);
    Copy(Asset->FileSize, Asset->Content, Data);
    
    return(1);
}

internal SQInteger
SQNATAssetGetSize(HSQUIRRELVM VM)
{
    SQUserPointer Ptr;
    sq_getuserpointer(VM, -1, &Ptr);
    asset_file *Asset = (asset_file *)Ptr;
    
    sq_pushinteger(VM, Asset->FileSize);
    
    return(1);
}

internal void
SQNATAsset(HSQUIRRELVM VM)
{
    F3DSQRegisterNative(VM, SQNATAssetRegister, "assetRegister", 4, ".dss");
    F3DSQRegisterNative(VM, SQNATAssetLoadByName, "assetLoadByName", 2, ".s");
    F3DSQRegisterNative(VM, SQNATAssetLoad, "assetLoad", 2, ".p");
    
    F3DSQRegisterNative(VM, SQNATAssetGetName, "assetGetName", 2, ".p");
    F3DSQRegisterNative(VM, SQNATAssetGetSize, "assetGetSize", 2, ".p");
    F3DSQRegisterNative(VM, SQNATAssetGetContent, "assetGetContent", 2, ".p");
    
    // NOTE(zaklaus): Constants.
    {
        for(s32 Idx = 0;
            Idx < Num_of_Asset_Types;
            ++Idx)
        {
            F3DSQRegisterVariable(VM, AssetTypeNames[Idx][1], Asset_None + Idx);
        }
    }
}

#define F3D_ASSET_H
#endif