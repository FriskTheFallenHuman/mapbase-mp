@echo off
cls

astyle.exe -v --formatted --options=astyle-options.ini --recursive *.h
astyle.exe -v --formatted --options=astyle-options.ini --exclude="src/mathlib/sse.cpp" --exclude="src/mathlib/3dnow.cpp" --exclude="src/game/server/hl2/npc_combine.cpp" --recursive *.cpp

@pause