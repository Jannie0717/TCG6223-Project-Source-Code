# Caine & Abel

Made by ... ,Jaixen, ...

## Table of Content (Current)

- [Main fucntion]
- [3D Virtual World projection]
- [Model Loader]  -Jamie & Assited by Lau Zhi Xuan
- [Texture Loader] -Jamie
- [Character Models] - Kinger (Done & Loaded) & Caine (Done but not Loaded) -Jamie
- [Character Texture] - Kinger (Undone) & Caine (Currently Unavailable) -Jamie
- [stb_image.h] #dont remove this file
- Added Environment and Audio. (Animation for environment not done yet)
- Please refer below on how to check the linker for the audio connectivity.

## READ THIS

This is a C++ code project for Computer Graphics, please read and follow instruction below while installing this project.

# Message to the developers
!!DO NOT!!
1. DO NOT MODIFY any important code without any consent.
2. DO NOT move the model file. This will cause the Model Loader can't load the model.
3. DO NOT MODIFY the arraged model position.

!!DO!!
1. Read through the code file and the comments to understand each fucntions of the code files.
2. Understand where to put your code and check for each errors.
3. Save a backup project for safety purposes if any changes on the code causes a break down on the project.
4. Ensuring path of file declaration will not be specific (e.g. C://User//...) for easy compile purposes.

## Installation / Compile Instruction
1. Download the file
2. Decompress the file
3. Open CodeBlock version 17.12
4. Open the file as project
5. Select TCG6213-Project.cbp
6. Run the project file.

# IF you encounter any error, pls give a feedback to the previous contributors

# Audio Setup Guide for Code::Blocks

This project uses `AudioManager.cpp` and `AudioManager.hpp` to play background music using the Windows `PlaySound()` function.

Because `PlaySound()` belongs to the Windows Multimedia library, each computer must link the project with:

```text
-lwinmm
```

## Required Audio Files

Make sure these files/folders exist in the project:

```text
AudioManager.cpp
AudioManager.hpp

Audio
└── BGM
    └── DarkCircusTheme.wav
```

The background music should be in `.wav` format.

## How to Add the Linker in Code::Blocks

1. Open the project in Code::Blocks.
2. Go to:

```text
Project → Build options
```

3. Select the main project name on the left side, not only `Debug` or `Release`.
4. Open the tab:

```text
Linker settings
```

5. In the box called:

```text
Other linker options
```

add this line:

```text
-lwinmm
```

6. Click `OK`.
7. Rebuild the project:

```text
Build → Rebuild
```

## Common Error

If you see this error:

```text
undefined reference to PlaySoundA
```

it means the linker option was not added correctly.

Check again that:

```text
-lwinmm
```

is inside:

```text
Project → Build options → Linker settings → Other linker options
```

## Important Note

If the `.cbp` Code::Blocks project file is already updated and pushed to GitHub, the linker setting should be included automatically when other members pull the latest project.

However, if the error still appears, manually add `-lwinmm` using the steps above.

