# lvgl_simulator_window_vscode-v9.5.0
在 Window 上运行 LVGL 模拟器

# 运行
1.终端
mkdir build

cd build

cmake .. -G "MinGW Makefiles"

mingw32-make

.\bin\main.exe

2.vscode

直接按 F5 启动调试模式

# 配置
1.MinGW  编译器

mingw-w64-v14.0.0.zip

2.SDL2
安装包：
https://www.libsdl.org/release/  ->  SDL2-devel-2.30.2-mingw.tar.gz

解压后， x86_64-w64-mingw32 文件夹内部的
	include/（包含 SDL2/SDL.h 等头文件）、
	lib/（包含 libSDL2.dll.a、libSDL2main.a 等静态/导入库）、
	bin/（包含 SDL2.dll 动态库），
	将这些文件夹的内容 覆盖 复制到 C:/mingw64 的对应目录下

验证：
编译时版本 2.30.2，

路径： C:/mingw64/include/SDL2/SDL_version.h

运行时版本 2.30.2.0，

路径： C:/mingw64/bin, 右键点击 SDL2.dll，选择"属性",切换到 "详细信息" 选项卡，查看 "文件版本"

3.gcc

 (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0
 
安装包：x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z

4.cmake

4.4.3 ,安装包： cmake-4.4.3-windows-x86_64.msi

# 环境变量
win+R ，输入 sysdm.cpl, 高级 -> 环境变量

在 系统变量（或用户变量）区域，找到 Path 变量，双击。

点击 新建，依次添加以下路径：

C:\mingw64\bin（MinGW 的 gcc/g++/gdb 等）

C:\Program Files\CMake\bin（CMake 可执行文件）
