# Legacy Console Asset Tools

A comprehensive suite of C++ utilities for managing asset formats used in Minecraft Legacy Console Edition. This project provides low-level access to proprietary binary containers, string tables, color configurations, and game rules.

## Core Asset Tools

### dlc-reader
Analyzes and displays detailed metadata for Legacy Console asset files. It features advanced format detection for Structured DLC packs and sequential Media Archives.

**Usage:**
```bash
./dlc-reader <input_file>
```

### dlc-extractor
Unpacks the contents of supported containers. It automatically handles Big-Endian data and performs Zlib decompression for compressed streams.

**Usage:**
```bash
./dlc-extractor <input_file> [output_directory]
```

### dlc-creator
Generates production-ready asset files. It supports two modes:
1. **Archive Mode**: Creates sequential .pck/.arc files from a directory.
2. **Manifest Mode**: Creates structured .dlc files using a manifest for precise control over metadata and parameters.

**Usage:**
```bash
./dlc-creator <manifest_or_directory> [output_file] [--compress] [--dlc]
```

## Specialized Format Tools

### loc-tool
Manages 4J Studios StringTable (`.loc`) files.
- `export`: Converts binary `.loc` to an editable text format.
- `import`: Recompiles text back into a binary `.loc`.

### col-tool
Manages 4J Studios ColourTable (`.col`) files.
- `export`: Extracts color definitions (RGBA) to text.
- `import`: Compiles text definitions into a binary `.col`.

### grf-tool
Analyzes and extracts data from 4J Studios GameRules (`.grf`/`.grh`) containers. It decompresses the payload and reports the internal string lookup table.

### xzp-tool
Extracts assets from XUI Package (`.xzp`) files used by the game's UI system.

## Supported Formats

| Extension | Format Type | Description |
|-----------|-------------|-------------|
| `.pck` / `.arc` | Media Archive | Sequential Big-Endian file container. |
| `.dlc` / `.pck` | Structured DLC | Complex container with global/file metadata (v2/v3). |
| `.loc` | StringTable | Localization data with offset-based lookup. |
| `.col` | ColourTable | Versioned RGBA color definition table. |
| `.grf` / `.grh` | GameRules | Compressed XML-style game configuration. |
| `.xzp` | XUIZ Package | XUI asset container. |

## Build Instructions

These tools require a C++17 compliant compiler and CMake 3.10+.

```bash
mkdir build
cd build
cmake ..
make
```

## License

MIT License. Includes Zlib components.
