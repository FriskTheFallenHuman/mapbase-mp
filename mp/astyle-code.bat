@echo off
cls

astyle.exe -v --formatted --options=astyle-options.ini --exclude="src/thirdparty" --recursive *.h
astyle.exe -v --formatted --options=astyle-options.ini --exclude="src/thirdparty" --exclude="src/mathlib/sse.cpp" --exclude="src/mathlib/3dnow.cpp" --exclude="src/game/server/hl2/npc_combine.cpp" --recursive *.cpp

@pause