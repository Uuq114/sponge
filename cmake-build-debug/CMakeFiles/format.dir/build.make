# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.24

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\clion\CLion 2022.3.2\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\clion\CLion 2022.3.2\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge\cmake-build-debug

# Utility rule file for format.

# Include any custom commands dependencies for this target.
include CMakeFiles/format.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/format.dir/progress.make

CMakeFiles/format:
	echo "Could not find clang-format. Please install and re-run cmake"

format: CMakeFiles/format
format: CMakeFiles/format.dir/build.make
.PHONY : format

# Rule to build all files generated by this target.
CMakeFiles/format.dir/build: format
.PHONY : CMakeFiles/format.dir/build

CMakeFiles/format.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\format.dir\cmake_clean.cmake
.PHONY : CMakeFiles/format.dir/clean

CMakeFiles/format.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge\cmake-build-debug \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge\cmake-build-debug \\wsl$$\Ubuntu-18.04\home\uuq114\cs144\sponge\cmake-build-debug\CMakeFiles\format.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/format.dir/depend

