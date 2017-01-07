// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_SQUIRREL_H)

#define SQ_MAX_VM 512

global_variable HSQUIRRELVM GlobalSQVM[SQ_MAX_VM];

internal void
F3DSQRegister(char *Name)
{
    char Temp[256];
    sprintf(Temp, "%s%s.nut", AssetTypeNames[Asset_Script][0], Name);
    
    F3DAssetRegister(Name, Temp, Asset_Script);
}

internal SQInteger
F3DSQRegisterNative(HSQUIRRELVM VM, SQFUNCTION Function, char *Name, s32 Params, char *Template)
{
    sq_pushroottable(VM);
    sq_pushstring(VM, Name, -1);
    sq_newclosure(VM, Function, 0);
    
    if(Params != -1)
    {
        sq_setparamscheck(VM, Params, Template);
    }
    
    sq_newslot(VM, -3, SQFalse);
    sq_pop(VM, 1);
    
    return(1);
}

internal SQInteger
F3DSQRegisterVariableb(HSQUIRRELVM VM, char *Name, b32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushbool(VM, Value);
    sq_createslot(VM, -3);
    
    return(1);
}

internal SQInteger
F3DSQRegisterVariable(HSQUIRRELVM VM, char *Name, s32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushinteger(VM, Value);
    sq_createslot(VM, -3);
    
    return(1);
}

internal SQInteger
F3DSQRegisterVariablef(HSQUIRRELVM VM, char *Name, r32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushfloat(VM, Value);
    sq_createslot(VM, -3);
    
    return(1);
}

internal SQInteger
F3DSQRegisterVariables(HSQUIRRELVM VM, char *Name, char *Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushstring(VM, Value, -1);
    sq_createslot(VM, -3);
    
    return(1);
}

internal void
F3DSQCall(HSQUIRRELVM VM, char *FuncName, s32 ArgCount, SQObject *Args)
{
        Assert(VM);
    
    s32 Top = (s32)sq_gettop(VM);
    
    sq_pushroottable(VM);
    
    sq_pushstring(VM, FuncName, -1);
    
    if(SQ_SUCCEEDED(sq_get(VM, -2)))
    {
        sq_pushroottable(VM);
        
        if(Args)
        {
            s32 Idx2;
            for(Idx2 = 0;
                Idx2 < ArgCount;
                ++Idx2)
            {
                sq_pushobject(VM, Args[Idx2]);
            }
        }
        
        sq_call(VM, ArgCount + 1, 1, 1);
    }
    
    sq_settop(VM, Top);
}

internal void
F3DSQBroadcast(char *FuncName, s32 ArgCount, SQObject *Args)
{
    s32 Idx;
    for(Idx = 0;
        Idx < SQ_MAX_VM;
        ++Idx)
    {
         HSQUIRRELVM VM = GlobalSQVM[Idx];
        
        if(!VM)
        {
            continue;
        }
        
        F3DSQCall(VM, FuncName, ArgCount, Args);
    }
}

internal void
F3DSQPrintFunc(HSQUIRRELVM VM, const char *Format, ...)
{
    va_list VarArgs;
    char Text[512];
    
    va_start(VarArgs, Format);
    vsprintf(Text, Format, VarArgs);
    va_end(VarArgs);
    
    printf("%s", Text);
}

internal void
F3DSQErrorFunc(HSQUIRRELVM VM, const char *Format, ...)
{
    va_list VarArgs;
    char Text[512];
    
    va_start(VarArgs, Format);
    vsprintf(Text, Format, VarArgs);
    va_end(VarArgs);
    
    printf("%s", Text);
    //sqstd_printcallstack(VM);
}

internal s32
F3DSQFindFree(void)
{
    for(s32 Idx = 0;
        Idx < SQ_MAX_VM;
        ++Idx)
    {
        if(!GlobalSQVM[Idx])
        {
            return(Idx);
        }
    }
    return(-1);
}

internal HSQUIRRELVM
F3DSQLoadVM(HSQUIRRELVM VM)
{
    sq_setprintfunc(VM, F3DSQPrintFunc, F3DSQErrorFunc);
    sqstd_seterrorhandlers(VM);
    
    sq_pushroottable(VM);
    
    sqstd_register_bloblib(VM);
    sqstd_register_iolib(VM);
    sqstd_register_mathlib(VM);
    sqstd_register_stringlib(VM);
    sqstd_register_systemlib(VM);
    
    // NOTE(zaklaus): Natives
    {
        SQNATAsset(VM);
        SQNATAsync(VM);
    }
    
    return(VM);
   }
   
   internal HSQUIRRELVM
       F3DSQLoadScript(HSQUIRRELVM VM, char *Code, ms CodeSize=0)
   {
       F3DSQLoadVM(VM);
       
       SQInteger Top = sq_gettop(VM);
       if(Code)
           if(SQ_FAILED(sq_compilebuffer(VM, Code, CodeSize, "compile", 1)))
       {
           printf("Squirrel error for %.*s:\n", (s32)CodeSize, Code); 
           sq_close(VM);
           VM = 0;
           return(0);
       }
       
       sq_pushroottable(VM);
       sq_call(VM, 1, SQFalse, SQTrue);
       sq_settop(VM, Top);
       sq_pop(VM, 1);
       
       return(VM);
   }
   
   internal HSQUIRRELVM
       F3DSQLoadScriptFile(char *Name)
   {
       s32 Slot = F3DSQFindFree();
       
       asset_file *Script = F3DAssetFind(Name);
       
       if(!Script->Loaded)
       {
           F3DAssetLoadInternal(Script);
       }
       
       *(GlobalSQVM + Slot) = sq_open(1024);
       
       HSQUIRRELVM VM = F3DSQLoadScript(*(GlobalSQVM + Slot), (char *)Script->Content, Script->FileSize);
       F3DSQCall(VM, "onScriptInit", 0, 0);
       
       return(VM);
   }
   
   #define F3D_SQUIRREL_H
#endif