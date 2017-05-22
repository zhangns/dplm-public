GLASS := glass
BATCHGLASS := batchglass

# Dirs
BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src

# Files
GLASS_SRC_FILES := $(wildcard $(SRC_DIR)/$(GLASS)/*.cpp)
GLASS_OBJ_FILES := $(addprefix $(OBJ_DIR)/$(GLASS)/,$(notdir $(GLASS_SRC_FILES:.cpp=.o)))

BATCHGLASS_SRC_FILES := $(wildcard $(SRC_DIR)/$(BATCHGLASS)/*.cpp)
BATCHGLASS_OBJ_FILES := $(addprefix $(OBJ_DIR)/$(BATCHGLASS)/,$(notdir $(BATCHGLASS_SRC_FILES:.cpp=.o)))

# Environment
CC := g++
LD := g++

# Flags
W_FLAG := -Wall
STD_FLAG := -std=c++11
OP_FLAG := -O3 -Ofast -fexpensive-optimizations -ffast-math -funroll-loops
FLAGS := $(W_FLAG) $(STD_FLAG) $(OP_FLAG) -pthread
FL_CC_FLAGS := `fltk-config --use-gl --cxxflags`
FL_LD_FLAGS := `fltk-config --use-gl --ldflags`
LIB_PATHS := /usr/local/lib64 /opt/gcc-4.8.1/lib64
COMMA := ,
LIB_FLAGS := $(addprefix -Wl$(COMMA)-rpath$(COMMA),$(LIB_PATHS))

# Targets
all: $(GLASS) $(BATCHGLASS) $(GLASSCALC)

$(GLASS): $(GLASS_OBJ_FILES) $(BIN_DIR) FORCE
	$(CC) $(GLASS_OBJ_FILES) $(FLAGS) $(FL_LD_FLAGS) -lGL -lpng $(LIB_FLAGS) -o $(BIN_DIR)/$(GLASS)

$(OBJ_DIR)/$(GLASS)/%.o: $(SRC_DIR)/$(GLASS)/%.cpp $(OBJ_DIR)/$(GLASS) FORCE
	$(LD) $< -c $(FLAGS) $(FL_CC_FLAGS) -o $@

$(BATCHGLASS): $(BATCHGLASS_OBJ_FILES) $(BIN_DIR) FORCE
	$(CC) $(BATCHGLASS_OBJ_FILES) $(FLAGS) $(LIB_FLAGS) -o $(BIN_DIR)/$(BATCHGLASS)

$(OBJ_DIR)/$(BATCHGLASS)/%.o: $(SRC_DIR)/$(BATCHGLASS)/%.cpp $(OBJ_DIR)/$(BATCHGLASS) FORCE
	$(LD) $< -c $(FLAGS) -o $@
	
$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR)/$(GLASS):
	mkdir -p $@

$(OBJ_DIR)/$(BATCHGLASS):
	mkdir -p $@
	
clean:
	rm -rf $(OBJ_DIR)

FORCE:
