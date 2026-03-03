# Legacy-Tools

A suite of high-performance C++ tools designed for managing Minecraft Legacy Console Edition DLC and related assets.

## Tools Included

### dlc-creator
Pack files and metadata into the proprietary DLC binary format (Version 3) used by Legacy Console editions. Supports optional zlib compression.

**Usage:**
```bash
./dlc-creator <output_file.dlc> <input_directory> [--compress]
```

### dlc-reader
Analyze and display the internal structure, metadata, and parameters of existing DLC packs. Detects zlib-compressed streams.

**Usage:**
```bash
./dlc-reader <input_file.dlc>
```

### dlc-extractor
Unpack all files from a DLC pack into a specified directory, preserving the original file structure. Automatically handles zlib decompression.

**Usage:**
```bash
./dlc-extractor <input_file.dlc> [output_directory]
```

## Build Instructions

These tools require a C++17 compliant compiler and CMake 3.10 or higher.

```bash
mkdir build
cd build
cmake ..
make
```

## Repository Structure

- `tools/`: Source code for the DLC management tools.
- `DLC/`: Original game logic for DLC handling (reference).
- `Ps3 Dependencies/`: Supporting libraries and assets for the PlayStation 3 environment.
  - See `Ps3 Dependencies/REVIEW.md` for a detailed analysis of these components.
- `GameRules/`: Documentation and tools for game rule management.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
Includes components from `zlib` (see LICENSE for the zlib license).
