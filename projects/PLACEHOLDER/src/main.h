#pragma once


#ifdef _DEBUG
#pragma comment(lib, "Fusion3D_Debug.lib")
#else
#pragma comment(lib, "Fusion3D.lib")
#endif

#include "main.generated.h"

int main (int argc, char** argv);