CXX = g++
CXXFLAGS = -O3 -fdiagnostics-generate-patch -fdiagnostics-path-format=inline-events \
           -MMD -MP \
           -pg -ggdb -std=c++17 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
           -Wc++14-compat -Wuseless-cast -Wmissing-declarations -Wcast-align -Wcast-qual \
           -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
           -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
           -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor \
           -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
           -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo \
           -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
           -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override \
           -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code \
           -Wunused -Wvariadic-macros -Wno-literal-suffix \
           -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast \
           -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
           -fstack-protector -fstrict-overflow -flto-odr-type-merging \
           -fno-omit-frame-pointer -Wstack-usage=8192 \
           -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
EXECUTABLE = sim

SRC_DIR = ./source
OBJ_DIR = Apps

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEP_FILES = $(OBJECTS:.o=.d)

.PHONY: all clean run check-style format

all: check-style format $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking $@..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build completed successfully!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP_FILES)

$(OBJ_DIR):
	@mkdir -p $@
	@echo "Created object directory: $@"

clean:
	@rm -rf $(OBJ_DIR) $(EXECUTABLE)
	@echo "Clean completed!"

run:
	@./$(EXECUTABLE)

SRC_FILES = $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.hpp')

check-style:
	@which clang-format >/dev/null || (echo "Error: clang-format not installed"; exit 1)
	@clang-format --dry-run --Werror -n $(SRC_FILES) && echo "Style check passed!"

format:
	@which clang-format >/dev/null || (echo "Error: clang-format not installed"; exit 1)
	@clang-format -i -style=file $(SRC_FILES)
	@echo "Code formatted successfully!"