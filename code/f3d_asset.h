// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_ASSET_H)

#include "formats/hformat_pak.h"
#include <io.h>
#include <fcntl.h>

#define ASSET_MAX 512*1024
#define PACK_MAX 32

typedef struct
{
    char *Name;
    char *FilePath;
    u32 Type;
    
    b32 Loaded;
    u8 *Content;
    ms FileSize;
    
    b32 LoadFromPack;
    hformat_pak_file *PakFile;
    hformat_pak *Pak;
} asset_file;

typedef struct
{
    char Name[MAX_PATH];
    hformat_pak *Data;
    
    b32 Loaded;
} asset_pack;

enum
{
    Asset_None,
    Asset_Script,
    Asset_Texture,
    Asset_Shader,
    Asset_Model,
    Asset_Scene,
    Asset_Pack,
    Num_of_Asset_Types,
};

global_variable char *AssetTypeNames[][2] = {
    {"", "Asset_None"},
    {"scripts/", "Asset_Script"},
        {"maps/", "Asset_Texture"},
        {"shaders/", "Asset_Shader"},
        {"models/", "Asset_Model"},
        {"missions/", "Asset_Scene"},
        {"packs", "Asset_Pack"},
};

global_variable char *GlobalGamePath = "game/default";
global_variable asset_file GlobalAssets[ASSET_MAX];
global_variable asset_pack GlobalPacks[PACK_MAX];

internal void
AssetInitialize(char *GamePath)
{
    local_persist b32 AlreadyLoaded = 0;
    if(AlreadyLoaded)
    {
        return;
    }
    AlreadyLoaded = 1;
    
    char Temp[MAX_PATH];
    sprintf(Temp, "%s", GamePath);
    GlobalGamePath = (char *)PlatformMemAlloc(strlen(Temp)+1);
    Copy(strlen(Temp)+1, Temp, GlobalGamePath);
    
    // NOTE(zaklaus): Packs
    {
        WIN32_FIND_DATA FindData;
        HANDLE Find = 0;
        char Dir[MAX_PATH];
        
        sprintf(Dir, "%s\\%s\\*", GamePath, AssetTypeNames[Asset_Pack][0]);
        
        Find = FindFirstFile(Dir, &FindData);
        
        if(Find == INVALID_HANDLE_VALUE)
        {
            sprintf(Dir, "%s\\*", AssetTypeNames[Asset_Pack][0]);
            
            Find = FindFirstFile(Dir, &FindData);
            
            if(Find == INVALID_HANDLE_VALUE)
            {
                return;
            }
        }
        
        s32 PackIndex = 0;
        
        do
        {
            if(FindData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {
                char *FileExtension = PathFindExtension(FindData.cFileName);
                
                if(!StringsAreEqual(FileExtension, ".pak"))
                {
                    continue;
                }
                
                asset_pack *Pack = GlobalPacks + PackIndex++;
                
                Copy(MAX_PATH, FindData.cFileName, Pack->Name);
                
                char Path[MAX_PATH];
                sprintf(Path, "%s\\packs\\%s", GlobalGamePath, Pack->Name);
                
                s32 FileIdx = IOFileOpenRead((s8 *)Path, 0);
                
                if(FileIdx == -1)
                {
                    sprintf(Path, "packs\\%s", Pack->Name);
                    FileIdx = IOFileOpenRead((s8 *)Path, 0);
                }
                
                Pack->Data = HFormatLoadPakArchive(FileIdx);
                
                Pack->Loaded = 1;
            }
        }
        while(FindNextFile(Find, &FindData));
        
        FindClose(Find);
    }
}

internal asset_file *
AssetRegister(char *Name, char *FilePath, u32 AssetType)
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
    
    char Temp[MAX_PATH];
    b32 InPack = 0;
    hformat_pak_file *PakFile;
    hformat_pak *Pak;
    ms FileSize;
    // NOTE(zaklaus): Check if file is in packs.
    {
        for(s32 Idx = 0;
            Idx < PACK_MAX;
            ++Idx)
        {
            asset_pack *Pack = GlobalPacks + Idx;
            
            if(!Pack->Loaded)
            {
                break;
            }
            
            Pak = Pack->Data;
            
            for(mi Idx2 = 0;
                Idx2 < Pak->FileCount;
                Idx2++)
            {
                PakFile = Pak->Files + Idx2;
                if(StringsAreEqual((char *)PakFile->FileName, FilePath))
                {
                    sprintf(Temp, "%s", FilePath);
                    InPack = 1;
                    FileSize = PakFile->FileLength;
                    break;
                }
            }
        }
    }
    
    if(!InPack)
    {
        sprintf(Temp, "%s/%s", GlobalGamePath, FilePath);
        
    s32 FileIndex = IOFileOpenRead((s8 *)Temp, &FileSize);
    
    if(FileIndex == -1)
    {
        // NOTE(zaklaus): Check the working directory instead:
        {
            sprintf(Temp, "%s", FilePath);
            FileIndex = IOFileOpenRead((s8 *)Temp, &FileSize);
            
            Assert(FileSize != -1);
        }
    }
    
    IOFileClose(FileIndex);
    
    PakFile = 0;
    Pak = 0;
}

    asset_file Asset = {0};
    Asset.Name = (char *)PlatformMemAlloc(strlen(Name)+1);
    Copy(strlen(Name)+1, Name, Asset.Name);
    Asset.FilePath = (char *)PlatformMemAlloc(strlen(Temp)+1);
    Copy(strlen(Temp)+1, Temp, Asset.FilePath);
    Asset.Type = AssetType;
    Asset.FileSize = FileSize;
    Asset.LoadFromPack = InPack;
    Asset.PakFile = PakFile;
    Asset.Pak = Pak;
    *File = Asset;
    
    return(File);
}

internal s32
AssetOpenHandle(asset_file *Asset, b32 LoadFromMemory)
{
    s32 FileIdx = 0;
    
    if(LoadFromMemory)
    {
    HANDLE ReadPipe;
    HANDLE WritePipe;
    
        CreatePipe(&ReadPipe, &WritePipe, 0, (DWORD)Asset->FileSize);
    WriteFile(WritePipe, Asset->Content, (DWORD)Asset->FileSize, 0, 0);
        CloseHandle(WritePipe);
    
     FileIdx = IOFindHandle();
    
    Assert(FileIdx != -1);
    
    s32 FileDescriptor = _open_osfhandle((intptr_t)ReadPipe, _O_RDONLY);
    FILE *FileHandle = _fdopen(FileDescriptor, "rb");
    
    FileHandles[FileIdx] = FileHandle;
    }
    else
    {
        FileIdx = IOFileOpenRead((s8 *)Asset->FilePath, 0);
    }
    
    return(FileIdx);
}

internal asset_file *
AssetFind(char *Name)
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
AssetLoadInternal(asset_file *Asset)
{
    if(Asset->Loaded)
    {
        return(Asset);
    }
    
    s32 File = -1;
    if(!Asset->LoadFromPack)
    {
     File = IOFileOpenRead((s8 *)Asset->FilePath, 0);
    }
    else
    {
        File = Asset->Pak->PakHandle;
        IOFileSeek(File, Asset->PakFile->FilePosition, SeekOrigin_Set);
    }
    
    Asset->Content = (u8 *)PlatformMemAlloc(Asset->FileSize);
    IOFileRead(File, Asset->Content, Asset->FileSize);
    
    if(!Asset->LoadFromPack)
    {
        IOFileClose(File);
    }
    
    Asset->Loaded = 1;
    
    return(Asset);
}

internal asset_file *
AssetLoad(char *Name, b32 Async)
{
    asset_file *Asset = AssetFind(Name);
    AssetLoadInternal(Asset);
    
    return(Asset);
}

#define F3D_ASSET_H
#endif