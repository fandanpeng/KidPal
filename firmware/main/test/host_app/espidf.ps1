# ESP-IDF run helper (Windows host). Compensates for two environment quirks seen
# on this machine when invoked from Git Bash / MSYS:
#   1. The IDF 5.5.1 python env is idf5.5_py3.10_env, but the default `python` on
#      PATH is 3.14 -> export scripts would look for a non-existent py3.14 env.
#   2. export.bat / export.ps1 refuse to run when MSYSTEM is set (Git Bash sets it).
# Usage:  powershell -NoProfile -ExecutionPolicy Bypass -File <this> [idf.py args...]
# Example: powershell ... -File espidf.ps1 --preview set-target linux
#          powershell ... -File espidf.ps1 -T main build
param([Parameter(ValueFromRemainingArguments=$true)]$idfArgs)

$ErrorActionPreference = 'Continue'

# Prepend the IDF 5.5 py3.10 venv so `python` resolves to the matching env.
$venv = "$env:USERPROFILE\.espressif\python_env\idf5.5_py3.10_env\Scripts"
if (Test-Path "$venv\python.exe") {
    $env:Path = "$venv;" + $env:Path
} else {
    Write-Error "IDF python venv not found: $venv"
    exit 2
}

# Clear MSYSTEM so ESP-IDF export scripts accept this session as a supported shell.
Remove-Item Env:MSYSTEM -ErrorAction SilentlyContinue

# Seed IDF_PATH, then source ESP-IDF export.ps1 to add tools (cmake/ninja) to PATH.
$env:IDF_PATH = 'C:\esp\v5.5.1\esp-idf'
. 'C:\esp\v5.5.1\esp-idf\export.ps1'

& idf.py @idfArgs
exit $LASTEXITCODE