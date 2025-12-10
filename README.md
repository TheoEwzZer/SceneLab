# SceneLab

**A feature-rich 3D/2D scene editor with real-time path tracing**

![Build](https://github.com/TheoEwzZer/SceneLab/actions/workflows/docker-build.yml/badge.svg)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%2B-green.svg)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-red.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

SceneLab is a comprehensive 3D/2D scene editor developed as part of the **Computer Graphics course (IFT-3100)** at **Université Laval**. It demonstrates a wide range of computer graphics concepts, from basic 2D primitives to advanced GPU-accelerated path tracing with physically-based rendering.

---

## Features

### 1. Image
- **Image import** - Drag-and-drop with multiple format support (JPG, PNG, BMP, etc.)
- **Image export** - Frame capture and batch export sequences
- **Image sampling** - Configurable resolution and tile size sampling
- **Color palette** - Automatic palette extraction from imported images
- **Histogram** - Per-image histogram computation and visualization

### 2. Vector Drawing
- **Dynamic cursor** - Real-time cursor feedback during drawing operations
- **Drawing tools** - ImGui-based interface for primitive creation
- **Vector primitives** - Lines, circles, ellipses, rectangles, squares, triangles, regular polygons
- **Vector shapes** - Composite shapes (House, Doll, Letter A) with hierarchical composition
- **Interface** - Complete UI for vector drawing with fill/outline colors and width control

### 3. Transformations
- **Scene graph** - Hierarchical parent-child node structure with world matrix propagation
- **Multi-selection** - Select multiple objects with parent/child conflict validation
- **Interactive transforms** - ImGuizmo integration for translate/rotate/scale gizmos
- **Coordinate system** - World and local space transformation modes

### 4. Geometry
- **Bounding boxes** - Axis-aligned bounding box (AABB) visualization
- **Geometric primitives** - Cube, sphere, cylinder, plane, torus, cone
- **3D models** - OBJ file import with material support

### 5. Camera
- **Interactive camera** - WASD movement with mouse look controls
- **Multiple cameras** - Multi-camera management system
- **Multiple viewpoints** - Per-camera framebuffer rendering with independent viewports
- **Auto-focus** - Click-to-focus on image objects

### 6. Textures
- **Texture coordinates** - UV mapping support for all geometry
- **Filtering** - Multiple filter modes (None, Grayscale, Sharpen, Edge Detect, Blur)
- **Tone mapping** - Off, Reinhard, and ACES operators
- **Cubemap** - Skybox rendering and equirectangular-to-cubemap conversion
- **Procedural textures** - Checkerboard, Perlin noise, solid colors

### 7. Classic Illumination
- **Lighting models** - Lambert, Phong, Blinn-Phong, Gouraud shading
- **Materials** - Material presets with ambient, diffuse, specular, and shininess
- **Light types** - Directional, point (with attenuation), and spot lights
- **Multiple lights** - Support for multiple simultaneous light sources

### 8. Topology
- **Triangulation** - Delaunay triangulation for 2D point sets
- **Parametric curves** - Catmull-Rom spline interpolation
- **Normal mapping** - Relief effect via normal map textures

### 9. Ray Tracing
- **Intersection** - BVH acceleration structure for efficient ray-geometry tests
- **Reflection** - Specular reflection with importance sampling
- **Refraction** - Fresnel equations with configurable index of refraction
- **Shading** - Full path tracing with proper material shading
- **Global illumination** - Monte Carlo path tracing with frame accumulation

### 10. Modern Illumination
- **PBR** - Physically-Based Rendering with Cook-Torrance BRDF
- **Metallic** - Metallic/roughness workflow for materials
- **Microfacets** - GGX distribution with Schlick-Smith geometry term
- **Environment lighting** - Image-Based Lighting (IBL) with HDRI environment maps
- **Deferred rendering** - G-Buffer based deferred shading pipeline

---

## Technical Highlights

| Feature | Description |
|---------|-------------|
| **Dual Rendering Pipeline** | Seamlessly switch between rasterization and path tracing at runtime |
| **Modern C++20** | Structured bindings, concepts, and modern language features |
| **Clean Architecture** | `IRenderer` interface abstraction, composition-based feature managers |
| **23 GLSL Shaders** | Covering rasterization, path tracing, PBR, IBL, deferred rendering, and 2D graphics |
| **Scene Graph** | Hierarchical transform propagation with efficient traversal |

---

## Building

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+)
- CMake 3.16 or higher
- Git (for submodules)
- OpenGL 3.3+ compatible GPU

### Linux / macOS

```bash
git clone --recursive https://github.com/TheoEwzZer/scenelab.git
cd scenelab
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
./scenelab
```

### Windows (MinGW)

```powershell
git clone --recursive https://github.com/TheoEwzZer/scenelab.git
cd scenelab
mkdir build; cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./scenelab.exe
```

> **Note for Windows:** This project uses MinGW-w64. Install via [MSYS2](https://www.msys2.org/) or Chocolatey: `choco install mingw`

### Docker (Linux build)

Build and run using Docker:

```bash
docker build -t scenelab .
docker run --rm scenelab ./scenelab_tests
```

Or use docker-compose:

```bash
docker-compose build
```

### Running Tests

Unit tests use [GoogleTest](https://github.com/google/googletest) and are built with the `-DBUILD_TESTS=ON` flag:

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
cmake --build .
./scenelab_tests
```

### Pre-built Binaries

Linux binaries are automatically built and published to [GitHub Releases](https://github.com/TheoEwzZer/SceneLab/releases) on every push to `main`.

---

## Project Structure

```
scenelab/
├── src/                    # Implementation files
│   ├── App.cpp             # Main application controller
│   ├── renderer/           # Rasterization & path tracing renderers
│   ├── objects/            # Renderable objects (3D, 2D, lights)
│   ├── pbr/                # PBR and IBL implementation
│   ├── deferred/           # Deferred rendering pipeline
│   └── shapes/             # Parametric curves and triangulation
├── include/                # Header files
│   ├── renderer/
│   │   └── interface/      # IRenderer abstraction
│   └── ...
├── assets/
│   ├── shaders/            # GLSL vertex and fragment shaders
│   ├── hdri/               # HDR environment maps
│   └── objects/            # Example OBJ models
└── external/               # Git submodules (dependencies)
```

---

## Dependencies

All dependencies are included as Git submodules in `external/`:

| Library | Purpose |
|---------|---------|
| [GLFW](https://www.glfw.org/) | Window creation and input handling |
| [GLAD](https://glad.dav1d.de/) | OpenGL function loader |
| [GLM](https://github.com/g-truc/glm) | Mathematics library (vectors, matrices) |
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate-mode GUI framework |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) | 3D transform gizmos |
| [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) | File browser dialog |
| [stb_image](https://github.com/nothings/stb) | Image loading |
| [GoogleTest](https://github.com/google/googletest) | Unit testing framework (fetched via CMake) |

---

## Controls

| Key | Action |
|-----|--------|
| `W/A/S/D` | Camera movement |
| `Space/Ctrl` | Move up/down |
| `Right Click + Drag` | Camera look |
| `R` | Toggle renderer (Rasterization ↔ Path Tracing) |
| `Delete` | Delete selected object |

---

## License

This project was developed for educational purposes at Université Laval.
