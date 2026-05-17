#!/usr/bin/env bash
# init-env.sh — source this before launching VS Code so that the env-var
# references in .vscode/c_cpp_properties.json resolve to real paths.
#
#   source scripts/init-env.sh && code .
#
# What it does:
#   1. Verifies `compendium` is installed.
#   2. Activates the compendium environment for this project, which exports
#      CC, CXX, COMPENDIUM_CC_CXX_DIR, VCPKG_INSTALLED_DIR, VCPKG_ROOT, etc.
#      (run `compendium env` to see the full list).
#   3. Derives VCPKG_TARGET_TRIPLET from the vcpkg-installed layout so that
#      paths like ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
#      work cross-platform (arm64-osx, x64-osx, x64-linux, arm64-linux).

(return 0 2>/dev/null) || {
    echo "init-env.sh must be SOURCED, not executed — exports would vanish with the subshell:" >&2
    echo "  source scripts/init-env.sh" >&2
    exit 1
}

if ! command -v compendium >/dev/null 2>&1; then
    echo "init-env.sh: 'compendium' not found on PATH. Install it from" >&2
    echo "  https://compendium.ilean.me  then re-source this script." >&2
    return 1
fi

# shellcheck disable=SC1090
source <(compendium activate)

if [[ -d "${VCPKG_INSTALLED_DIR:-}" ]]; then
    triplet=$(find "${VCPKG_INSTALLED_DIR}" -mindepth 1 -maxdepth 1 -type d \
        ! -name 'vcpkg' -exec basename {} \; 2>/dev/null | head -n 1)
fi

if [[ -z "${triplet:-}" ]]; then
    case "$(uname -s)-$(uname -m)" in
        Linux-x86_64)   triplet="x64-linux"   ;;
        Linux-aarch64)  triplet="arm64-linux" ;;
        Darwin-arm64)   triplet="arm64-osx"   ;;
        Darwin-x86_64)  triplet="x64-osx"     ;;
        *)
            echo "init-env.sh: could not detect vcpkg triplet for $(uname -s)-$(uname -m)" >&2
            ;;
    esac
fi

if [[ -n "${triplet:-}" ]]; then
    export VCPKG_TARGET_TRIPLET="${triplet}"
fi

unset triplet
