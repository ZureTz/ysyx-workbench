# Usage Examples

## Example 1: Quick Generate for Default Architecture

```bash
cd /home/trozure/ysyx-workbench/abstract-machine
./gen_compile_commands.sh
```

Output:
```
Generating compile_commands.json for ARCH=riscv32-nemu
==============================================
[1/5] Cleaning previous builds...
[2/5] Generating compile_commands for am...
+ CC src/platform/nemu/trm.c
+ CC src/platform/nemu/ioe/ioe.c
...
==============================================
✓ Successfully generated compile_commands.json
  - Location: /home/trozure/ysyx-workbench/abstract-machine/compile_commands.json
  - Size: 28K
  - Entries: 17
  - Architecture: riscv32-nemu
==============================================
```

## Example 2: Generate Using Makefile

```bash
cd /home/trozure/ysyx-workbench/abstract-machine
make ARCH=riscv32-nemu compile_commands
```

## Example 3: Generate for Different Architecture

```bash
cd /home/trozure/ysyx-workbench/abstract-machine
./gen_compile_commands.sh riscv64-nemu
```

## Example 4: Integrate with Your Workflow

Add to your project's workflow:

```bash
# In your build script
cd abstract-machine
make ARCH=riscv32-nemu compile_commands
cd ..

# Now your IDE will have proper code completion
```

## Verifying the Generated File

```bash
# Check file size and entries
python3 -c "import json; d=json.load(open('compile_commands.json')); print(f'Entries: {len(d)}')"

# View first entry
python3 -c "import json, pprint; d=json.load(open('compile_commands.json')); pprint.pprint(d[0])"
```
