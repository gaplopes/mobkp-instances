# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build

# Include any dependencies generated for this target.
include CMakeFiles/mobkp-instances.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mobkp-instances.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mobkp-instances.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mobkp-instances.dir/flags.make

CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o: CMakeFiles/mobkp-instances.dir/flags.make
CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o: ../apps/instances.cpp
CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o: CMakeFiles/mobkp-instances.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o -MF CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o.d -o CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o -c /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/apps/instances.cpp

CMakeFiles/mobkp-instances.dir/apps/instances.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mobkp-instances.dir/apps/instances.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/apps/instances.cpp > CMakeFiles/mobkp-instances.dir/apps/instances.cpp.i

CMakeFiles/mobkp-instances.dir/apps/instances.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mobkp-instances.dir/apps/instances.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/apps/instances.cpp -o CMakeFiles/mobkp-instances.dir/apps/instances.cpp.s

# Object files for target mobkp-instances
mobkp__instances_OBJECTS = \
"CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o"

# External object files for target mobkp-instances
mobkp__instances_EXTERNAL_OBJECTS =

mobkp-instances: CMakeFiles/mobkp-instances.dir/apps/instances.cpp.o
mobkp-instances: CMakeFiles/mobkp-instances.dir/build.make
mobkp-instances: /usr/lib/x86_64-linux-gnu/libfmt.so.8.1.1
mobkp-instances: CMakeFiles/mobkp-instances.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable mobkp-instances"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mobkp-instances.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mobkp-instances.dir/build: mobkp-instances
.PHONY : CMakeFiles/mobkp-instances.dir/build

CMakeFiles/mobkp-instances.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mobkp-instances.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mobkp-instances.dir/clean

CMakeFiles/mobkp-instances.dir/depend:
	cd /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build /home/gaplopes/Documents/gaplopes/Doutoramento/GITHUB/mobkp-instances/build/CMakeFiles/mobkp-instances.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mobkp-instances.dir/depend
