# Physics Playground 🚀

An interactive visual sandbox and learning environment designed for **Technical Artists** and game developers to explore, visualize, and master real-time physics in game engines from first principles.

---

## 🎯 About The Project

In modern game engines (Unreal Engine, Unity, Godot, or custom proprietary engines), physics often feels like a black box of pre-packaged components, collision presets, and cryptic parameters (damping, restitution, stiffness, impulse thresholds).

**Physics Playground** bridges the gap between artistic intuition (motion curves, force behaviors, physical feel) and underlying engine mathematics. By providing a clean, zero-bloat C++ playground with live visual feedback, it enables Technical Artists to:
- **Demystify game physics equations** by testing and visualizing them directly in code.
- **Experiment with physical forces and constraints** in real time using interactive GUI controls.
- **Understand coordinate spaces and units** without the overhead of heavy game engines.
- **Build intuition for procedural animation and simulations** that can be directly applied to engine shaders, VFX graphs, rig setups, and gameplay mechanics.

---

## ✨ Features

- **📐 Interactive Coordinate System & Unit Scaling**
  - Dynamic Cartesian coordinate grid with origin centering and directional axes.
  - Bidirectional coordinate transformation: `worldToScreen()` and `screenToWorld()`.
  - Configurable unit scaling (`pixelsPerUnit`) to understand spatial scale and meter-to-pixel conversions.

- **🎛️ Real-Time Parameter Tuning (raygui)**
  - Live parameter tweaking (gravity, scale, forces) via lightweight immediate-mode GUI sliders.
  - Zero recompile turnaround: adjust values and immediately observe simulation response.

- **🪟 Minimalist, Modern Interface**
  - Custom borderless draggable window with edge-resizing and clean modern controls.
  - Custom typography loaded dynamically for crisp labeling and high-DPI readability.

- **⚡ Lightweight & Dependency-Free Setup**
  - Powered by **[raylib](https://www.raylib.com/)** and **[raygui](https://github.com/raysan5/raygui)**.
  - Automated dependency fetching via CMake `FetchContent`—no manual library installations or path configurations required.
  - Cross-platform targeting: Desktop (Windows, macOS, Linux) and Web (WebAssembly via Emscripten).

---

## 🗺️ Learning Roadmap & Physics Modules

This repository is structured to evolve into a comprehensive interactive textbook for Technical Artists:

1. **Vectors & Coordinate Spaces**
   - Vectors, magnitudes, normalization, and distance metrics.
   - Dot product (lighting, surface alignment, field of view) & 2D cross product / perpendiculars.
   - World space vs. Screen space vs. Local space transformations.

2. **Kinematics & Motion**
   - Position, linear velocity, acceleration, and jerk.
   - Projectile trajectories and gravity simulations.
   - Analytical trajectories vs. step-by-step numerical integration.

3. **Numerical Integration Methods**
   - Explicit Euler integration (and why it explodes).
   - Semi-implicit (Symplectic) Euler.
   - Verlet integration (ideal for cloth, ropes, and verlet ragdolls).
   - Runge-Kutta (RK4) for precision simulations.

4. **Forces, Friction & Damping**
   - Newton's Laws of Motion ($F = ma$).
   - Drag, air resistance, and terminal velocity.
   - Static vs. kinetic friction models.

5. **Springs, Constraints & Soft Bodies**
   - Hooke's Law and damped harmonic oscillators.
   - Distance constraints for ropes, chains, and hair dynamics.
   - Particle-based cloth simulation basics.

6. **Collision Detection & Impulse Resolution**
   - Broadphase vs. narrowphase concepts.
   - Circle-to-circle and AABB (Axis-Aligned Bounding Box) intersection tests.
   - Elasticity, restitution ($e$), and impulse-based collision response.

---

## 🛠️ Tech Stack & Requirements

- **Language:** Modern C++ (C++14 or higher)
- **Frameworks:**
  - [raylib 6.0](https://www.raylib.com/) (Graphics, input, and math primitives)
  - [raygui 5.0](https://github.com/raysan5/raygui) (Immediate mode GUI)
- **Build System:** [CMake](https://cmake.org/) (Version 3.14+)
- **Compiler:** Any modern C++ compiler:
  - Windows: MSVC (Visual Studio 2019+ or Build Tools), Clang, or MinGW
  - macOS: Apple Clang (Xcode / Command Line Tools)
  - Linux: GCC or Clang

> [!NOTE]
> Raylib and raygui are fetched automatically during the CMake configure step via `FetchContent`. You do not need to pre-install them on your system.

---

## 🚀 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/AminKhormaei/PhysicsPlayground.git
cd PhysicsPlayground/CMake
```

### 2. Build for Desktop

#### Using CMake CLI:
```bash
# Configure build files
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile the project
cmake --build build --config Release
```

The resulting executable will be located inside `build/` (or `build/Release/` on Windows MSVC).

#### Using an IDE:
- **Visual Studio / JetBrains Rider / CLion / VS Code**: Simply open the `CMake/` folder as a CMake project. Select `PhysicsPlayground` as the target and click **Build & Run**.

---

### 3. Build for Web (WebAssembly)

Compiling to WebAssembly allows sharing interactive physics demos in a browser using the [Emscripten SDK](https://emscripten.org/):

```bash
mkdir build-web
cd build-web
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
emmake make
```

Serve the resulting HTML with a local web server (e.g. `python -m http.server 8080`) and open `http://localhost:8080/PhysicsPlayground.html`.

---

## 📂 Project Structure

```text
PhysicsPlayground/CMake/
├── CMakeLists.txt        # Automated build setup & dependency management
├── main.cpp              # Application entry point, window management & main loop
├── Tools/
│   ├── coordinate.h      # Coordinate conversion, origin & tick helper declarations
│   └── coordinate.cpp    # Screen-to-world & world-to-screen math implementation
├── resources/
│   ├── app.ico / app.png # Application icons
│   ├── app.rc            # Windows resource file
│   └── fonts/            # Custom UI typography (BigShoulders, Saira Condensed)
└── README.md             # Project documentation & guide
```

---

## 💡 How to Add Your Own Physics Experiment

Want to test a new equation or simulation idea? Here is a quick guide to getting started:

1. **Convert to World Space**: Use `co->screenToWorld()` and `co->worldToScreen()` so your math stays in meaningful physical units (meters) rather than arbitrary pixel offsets.
2. **Expose Parameters**: Add a `GuiSliderBar()` in `main.cpp` for any variable you want to tweak (e.g., damping coefficient, spring stiffness, mass).
3. **Simulate in Fixed Steps**: Accumulate `GetFrameTime()` to run your physics simulation with a fixed time-step $\Delta t$ for stability.
4. **Draw the Visualization**: Use raylib's drawing routines (`DrawCircleV`, `DrawLineEx`, `DrawSplineSegmentLinear`) to render positions, velocity vectors, and trajectory trails.

---

## 🤝 Contributing

Contributions, feature requests, and new physics visualizers are welcome!
If you are a Technical Artist or Engine Developer with an interesting simulation problem or demonstration, feel free to open an Issue or submit a Pull Request.

---

## 📄 License

This project is open-source. Please see the [LICENSE](LICENSE) file for more information.
