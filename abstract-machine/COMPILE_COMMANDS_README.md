# Compile Commands Generator

This directory contains tools to generate `compile_commands.json` for the Abstract Machine project, which provides IDE support for code completion, navigation, and analysis.

## Quick Start

### Method 1: Using Makefile (Recommended)

```bash
# Generate compile_commands.json for riscv32-nemu architecture
make ARCH=riscv32-nemu compile_commands

# Generate for other architectures
make ARCH=riscv64-nemu compile_commands
make ARCH=native compile_commands
```

### Method 2: Using the Shell Script

```bash
# Generate with default architecture (riscv32-nemu)
./gen_compile_commands.sh

# Generate for a specific architecture
./gen_compile_commands.sh riscv32-nemu
./gen_compile_commands.sh riscv64-nemu
```

## What Does It Do?

The script/Makefile target performs the following steps:

1. **Clean** previous builds in `am/` and `klib/` directories
2. **Build** the `am` (Abstract Machine) library with bear to capture compilation commands
3. **Build** the `klib` (Kernel Library) with bear to capture compilation commands
4. **Merge** all captured compilation commands into a single `compile_commands.json`
5. **Cleanup** temporary files

## Output

- **File**: `compile_commands.json`
- **Location**: `/home/trozure/ysyx-workbench/abstract-machine/`
- **Format**: JSON array containing compilation database entries
- **Usage**: Automatically recognized by clangd, ccls, and most modern IDEs (VSCode, CLion, Vim, etc.)

## Supported Architectures

All architectures supported by Abstract Machine:
- `riscv32-nemu`, `riscv64-nemu`, `riscv32e-nemu`
- `x86-nemu`, `x86_64-qemu`
- `native`
- And more...

## IDE Configuration

### VSCode
No additional configuration needed. Just open the project and the C/C++ extension will automatically use the `compile_commands.json`.

### CLion
CLion automatically detects `compile_commands.json` in the project root.

### Vim/Neovim with coc.nvim or ALE
The LSP server (clangd) will automatically discover and use the compilation database.

## Troubleshooting

### No compilation commands generated
- Make sure `bear` is installed: `sudo apt install bear`
- Check that `ARCH` is set correctly
- Verify that source files exist in `am/src/` and `klib/src/`

### Compilation errors during generation
This is expected! The script captures compilation commands even if linking fails (e.g., missing `main` function). The compilation commands are captured before the linking stage.

## Files

- `gen_compile_commands.sh` - Main script for generating compilation database
- `compile_commands.json` - Generated compilation database (auto-generated)
- `Makefile` - Contains the `compile_commands` target
