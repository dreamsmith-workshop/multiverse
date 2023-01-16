# Multiverse

This repository contains general-purpose `c++` utilities.

# Build

This project uses `cmake` for its build and package specifications. It includes a `vcpkg` manifest for `vcpkg` integration.

To run a basic build and install, ensure prerequisites are met, and run the following:
```
> cmake -S <repo-root> -B <build-artifacts-dir>
> cmake --build <build-artifacts-dir>
> cmake --build <build-artifacts-dir> --target install
```
where:
    - `<build-artifacts-dir>` is a directory outside of `<repo-root>`
    - building the `install` target requires privilege elevation in most cases
        - when elevation isn't required to `install`, it may be done without separately building everything else

## Prerequisites

This project requires the following tools:
- `cmake`
- a `c++` toolchain that supports `c++20`
and optionally:
- `git`
- `vcpkg`

This project depends on the libraries specified in the [package manifest](vcpkg.json). `vcpkg` can automatically manage meeting those dependencies. See [Microsoft documentation](https://vcpkg.io/en/getting-started.html) for details.

### Environment Setup

For a quick start, set up `vcpkg` as follows:
```
> git clone https://github.com/microsoft/vcpkg
> cd vckpkg
> git checkout tags/<latest-release-tag>
> ./bootstrap-vcpkg.<sh|bat>
> ./vcpkg integrate install
```

Then, when configuring this project, ensure the `vcpkg` toolchain file is used as follows:
```
> cmake -S <repo-root> -B <build-artifacts-dir> -DCMAKE_TOOLCHAIN_FILE=<vcpkg-repo-root>/scripts/buildsystems/vcpkg.cmake
```

#### VSCode

It's possible to direct `VSCode` to the `vcpkg` toolchain file via settings, or to create specific build kits that use `vcpkg` for a particular workspace.

##### Build Kit

This method requires the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension from Microsoft.

To create specific `vcpkg` build kits on a per-workspace basis, add a build kit entry to `.vscode/cmake-kits.json` in that workspace as follows:
```json
{
    "name": "a-fun-name",
    "toolchainFile": "<vcpkg-repo-root>/scripts/buildsystems/vcpkg.cmake"
}
```

##### Settings Redirect

To direct `VSCode` to `vcpkg`, in `VSCode` settings, under `cmake.configureSettings`, add the following entry:
```json
"CMAKE_TOOLCHAIN_FILE": "<vcpkg-repo-root>/scripts/buildsystems/vcpkg.cmake"
```
