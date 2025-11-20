TOPNAME = shiftreg
VSRCS = shifter.v
CSRCS = sim_main.cpp

OBJ_DIR = obj_dir
BIN = $(OBJ_DIR)/V$(TOPNAME)

VERILATOR = verilator
VERILATOR_FLAGS = --cc --exe --build --trace -j 0 -Wall -Wno-DECLFILENAME

default: $(BIN)

$(BIN): $(VSRCS) $(CSRCS)
	$(VERILATOR) $(VERILATOR_FLAGS) $(VSRCS) $(CSRCS) --top-module $(TOPNAME)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(OBJ_DIR) dump.vcd

.PHONY: default run clean
