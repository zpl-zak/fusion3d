// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_ASSET_H)

#include "formats/hformat_pak.h"
#include <io.h>
#include <fcntl.h>

#define ASSET_MAX 512*1024
#define PACK_MAX 32

typedef struct
{
    char Name[MAX_PATH];
    char Path[MAX_PATH];
    hformat_pak *Data;
    
    b32 Loaded;
} asset_pack;

typedef struct asset_file_
{
    char *Name;
    char *FilePath;
    u32 Type;
    
    b32 Loaded;
    u8 *Content;
    ms FileSize;
    u64 Checksum;
    b32 HasChanged;
    void (*ReloadCallback)(struct asset_file_ *Asset);
    
    b32 LoadFromPack;
    asset_pack *PackPtr;
    hformat_pak_file *PakFile;
    hformat_pak *Pak;
} asset_file;

typedef void asset_reload_callback(asset_file *Asset);

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
                
                Copy(MAX_PATH, Path, Pack->Path);
                
                s32 FileIdx = IOFileOpenRead((s8 *)Path, 0);
                
                if(FileIdx == -1)
                {
                    sprintf(Path, "packs\\%s", Pack->Name);
                    Copy(MAX_PATH, Path, Pack->Path);
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
    asset_pack *PackPtr = 0;
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
                    PackPtr = Pack;
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
    Asset.PackPtr = PackPtr;
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
AssetLoadInternal(asset_file *Asset, b32 Forced = 0, ms FileSize = 0)
{
    if(Asset->Loaded && !Forced)
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
        File = IOFileOpenRead((s8 *)Asset->PackPtr->Path, 0);
        IOFileSeek(File, Asset->PakFile->FilePosition, SeekOrigin_Set);
    }
    
    if(Asset->Content)
    {
        PlatformMemFree(Asset->Content);
        
        if(FileSize)
        {
            Asset->FileSize = FileSize;
        }
    }
    Asset->Content = (u8 *)PlatformMemAlloc(Asset->FileSize);
    IOFileRead(File, Asset->Content, Asset->FileSize);
    
    IOFileClose(File);
    
    Asset->Loaded = 1;
    
    u64 FileChecksum = 0;
    for(u64 Idx = 0;
        Idx < Asset->FileSize;
        ++Idx)
    {
        FileChecksum += *(Asset->Content + Idx);
    }
    
    Asset->Checksum = FileChecksum;
    
    return(Asset);
}

internal asset_file *
AssetLoad(char *Name, b32 Async)
{
    asset_file *Asset = AssetFind(Name);
    AssetLoadInternal(Asset);
    
    return(Asset);
}

// NOTE(ZaKlaus): This is what we could actually call an asset hot-reloading.
// currently supported only by PAK archive.
// We basically reload the meta information of archive from the file
// and then simply walk through every affected asset that comes from the archive,
// reloading the asset's content and if possible, triggering an event which
// should handle our asset changes. Assets are compared by simple SUM checksum 
// algorihtm, so there might be occasional collisions, I suggest using either 
// MD5 or CRC32 for such task, which might be considered as a TODO.
internal void
AssetSyncPack(void)
{
    // NOTE(ZaKlaus): Update PAKs first.
    for(s32 Idx = 0;
        Idx < PACK_MAX;
        ++Idx)
    {
        asset_pack *Pack = GlobalPacks + Idx;
        
        if(!Pack->Loaded)
        {
            break;
        }
        
        // TODO(ZaKlaus): De-allocate the pack to avoid mem leak.
        //HFormatReleasePakArchive(Pack->Data);
        
        s32 NewHandle = IOFileOpenRead((s8 *)Pack->Path, 0);
        
        Pack->Data = HFormatLoadPakArchive(NewHandle);
    }
    
    // NOTE(ZaKlaus): Check individual assets belonging to PAK and reload if required.
    for(s32 Idx = 0;
        Idx < ASSET_MAX;
        ++Idx)
    {
        asset_file *Asset = GlobalAssets + Idx;
        
        for(s32 Idx = 0;
            Idx < PACK_MAX;
            ++Idx)
        {
            asset_pack *Pack = GlobalPacks + Idx;
            
            if(!Pack->Loaded)
            {
                break;
            }
            
            hformat_pak *Pak = Pack->Data;
            
            for(mi Idx2 = 0;
                Idx2 < Pak->FileCount;
                Idx2++)
            {
                hformat_pak_file *PakFile = Pak->Files + Idx2;
                if(StringsAreEqual((char *)PakFile->FileName, Asset->FilePath))
                {
                    if(Pak->Checksums[Idx2] != Asset->Checksum)
                    {
                        AssetLoadInternal(Asset, 1, Pak->Files[Idx2].FileLength);
                        
                        if(Asset->ReloadCallback)
                        {
                            Asset->ReloadCallback(Asset);
                        }
                        Asset->HasChanged = 1;
                    }
                    else
                    {
                        Asset->HasChanged = 0;
                    }
                    break;
                }
            }
        }
    }
}

internal void
AssetAssignReloadCallbackInternal(asset_file *Asset, asset_reload_callback *Callback)
{
    Assert(Asset);
    
    Asset->ReloadCallback = Callback;
}

internal void
AssetAssignReloadCallback(s32 AssetIdx, asset_reload_callback *Callback)
{
    Assert(AssetIdx != 1);
    
    AssetAssignReloadCallbackInternal(GlobalAssets + AssetIdx, Callback);
}


// NOTE(ZaKlaus): This should be used, once the host has the ability to check for
// changes when possible and dealing with them on his own. 
// While this is not following any event system, there are occasions where callbacks
// won't be able to process any asset changes at that time, therefore leaving
// the job to a simple check, which might be performed whenever required.
// Point of WARNing though, you NEED to perform the check after the latest Pack Sync
// or you will hit false checks everytime.
internal b32
AssetScanChanges(s32 AssetIdx)
{
    Assert(AssetIdx != -1);
    asset_file *Asset = (GlobalAssets + AssetIdx);
    
    b32 Result = Asset->HasChanged;
    
    return(Result);
}

#define F3D_ASSET_H
#endif