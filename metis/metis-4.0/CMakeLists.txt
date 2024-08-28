cmake_minimum_required(VERSION 3.5)
project(METIS)

set(SHARED FALSE CACHE BOOL "build a shared library")

if(MSVC)
  set(METIS_INSTALL FALSE)
else()
  set(METIS_INSTALL TRUE)
endif()

# Configure metis library.
if(SHARED)
  set(METIS_LIBRARY_TYPE SHARED)
else()
  set(METIS_LIBRARY_TYPE STATIC)
endif(SHARED)

# Add include directories.
include_directories(include)
include_directories(lib)

# Add source files. Adjust this list to match the files in your METIS 4.0 directory.
set(METIS_SRC
    lib/balance.c
    lib/bucketsort.c
    lib/ccgraph.c
    lib/coarsen.c
    lib/debug.c
    lib/fm.c
    # lib/fm2.c  # Removed because it's missing.
    lib/frename.c
    # lib/gk_metis.c
    lib/graph.c
    lib/initpart.c
    lib/kmetis.c
    lib/memory.c
    lib/mesh.c
    lib/meshpart.c
    lib/mincover.c
    lib/mmd.c
    # lib/multilevel.c
    lib/myqsort.c
    lib/pmetis.c
    lib/refine.c
    # lib/split.c
    lib/stat.c
    lib/subdomains.c
    lib/timing.c
    lib/util.c
)

# Create the METIS library.
add_library(metis ${METIS_LIBRARY_TYPE} ${METIS_SRC})

# Optionally install the library.
if(METIS_INSTALL)
  install(TARGETS metis
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
  install(DIRECTORY include/ DESTINATION include)
endif()

# Remove the metis_test executable section if you don't have a test program.
