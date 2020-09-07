ECHO OFF

REM Start the DPMI server
dpmi.exe


REM Start the launcher
:launchstart
gdc.exe high
launcher.exe


REM On exit, run.bat will contain the startup for our selected game
call run.bat


REM Once exited from the game, return back to launcher
REM goto launchstart
