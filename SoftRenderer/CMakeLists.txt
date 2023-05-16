# CMakeList.txt: SoftRenderer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

set(LOCAL_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(DLL_PATH 
	${LOCAL_PATH}/external/SDL/x64/Debug/SDL2.dll
	${LOCAL_PATH}/external/SDL_Image/lib/x64/SDL2_image.dll
	${LOCAL_PATH}/external/Assimp/bin/Debug/assimp-vc142-mtd.dll
)

set(MODEL_PATH ${LOCAL_PATH}/model)

# message("LOCAL_PATH : " ${LOCAL_PATH})# 头文件
include_directories (
	"${LOCAL_PATH}/external/SDL/include"
	"${LOCAL_PATH}/external/SDL_Image/include"
	"${LOCAL_PATH}/external/Assimp/include"
)

# link目录，必须放在add_executable之前
link_directories(
	"${LOCAL_PATH}/external/SDL/x64/Debug"
	"${LOCAL_PATH}/external/SDL_Image/lib/x64"
	"${LOCAL_PATH}/external/Assimp/lib/Debug"
)

# files
file(GLOB_RECURSE SRC_LIST
	"src/*.h"
	"src/*.cpp"
)

# 将源代码添加到此项目的可执行文件。
add_executable (SoftRenderer 
	${SRC_LIST}
)

# 静态链接库，链接两个静态库
target_link_libraries(SoftRenderer
	SDL2.lib
	SDL2main.lib
	SDL2_image.lib
	assimp-vc142-mtd.lib
)

# 动态链接库，将文件移动到可执行文件的输出目录
file(COPY ${DLL_PATH}  DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

# 模型文件
file(COPY ${MODEL_PATH} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SoftRenderer PROPERTY CXX_STANDARD 20)
endif()
# TODO: 如有需要，请添加测试并安装目标。
