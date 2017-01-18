// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_SHADER_H)

#define F3D_SHADER_MAX 32

global_variable GLuint GlobalShaderPrograms[F3D_SHADER_MAX] = {0};

internal GLuint
ShaderProgramInit(void)
{
    s32 Slot = -1;
    for(s32 Idx = 0;
        Idx < F3D_SHADER_MAX;
        ++Idx)
    {
        if(!GlobalShaderPrograms[Idx])
        {
            Slot = Idx;
            break;
        }
    }
    
    Assert(Slot != -1);
    
     GLuint Result = GlobalShaderPrograms[Slot] = glCreateProgram();
    
    return(Result);
}

internal b32
ShaderPrintLog(GLuint Object)
{
    GLint ErrResult = 0;
    s32 InfoLogLength;
    
    glGetShaderiv(Object, GL_COMPILE_STATUS, &ErrResult);
    glGetShaderiv(Object, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0)
    {
        char *InfoLog = (char *)PlatformMemAlloc(InfoLogLength);
        glGetShaderInfoLog(Object, InfoLogLength, 0, InfoLog);
        printf("Object[%d]: %.*s\n", ErrResult, InfoLogLength, InfoLog);
        PlatformMemFree(InfoLog);
        
        return(0);
    }
    
    return(1);
}


internal b32
F3DProgramPrintLog(GLuint Object)
{
    GLint ErrResult = 0;
    s32 InfoLogLength;
    
    glGetProgramiv(Object, GL_COMPILE_STATUS, &ErrResult);
    glGetProgramiv(Object, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if(InfoLogLength > 0)
    {
        char *InfoLog = (char *)PlatformMemAlloc(InfoLogLength);
        glGetShaderInfoLog(Object, InfoLogLength, 0, InfoLog);
        printf("Object[%d]: %.*s\n", ErrResult, InfoLogLength, InfoLog);
        PlatformMemFree(InfoLog);
        
        return(0);
    }
    
    return(1);
}

internal asset_file *
ShaderLoad(char *Name, char *FileName)
{
    char Temp[256];
    sprintf(Temp, "%s%s", AssetTypeNames[Asset_Shader][0], FileName);
    
    asset_file *Asset = AssetRegister(Name, Temp, Asset_Shader);
    AssetLoadInternal(Asset);
    
    return(Asset);
}

internal GLuint
ShaderLink(GLuint Program, asset_file *Asset, GLenum Type)
{
    Assert(Program && Asset && Asset->Content);
    glUseProgram(Program);
    
    GLuint Shader = glCreateShader(Type);
    
    char *Source = (char *)PlatformMemAlloc(Asset->FileSize + 1);
    Copy(Asset->FileSize, Asset->Content, Source);
    Source[Asset->FileSize] = 0;
    
    glShaderSource(Shader, 1, (const char **)&Source, 0);
    glCompileShader(Shader);
    
    PlatformMemFree(Source);
    
    Assert(ShaderPrintLog(Shader));
    
    glAttachShader(Program, Shader);
    
    return(Shader);
}

internal void
ShaderUnload(GLuint Program, GLuint Shader)
{
    Assert(Program && Shader);
    glUseProgram(Program);
    
    glDetachShader(Program, Shader);
    glDeleteShader(Shader);
}

internal GLuint
ShaderProgramLink(GLuint Program)
{
    Assert(Program);
    
    glLinkProgram(Program);
    
    //ShaderPrintLog(Program);
    
            return(Program);
}

#define F3D_SHADER_H
#endif