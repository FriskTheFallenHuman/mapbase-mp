@echo off
cls

astyle.exe -v --formatted --options=astyle-options.ini --exclude="src/thirdparty" --recursive *.h  --exclude="src/public/steam" --exclude="src/materialsystem/stdshaders"
astyle.exe -v --formatted --options=astyle-options.ini --exclude="src/thirdparty"  --exclude="src/materialsystem/stdshaders" --exclude="src/mathlib/sse.cpp" --exclude="src/mathlib/3dnow.cpp" --exclude="src/game/server/hl2/npc_combine.cpp" --recursive *.cpp

@pause