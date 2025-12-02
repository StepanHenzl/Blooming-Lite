**Bloom-Lite** 


Bloom-Lite is a lightweight rogue-like game running on a custom C++ engine compiled to WebAssembly using Emscripten.

**How to build** 

Optional: The Built game is available under Releases. 
The game is also playable on: https://stevebohrich.itch.io/blooming-like 

Requires: 
  * Emscripten SDK (available in your PATH)
  * CMake
  * A C++ compiler
  
  1) Clone the repository
  2) Run "CompileAndRun.bat"
  
**Technical Details** 

Bloom-Lite is built on a lightweight, purpose-built C++ engine compiled to WebAssembly via Emscripten. 
Features: 
  * Managing a 2D pixel buffer and rendering it using WebGL through Emscripten’s HTML5 API
  * Translating browser input callbacks (keyboard/mouse) into an internal input state used by the game
  * Data-driven tile/grid system optimised for deterministic gameplay
  * Basic memory management and performance tuning for WASM
  
Tech used:
  * C++17 for all game logic
  * Emscripten for WebAssembly compilation and HTML5 API bindings
  * JavaScript glue for small tasks like canvas setup and WASM communication
  * Python scripts used to automate asset/data generation
  
What I built:

I developed the complete engine layer, rendering pipeline, game architecture, and all gameplay systems myself. 

The project served as a testbed for my low-level WebAssembly rendering pipeline and gameplay architecture. 

**Game Concept** 

Bloom-Lite is a rogue-like strategy game played on a 64-tile grid. 
Each round gives you a limited number of clicks to build and upgrade plants. 
Your goal is to reach a required score before each deadline. 
At the end of every deadline, you choose one of three upgrade cards. 

Cards provide you with effects that help you get more score at the end of each deadline. 
By combining card choices and strategic placement, you try to push your score as far as possible.
