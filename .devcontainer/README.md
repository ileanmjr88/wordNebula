# WordNebula Development Container

This directory contains the configuration for a professional-grade development
container optimized for C++20 development.

## Architecture

### Custom Docker Image

We use a **custom multi-stage Dockerfile** instead of a base image + install
script approach for several key benefits:

#### Advantages:

- **Fast Startup**: ~30 seconds vs 5-10 minutes with install scripts
- **Reproducible**: Locked package versions, identical across all machines
- **Offline Development**: No network dependency after initial build
- **Professional**: Production-grade approach for portfolio showcase
- **Optimized**: Multi-stage build with layer caching

#### Multi-Stage Build:

1. **base**: Debian Bookworm slim with locale configuration
1. **dev-tools**: Build essentials, compilers, debugging tools
1. **cpp-libs**: C++ libraries and testing frameworks
1. **dev-utils**: GitHub CLI, Python tools, vcpkg
1. **final**: User environment with oh-my-zsh

## Included Tools

### Compilers & Build Tools

- GCC 12 (default)
- Clang 16 (with LLVM toolchain)
- CMake 3.25+
- Ninja build system
- ccache (build acceleration)

### C++ Libraries

- ncurses (terminal UI)
- spdlog (logging)
- Google Test & Google Mock (testing)

### Code Quality

- clang-format-16 (code formatting)
- clang-tidy-16 (static analysis)
- cppcheck (additional static analysis)
- include-what-you-use (header optimization)

### Debugging & Analysis

- GDB (GNU debugger)
- LLDB 16 (LLVM debugger)
- Valgrind (memory analysis)
- AddressSanitizer (built into compilers)
- UndefinedBehaviorSanitizer (built into compilers)

### Documentation

- Doxygen (API documentation)
- GraphViz (visualization)

### Code Coverage

- lcov (line coverage)
- gcovr (coverage reports)

### Package Management

- vcpkg (C++ packages)
- pip3 (Python packages)

### Version Control

- Git
- Git LFS
- GitHub CLI (gh)

### Python Tools

- pre-commit framework
- cmake-format
- gcovr

## Performance Optimizations

### Persistent Volume Mounts

```json
"mounts": [
    "source=wordnebula-ccache,target=/workspace/.ccache,type=volume",
    "source=wordnebula-vcpkg-cache,target=/opt/vcpkg/downloads,type=volume"
]
```

These volumes persist between container rebuilds:

- **ccache**: Compilation cache (speeds up rebuilds by 5-10x)
- **vcpkg cache**: Package downloads (avoid re-downloading)

### ccache Configuration

- Max size: 5GB
- Compression: enabled
- Automatically used via CMake alias in .zshrc

## First-Time Setup

### 1. Rebuild Container

```bash
# In VS Code Command Palette (Cmd/Ctrl+Shift+P):
Dev Containers: Rebuild Container
```

**Note**: First build takes 5-10 minutes as it downloads and compiles
everything. Subsequent rebuilds use Docker layer cache and are much faster.

### 2. Verify Installation

```bash
# Check installed tools
cmake --version        # Should be 3.25+
ninja --version        # Should be 1.11+
g++ --version          # Should be 12.x
clang++ --version      # Should be 16.x
gdb --version
valgrind --version

# Check libraries
pkg-config --modversion ncurses
pkg-config --modversion spdlog

# Check Python tools
pre-commit --version
```

### 3. Install Git Hooks

```bash
pre-commit install --install-hooks
```

## Environment Variables

Set automatically in the container:

```bash
VCPKG_ROOT=/opt/vcpkg           # vcpkg installation
CCACHE_DIR=/workspace/.ccache   # ccache directory
CC=gcc-12                       # Default C compiler
CXX=g++-12                      # Default C++ compiler
```

## Shell Configuration (zsh)

The container uses **zsh with oh-my-zsh** for an enhanced terminal experience.

Custom aliases in `~/.zshrc`:

```bash
# CMake with ccache by default
alias cmake="cmake -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
```

## Customization

### Adding Dependencies

**Option 1: Modify Dockerfile**

```dockerfile
# In the cpp-libs stage, add to apt-get install:
RUN apt-get update && apt-get install -y --no-install-recommends \
    # ... existing packages ...
    libyour-new-lib-dev \
    && rm -rf /var/lib/apt/lists/*
```

**Option 2: Use vcpkg**

```bash
# Add to vcpkg.json
{
  "dependencies": [
    "your-package-name"
  ]
}
```

### Changing Compiler

To use Clang instead of GCC:

```bash
# In devcontainer.json, change:
"containerEnv": {
    "CC": "clang-16",
    "CXX": "clang++-16"
}
```

## Troubleshooting

### Container Build Fails

```bash
# Clean Docker cache and rebuild
docker system prune -a
# Then rebuild container in VS Code
```

### ccache Not Working

```bash
# Check ccache status
ccache -s

# Clear cache if needed
ccache -C

# Verify CMake is using ccache
cmake -B build -G Ninja -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Permission Issues

The container automatically syncs UID/GID with your host user via:

```json
"updateRemoteUserUID": true
```

If you still have issues:

```bash
# Inside container
sudo chown -R vscode:vscode /workspace
```

## Best Practices

### 1. Use Volume Mounts

Never delete the Docker volumes - they contain your build cache:

```bash
# View volumes
docker volume ls | grep wordnebula

# If accidentally deleted, ccache will rebuild from scratch
```

### 2. Regular Updates

Update the base image periodically:

```dockerfile
# In Dockerfile, update:
FROM debian:bookworm-slim AS base
# Check for newer Debian releases
```

### 3. Layer Optimization

When modifying Dockerfile:

- Put frequently changing commands later
- Combine related RUN commands
- Clean apt cache in same layer: `&& rm -rf /var/lib/apt/lists/*`

### 4. Build Image Locally

For team sharing, build and push to registry:

```bash
# Build image
docker build -t wordnebula-dev:latest .devcontainer/

# Tag for registry
docker tag wordnebula-dev:latest ghcr.io/ileanmjr88/wordnebula-dev:latest

# Push to GitHub Container Registry
docker push ghcr.io/ileanmjr88/wordnebula-dev:latest
```

Then update devcontainer.json:

```json
{
    "image": "ghcr.io/ileanmjr88/wordnebula-dev:latest"
}
```

## Comparison: Custom Image vs Base + Script

| Aspect              | Custom Dockerfile   | Base Image + Script |
| ------------------- | ------------------- | ------------------- |
| **First startup**   | 30 seconds          | 5-10 minutes        |
| **Rebuild time**    | 1-2 minutes         | 5-10 minutes        |
| **Reproducibility** | Perfect             | Version drift risk  |
| **Offline work**    | Yes                 | No                  |
| **Disk space**      | ~2GB                | ~1.5GB              |
| **Maintenance**     | Dockerfile updates  | Script updates      |
| **Professional**    | ✅ Portfolio-ready  | ❌ Development only |
| **Team sharing**    | ✅ Push to registry | ❌ Everyone waits   |

## References

- [Docker Multi-Stage Builds](https://docs.docker.com/build/building/multi-stage/)
- [VS Code Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers)
- [vcpkg Documentation](https://vcpkg.io/)
- [ccache Manual](https://ccache.dev/manual/latest.html)
