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
    ASSET_SCRIPT
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
    
    s32 Idx;
    for(Idx = 0;
        Idx < ASSET_MAX;
        ++Idx)
    {
        asset_file *File = GlobalAssets + Idx;
        if(!File->Name)
        {
            *File = Asset;
            return(File);
        }
    }
    Assert(!"Max capacity has been reached!");
    return(0);
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
    s32 File = IOFileOpenRead(Asset->Name, 0);
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

#define F3D_ASSET_H
#endif