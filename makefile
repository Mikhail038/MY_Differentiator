
VR_FLAGS = -D _DEBUG -g -ggdb3 -std=c++17 -O0

ASSAN = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DOP = -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE

DIRECTORIES = -IDIFFERENTIATOR -IEXTRA

VR_FLAGS += $(ASSAN)

VR_FLAGS += $(DIRECTORIES)

VR_FLAGS += $(DOP)

#==============================================================================================================================================

VR_COMPILER = g++

#==============================================================================================================================================

DO: DO_DIFFERENTIATOR

#==============================================================================================================================================

DO_DIFFERENTIATOR: FOLDERS OBJECTS/diff.o OBJECTS/main_diff.o
	$(VR_COMPILER) OBJECTS/diff.o OBJECTS/main_diff.o -o diff $(VR_FLAGS)

#==============================================================================================================================================

OBJECTS/diff.o: DIFFERENTIATOR/diff.cpp
	$(VR_COMPILER) -c -o  OBJECTS/diff.o DIFFERENTIATOR/diff.cpp $(VR_FLAGS)

OBJECTS/main_diff.o: DIFFERENTIATOR/main_diff.cpp
	$(VR_COMPILER) -c -o OBJECTS/main_diff.o DIFFERENTIATOR/main_diff.cpp $(VR_FLAGS)

#==============================================================================================================================================

FOLDERS:
	mkdir -p OBJECTS; mkdir -p GRAPH_VIZ
