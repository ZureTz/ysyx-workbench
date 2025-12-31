TOPNAME = top
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.cc" -or -name "*.cpp")

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

VERILATOR = verilator
VERILATOR_FLAGS = --cc --exe --build --trace -j 0 -Wall -Wno-DECLFILENAME -CFLAGS "-std=c++23 -I$(abspath ./csrc/include)"

IMG ?=

sim: $(BIN)
	$(BIN) $(IMG)

$(BIN): $(VSRCS) $(CSRCS)
	@mkdir -p $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $(VSRCS) $(CSRCS) \
		--Mdir $(OBJ_DIR) -o $(abspath $(BIN))

clean:
	rm -rf $(BUILD_DIR)

