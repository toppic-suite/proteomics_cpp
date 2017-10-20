rmdir /S toppic-win-gui-%1
mkdir toppic-win-gui-%1
copy ..\bin\toppic_gui.exe toppic-win-gui-%1\toppic.exe
copy LICENSE toppic-win-gui-%1
xcopy /S ..\toppic_resources toppic-win-gui-%1\toppic_resources

copy C:\msys64\mingw64\bin\libwinpthread-1.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libgcc_s_seh-1.dll toppic-win-gui-%1
copy "C:\msys64\mingw64\bin\libstdc++-6.dll" toppic-win-gui-%1
copy C:\msys64\mingw64\bin\zlib1.dll toppic-win-gui-%1

copy C:\msys64\mingw64\bin\libboost_program_options-mt.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libboost_system-mt.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libboost_thread-mt.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libboost_filesystem-mt.dll toppic-win-gui-%1

copy C:\msys64\mingw64\bin\libxerces-c.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libxalan-c1_11_0.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libicuuc58.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libxalanMsg1_11_0.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libicudt58.dll toppic-win-gui-%1

copy C:\msys64\mingw64\bin\Qt5Core.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\Qt5Gui.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\Qt5Widgets.dll toppic-win-gui-%1
mkdir toppic-win-gui-%1\platforms
copy C:\msys64\mingw64\share\qt5\plugins\platforms\qwindows.dll  toppic-win-gui-%1\platforms

copy C:\msys64\mingw64\bin\libicuin58.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libpng16-16.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libharfbuzz-0.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libgraphite2.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libfreetype-6.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libbz2-1.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libglib-2.0-0.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libintl-8.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libiconv-2.dll toppic-win-gui-%1
copy C:\msys64\mingw64\bin\libpcre-1.dll toppic-win-gui-%1

mkdir toppic-win-gui-%1\example_files
copy ..\testcases\data\mzxml_test.msalign toppic-win-gui-%1\example_files
copy ..\testcases\data\mzxml_test.fasta toppic-win-gui-%1\example_files
