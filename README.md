![alt tag](http://i.imgur.com/y7jhAYZ.png)

Fusion3D
==

Simple 3D Game Engine with dynamic shading, physics and reflected modular programming.

##Quickstart Guide
- Open (or firstly build) our Build Tool file (build.exe)
- Click "Create Project" and type in the name of the project
- Make sure you've already built engine project inside engine/ folder
- When the project gets created, open project and click Copy Dependencies
- Those are engine's libraries, asset files and required binaries for the project to work properly
- Click Build Project, to generate reflection code and build your game project.
- Open game project's .sln with Visual Studio and enjoy programming!
- If you want to copy asset files from data/ folder, use Copy Data in Build Tool

###Programs
- Programs (or scripts) represent behaviour of entity (game object).
- Program can be generated via Build Tool by accessing Program Editor
- Please, build project via Build Tool if you make any changes in program editor.

##Credits
Developed by Dominik "ZaKlaus" Madarász and other direct or indirect contributors to the GitHub.

Embeds [stb_image.h, stb_image.c, stb_textedit.h, stb_truetype.h, stb_rectpack.h](https://github.com/nothings/stb/) by Sean Barrett (public domain).

Embeds [imgui.cpp, imgui.h, imgui_draw.cpp, imgui_impl_sdl_gl3.h, imgui_impl_sdl_gl3.cpp, imgui_demo.cpp, imgui_internal.h](https://github.com/ocornut/imgui/blob/master/README.md) by Omar Cornut (MIT License).

Embeds [Bullet Physics](http://bulletphysics.org/), [SDL2](http://www.libsdl.org/), [GLEW](http://glew.sourceforge.net/) and [Asset Importer](http://assimp.sourceforge.net/).

Libraries are still subject to change and in no way should be considered available in the later versions.

##License
Fusion3D is licensed under the MIT License, see LICENSE for more information.