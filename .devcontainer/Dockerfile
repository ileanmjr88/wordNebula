# ============================================================================
# wordNebula Development Container
# Multi-stage Dockerfile for optimal caching and size
# ============================================================================

FROM debian:bookworm-slim AS base

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Set locale
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# ============================================================================
# Stage 1: Development Tools Layer
# ============================================================================
FROM base AS dev-tools

RUN apt-get update && apt-get install -y --no-install-recommends \
    # Build essentials
    build-essential \
    cmake \
    ninja-build \
    ccache \
    pkg-config \
    ca-certificates \
    # Compilers and toolchains
    gcc-12 \
    g++-12 \
    clang-16 \
    clang-format-16 \
    clang-tidy-16 \
    llvm-16 \
    lld-16 \
    # Debugging and analysis tools
    gdb \
    lldb-16 \
    valgrind \
    cppcheck \
    iwyu \
    # Version control
    git \
    git-lfs \
    # Utilities
    curl \
    wget \
    unzip \
    zip \
    tar \
    gnupg2 \
    software-properties-common \
    locales \
    # Shell
    zsh \
    && rm -rf /var/lib/apt/lists/*

# Set up alternatives for clang tools
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-16 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-16 100 && \
    update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-16 100 && \
    update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-16 100 && \
    update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-16 100

# Configure locale
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen

# ============================================================================
# Stage 2: C++ Libraries and Dependencies
# ============================================================================
FROM dev-tools AS cpp-libs

RUN apt-get update && apt-get install -y --no-install-recommends \
    # Project dependencies
    libncurses5-dev \
    libncursesw5-dev \
    libspdlog-dev \
    # Testing frameworks
    libgtest-dev \
    libgmock-dev \
    google-mock \
    # Documentation tools
    doxygen \
    graphviz \
    # Code coverage
    lcov \
    gcovr \
    # Font utilities (needed for fc-cache)
    fontconfig \
    # Python for scripting and pre-commit
    python3 \
    python3-pip \
    python3-dev \
    python3-lldb-16 \
    && rm -rf /var/lib/apt/lists/*

# Create symbolic link for lldb Python bindings
RUN ln -sf /usr/lib/llvm-16/lib/python3.11/dist-packages/lldb /usr/lib/python3/dist-packages/lldb || true

# ============================================================================
# Stage 3: Development Tools and Utilities
# ============================================================================
FROM cpp-libs AS dev-utils

# Install GitHub CLI
RUN curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | \
    dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg && \
    chmod go+r /usr/share/keyrings/githubcli-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | \
    tee /etc/apt/sources.list.d/github-cli.list > /dev/null && \
    apt-get update && \
    apt-get install -y gh && \
    rm -rf /var/lib/apt/lists/*

# Install pipx for Python CLI tools (respects PEP 668)
RUN apt-get update && apt-get install -y --no-install-recommends pipx && \
    rm -rf /var/lib/apt/lists/*

# Install vcpkg
ENV VCPKG_ROOT=/opt/vcpkg
RUN git clone --depth 1 https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT} && \
    ${VCPKG_ROOT}/bootstrap-vcpkg.sh && \
    ln -s ${VCPKG_ROOT}/vcpkg /usr/local/bin/vcpkg && \
    # Set vcpkg to use system packages when available
    echo 'set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_LIST_DIR}/triplets")' >> ${VCPKG_ROOT}/triplets/community/x64-linux.cmake

# Configure ccache
ENV CCACHE_DIR=/workspace/.ccache
ENV CCACHE_MAXSIZE=5G
RUN ccache --set-config=compression=true && \
    ccache --set-config=max_size=5G

# ============================================================================
# Stage 4: User Environment Setup
# ============================================================================
FROM dev-utils AS final

# Create non-root user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/zsh && \
    # Add sudo support
    apt-get update && \
    apt-get install -y sudo && \
    echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME && \
    rm -rf /var/lib/apt/lists/*

# Install Starship prompt
USER root
RUN curl -sS https://starship.rs/install.sh | sh -s -- --yes

# Install Nerd Fonts for icon support in terminal
# Pinned to v3.1.1 for reproducible builds
ARG NERD_FONTS_VERSION=v3.1.1
RUN mkdir -p /usr/share/fonts/truetype/nerd-fonts && \
    cd /tmp && \
    curl -fLo "FiraCode.zip" "https://github.com/ryanoasis/nerd-fonts/releases/download/${NERD_FONTS_VERSION}/FiraCode.zip" && \
    unzip -qo FiraCode.zip -d /usr/share/fonts/truetype/nerd-fonts/ && \
    curl -fLo "Hack.zip" "https://github.com/ryanoasis/nerd-fonts/releases/download/${NERD_FONTS_VERSION}/Hack.zip" && \
    unzip -qo Hack.zip -d /usr/share/fonts/truetype/nerd-fonts/ && \
    rm -f FiraCode.zip Hack.zip && \
    fc-cache -fv

# Install oh-my-zsh for better terminal experience
USER $USERNAME
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended

# Install Python CLI tools via pipx for vscode user
RUN pipx install pre-commit && \
    pipx install cmakelang && \
    pipx install gcovr

# Configure zsh with Starship
RUN echo 'export VCPKG_ROOT=/opt/vcpkg' >> ~/.zshrc && \
    echo 'export PATH=$VCPKG_ROOT:$PATH' >> ~/.zshrc && \
    echo 'export PATH=$HOME/.local/bin:$PATH' >> ~/.zshrc && \
    echo 'export CCACHE_DIR=/workspace/.ccache' >> ~/.zshrc && \
    echo 'alias cmake="cmake -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"' >> ~/.zshrc && \
    echo 'eval "$(starship init zsh)"' >> ~/.zshrc

# Create Starship config
RUN mkdir -p ~/.config && \
    cat > ~/.config/starship.toml <<'STARSHIP_EOF'
# Starship configuration for wordNebula development
format = """
[┌───────────────────────────────────────────>](bold green)
[│](bold green)$directory$git_branch$git_status$git_state$cmake
[└─>](bold green) """

[directory]
style = "blue bold"
truncation_length = 3
truncate_to_repo = true

[git_branch]
format = "[$symbol$branch]($style) "
symbol = " "
style = "bright-black"

[git_status]
format = '([\[$all_status$ahead_behind\]]($style) )'
style = "cyan"

[git_state]
format = '\([$state( $progress_current/$progress_total)]($style)\) '
style = "bright-black"

[cmake]
format = "via [$symbol($version )]($style)"
symbol = "△ "
style = "bold blue"

[cmd_duration]
format = "took [$duration]($style) "
style = "yellow bold"

[character]
success_symbol = "[❯](purple)"
error_symbol = "[❯](red)"
STARSHIP_EOF

USER root

# Set up workspace directory
RUN mkdir -p /workspace && \
    chown -R $USERNAME:$USERNAME /workspace

# Environment variables - Use Clang as default compiler
ENV CC=clang
ENV CXX=clang++

# Reset to non-interactive
ENV DEBIAN_FRONTEND=

# Default to non-root user
USER $USERNAME
WORKDIR /workspace

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD which cmake && which ninja && which gdb || exit 1

# Metadata
LABEL org.opencontainers.image.title="wordNebula Development Environment"
LABEL org.opencontainers.image.description="Complete C++20 development environment for wordNebula"
LABEL org.opencontainers.image.version="1.0.0"
LABEL org.opencontainers.image.authors="wordNebula Team"

CMD ["/bin/zsh"]
