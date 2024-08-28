cmake_minimum_required(VERSION 3.5)
project(libpng VERSION 1.2.18)

# Set the source directory for libpng
set(LIBPNG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add include directories
include_directories(${LIBPNG_SRC_DIR}/lib)
include_directories(C:/levolver64/zlib-1.2.8) # Adjust this to your actual path if needed

# List of source files for libpng 1.2.18
set(LIBPNG_SRC
    lib/adler32.c
    lib/compress.c
    lib/crc32.c
    lib/deflate.c
    lib/example.c
    lib/gzio.c
    lib/infback.c
    lib/inffast.c
    lib/inflate.c
    lib/inftrees.c
    lib/minigzip.c
    lib/png.c
    lib/pngerror.c
    lib/pnggccrd.c
    lib/pngget.c
    lib/pngmem.c
    lib/pngpread.c
    lib/pngread.c
    lib/pngrio.c
    lib/pngrtran.c
    lib/pngrutil.c
    lib/pngset.c
    lib/pngtest.c
    lib/pngtrans.c
    lib/pngvcrd.c
    lib/pngwio.c
    lib/pngwrite.c
    lib/pngwtran.c
    lib/pngwutil.c
    lib/trees.c
    lib/uncompr.c
    lib/xplanet_png.c
    lib/zutil.c
)

# Create the static library
add_library(png STATIC ${LIBPNG_SRC})

# Link with the Zlib library
target_link_libraries(png "C:/levolver64/zlib-1.2.8/lib/zlib.lib")

# Optionally install the library and headers
install(TARGETS png
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES
        lib/png.h
        lib/pngconf.h
        DESTINATION include)
