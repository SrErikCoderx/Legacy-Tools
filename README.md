# Legacy Console Asset Tools

A specialized suite of C++ utilities for managing asset formats used in Minecraft Legacy Console Edition. These tools support the structured DLC pack format and the standard game media archives.

## Tools Overview

### dlc-reader
Analyzes and displays metadata for Legacy Console asset files. It automatically detects and parses both the Structured DLC (.dlc/.pck) format and the Media Archive (.arc/.pck) format.

**Usage:**
```bash
./dlc-reader <input_file>
```

### dlc-extractor
Extracts the contents of a Legacy Console archive or DLC pack. It features automatic format detection, Big-Endian handling, and integrated Zlib decompression for compressed file entries.

**Usage:**
```bash
./dlc-extractor <input_file> [output_directory]
```

### dlc-creator
Generates Legacy Console compatible asset files. The tool selects the appropriate format based on the output file extension or explicit flags.

**Usage:**
```bash
./dlc-creator <output_file> <input_directory> [--compress] [--dlc]
```
- `--compress`: Applies Zlib compression to all file entries.
- `--dlc`: Forces the Structured DLC format regardless of the file extension.

## Supported Formats

- **Media Archive (.arc / .pck)**: A simple Big-Endian archive format used for general media and localization data.
- **Structured DLC Pack (.pck / .dlc)**: A complex container format (Versions 2 and 3) supporting global metadata, file-specific parameters, and UTF-16BE string encoding.

## Technical Specifications

- **Endianness**: Fully Big-Endian aware, compatible with PlayStation 3 and Xbox 360 binary layouts.
- **Strings**: Robust handling of UTF-16BE to UTF-8 conversion.
- **Compression**: Native support for Zlib (DEFLATE) streams within asset containers.

## Build Requirements

- C++17 compliant compiler
- CMake 3.10 or higher
- Zlib (provided in dependencies)

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Repository Structure

- `tools/`: Source code for the asset management tools.
- `DLC/`: Reference implementation of the game's DLC management logic.
- `Ps3 Dependencies/`: Supporting libraries and a comprehensive review of the PlayStation 3 environment.
- `GameRules/`: Documentation and assets related to game rule definitions.

## License

This project is released under the MIT License. It includes components from the Zlib library, which is subject to its own license terms. See the `LICENSE` file for full details.
