// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_SQUIRREL_H)

#if defined(_MSC_VER) && defined(_DEBUG)
#include "crtdbg.h"
#include "conio.h"
#endif
#include "squirrel.h"
#include "sqstdblob.h"
#include "sqstdsystem.h"
#include "sqstdio.h"
#include "sqstdmath.h"	
#include "sqstdstring.h"
#include "sqstdaux.h"

#ifdef SQUNICODE
#define scfprintf fwprintf
#define scvprintf vfwprintf
#else
#define scfprintf fprintf
#define scvprintf vfprintf
#endif

#define SQ_MAX_VM 512

global_variable HSQUIRRELVM GlobalSQVM[SQ_MAX_VM];

internal void
F3DSQRegister(char *Name)
{
    char Temp[256];
    sprintf(Temp, "scripts/%s.nut", Name);
    
    F3DAssetRegister(Name, Temp, ASSET_SCRIPT);
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
}

internal SQInteger
F3DSQRegisterVariableb(HSQUIRRELVM VM, char *Name, b32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushbool(VM, Value);
    sq_createslot(VM, -3);
}

internal SQInteger
F3DSQRegisterVariable(HSQUIRRELVM VM, char *Name, s32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushinteger(VM, Value);
    sq_createslot(VM, -3);
}

internal SQInteger
F3DSQRegisterVariablef(HSQUIRRELVM VM, char *Name, r32 Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushfloat(VM, Value);
    sq_createslot(VM, -3);
}

internal SQInteger
F3DSQRegisterVariables(HSQUIRRELVM VM, char *Name, char *Value)
{
    sq_pushstring(VM, Name, -1);
    sq_pushstring(VM, Value, -1);
    sq_createslot(VM, -3);
}

internal void
F3DSQCall(char *FuncName, s32 ArgCount, SQObject *Args)
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

internal HSQUIRRELVM
F3DSQLoadScript(char *Name)
{
    s32 Slot;
    for(s32 Idx = 0;
        Idx < SQ_MAX_VM;
        ++Idx)
    {
        if(!GlobalSQVM[Idx])
        {
            Slot = Idx;
            break;
        }
    }
    
    asset_file *Script = F3DAssetFind(Name);
    
    HSQUIRRELVM *VMPtr = GlobalSQVM + Slot;
    *VMPtr = sq_open(1024);
    
    HSQUIRRELVM VM = *VMPtr;
    
    sq_setprintfunc(VM, F3DSQPrintFunc, F3DSQErrorFunc);
    sqstd_seterrorhandlers(VM);
    
    sq_pushroottable(VM);
    
    sqstd_register_bloblib(VM);
    sqstd_register_iolib(VM);
    sqstd_register_mathlib(VM);
    sqstd_register_stringlib(VM);
    sqstd_register_systemlib(VM);
    
    if(SQ_FAILED(sqstd_dofile(VM, Script->FilePath, SQFalse, SQTrue)))
    {
        printf("Squirrel error[%s]\n", Name); 
        sq_close(VM);
        *VMPtr = 0;
        return(0);
    }
    
    sq_pop(VM, 1);
    
    F3DSQCall("onScriptInit", 0, 0);
    
    return(VM);
   }
   
   #define F3D_SQUIRREL_H
#endif