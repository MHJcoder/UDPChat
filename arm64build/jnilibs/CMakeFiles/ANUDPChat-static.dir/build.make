# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/mahuijie/Desktop/ChatLib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/mahuijie/Desktop/ChatLib/arm64build

# Include any dependencies generated for this target.
include jnilibs/CMakeFiles/ANUDPChat-static.dir/depend.make

# Include the progress variables for this target.
include jnilibs/CMakeFiles/ANUDPChat-static.dir/progress.make

# Include the compile flags for this target's objects.
include jnilibs/CMakeFiles/ANUDPChat-static.dir/flags.make

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o: jnilibs/CMakeFiles/ANUDPChat-static.dir/flags.make
jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o: ../jnilibs/udpchat.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/mahuijie/Desktop/ChatLib/arm64build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o"
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && /Users/mahuijie/Desktop/android-toolchain-24/bin/aarch64-linux-android-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o -c /Users/mahuijie/Desktop/ChatLib/jnilibs/udpchat.cpp

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.i"
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && /Users/mahuijie/Desktop/android-toolchain-24/bin/aarch64-linux-android-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/mahuijie/Desktop/ChatLib/jnilibs/udpchat.cpp > CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.i

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.s"
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && /Users/mahuijie/Desktop/android-toolchain-24/bin/aarch64-linux-android-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/mahuijie/Desktop/ChatLib/jnilibs/udpchat.cpp -o CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.s

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.requires:

.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.requires

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.provides: jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.requires
	$(MAKE) -f jnilibs/CMakeFiles/ANUDPChat-static.dir/build.make jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.provides.build
.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.provides

jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.provides.build: jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o


# Object files for target ANUDPChat-static
ANUDPChat__static_OBJECTS = \
"CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o"

# External object files for target ANUDPChat-static
ANUDPChat__static_EXTERNAL_OBJECTS =

jnilibs/libANUDPChat.a: jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o
jnilibs/libANUDPChat.a: jnilibs/CMakeFiles/ANUDPChat-static.dir/build.make
jnilibs/libANUDPChat.a: jnilibs/CMakeFiles/ANUDPChat-static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/mahuijie/Desktop/ChatLib/arm64build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libANUDPChat.a"
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && $(CMAKE_COMMAND) -P CMakeFiles/ANUDPChat-static.dir/cmake_clean_target.cmake
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ANUDPChat-static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
jnilibs/CMakeFiles/ANUDPChat-static.dir/build: jnilibs/libANUDPChat.a

.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/build

jnilibs/CMakeFiles/ANUDPChat-static.dir/requires: jnilibs/CMakeFiles/ANUDPChat-static.dir/udpchat.cpp.o.requires

.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/requires

jnilibs/CMakeFiles/ANUDPChat-static.dir/clean:
	cd /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs && $(CMAKE_COMMAND) -P CMakeFiles/ANUDPChat-static.dir/cmake_clean.cmake
.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/clean

jnilibs/CMakeFiles/ANUDPChat-static.dir/depend:
	cd /Users/mahuijie/Desktop/ChatLib/arm64build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/mahuijie/Desktop/ChatLib /Users/mahuijie/Desktop/ChatLib/jnilibs /Users/mahuijie/Desktop/ChatLib/arm64build /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs /Users/mahuijie/Desktop/ChatLib/arm64build/jnilibs/CMakeFiles/ANUDPChat-static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : jnilibs/CMakeFiles/ANUDPChat-static.dir/depend

