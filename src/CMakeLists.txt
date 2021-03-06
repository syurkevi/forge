CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(USE_SYSTEM_GLM)
    # Prior to GLM 0.9.7.0, the package is found by the FindGLM.cmake module.
    # This was removed with GLM 0.9.7.0, instead a glm-config.cmake configuration
    # file is provided. Therefore, both FIND_PACKAGE calls are necessary.
    FIND_PACKAGE(GLM QUIET)
    FIND_PACKAGE(glm QUIET)
    IF(NOT glm_FOUND AND NOT GLM_FOUND)
        MESSAGE(FATAL_ERROR "GLM Not Found")
    ENDIF()
ELSE(USE_SYSTEM_GLM)
    INCLUDE("${CMAKE_MODULE_PATH}/build_glm.cmake")
ENDIF(USE_SYSTEM_GLM)

IF(USE_SYSTEM_FREETYPE)
    FIND_PACKAGE(Freetype REQUIRED)
ELSE(USE_SYSTEM_FREETYPE)
    INCLUDE("${CMAKE_MODULE_PATH}/build_freetype.cmake")
ENDIF(USE_SYSTEM_FREETYPE)

IF(NOT FREETYPE_FOUND)
    MESSAGE(FATAL_ERROR "FreeType Not Found")
ENDIF()

IF(UNIX)
    FIND_PACKAGE(FontConfig REQUIRED)
ENDIF(UNIX)


IF(${USE_WINDOW_TOOLKIT} STREQUAL "glfw3")
    FIND_PACKAGE(GLFW REQUIRED)
    IF(GLFW_FOUND)
        SET(WTK_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
        SET(WTK_LIBRARIES ${GLFW_LIBRARY})
        ADD_DEFINITIONS(-DUSE_GLFW)
    ELSE(GLFW_FOUND)
        MESSAGE(FATAL_ERROR "GLFW not found")
    ENDIF(GLFW_FOUND)
ENDIF()

IF(${USE_WINDOW_TOOLKIT} STREQUAL "sdl2")
    FIND_PACKAGE(SDL2 REQUIRED)
    IF(SDL2_FOUND)
        SET(WTK_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
        SET(WTK_LIBRARIES ${SDL2_LIBRARY})
        ADD_DEFINITIONS(-DUSE_SDL)
    ELSE(SDL2_FOUND)
        MESSAGE(FATAL_ERROR "SDL2 not found")
    ENDIF(SDL2_FOUND)
ENDIF()


IF(NOT UNIX)
    ADD_DEFINITIONS(-DFGDLL)
ENDIF(NOT UNIX)
# OS Definitions
IF(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ELSE(UNIX) #Windows
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF(UNIX)


INCLUDE_DIRECTORIES(
    ${CMAKE_INCLUDE_PATH}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${WTK_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/src"
    )

IF(UNIX)
    INCLUDE_DIRECTORIES("${FONTCONFIG_INCLUDE_DIR}")
ENDIF(UNIX)

FILE(GLOB api_headers
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/fg/*.h"
    )

FILE(GLOB headers
    "*.hpp"
    "*.h"
    )

FILE(GLOB sources
    "*.c"
    "*.cpp"
    )

SOURCE_GROUP(API FILES ${api_headers})
SOURCE_GROUP(Headers FILES ${headers})
SOURCE_GROUP(Sources FILES ${sources})
IF(${USE_WINDOW_TOOLKIT} STREQUAL "glfw3")
    FILE(GLOB wtk_headers
        "glfw/*.hpp")
    FILE(GLOB wtk_sources
        "glfw/*.cpp")
    SOURCE_GROUP(Headers\\glfw FILES ${wtk_headers})
    SOURCE_GROUP(Sources\\glfw FILES ${wtk_sources})
ENDIF()
IF(${USE_WINDOW_TOOLKIT} STREQUAL "sdl2")
    FILE(GLOB wtk_headers
        "sdl/*.hpp")
    FILE(GLOB wtk_sources
        "sdl/*.cpp")
    SOURCE_GROUP(Headers\\sdl FILES ${wtk_headers})
    SOURCE_GROUP(Sources\\sdl FILES ${wtk_sources})
ENDIF()

ADD_LIBRARY(forge SHARED
    ${api_headers}
    ${headers}
    ${sources}
    ${wtk_headers}
    ${wtk_sources}
    )

TARGET_LINK_LIBRARIES(forge
    ${FONTCONFIG_LIBRARY}
    ${FREETYPE_LIBRARIES}
    ${WTK_LIBRARIES}
    ${GL_LIBS}
    ${X11_LIBS}
    )

INSTALL(TARGETS forge DESTINATION ${FG_INSTALL_LIB_DIR})
