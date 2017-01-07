// (c) 2016 ZaKlaus; All Rights Reserved

#if !defined(F3D_ASYNC_H)

#define ASYNC_MAX_THREADS 64

global_variable HANDLE GlobalThreads[ASYNC_MAX_THREADS] = {0};
global_variable HANDLE GlobalThreadIDs[ASYNC_MAX_THREADS] = {0};

DWORD WINAPI TaskHandler(LPVOID Param)
{
    return(0);
}

#define F3D_ASYNC_H
#endif