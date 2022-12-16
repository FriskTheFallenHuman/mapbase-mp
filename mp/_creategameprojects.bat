@echo off
cls
	cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DBUILD_GROUP=game
@pause