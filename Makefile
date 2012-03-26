#  Copyright (C) 2002 -- Daniel Neilson

# To add files to the project, scroll down to OBJ_FILES =
#  add any files that you add to the project to that assignment.
#  That should be all that is required. It's like magic!

.SUFFIXES = .o .cpp .c

# What compiler are we going to use?
CC = gcc
CXX = g++

# Version #'s of the program

# Flags for the preprocessor.
CPPFLAGS = -Iinclude -Idependencies/include/assimp

# Our include directories
LDFLAGS = -Ldependencies/bin -Wl,-Bstatic -lassimp -Wl,-Bdynamic -lGL -lglfw -lm -lXrandr -lpng

# Set the compile flags depending on the make target

ifeq ($(MAKECMDGOALS),release)
#  If the release target is selected, then add in optimizations
# and turn off debugging
CPPFLAGS := $(CPPFLAGS) -DNDEBUG
CFLAGS = -Wall -msse2 -O2 -mfpmath=sse
else
#  If not release, then it's debug. 
CFLAGS = -Wall -Wextra -g
endif

# Code uses stuff from the C++0x standard, so set the dialect to that.
CXXFLAGS = $(CFLAGS) -std=c++0x

OBJ_FILES = \
	src/root.o \
	src/main.o \
	src/camera.o \
	src/glUtils.o \
	src/ui.o \
	src/gl3/gl3w.o \
	src/shaders/shader.o \
	src/shaders/manager.o \
	src/shaders/glprogram.o \
	src/shaders/material.o \
	src/shaders/constant/specular/gouraud.o \
	src/shaders/constant/specular/phong.o \
	src/shaders/constant/lambertian/gouraud.o \
	src/shaders/constant/lambertian/phong.o \
	src/shaders/constant/simple.o \
	src/shaders/constant/depth.o \
	src/shaders/texture/specular/gouraud.o \
	src/shaders/texture/specular/phong.o \
	src/shaders/texture/lambertian/gouraud.o\
	src/shaders/texture/lambertian/phong.o \
	src/shadowmap.o \
	src/texture/texture.o \
	src/texture/decoders/decoder.o \
	src/texture/decoders/png.o \
	src/objects/mesh.o \
	src/objects/models/sphere.o \
	src/objects/models/octahedron.o \
	src/objects/models/plane.o \
	src/objects/object.o \
	src/objects/geometry.o \
	src/external/lodepng.o \
	src/shaders/deferred/geometrypass.o \
	src/shaders/deferred/directionallightpass.o \
	src/shaders/deferred/pointlightpass.o \
	src/lights.o \
	src/colors.o \
	src/objects/models/quad.o \
	src/gbuffer.o


# What are we going to call our executable
OUT_FILE = fan780-Deferred-CSM

debug: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(OUT_FILE) $(LDFLAGS)

release: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(OUT_FILE) $(LDFLAGS)


# The rule for making the .d files from the .c & .cpp files
# The 'sed' part just makes it so that the generated .d file will depend on 
# the same things as the .o file -- It also adds a dependancy on this file
# to the .o & the .d
%.d: %.cpp
	@echo 'Creating/Updating $*.d'
	@set -e; $(CXX) -MM -w $(CXXFLAGS) $< \
	| sed 's/\($(notdir $*)\)\.o[ :]*/$(subst /,\/,$(@:.d=.o)) $(subst /,\/,$(@)) : Makefile /g' \
	> $@; \
	[ -s $@ ] || rm -f $@
%.d: %.c
	@echo 'Creating/Updating $*.d'
	@set -e; $(CXX) -MM -w $(CXXFLAGS) $< \
	| sed 's/\($(notdir $*)\)\.o[ :]*/$(subst /,\/,$(@:.d=.o)) $(subst /,\/,$(@)) : Makefile /g' \
	> $@; \
	[ -s $@ ] || rm -f $@

# include the dependency files.
# -- If they're not there, then the "%.d: %.c" & "%.d: %.cpp" rule will 
# make them
ifneq (clean,$(findstring clean,$(MAKECMDGOALS)))
-include $(OBJ_FILES:.o=.d)
endif

clean: clean_obj clean_tilde clean_core
	@echo Deleting executable
	@[ ! -f $(OUT_FILE) ] || rm $(OUT_FILE)

clean_obj: FORCE
	@echo Deleting object files
	@rm -f $(OBJ_FILES)
	@rm -f $(OBJ_FILES:.o=.d)

clean_tilde: FORCE
	@echo Deleting temporary files.
	@rm -f `find . -name "*~"` 
	@rm -f `find . -name ".#*"`

clean_core: FORCE
	@echo Deleting core
	@[ ! -f core ] || rm -f core
	@rm -f core*

FORCE:

