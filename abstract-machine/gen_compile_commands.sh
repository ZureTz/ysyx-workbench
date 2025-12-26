#!/bin/bash
# Script to generate compile_commands.json for abstract-machine project
# Usage: ./gen_compile_commands.sh [ARCH]

set -e

# Default architecture
ARCH="${1:-riscv32-nemu}"

echo "Generating compile_commands.json for ARCH=$ARCH"
echo "=============================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Clean previous builds
echo "[1/5] Cleaning previous builds..."
(cd am && make clean > /dev/null 2>&1)
(cd klib && make clean > /dev/null 2>&1)
rm -f compile_commands_am.json compile_commands_klib.json

# Generate compile_commands.json for am
echo "[2/5] Generating compile_commands for am..."
(cd am && bear --output ../compile_commands_am.json -- make ARCH=$ARCH -j$(nproc) 2>&1 | grep -E "^\+" || true)

# Generate compile_commands.json for klib
echo "[3/5] Generating compile_commands for klib..."
(cd klib && bear --output ../compile_commands_klib.json -- make ARCH=$ARCH -j$(nproc) 2>&1 | grep -E "^\+" || true)

# Merge compile_commands.json files
echo "[4/5] Merging compile_commands.json files..."
python3 - <<'EOF'
import json
import os

commands = []
files = ['compile_commands_am.json', 'compile_commands_klib.json']

for file in files:
    if os.path.exists(file):
        with open(file, 'r') as f:
            data = json.load(f)
            commands.extend(data)
            print(f"  - Added {len(data)} entries from {file}")

with open('compile_commands.json', 'w') as f:
    json.dump(commands, f, indent=2)

print(f"  - Total: {len(commands)} compilation commands")

# Clean up temporary files
for file in files:
    if os.path.exists(file):
        os.remove(file)
EOF

echo "[5/5] Cleanup and verification..."
FILE_SIZE=$(du -h compile_commands.json | cut -f1)
ENTRY_COUNT=$(python3 -c "import json; print(len(json.load(open('compile_commands.json'))))")

echo ""
echo "=============================================="
echo "✓ Successfully generated compile_commands.json"
echo "  - Location: $SCRIPT_DIR/compile_commands.json"
echo "  - Size: $FILE_SIZE"
echo "  - Entries: $ENTRY_COUNT"
echo "  - Architecture: $ARCH"
echo "=============================================="
