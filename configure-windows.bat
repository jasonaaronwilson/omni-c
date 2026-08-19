where cl >nul 2>nul || call "c:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

if not exist "build-dir" mkdir "build-dir"
if not exist "build-dir\bin" mkdir "build-dir\bin"
if not exist "build-dir\gen-files" mkdir "build-dir\gen-files"

rem cl /std:clatest /Ic:\Users\jason\vcpkg\installed\x64-windows-static\include /Fo:build-dir\ /Fe:build-dir\bin\omni-c-stable.exe src\omni-c-stable-windows.c /link c:\Users\jason\vcpkg\installed\x64-windows-static\lib\gc.lib

clang -Wno-format-security -Wno-parentheses-equality -Wno-deprecated-declarations -Ic:/Users/jason/vcpkg/installed/x64-windows-static/include src/omni-c-stable-windows.c c:/Users/jason/vcpkg/installed/x64-windows-static/lib/gc.lib -ldbghelp -o build-dir/bin/omni-c-stable




