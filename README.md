# **wxwidgets-opengl-demo**

A minimal desktop application example built with **wxWidgets + OpenGL**, demonstrating rendering of a 2D scene inside wxGLCanvas, and drawing a **clickable overlay button (from a PNG icon) directly in the OpenGL context**. Clicking the overlay button toggles the right-side **wxWidgets native controls** panel (wxSlider, wxCheckBox); the panel controls directly affect the OpenGL scene (rotation angle, object visibility).

> Goal: demonstrate **decoupling of rendering logic and UI code**, a clean maintainable project structure, and **modern CMake** self-contained builds.

------

## **Feature Overview**

- Main window created with **wxWidgets**, with the main content area as wxGLCanvas.
- **OpenGL** renders a simple 2D scene (colored triangle) in the canvas.
- Overlay button drawn inside **OpenGL** (screen coordinates, PNG texture) with **pixel-level hit testing**.
- Clicking the overlay button → toggles visibility of the **wxWidgets side panel** on the right.
- Side panel includes:
  - wxSlider: adjusts scene rotation angle (0–360°).
  - wxCheckBox: toggles object visibility.
- **HiDPI**: both drawing and hit testing account for DPI scaling.

------

## **Directory Structure**

```
wxwidgets-opengl-demo/
├─ CMakeLists.txt
├─ README.md                          # Linux build instructions (clone → configure → build → run)
├─ LICENSE
├─ .gitignore
├─ cmake/
│  ├─ ConfigWarnings.cmake            # Unified compiler warnings and strict build options
│  └─ ToolchainHints.cmake            # Optional: hints/reads CMAKE_PREFIX_PATH etc.
├─ resources/
│  └─ icons/
│     └─ toggle.png                   # PNG icon for the in-canvas overlay button
├─ third_party/
│  ├─ glad/
│  │  ├─ include/
│  │  │  ├─ glad/glad.h
│  │  │  └─ KHR/khrplatform.h
│  │  └─ src/glad.c                   # OpenGL function loader (compiled with project)
│  └─ stb/
│     └─ stb_image.h                  # Single-header PNG decoder (for icon loading)
├─ src/
│  ├─ app/                            # wxWidgets-related code only (UI layer)
│  │  ├─ main.cpp                     # wxApp entry point
│  │  ├─ MainFrame.h/.cpp             # Main window: left GL canvas + right side panel
│  │  ├─ GLCanvas.h/.cpp              # Derived from wxGLCanvas; forwards size/draw/mouse events
│  │  ├─ SidePanel.h/.cpp             # Right panel: wxSlider (rotation) + wxCheckBox (visibility)
│  │  └─ Events.h                     # Custom event declarations (e.g. EVT_TOGGLE_SIDEBAR)
│  └─ render/                         # Pure rendering module (wxWidgets-independent)
│     ├─ RenderState.h                # State owned/passed by UI: rotation, scale, visibility
│     ├─ Renderer.h/.cpp              # Rendering core: init/reset viewport/draw/hit testing
│     ├─ Scene.h/.cpp                 # Simple 2D geometry scene, applies RenderState
│     ├─ UIOverlay.h/.cpp             # Overlay button (textured quad) and screen-space layout
│     ├─ Texture.h/.cpp               # PNG → OpenGL texture (wraps stb_image + GL objects)
│     ├─ Shader.h/.cpp                # Shader compile/link and error logging
│     ├─ Mesh.h/.cpp                  # RAII wrapper for generic mesh (VBO/IBO/VAO)
│     ├─ Quad.h/.cpp                  # Reusable rectangle mesh (for overlay/button/background)
│     └─ GlCheck.h                    # GL debug macros/error checks (compile-time switch)
└─ .github/
   └─ workflows/
      └─ ci.yml                       # (Optional) Linux CI: cmake -B build && cmake --build build
```

------

## **Build Requirements**

- **CMake ≥ 3.16**
- **C++14** (minimum; can be raised to C++17/20)
- **wxWidgets** (recommended 3.2.x, GTK3 backend)
- **OpenGL development package** (Mesa/driver headers + libs)
- C/C++ compiler (GCC/Clang/MSVC, etc.)

> The repository already includes **glad** and **stb_image**, no extra downloads required.

------

## **Linux Build from Scratch (Example)**

> Package names may vary slightly by distribution version.

### **1) Install Dependencies**

**Debian/Ubuntu (22.04+/24.04+)**

```
sudo apt-get update
sudo apt-get install -y build-essential cmake \
    libgl1-mesa-dev libgtk-3-dev libwxgtk3.2-dev
```

**Fedora/RHEL (Fedora 39+ example)**

```
sudo dnf install -y gcc-c++ cmake make \
    wxGTK3-devel gtk3-devel mesa-libGL-devel
```

**Arch Linux**

```
sudo pacman -S --needed base-devel cmake wxwidgets-gtk3 mesa
```

**openSUSE (Leap/Tumbleweed)**

```
sudo zypper install -y gcc-c++ cmake make \
    wxWidgets-3_2-devel libGL-devel gtk3-devel
```

> If multiple wx-config versions are installed, specify the path via a CMake variable (see **Toolchain Hints**).

### **2) Configure & Build**

```
git clone https://github.com/07c2/wxwidgets-opengl-demo.git
cd wxwidgets-opengl-demo

cmake -B build -S .
cmake --build build -j
```

### **3) Run**

```
./build/wxwidgets_opengl_demo
```

> After build, CMake copies resources/ into the executable directory for direct run.

------

## **macOS & Windows (Summary)**

- **macOS (Homebrew)**

```
brew install wxwidgets cmake
cmake -B build -S .
cmake --build build -j
./build/wxgl_overlay_demo.app/Contents/MacOS/wxwidgets_opengl_demo
```

- macOS OpenGL is “deprecated” but still usable. Works fine on wxWidgets 3.2+. If wx-config isn’t found, add Homebrew prefix to PATH or use -DWXGL_WX_CONFIG=/opt/homebrew/bin/wx-config.
- **Windows (MSYS2 MinGW64)**

```
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-wxWidgets
cmake -B build -S . -G "MinGW Makefiles"
cmake --build build -j
build\wxwidgets_opengl_demo.exe
```

- **Windows (vcpkg, optional)**

```
vcpkg install wxwidgets
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake
cmake --build build -j
```

------

## **Run & Interaction**

- **Overlay button**: bottom-right of canvas (PNG icon); toggles side panel on/off.
- **Slider (Rotation)**: adjusts triangle rotation angle (degrees).
- **Checkbox (Show Object)**: toggles triangle visibility.
- Refresh via wxTimer (~60 FPS), can be changed to “redraw on interaction only.”

------

## **Architecture Notes**

- src/app/* (UI layer, **depends on wxWidgets**)
  - MainFrame: main window + layout (left GLCanvas, right SidePanel).
  - GLCanvas: derived from wxGLCanvas, manages wxGLContext, bridges Paint/Size/Mouse events to Renderer.
  - SidePanel: native controls (Slider/CheckBox), calls GLCanvas setters to drive render state.
  - Events.h: custom events (overlay click → toggle sidebar).
- src/render/* (Rendering layer, **wxWidgets-independent**)
  - Renderer: main entry; manages Scene, UIOverlay, viewport/DPI; exposes SetRotation/SetScale/SetObjectVisible + HitTestOverlay.
  - Scene: draws simple 2D triangle; applies RenderState.
  - UIOverlay: screen-space button, loads Texture (PNG), hit-test & draw.
  - Texture: stb_image-based PNG → OpenGL texture (RAII).
  - Shader, Mesh, Quad: reusable OpenGL resource/mesh wrappers.
  - GlCheck.h: GL debug/error macros (switchable).

> This separation ensures rendering components are reusable; UI acts as a “client” communicating through clean interfaces.

------

## **CMake Options & Toolchain Hints**

- **C++ Standard**: default C++14 (-DCMAKE_CXX_STANDARD=17 to raise).
- **Warnings & Werror** (cmake/ConfigWarnings.cmake):
  - -DWXGL_ENABLE_WARNINGS=ON|OFF (default ON)
  - -DWXGL_WARNINGS_AS_ERRORS=ON|OFF (default ON in CI)
- **OpenGL Debug** (src/render/GlCheck.h):
  - -DWXGL_ENABLE_GL_CHECKS=ON|OFF (default ON in Debug)
- **wxWidgets Location** (cmake/ToolchainHints.cmake):
  - -DWXGL_WX_CONFIG=/path/to/wx-config
  - -DWXGL_EXTRA_PREFIX_PATH=/opt/local (append to CMAKE_PREFIX_PATH)
- **Resource Directory Macro**:
  - APP_RESOURCE_DIR defined at build time, points to resources/, copied alongside executable after build.

------

## **FAQ**

- **CMake can’t find wxWidgets / wx-config**
  - Ensure libwxgtk3.2-dev (or equivalent) installed and wx-config --version works.
  - Use -DWXGL_WX_CONFIG=/full/path/to/wx-config or add dir to PATH.
  - Or add -DWXGL_EXTRA_PREFIX_PATH=/usr/local etc.
- **Overlay button missing at runtime**
  - Ensure resources/icons/toggle.png exists next to binary (CMake auto-copies).
  - Or define APP_RESOURCE_DIR pointing to resources dir.
- **OpenGL function loading fails**
  - gladLoadGL() must be called with a valid context; project calls after GLCanvas init.
  - Verify drivers and OpenGL dev packages installed.
- **Wayland flicker/compatibility issues (some distros)**
  - Temporarily set GDK_BACKEND=x11 before run to confirm backend issue.

------

## **License & Third-Party Credits**

- Licensed under repository root **LICENSE**.
- Bundled third-party components:
  - **glad** (minimal loader, header + src): OpenGL function loading.
  - **stb_image.h**: Sean Barrett’s public-domain/MIT-compatible header-only image decoder (used only for PNG).

> Replace with official/full versions and follow licenses for production use.

------

## **References (Further Reading)**

- wxWidgets manual (wxGLCanvas, events/layout, wxTimer, etc.)
- CMake docs: find_package(wxWidgets), FindOpenGL, modern targets
- OpenGL tutorials: projection/viewport, shaders, textures, blending
- stb (stb_image.h) & glad project homepages

> See official sites or repos for resources.

------

## **Version & Standards**

- **Minimum C++ standard**: C++14
- **Recommended**: C++17 (for modern features like std::optional)