cmake_minimum_required(VERSION 3.16)
project(lab04)
set(CMAKE_C_STANDARD 99)  # 使用 C99 标准

# 使用 cmake 的 Flex 模块
find_package(FLEX REQUIRED)
message("Info: flex found!")

# 使用 cmake 的 Bison 模块
find_package(BISON REQUIRED)
message("Info: bison found!")

# 为了 Flex 和 Bison 新增头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # Bison 和 Flex 通常在这里输出

# 指定 Flex 编译目标
FLEX_TARGET(MyScanner lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)

# 指定 Bison 编译目标
BISON_TARGET(MyParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)

# 指定 Bison 与 Flex 的依赖关系
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# 添加编译选项
add_compile_options(-pedantic)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# 创建可执行文件，包括 Bison 和 Flex 生成的源文件
add_executable(Compilerlab4
    ${FLEX_MyScanner_OUTPUTS}
    ${BISON_MyParser_OUTPUTS}
	ast.c
    # 这里添加其他源文件，例如 main.c 等
)

# 设置目标编译特性可以移除，因为 CMake 默认适用 C 标准
# target_compile_features(Compilerlab2 PRIVATE c_std_99)  // 如果需要明确指定，也可以这样写

