# DirectX Learning Project Documentation

## Project Overview   ![CodeRabbit Pull Request Reviews](https://img.shields.io/coderabbit/prs/github/Raziel-chan/DirectXLearning?utm_source=oss&utm_medium=github&utm_campaign=Raziel-chan%2FDirectXLearning&labelColor=171717&color=FF570A&link=https%3A%2F%2Fcoderabbit.ai&label=CodeRabbit+Reviews)
A learning project for understanding DirectX 11 graphics programming, starting from basic Windows application development to advanced graphics techniques.

## Project Setup
### Requirements
- Visual Studio 2022
- Windows SDK
- DirectX Development Tools
- Windows 10/11 with DirectX 11 support

### Project Structure
```
DirectXLearning/
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── Graphics/
│   │   ├── GraphicsEngine.h     # DirectX core functionality
│   │   └── GraphicsEngine.cpp   # Graphics implementation
│   └── Window/
│       ├── Window.h             # Window management
│       └── Window.cpp           # Window implementation
├── shaders/
│   ├── VertexShader.hlsl       # Vertex shader
│   └── PixelShader.hlsl        # Pixel shader
└── README.md                    # Project documentation                
```

## Learning Progress
### Phase 1: Project Setup and Planning ✓
- [x] Created Visual Studio 2022 project
- [x] Set up project structure
- [x] Initialized Git repository
- [x] Created documentation structure

### Phase 2: Windows Programming Fundamentals ✓
- [x] Created Window class with RAII principles
- [x] Implemented window message handling system
- [x] Set up main application loop with proper resource management
- [x] Added debug output system for development

### Phase 3: DirectX Initialization ✓
- [x] DirectX device setup with error handling
- [x] Swap chain creation and configuration
- [x] Render target view implementation
- [x] Basic frame clearing and presentation
- [x] Integration with window system

### Phase 4: Graphics Pipeline Understanding ✓
- [x] Implemented VSync control
- [x] Understood buffering systems (double/triple buffering)
- [x] Learned about monitor refresh rate synchronization
- [x] Implemented proper graphics resource management using ComPtr

### Phase 5: Basic Rendering ✓
- [x] Created and managed vertex buffers
- [x] Implemented basic shader system
- [x] Set up vertex and pixel shaders
- [x] Implemented basic shape rendering (colored triangle)
- [x] Added transformation systems (rotation)

### Phase 6: User Interaction ✓
- [x] Implemented keyboard input handling
- [x] Added mouse movement tracking
- [x] Created camera controls (WASD movement)
- [x] Added object manipulation (rotation with arrow keys)
- [x] Implemented view rotation with mouse

### Phase 7: Texturing (Current)
- [ ] Update vertex format to include texture coordinates
- [ ] Create texture loading system
- [ ] Implement texture sampling in shaders
- [ ] Apply textures to geometry
- [ ] Add texture filtering and addressing modes

### Future Phases
- [ ] Lighting Systems
- [ ] 3D Model Loading
- [ ] Advanced Rendering Techniques
- [ ] Performance Optimization


## Learning Goals
1. Graphics Programming Fundamentals
   - Modern C++ practices in graphics programming
   - DirectX 11 core concepts
   - Graphics pipeline architecture
   - Resource management and optimization

2. Advanced Graphics Techniques
   - Shader development and HLSL
   - Buffer management and memory optimization
   - Advanced rendering techniques
   - Performance optimization

## Development Notes
- Started: Dec 12, 2024
- Focuses on practical DirectX 11 implementation
- Implements modern C++ practices
- Emphasizes proper resource management
- Includes detailed error handling and debugging

## Technical Achievements
- Stable window management with proper cleanup
- DirectX device and swap chain initialization
- Proper resource management using ComPtr
- Frame presentation with VSync support
- Comprehensive error handling system
- Interactive 3D scene with user controls

## Resources and References
- Microsoft DirectX Documentation
- Windows Graphics Programming Guide
- Microsoft DirectX Graphics Samples
- Claude AI
