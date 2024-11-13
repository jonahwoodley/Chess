<a name="readme-top"></a>

<div align="center">
  <a href="https://github.com/jonahwoodley/Chess">
    <img src="https://github.com/user-attachments/assets/32d7ca7b-5078-4281-86d5-a42365f49d56" alt="Logo" width="140" height="140">
  </a>
  <h3 align="center">Chess</h3>

  <p align="center">
    A simple chess game built in C++ using SDL2.
  </p>
</div>

<div align="center">
  <img src="https://github.com/user-attachments/assets/2d9c0796-a6f6-41e9-a758-af0e10cd7ac0" alt="Project Banner">
</div>

## Table of Contents

- [Features](#features)
- [TODO](#todo)
- [Build Instructions](#build-instructions)
  - [Prerequisites](#prerequisites)
  - [Cloning the Repository](#cloning-the-repository)
  - [Building with vcpkg](#building-with-vcpkg)
  - [Building without vcpkg](#building-without-vcpkg)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- Complete move logic for all pieces (except castling, will be added soon)

- Captured pieces display to track game progression
 
- Current turn indicator for clear gameplay flow

- Piece promotion

- Highlighted possible moves for selected pieces (will eventually become a option in the UI)

- Board setup with proper light and dark colour scheme

- Very low CPU and memory usage

<br />

## TODO

- Implement stalemate detection

- Add castling move logic for both kingside and queenside
 
- Fix audio bugs causing sound failures on certain Windows versions

- Highlight the currently selected piece

- Integrate Stockfish for AI opponent with adjustable difficulty levels

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<br />

## Build Instructions

Tested on Linux and Windows (not yet tested on macOS).

### Prerequisites
- [CMake](https://cmake.org/download/) (version 3.20 or higher)
- [vcpkg](https://github.com/microsoft/vcpkg) for dependency management (recommended)

<br />

### Cloning the Repository
Clone the repository and navigate to the project folder:
```bash
git clone https://github.com/jonahwoodley/Chess
cd Chess
```

<br />

### Building with vcpkg

1. Install required packages with vcpkg:
```
vcpkg install sdl2 sdl2-mixer sdl2-image
```

2. Create a `build` directory and navigate to it
```bash
mkdir build
cd build
```

3. Run CMake, specifying the path to the `vcpkg` toolchain file:
```
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

4. Build the project
```
cmake --build .
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Building without vcpkg
If you are not using vcpkg, make sure that all required libraries (such as `SDL2`, `SDL2_mixer`, and `SDL2_image`) are correctly installed. On Windows, make sure to copy the library files into the executable directory once built.

1. Create a `build` directory and navigate to it
```bash
mkdir build
cd build
```

2. Run CMake without specifying the vcpkg toolchain file:
```
cmake ..
```

3. Build the project:
```
cmake --build .
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Acknowledgments
- Sounds used in this project are courtesy of [chess.com](https://www.chess.com).

<p align="right">(<a href="#readme-top">back to top</a>)</p>





