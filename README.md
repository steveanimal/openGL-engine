# OpenGL Engine

A lightweight C++ OpenGL engine for rendering 3D models, handling input, and managing shaders and textures. Built with **GLFW**, **GLEW**, **GLM**, **STB Image**, and **Assimp**, this engine provides a structured framework for building 3D applications and games.

---

## Features

- **Window Management**: Create and manage OpenGL windows with GLFW  
- **Shader System**: Compile, link, and manage vertex and fragment shaders  
- **Texture Handling**: Supports 2D, 2D array, and 3D textures  
- **Model Loading**: Load `.obj` and `.glb` models using Assimp, including embedded textures  
- **Camera & Player**: First-person camera controls with WASD + mouse movement  
- **Uniform Management**: Easy-to-use `uniform` wrapper for `glm::mat4`  
- **Input Handling**: Keyboard and mouse input abstraction  

---

## Controls

- **W / A / S / D** – Move forward, left, back, right  
- **Space / Left Shift** – Move up / down  
- **Mouse** – Look around  
- **ESC** – Release mouse cursor  
- **HOME** – Close window  

---

## File Structure (Source Code)

```text
/source  
  └── main.cpp  
/resource  
  ├── /model  
  │   └── house.obj  
  ├── /texture  
  │   └── house_texture.png  
  └── /shader  
      ├── vert.glsl  
      └── frag.glsl  
/dependencies  
  ├── /assimp  
  │   └── ...  
  ├── /GLFW  
  │   └── ...  
  ├── /GLEW  
  │   └── ...  
  ├── /glm  
  │   └── ...  
  ├── /header  
  │   ├── Game.hpp  
  │   ├── Mesh.hpp  
  │   ├── Utils.hpp  
  │   └── Window.hpp  
  ├── /imgui  
  │   └── ...  
  └── /stb_image  
      └── ...  
```

---

## File Structure (Executable)

```text
/resource  
├── /model  
│   └── house.obj  
├── /texture  
│   └── house_texture.png  
└── /shader  
    ├── vert.glsl  
    └── frag.glsl  
─openGL.exe  
─assimp-vc143-mt.dll  
─glew32.dll  
─glfw.dll  
```
