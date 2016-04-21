# Install script for directory: C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Box2D")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/Box2D/cmake_install.cmake")
  include("C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/HelloWorld/cmake_install.cmake")
  include("C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/glew/cmake_install.cmake")
  include("C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/glfw/cmake_install.cmake")
  include("C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/Testbed/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

file(WRITE "C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/${CMAKE_INSTALL_MANIFEST}" "")
foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  file(APPEND "C:/Users/Carl/Documents/GitHub/SDL3D/lib/Box2D/Build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
endforeach()
