# Multiverse

This repository contains general-purpose `c++` utilities.

# Build

This project uses `cmake` for its build and package specifications. It includes a `vcpkg` manifest for `vcpkg` integration. See [`cmake` documentation](https://cmake.org/documentation/) and [`vcpkg` documentation](https://vcpkg.io/en/docs/README.html) for details.

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

## Options

This project has the following configuration options:
| Option                                        | Type    | Notes                                  |
|:----------------------------------------------|:--------|:---------------------------------------|
| `MLTVRS_ENABLE_TESTING`                       | boolean | Explicitly enable project unit tests   |
| `MLTVRS_ENABLE_CONAN_OVERRIDE`                | boolean | Explicitly use Conan integration       |
| `MLTVRS_STDLIB`                               | string  | The Standard Library to use            |
| `MLTVRS_STDLIB_PATH`                          | path    | Explicit path to Standard Library      |
| `MLTVRS_STDLIB_INCLUDE`                       | path    | Explicit Standard Library include path |
| `MLTVRS_ENABLE_STATIC_STDLIB`                 | boolean | Statically link Standard Library       |
| `MLTVRS_ENABLE_WALL`                          | boolean | Enable strict warnings                 |
| `MLTVRS_ENABLE_WERROR`                        | boolean | Treat warnings as errors               |
| `MLTVRS_ENABLE_DETAILED_CONCEPTS_DIAGNOSTICS` | boolean | Longer concepts backtrace              |
| `MLTVRS_DISABLE_EXCEPTIONS`                   | boolean | Disable exceptions                     |
| `MLTVRS_DISABLE_RTTI`                         | boolean | Disable runtime type information       |

Boolean options default to `OFF`, and other options default to undefined. When unspecified, Standard Library options take the toolchain default.

The `MLTVRS_ENABLE_TESTING` option does not enable unit test by itself; this project also respects `cmake`'s `ENABLE_TESTING` option. Both options need to be enabled to build and run unit tests.

## Prerequisites

This project requires the following tools:
- `cmake`
- a `c++` toolchain that supports `c++20`
and optionally:
- `conan`

This project depends on the libraries specified in the [Conan specification](conanfile.py). Conan can automatically manage meeting those dependencies. See [Conan documentation](https://conan.io/) for details.

For a quick start, when configuring this project, use Conan integration as follows:
```
> cmake -S <repo-root> -B <build-artifacts-dir> -DMLTVRS_ENABLE_CONAN_OVERRIDE=ON
```
