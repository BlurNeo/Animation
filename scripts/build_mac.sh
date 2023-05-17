# Commands to build on macOS

ANIMATION_ROOT="$( cd "$(dirname "$0")" ; pwd -P)"

#sh $ANIMATION_ROOT/submodule.sh

if [ -z ${IOS_PLATFORM+x} ]; then
    MAC_PLATFORM="MAC"
fi

if [ -z ${IOS_ARCH+x} ]; then
    MAC_ARCH=""
fi

if [ -z ${CMAKE_GENERATOR+x} ]; then
    CMAKE_GENERATOR="Xcode"
fi

if [ -z ${BUILD_DIR+x} ]; then
    BUILD_DIR=build-macos
fi

if [ -z ${BUILD_AND_INSTALL+x} ]; then
    BUILD_AND_INSTALL=ON
fi

if [ -z ${ANIMATION_BUILD_TYPE+x} ]; then
    ANIMATION_BUILD_TYPE=Release
fi

if [ -z ${ANIMATION_ENABLE_SANITIZER+x} ]; then
    ANIMATION_ENABLE_SANITIZER=OFF
fi

if [ -z ${ANIMATION_Install_PREFIX+x} ]; then
    ANIMATION_Install_PREFIX="macOS"
fi

if [ -z ${INSTALL_DIR+x} ]; then
    INSTALL_DIR="release"
fi

if [ -z ${ANIMATION_BUILD_TESTS+x} ]; then
    ANIMATION_BUILD_TESTS=ON
fi

if [ -z ${ANIMATION_BUILD_BENCH+x} ]; then
    ANIMATION_BUILD_BENCH=OFF
fi

if [ -z ${ANIMATION_ROSETTA2_COMPATIBLE+x} ]; then
    ANIMATION_ROSETTA2_COMPATIBLE=ON
fi

if [ -z ${ANIMATION_BUILD_TOOLS+x} ]; then
    ANIMATION_BUILD_TOOLS=ON
fi

###########################################
mkdir -p $ANIMATION_ROOT/$BUILD_DIR
cd $ANIMATION_ROOT/$BUILD_DIR

# Generate projects
echo "Generating ANIMATION projects"
cmake $ANIMATION_ROOT/.. \
    -G"$CMAKE_GENERATOR" \
    -DCMAKE_TOOLCHAIN_FILE=$ANIMATION_ROOT/../cmake/ios.toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$ANIMATION_BUILD_TYPE \
    -DCMAKE_CONFIGURATION_TYPES=$ANIMATION_BUILD_TYPE \
    -DANIMATION_Install_PREFIX="$ANIMATION_Install_PREFIX" \
    -DPLATFORM=$MAC_PLATFORM \
    -DARCHS="$MAC_ARCH" \
    -DINSTALL_DIR=$INSTALL_DIR \
    -DANIMATION_ENABLE_SANITIZER=$ANIMATION_ENABLE_SANITIZER \
    -DANIMATION_ROSETTA2_COMPATIBLE=$ANIMATION_ROSETTA2_COMPATIBLE \
    || exit 1

echo "ANIMATION projects generate successfully"

if [ ${BUILD_AND_INSTALL} = "ON" ]; then
    # NOTE: The "--config" argument matters when your build tool is a multi-
    #       configuration one, otherwise it is ignored
    #       E.g., for Xcode build tool, CMAKE_BUILD_TYPE will be ignored by
    #       cmake --build if "--config" is not specified

    echo "Building ANIMATION"
    cmake --build . --config $ANIMATION_BUILD_TYPE -- $@ || exit 1
    echo "ANIMATION build successfully"

    # echo "Installing ANIMATION"
    # cmake --build . --target install --config $ANIMATION_BUILD_TYPE || exit 1
    # echo "ANIMATION install successfully"
fi
