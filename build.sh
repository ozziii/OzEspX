#!/bin/bash
set -e

# Utility
is_git() {
    command -v git >/dev/null 2>&1 || return 1
    command git rev-parse >/dev/null 2>&1 || return 1

    return 0
}

# Script settings
version=$(grep APP_VERSION espurna/config/version.h | awk '{print $3}' | sed 's/"//g')

if is_git; then
    git_revision=$(git rev-parse --short HEAD)
    git_version=${version}-${git_revision}
else
    git_revision=
    git_version=$version
fi

par_build=false
par_thread=${BUILDER_THREAD:-0}
par_total_threads=${BUILDER_TOTAL_THREADS:-4}
if [ ${par_thread} -ne ${par_thread} -o \
    ${par_total_threads} -ne ${par_total_threads} ]; then
    echo "Parallel threads should be a number."
    exit
fi
if [ ${par_thread} -ge ${par_total_threads} ]; then
    echo "Current thread is greater than total threads. Doesn't make sense"
    exit
fi

# Available environments
list_envs() {
    grep env: platformio.ini | sed 's/\[env:\(.*\)\]/\1/g'
}

travis=$(list_envs | grep travis | sort)
available=$(list_envs | grep -Ev -- '-ota$|-ssl$|^travis' | sort)

# Build tools settings
export PLATFORMIO_BUILD_FLAGS="${PLATFORMIO_BUILD_FLAGS} -DAPP_REVISION='\"$git_revision\"'"

# Functions
print_available() {
    echo "--------------------------------------------------------------"
    echo "Available environments:"
    for environment in $available; do
        echo "* $environment"
    done
}

print_environments() {
    echo "--------------------------------------------------------------"
    echo "Current environments:"
    for environment in $environments; do
        echo "* $environment"
    done
}

set_default_environments() {
    # Hook to build in parallel when using travis
    if [[ "${TRAVIS_BUILD_STAGE_NAME}" = "Release" ]] && ${par_build}; then
        environments=$(echo ${available} | \
            awk -v par_thread=${par_thread} -v par_total_threads=${par_total_threads} \
            '{ for (i = 1; i <= NF; i++) if (++j % par_total_threads == par_thread ) print $i; }')
        return
    fi

    # Only build travisN
    if [[ "${TRAVIS_BUILD_STAGE_NAME}" = "Test" ]]; then
        environments=$travis
        return
    fi

    # Fallback to all available environments
    environments=$available
}

build_webui() {
    # Build system uses gulpscript.js to build web interface
    if [ ! -e node_modules/gulp/bin/gulp.js ]; then
        echo "--------------------------------------------------------------"
        echo "Installing dependencies..."
        npm install --only=dev
    fi

    # Recreate web interface (espurna/data/index.html.*.gz.h)
    echo "--------------------------------------------------------------"
    echo "Building web interface..."
    node node_modules/gulp/bin/gulp.js || exit
}

build_environments() {
    echo "--------------------------------------------------------------"
    echo "Building firmware images..."
    mkdir -p ../firmware/espurna-$version

    for environment in $environments; do
        echo -n "* espurna-$version-$environment.bin --- "
        platformio run --silent --environment $environment || exit 1
        stat -c %s .pioenvs/$environment/firmware.bin
        [[ "${TRAVIS_BUILD_STAGE_NAME}" = "Test" ]] || \
            mv .pioenvs/$environment/firmware.bin ../firmware/espurna-$version/espurna-$version-$environment.bin
    done
    echo "--------------------------------------------------------------"
}

# Parameters
while getopts "lp" opt; do
  case $opt in
    l)
        print_available
        exit
        ;;
    p)
        par_build=true
        ;;
    esac
done

shift $((OPTIND-1))

# Welcome
echo "--------------------------------------------------------------"
echo "ESPURNA FIRMWARE BUILDER"
echo "Building for version ${git_version}"

# Environments to build
environments=$@

if [ $# -eq 0 ]; then
    set_default_environments
fi

if ${CI:-false}; then
    print_environments
fi

build_webui
build_environments