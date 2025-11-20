# GitHub Copilot Instructions for ysyx-workbench

This repository is the workspace for the "One Student One Chip" (ysyx) project. It contains Verilog hardware designs, C++ simulation testbenches (Verilator), and the NVBoard virtual FPGA platform.

## Project Architecture

- **`npc/` (New Processor Core)**: The main CPU implementation.
  - Uses `xmake` or `Makefile` for build management.
  - Source code split into `vsrc/` (Verilog) and `csrc/` (C++ testbench).
- **`nvboard/` (NJU Virtual Board)**: A virtual FPGA board library based on SDL.
  - Provides virtual peripherals (LEDs, VGA, UART, Keyboard).
  - **Integration**: Projects link against `nvboard` and use `.nxdc` files for pin constraints.
- **`verilog-simulation/`**: Standalone Verilog examples and unit tests.
  - Each subdirectory is a self-contained Verilator project.
- **`HDLBits/`**: Verilog practice exercises.

## Critical Workflows

### 1. Building and Running Simulations
- **Standard Verilator Flow** (used in `verilog-simulation`):
  - First `cd` into the project directory, e.g., `cd verilog-simulation/counter`.
  - Run `make run` to compile and execute the simulation.
  - Run `make clean` to remove build artifacts (`obj_dir`, `dump.vcd`).
  - Run `make clean && bear -- make run` to compile and generate compilation database (`compile_commands.json`).
  - **Command**: `verilator --cc --exe --build --trace ...`
- **NPC Simulation**:
  - Use `xmake run npc` or `make sim` (if configured).

### 2. NVBoard Projects
- **Pin Binding**:
  - Define pin constraints in `constr/*.nxdc`.
  - The build process uses `python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py` to generate C++ binding code (`auto_bind.cpp`).
- **Build**:
  - Typically uses a `Makefile` that includes `$(NVBOARD_HOME)/scripts/nvboard.mk`.
  - Generates executable in `build/`.

### 3. Debugging
- **Waveforms**:
  - Simulations typically generate `dump.vcd`.
  - View with GTKWave: `gtkwave dump.vcd`.
- **Verilator Testbenches (`sim_main.cpp`)**:
  - Use `VerilatedContext` and `VerilatedVcdC` for time management and tracing.
  - **Pattern**:
    ```cpp
    void step_and_dump_wave() {
      top->eval();
      contextp->timeInc(1);
      tfp->dump(contextp->time());
    }
    ```

## Project Conventions

- **File Structure**:
  - `vsrc/`: Verilog source files (`.v`).
  - `csrc/`: C++ simulation sources (`.cpp`, `.cc`).
  - `constr/`: Constraint files (`.nxdc`).
  - `build/` or `obj_dir/`: Compilation artifacts (ignored by git).
- **Environment Variables**:
  - `NVBOARD_HOME`: Must point to the `nvboard` directory for NVBoard-based projects.
- **Verilator Usage**:
  - Always enable tracing (`--trace`).
  - Use `top->eval()` to advance logic.
  - Manage simulation time manually via `contextp->timeInc()`.

## Code Style
- **Verilog**: Use explicit module port declarations.
- **C++**: Follow standard C++11/14 practices for testbenches.
