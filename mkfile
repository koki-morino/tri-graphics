PROJECT=gfx-gles

G=Ninja

all:V:
    # nothing to do

build:V:
    cmake -B build -S . -G '$G'
    cmake --build build -j

clean:V:
    rm -rf $PROJECT.zip
    rm -rf .cache
    rm -rf .idea
    rm -rf build

format:V:
    clang-format --style=LLVM -i $(find src -name '*.cpp' -o -name '*.h')

fix-perms:V: clean
    find . -type d -exec chmod 755 {} +
    find . -type f -exec chmod 644 {} +

init:V:
    git submodule update --init --recursive

run:V: build
    # This enables hardware acceleration on WSL2
    GALLIUM_DRIVER=d3d12 ./build/$PROJECT

zip:V: fix-perms
    project_directory=$(basename $(pwd))
    cd ..
    zip -r "$project_directory/$PROJECT.zip" \
           "$project_directory"              \
        -x "$project_directory/.git/*"       \
        -x "$project_directory/external/*"
