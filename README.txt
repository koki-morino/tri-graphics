This binary only works on Linux because resolving OpenGL ES headers and function
pointers is a bit of a nightmare on Windows and MacOS. I'm planning to use
Vulkan or SDL_GPU in the future, which should be much easier to support on all
platforms.

The SDL libraries take a long time for cloning the repositories. That's the why
I use submodules instead of FetchContent dependencies. This way, the library
download process is isolated from the build process.
