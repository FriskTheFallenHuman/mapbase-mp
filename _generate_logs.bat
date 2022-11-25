@echo OFF
cls
rem Print out the version
.\git-chglog --version
timeout /t 10
cls
.\git-chglog --init
@pause