# PS3 Dependencies Review

This document provides a summary of the libraries and assets found in the `Ps3 Dependencies` directory.

## 4JLibs
- **Purpose**: Core engine libraries developed by 4J Studios.
- **Contents**:
  - `inc/`: Header files for Input, Profile, Render, and Storage management.
  - `STO_TitleSmallStorage.cpp/h`: Implementation of small storage handling for the PS3.
  - `Media/`: Localization strings for the engine.

## Audio
- **Purpose**: PlayStation 3 specific sound engine implementation.
- **Contents**: `PS3_SoundEngine.cpp`.

## Colours
- **Purpose**: Management of colour tables, likely for UI or game elements.
- **Contents**: `ColourTable.cpp/h`.

## Iggy
- **Purpose**: UI Rendering middleware (likely Autodesk Scaleform/Iggy).
- **Contents**:
  - `gdraw/`: PS3 GCM (Graphics Control Management) implementation for Iggy.
  - `include/`: Header files for the Iggy UI system.

## Leaderboards
- **Purpose**: Management of in-game leaderboards and statistics.
- **Contents**: `LeaderboardManager.cpp/h`.

## Miles
- **Purpose**: Miles Sound System middleware.
- **Contents**:
  - `include/`: MSS header files.
  - `lib/spu/`: SPU (Synergistic Processing Unit) build scripts for sound processing on the Cell broadband engine.

## Network
- **Purpose**: Comprehensive network stack including Sony NP (Network Platform) integration.
- **Contents**:
  - `Sony/`: Low-level Sony network and commerce (DLC store) wrappers.
  - `SonyRemoteStorage_PS3.cpp/h`: PS3-specific remote storage implementation.

## Sentient
- **Purpose**: Telemetry and statistics tracking system.
- **Contents**: Headers for telemetry, stats, and configurations.

## XUI
- **Purpose**: XML-based UI system (XUI) implementation for Minecraft.
- **Contents**: Extensive collection of `.cpp` and `.h` files for various UI scenes (Inventory, Main Menu, Crafting, etc.) and custom controls.

## zlib
- **Purpose**: Standard compression library (version 1.2.8).
- **Contents**: Full source code for zlib, used for data compression/decompression in DLC and game saves.
