# Install script for directory: C:/levolver/yonkang-sefit/levolver/zlib-1.2.8

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files/zlib")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlibd.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Debug/zlibd.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlib.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Release/zlib.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlib.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/MinSizeRel/zlib.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlib.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/RelWithDebInfo/zlib.lib")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/bin/zlibd.dll")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/bin" TYPE SHARED_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Debug/zlibd.dll")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/bin/zlib.dll")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/bin" TYPE SHARED_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Release/zlib.dll")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/bin/zlib.dll")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/bin" TYPE SHARED_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/MinSizeRel/zlib.dll")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/bin/zlib.dll")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/bin" TYPE SHARED_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/RelWithDebInfo/zlib.dll")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlibstaticd.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Debug/zlibstaticd.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlibstatic.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/Release/zlibstatic.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlibstatic.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/MinSizeRel/zlibstatic.lib")
  ELSEIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files/zlib/lib/zlibstatic.lib")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/lib" TYPE STATIC_LIBRARY FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/RelWithDebInfo/zlibstatic.lib")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/zlib/include/zconf.h;C:/Program Files/zlib/include/zlib.h")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/include" TYPE FILE FILES
    "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/zconf.h"
    "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/zlib.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/zlib/share/man/man3/zlib.3")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/share/man/man3" TYPE FILE FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/zlib.3")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files/zlib/share/pkgconfig/zlib.pc")
FILE(INSTALL DESTINATION "C:/Program Files/zlib/share/pkgconfig" TYPE FILE FILES "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/zlib.pc")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "C:/levolver/yonkang-sefit/levolver/zlib-1.2.8/win64/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
