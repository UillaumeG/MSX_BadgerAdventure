echo off
setlocal EnableDelayedExpansion

:: Path
set Tools=..\..\tools
set MSXtk=%Tools%\MSXtk\bin
set AudioMisc=%Tools%\audio\misc
set Src=datasrc
set Dest=content
set FullPath=%~d0%~p0

if not exist %Dest% md %Dest%

::=============================================================================
:: AUDIO
::=============================================================================

::-----------------------------------------------------------------------------
:: Build Arkos data : arkos > pletter > msxbin
echo ----------------------------------------
echo Building music Arkos Test1...
%Tools%\compress\Pletter\pletter %Src%\sound\Test1.akg  %Src%\sound\g_AKG_Test1.pl5
%MSXtk%\MSXbin %Src%\sound\g_AKG_Test1.pl5 -ad -t g_AKG_Test1 -o %Dest%\sound\g_AKG_Test1.h

::=============================================================================
:: TILES
::=============================================================================

::-----------------------------------------------------------------------------
:: Tile BACKGROUND
echo ----------------------------------------
echo Building tiles BACKGROUND...
E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	%Src%\img\tiles_background.png                      ^
	-out %Dest%/img/data_tile_background.h              ^
	-name g_DataTileBackground                          ^
	-mode gm1                                           ^
	-size 64 32                                         ^
	-pos  0 8                                           ^
	-gap  0 0                                           ^
	-num  8 4                                           ^
	-pal msx1                  

::=============================================================================
:: SPRITES
::=============================================================================

::-----------------------------------------------------------------------------
:: Sprite BADGER
echo ----------------------------------------
echo Building sprite BADGER...
E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	%Src%\img\badger.png       ^
	-out %Dest%/img/data_sprite_badger.h  ^
	-name g_DataSprtLayer      ^
	-mode sprt                 ^
	-size 16 16                ^
	-pos 0 0                   ^
	-gap 1 1                   ^
	-num 8 2                   ^
	-pal msx1                  ^
	-l i16 0 0 1 1 0xFFFFFF    ^
	-l i16 0 0 1 1 0x000000
::-----------------------------------------------------------------------------
:: Sprite BOULDER
echo ----------------------------------------
echo Building sprite BOULDER...
E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	%Src%\img\badger.png	   ^
	-out %Dest%/img/data_sprite_boulder.h ^
	-name g_DataBoulderLayer   ^
	-mode sprt                 ^
	-size 16 16                ^
	-pos 0 34                  ^
	-gap 1 1                   ^
	-num 8 1                   ^
	-pal msx1                  ^
	-l i16 0 0 1 1 0x000000    ^
	-l i16 0 0 1 1 0xFFFFFF
::-----------------------------------------------------------------------------
:: Sprite EAGLE
echo ----------------------------------------
echo Building sprite EAGLE...
E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	%Src%\img\badger.png	   ^
	-out %Dest%/img/data_sprite_eagle.h   ^
	-name g_DataEagleLayer	   ^
	-mode sprt                 ^
	-size 16 16                ^
	-pos 0 51                  ^
	-gap 1 1                   ^
	-num 4 1                   ^
	-pal msx1                  ^
	-l i16 0 0 1 1 0x000000
REM ::-----------------------------------------------------------------------------
REM :: Tile FLOWER
REM echo ----------------------------------------
REM echo Building sprite FLOWER...
REM E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	REM %Src%\img\badger.png       ^
	REM -out %Dest%/img/data_sprite_flower.h  ^
	REM -name g_DataFlowerLayer	   ^
	REM -mode sprt                 ^
	REM -size 16 16                ^
	REM -pos 0 68                  ^
	REM -gap 1 1                   ^
	REM -num 4 1                   ^
	REM -pal msx1                  ^
	REM -l i16 0 0 1 1 0xffffff
REM ::-----------------------------------------------------------------------------
REM :: Tile CRISTAL
REM echo ----------------------------------------
REM echo Building sprite CRISTAL...
REM E:\users\gilbert\Documents\MSXgl-1.2.16\tools\MSXtk\bin\MSXimg ^
	REM %Src%\img\badger.png	   ^
	REM -out %Dest%/img/data_sprite_cristal.h  ^
	REM -name g_DataCristalLayer   ^
	REM -mode sprt                 ^
	REM -size 16 16                ^
	REM -pos 0 85                  ^
	REM -gap 1 1                   ^
	REM -num 4 1                   ^
	REM -pal msx1                  ^
	REM -l i16 0 0 1 1 0xffffff
