cmake_minimum_required(VERSION 3.24)
project(ScifiShooter)

set(CMAKE_REQUIRED_LIBRARIES "-L${atomic_lib_dir}" "-latomic")

find_package(PkgConfig REQUIRED)

# set(CMAKE_TOOLCHAIN_FILE /home/henry/emsdk/)
if(EMSCRIPTEN)
    MESSAGE("BUILDING FOR THE WEB")
    set(CMAKE_EXECUTABLE_SUFFIX ".html ")
    # set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "public/")
else()
    find_package(X11 REQUIRED)
    find_package(ALSA REQUIRED)

    if(NOT X11_FOUND)
        message("ERROR: x11 not found")
    endif(NOT X11_FOUND)
endif()

find_package(OpenGL REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)
# pkg_search_module(GL REQUIRED gl)

set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui)
file(GLOB IMGUI_GLOB
        ${IMGUI_PATH}/imgui.h
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imconfig.h
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_internal.h
        ${IMGUI_PATH}/imstb_rectpack.h
        ${IMGUI_PATH}/imstb_textedit.h
        ${IMGUI_PATH}/imstb_truetype.h
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/misc/cpp/imgui_stdlib.h
        ${IMGUI_PATH}/misc/cpp/imgui_stdlib.cpp

        # specific bindings...
        ${IMGUI_PATH}/backends/imgui_impl_glfw.h
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.cpp)

add_library("imgui" SHARED ${IMGUI_GLOB})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})
target_link_libraries("imgui" PRIVATE glfw)

set(IMGUI_FILEBROWSER ${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui-filebrowser)

include_directories(${X11_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
MESSAGE("${CMAKE_CURRENT_LIST_DIR}")
find_package(HaGame2 REQUIRED)
MESSAGE("HAGAME_LIBRARY=${HAGAME2_LIBRARY}")

add_library("scripts" SHARED "assets/scripts/TestScript.cpp")
target_include_directories("scripts" PRIVATE ${HAGAME2_INCLUDE_DIR})
target_link_libraries("scripts" ${HAGAME2_LIBRARY})

add_executable(ScifiShooter main.cpp src/common/constants.h src/scifiGame.h src/scifiGame.cpp src/scenes/runtime.h src/scenes/runtime.cpp src/components/projectile.h src/components/actor.h src/common/weapons.cpp src/components/ai.h src/components/ai.cpp src/systems/renderer.h src/systems/renderer.cpp src/common/runtimeParams.h src/systems/player.cpp src/common/utils.h src/common/utils.cpp src/systems/enemies.h src/systems/enemies.cpp src/enemies/enemy.h src/systems/actors.h src/systems/actors.cpp src/systems/weapons.h src/systems/weapons.cpp src/common/gamestate.h src/common/gamestate.cpp src/components/explosive.h src/imgui_node.h src/imgui_node.h src/enemies/behaviors/canSee.h src/enemies/enemyState.h src/enemies/behaviors/canSee.cpp src/enemies/behaviors/moveTo.h src/enemies/behaviors/moveTo.cpp src/enemies/behaviors/findPath.cpp src/enemies/behaviors/findPath.h src/enemies/enemy.cpp src/enemies/behaviors/randomLocation.h src/enemies/behaviors/randomLocation.cpp src/enemies/behaviors/moveOnPath.h src/enemies/behaviors/moveOnPath.cpp src/enemies/behaviors/targetLocation.h src/enemies/behaviors/targetLocation.cpp src/enemies/behaviors/correctPath.h src/enemies/behaviors/correctPath.cpp src/enemies/behaviors/tags.h src/common/console.h src/common/console.cpp src/enemies/behaviors/explode.h src/enemies/behaviors/explode.cpp src/common/itemDef.h src/common/itemDef.cpp src/components/item.h src/systems/items.cpp src/systems/items.h src/scenes/loading.h src/scenes/loading.cpp src/systems/audio.h src/systems/audio.cpp src/components/light.h src/components/light.cpp src/systems/props.cpp src/components/prop.h src/common/propDef.h src/systems/props.h src/systems/worldObjects.h src/common/worldObjectDef.h src/components/inventory.h src/levelEditor/entityTree.cpp src/components/connection.h src/scenes/levelEditor.cpp src/scenes/mainMenu.cpp src/scenes/mainMenu.h src/levelEditor/tools/tilemapTool.h src/levelEditor/tools/tool.h src/components/tilemap.h src/levelEditor/tools/tilemapTool.cpp src/levelEditor/componentExplorer.cpp src/levelEditor/scriptExplorer.cpp src/levelEditor/entityViewer.cpp src/levelEditor/fileBrowser.h src/levelEditor/fileBrowser.cpp)
target_include_directories(ScifiShooter PUBLIC ${HAGAME2_INCLUDE_DIR})
target_include_directories(ScifiShooter PUBLIC ${FIND_HAGAME2_PATHS}/thirdparty/asio/asio/include/)
target_link_libraries(ScifiShooter ${HAGAME2_LIBRARY})
target_link_libraries(ScifiShooter ${GLFW_LIBRARIES})
target_link_libraries(ScifiShooter ${OPENGL_LIBRARIES})
target_include_directories(ScifiShooter PUBLIC ${IMGUI_PATH})
target_link_libraries(ScifiShooter imgui)
target_link_libraries(ScifiShooter ${X11_LIBRARIES})
target_link_libraries(ScifiShooter ${ALSA_LIBRARIES})
target_link_libraries(ScifiShooter ${OPENAL_LIBRARY})
target_include_directories(ScifiShooter PUBLIC ${IMGUI_FILEBROWSER})
# set(CMAKE_TOOLCHAIN_FILE /home/henry/emsdk/)
if(EMSCRIPTEN)
    set_target_properties(ScifiShooter PROPERTIES LINK_FLAGS "-DCMAKE_EXE_LINKER_FLAGS=-s -sMAX_WEBGL_VERSION=2 -sUSE_GLFW=3 -sTOTAL_MEMORY=512MB --preload-file ${CMAKE_CURRENT_LIST_DIR}/assets")
endif()