cmake_minimum_required(VERSION 3.10)
project(OpenGLArrowPointer)

# 必要なパッケージを見つける
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)  # GLFWの見つけ方を修正

# 実行可能ファイルを作成
add_executable(OpenGLArrowPointer main.cpp)

# ライブラリをリンク
target_link_libraries(OpenGLArrowPointer ${OPENGL_LIBRARIES} GLEW::GLEW glfw)  # GLFW::GLFWを修正