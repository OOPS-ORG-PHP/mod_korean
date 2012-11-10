# Microsoft Developer Studio Project File - Name="korean" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=korean - Win32 Debug_TS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "koren.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "koren.mak" CFG="korean - Win32 Debug_TS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "korean - Win32 Release_TS" ("Win32 (x86) Dynamic-Link Library")
!MESSAGE "korean - Win32 Debug_TS" ("Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "korean - Win32 Release_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "korean_Win32_Release_TS"
# PROP BASE Intermediate_Dir "korean_Win32_Release_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_TS"
# PROP Intermediate_Dir "Release_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KOREAN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D "NDEBUG" /D ZEND_DEBUG=0 /D ZTS=1 /D "COMPILE_DL_KOREAN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ZEND_WIN32" /D "PHP_WIN32" /D "HAVE_KRLIBGD=1" /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 php4ts.lib freetype2.lib gd.lib libjpeg.lib libtiff.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib resolv.lib lib44bsd95.lib /nologo /dll /machine:I386 /out:"..\..\Release_TS/php_korean.dll" /libpath:"..\..\Release_TS" /libpath:"..\..\Release_TS_Inline"

!ELSEIF  "$(CFG)" == "korean - Win32 Debug_TS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "korean_Win32_Debug_TS"
# PROP BASE Intermediate_Dir "korean_Win32_Debug_TS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "korean_Win32_Debug_TS"
# PROP Intermediate_Dir "korean_Win32_Debug_TS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KOREAN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\main" /I "..\..\Zend" /I "..\..\TSRM" /D ZEND_DEBUG=1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "KOREAN_EXPORTS" /D "COMPILE_DL_KOREAN" /D ZTS=1 /D "ZEND_WIN32" /D "PHP_WIN32" /D "HAVE_KRLIBGD=1" /D "HAVE_GD_PNG" /D "HAVE_GD_JPG" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 php4ts_debug.lib freetype2.lib gd.lib libjpeg.lib libtiff.lib libpng.lib zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib resolv.lib lib44bsd95.lib /nologo /dll /debug /machine:I386 /out:"..\..\Debug_TS/php_korean.dll" /pdbtype:sept /libpath:"..\..\Debug_TS"

!ENDIF 

# Begin Target

# Name "korean - Win32 Release_TS"
# Name "korean - Win32 Debug_TS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\korean.c
# End Source File
# Begin Source File

SOURCE=.\krfile.c
# End Source File
# Begin Source File

SOURCE=.\krcheck.c
# End Source File
# Begin Source File

SOURCE=.\krerror.c
# End Source File
# Begin Source File

SOURCE=.\krparse.c
# End Source File
# Begin Source File

SOURCE=.\krmail.c
# End Source File
# Begin Source File

SOURCE=.\krnetwork.c
# End Source File
# Begin Source File

SOURCE=.\krimage.c
# End Source File
# Begin Source File

SOURCE=.\krmath.c
# End Source File
# Begin Source File

SOURCE=.\krregex.c
# End Source File
# Begin Source File

SOURCE=..\..\win32\readdir.c
# End Source File

# Begin Source File

SOURCE=..\standard\base64.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cp949_table.h
# End Source File
# Begin Source File

SOURCE=.\php_korean.h
# End Source File
# Begin Source File

SOURCE=.\php_krcheck.h
# End Source File
# Begin Source File

SOURCE=.\php_krfile.h
# End Source File
# Begin Source File

SOURCE=.\php_krerror.h
# End Source File
# Begin Source File

SOURCE=.\php_krparse.h
# End Source File
# Begin Source File

SOURCE=.\php_krmail.h
# End Source File
# Begin Source File

SOURCE=.\php_krnetwork.h
# End Source File
# Begin Source File

SOURCE=.\php_krimage.h
# End Source File
# Begin Source File

SOURCE=.\php_krmath.h
# End Source File
# Begin Source File

SOURCE=.\krregex.h
# End Source File
# Begin Source File

SOURCE=.\unicode_cp949_ncr_table.h
# End Source File
# Begin Source File

SOURCE=..\..\win32\readdir.h
# End Source File
# Begin Source File

SOURCE=..\standard\base64.h
# End Source File
# End Group
# End Target
# End Project
