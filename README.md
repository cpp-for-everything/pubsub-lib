# 📦 pubsub-lib

> A lightweight, header-only C++20 Publish-Subscribe library with type-safe events, RAII-based unsubscription, and async support.

---

[![CI](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg)](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![Header-Only](https://img.shields.io/badge/library-header--only-green)
![License](https://img.shields.io/github/license/cpp-for-everything/pubsub-lib)

---

## ✨ Features

- ✅ Type-safe event publishing
- ✅ Support for multiple publishers and event types
- ✅ RAII-based unsubscription via `SubscriptionToken`
- ✅ Subscriber lifetime management
- ✅ Async dispatching with `emit_async`
- ✅ Header-only, C++20

---

## ✅ Compatibility

| OS           | Compiler | Generator             | Status |
|--------------|----------|------------------------|--------|
| macOS 13     | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-13-clang-ninja) |
| macOS 13     | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-13-clang-make) |
| macOS 13     | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-13-gcc-ninja) |
| macOS 13     | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-13-gcc-make) |
| macOS 14     | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-14-clang-ninja) |
| macOS 14     | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-14-clang-make) |
| macOS 14     | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-14-gcc-ninja) |
| macOS 14     | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-14-gcc-make) |
| macOS 15     | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-15-clang-ninja) |
| macOS 15     | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-15-clang-make) |
| macOS 15     | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-15-gcc-ninja) |
| macOS 15     | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=macos-15-gcc-make) |
| Ubuntu 22.04 | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-22.04-clang-ninja) |
| Ubuntu 22.04 | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-22.04-clang-make) |
| Ubuntu 22.04 | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-22.04-gcc-ninja) |
| Ubuntu 22.04 | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-22.04-gcc-make) |
| Ubuntu 24.04 | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-24.04-clang-ninja) |
| Ubuntu 24.04 | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-24.04-clang-make) |
| Ubuntu 24.04 | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-24.04-gcc-ninja) |
| Ubuntu 24.04 | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=ubuntu-24.04-gcc-make) |
| Windows 2019 | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2019-clang-ninja) |
| Windows 2019 | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2019-clang-make) |
| Windows 2019 | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2019-gcc-ninja) |
| Windows 2019 | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2019-gcc-make) |
| Windows 2019 | MSVC     | Visual Studio 17 2022  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2019-msvc-vs2022) |
| Windows 2022 | Clang    | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2022-clang-ninja) |
| Windows 2022 | Clang    | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2022-clang-make) |
| Windows 2022 | GCC      | Ninja                  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2022-gcc-ninja) |
| Windows 2022 | GCC      | Unix Makefiles         | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2022-gcc-make) |
| Windows 2022 | MSVC     | Visual Studio 17 2022  | ![Build](https://github.com/cpp-for-everything/pubsub-lib/actions/workflows/ci.yaml/badge.svg?branch=main&label=windows-2022-msvc-vs2022) |

| C++ Standard |
|---------------|
| ✅ C++20 |

(✔ Verified in CI using GitHub Actions)

---

## 🚀 Getting Started

### CMake Project Integration

**1. Clone or Fetch:**
```bash
git clone https://github.com/cpp-for-everything/pubsub-lib.git
```

**2. Add to your `CMakeLists.txt`:**
```cmake
add_subdirectory(pubsub-lib)

target_link_libraries(my_app PRIVATE pubsub::pubsub)
```

### Or Install System-Wide
```bash
cmake -B build -S pubsub-lib -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build build --target install
```

Then:
```cmake
find_package(pubsub REQUIRED)
target_link_libraries(my_app PRIVATE pubsub::pubsub)
```

---

## 🧪 Testing

Build and run the test suite:
```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build
```

---

## 📖 Citation

If you use `pubsub-lib` in your research or scientific work, please cite it as:

> Alex Tsvetanov. *pubsub-lib: A Type-Safe C++ Publish-Subscribe Framework*, 2024. https://github.com/cpp-for-everything/pubsub-lib

You can also use the provided `CITATION.cff` file for reference managers.

---

## 📄 License

This project is licensed under the **Apache License 2.0**.
See [LICENSE](./LICENSE) for full terms.

Each source file includes an SPDX identifier for traceability:
```cpp
// SPDX-License-Identifier: Apache-2.0
```

---

## 🤝 Contributing

Pull requests welcome! For major changes, open an issue first to discuss what you’d like to change.

---

## 📬 Contact

Open an issue or reach out via [GitHub Discussions](https://github.com/cpp-for-everything/pubsub-lib/discussions) if you need support or want to collaborate.