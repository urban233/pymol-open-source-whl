@echo off
uv run --project "%~dp0." --locked python "%~dp0pymakefile.py" %*
exit /b %ERRORLEVEL%
