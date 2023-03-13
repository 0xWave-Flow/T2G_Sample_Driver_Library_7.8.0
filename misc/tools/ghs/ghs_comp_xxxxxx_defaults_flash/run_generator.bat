@echo off
pushd "%~dp0"

REM The Python 3 executable must be in your PATH!

cd generator
python generate_odb.py

popd

pause
