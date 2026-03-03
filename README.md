# Legacy-Tools

A suite of high-performance C++ tools designed for managing Minecraft Legacy Console Edition assets, including DLC packs and general archives.

## Tools Included

### dlc-creator
Pack files and metadata into Legacy Console formats. It automatically chooses between the Archive format (.pck/.arc) and the structured DLC format (.dlc) based on the output file extension.

**Usage:**
```bash
./dlc-creator <output_file> <input_directory> [--compress] [--dlc]
```
- `--compress`: Enable zlib compression for all files.
- `--dlc`: Force structured DLC format regardless of extension.

### dlc-reader
Analyze and display the internal structure, metadata, and parameters of Legacy Console files. It supports auto-detection of both .pck/.arc and .dlc formats.

**Usage:**
```bash
./dlc-reader <input_file>
```

### dlc-extractor
Unpack all files from a Legacy Console archive or DLC pack into a specified directory. It handles Big-Endian data and automatically performs zlib decompression for compressed streams.

**Usage:**
```bash
./dlc-extractor <input_file> [output_directory]
```

## Supported Formats

- **Legacy Console Archive (.pck / .arc)**: High-level archive format used for game media and tutorials. Uses Big-Endian headers.
- **Structured DLC Pack (.dlc)**: Complex binary format for downloadable content, supporting global and file-specific parameters (Version 2 & 3).

## Build Instructions

These tools require a C++17 compliant compiler and CMake 3.10 or higher.

```bash
mkdir build
cd build
cmake ..
make
```

## Repository Structure

- `tools/`: Source code for the asset management tools.
- `DLC/`: Original game logic for DLC handling (reference).
- `Ps3 Dependencies/`: Supporting libraries and assets for the PlayStation 3 environment.
  - See `Ps3 Dependencies/REVIEW.md` for a detailed analysis.
- `GameRules/`: Documentation and tools for game rule management.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
Includes components from `zlib`.
