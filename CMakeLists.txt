cmake_minimum_required(VERSION 3.20)
project(VulkanRender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS  "Enable ASan/UBSan in Debug" ON)
option(PORTABLE_BUILD     "Bundle shared libs into <install>/lib" ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
    unofficial::shaderc::shaderc
    GLEW::GLEW
    glm::glm
    glfw
)

if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-O1 -g -fno-omit-frame-pointer
                           -fsanitize=address,undefined
                           -D_GLIBCXX_ASSERTIONS>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH          "$ORIGIN/lib"
    INSTALL_RPATH        "$ORIGIN/lib"
    INSTALL_RPATH_USE_LINK_PATH OFF
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/build/portable"
        CACHE PATH "Install prefix" FORCE)
endif()

if(PORTABLE_BUILD)
    install(CODE "
        file(GET_RUNTIME_DEPENDENCIES
            EXECUTABLES \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}\"
            RESOLVED_DEPENDENCIES_VAR   _r_deps
            UNRESOLVED_DEPENDENCIES_VAR _u_deps
            PRE_EXCLUDE_REGEXES
                [[libc\\.so]]
                [[libm\\.so]]
                [[libpthread\\.so]]
                [[libdl\\.so]]
                [[librt\\.so]]
                [[libstdc\\+\\+\\.so]]
                [[libgcc_s\\.so]]
                [[ld-linux]]
                # macOS system
                [[/usr/lib/libSystem]]
                [[/usr/lib/libc\\+\\+]]
        )
        foreach(_dep IN LISTS _r_deps)
            file(INSTALL \"\${_dep}\"
                DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/lib\"
                FOLLOW_SYMLINK_CHAIN
            )
        endforeach()
        if(_u_deps)
            message(WARNING \"Unresolved deps: \${_u_deps}\")
        endif()
    ")
endif()