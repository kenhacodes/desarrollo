@REM Compilación y Enlace con biblioteca gráfica.
@cls
@echo ---------------------------------------------------
@echo  ESAT Curso 2024-2025 Asignatura PRG Segundo
@echo ---------------------------------------------------
@echo  Proceso por lotes iniciado. UwU
@echo ---------------------------------------------------
@echo off

cl /nologo /Zi /GR- /EHs /MD %2 -I %1\Desarrollo\Lib_Graph\ESAT_rev250\include %1\Desarrollo\Lib_Graph\ESAT_rev250\bin\ESAT.lib   opengl32.lib user32.lib gdi32.lib shell32.lib Ws2_32.lib
 
@echo ---------------------------------------------------
@echo  Proceso por lotes finalizado.
@echo ---------------------------------------------------