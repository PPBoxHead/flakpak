# flakpak

**CLI tool to pack videogame assets into `.flk` files**

---

## Description

`flakpak` is a command-line tool for packing videogame resources into a custom `.flk` archive format. It is primarily written in C++ with some Lua scripting. The tool currently supports packing (write-only) – reading `.flk` files is not implemented; although `flakpak` provides decompression and decryption methods within the project, users must provide their own file reading logic within their project or game engine.

`.flk` file format is designed to be simple and easy to use, a little of the API is designed using [rres](https://github.com/raysan5/rres) as reference. Also the tool is meant to be simple-use.

> **Note:** The project is under active development. Future improvements will focus on making the codebase more C-style and expanding functionality.

---

## Usage

The basic command syntax for `flakpak` is:

```sh
.\flakpak <resource_dir> <output.flk> [options]
```

### Examples

```sh
# This will pack your resources with NO compression and encryption
.\flakpak resources resources.flk
```

```sh
# This will pack your resources with both compression, encryption, compression level and content version
.\flakpak resources resources.flk --compress --encrypt -c 22 --content-version 1
```

**Arguments:**

- `<resource_dir>`: Directory containing the assets you want to pack.
- `<output.flk>`: Output file name for the packed archive.

**Options:**

- `-h`, `-help` : Show command line options.
- `--compress` : Enable compression for packed assets.
- `--encrypt` : Encrypt the output archive.
- `-c <level>` : Compression level (1–22 for Zstd, default: 3).
- `--content-version <version>` : Specify a content version (default: 0).
- `input_dir` : Required. Directory to pack.
- `output` : Required. Output `.flk` file path.

**Packing modes:**
- Uncompressed + Unencrypted
- Compressed + Unencrypted
- Uncompressed + Encrypted
- Compressed + Encrypted

> **Note:** Reading/extracting `.flk` files is not supported yet. You will need to implement your own extraction logic. Also know that the current implementation of the packing modes will be tweaked to use a enum flag-style in the future

---

## Features

- Packs all files from a directory into a single `.flk` archive.
- Supports compression (Zstd, level 1–22).
- Supports encryption (XChaCha20-Poly1305 via libsodium, password protected).
- User-defined content versioning.
- Header and entry metadata for assets.
- (WIP) Optimized for compression-friendly patterns and padding.

---

## File Format and Limits

- Max entries per archive: 256
- Max file path length: 128 bytes
- Max file size: 1 GB
- All entries are packed into a custom header with metadata. See [flak_FLKDefinition.hpp](https://github.com/PPBoxHead/flakpak/blob/main/paker/include/flakpak/flak_FLKDefinition.hpp).

---

## Building with premake5

To generate project files for your platform using premake5:

1. Install [premake5](https://premake.github.io/download.html).
2. Run:

    ```sh
    premake5 vs2022
    ```
    *(Replace `vs2022` with your target platform, e.g., `gmake2` for GNU Make, `xcode4` for Xcode, etc.)*

3. Open the generated project files and build as usual.

- See [premake5.lua](https://github.com/PPBoxHead/flakpak/blob/main/premake5.lua) and [paker_pfile.lua](https://github.com/PPBoxHead/flakpak/blob/main/paker_pfile.lua) for configuration details.

> **Note:** At the moment this tool is only configured to work on Windows platforms and using MSVC, there's not any config for Linux or MacOS users yet
---

## Dependencies

- **C++17** (or newer)
- [CLI11](https://github.com/CLIUtils/CLI11) (CLI argument parsing)
- [zstd](https://github.com/facebook/zstd) (compression)
- [libsodium](https://github.com/jedisct1/libsodium) (encryption/XChaCha20-Poly1305 + Argon2 key derivation)
- [premake5](https://premake.github.io/) (project generation)

---

## Roadmap

- [ ] Add read/extract support for `.flk` files
- [ ] Refactor code for more C-style usage
- [ ] Improve CLI argument parsing
- [ ] Enhance documentation
- [ ] Add better console log support to display both progress and other info.

---

## Contributing

Pull requests at the moment are **not** welcome. But feel free to report issues or suggest improvements and other new features.

---

## References

- [Main entry point](https://github.com/PPBoxHead/flakpak/blob/main/paker/src/main.cpp)
- [Header/format definition](https://github.com/PPBoxHead/flakpak/blob/main/paker/include/flakpak/flak_FLKDefinition.hpp)
- [Packing logic](https://github.com/PPBoxHead/flakpak/blob/main/paker/include/flakpak/flak_FLKPacker.hpp)
- [Compression interface](https://github.com/PPBoxHead/flakpak/blob/main/paker/include/flakpak/zstd_Compressor.hpp)
- [Encryption interface](https://github.com/PPBoxHead/flakpak/blob/main/paker/include/flakpak/xccp20_Encryptor.hpp)

---

> *These instructions may be incomplete. For more details, see [the repo on GitHub](https://github.com/PPBoxHead/flakpak) and [search the codebase for additional information](https://github.com/PPBoxHead/flakpak/search).*