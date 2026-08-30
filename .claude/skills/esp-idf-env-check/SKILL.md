# Skill: ESP-IDF Environment Check & Repair for KidPal

## When to use

- KidPal firmware build fails due to ESP-IDF / toolchain / Python environment issues.
- Need to verify or upgrade ESP-IDF to the version required by Waveshare ESP32-S3-RLCD-4.2 examples / XiaoZhi V2.1.0.

## Target environment

- Windows, Git Bash / MSYS2 is NOT supported by ESP-IDF export scripts.
- KidPal target: ESP32-S3, screen 400x300.
- Recommended ESP-IDF version: `v5.5.1`.

## Steps

### 1. Check current ESP-IDF

```powershell
# In PowerShell or CMD
python %USERPROFILE%\.espressif\esp-idf\tools\idf.py --version
# or
C:\esp\v5.5.1\esp-idf\tools\idf.py --version
```

If the version is lower than required, or build fails with missing tools, proceed to install/reinstall.

### 2. Clone ESP-IDF v5.5.1 (shallow clone to avoid timeout)

```powershell
mkdir C:\esp\v5.5.1
# Use CMD for git to avoid MSYS path translation issues
cmd /c "git clone --depth 1 -b v5.5.1 --recursive https://github.com/espressif/esp-idf.git C:\esp\v5.5.1\esp-idf"
```

If submodule checkout fails (e.g. `components/esp_wifi/lib`), delete and re-fetch:

```powershell
cd C:\esp\v5.5.1\esp-idf
Remove-Item -Recurse -Force components\esp_wifi\lib
cmd /c "git submodule update --init --recursive components/esp_wifi/lib"
```

### 3. Install ESP-IDF tools

Do NOT run `install.ps1` or `export.ps1` directly from Git Bash / MSYS2. They detect MSys/Mingw and abort.

Use the Python wrapper approach:

```python
# esp_idf_install.py
import subprocess, os, sys

idf_path = r"C:\esp\v5.5.1\esp-idf"
env = dict(os.environ)
env.pop("MSYSTEM", None)
env.pop("MSYS", None)
env["IDF_PATH"] = idf_path
env["PYTHON"] = sys.executable

subprocess.run(
    [sys.executable, os.path.join(idf_path, "tools", "idf_tools.py"), "install"],
    env=env,
    check=True,
)
```

Run from a plain Windows Python (not MSYS Python):

```powershell
python esp_idf_install.py
```

### 4. Create / verify Python virtualenv

The installer creates a venv, usually:
`C:\Users\<user>\.espressif\python_env\idf5.5_py3.10_env`

Verify it exists and contains `Scripts\python.exe`.

### 5. Build KidPal firmware

Use a wrapper script that:
- Activates the ESP-IDF venv.
- Adds ESP-IDF tool directories (cmake, ninja, xtensa-elf-gcc, etc.) to PATH.
- Runs `idf.py` from the venv Python.

```python
# build_kidpal.py
import subprocess, os, sys, glob

idf_path = r"C:\esp\v5.5.1\esp-idf"
venv_python = r"C:\Users\<user>\.espressif\python_env\idf5.5_py3.10_env\Scripts\python.exe"
user = os.environ.get("USERPROFILE", r"C:\Users\<user>")
espressif = os.path.join(user, ".espressif")

tool_dirs = []
tool_root = os.path.join(espressif, "tools")
for tool in os.listdir(tool_root) if os.path.isdir(tool_root) else []:
    versions = [d for d in os.listdir(os.path.join(tool_root, tool)) if os.path.isdir(os.path.join(tool_root, tool, d))]
    if versions:
        tool_dirs.append(os.path.join(tool_root, tool, sorted(versions)[-1], "bin"))

env = dict(os.environ)
env.pop("MSYSTEM", None)
env.pop("MSYS", None)
env["IDF_PATH"] = idf_path
env["PYTHON"] = venv_python
env["PATH"] = os.pathsep.join(tool_dirs + [env.get("PATH", "")])

subprocess.run(
    [venv_python, os.path.join(idf_path, "tools", "idf.py"), "build"],
    env=env,
    cwd=r"C:\AI\ESP32\firmware\kidpal",
    check=True,
)
```

### 6. Verify a successful build

After build completes, confirm artifacts exist:

```powershell
Test-Path C:\AI\ESP32\firmware\kidpal\build\kidpal.bin
Test-Path C:\AI\ESP32\firmware\kidpal\build\kidpal.elf
```

## Common errors and fixes

| Error | Cause | Fix |
|-------|-------|-----|
| `MSys/Mingw is not supported` | Running ESP-IDF install/export in Git Bash / MSYS2 | Use plain Windows Python subprocess with MSYSTEM/MSYS removed |
| `cmake must be available on PATH` | Tool directories not on PATH | Add `.espressif/tools/<tool>/<version>/bin` folders to PATH |
| `No module named 'esp_idf_monitor'` | Wrong Python interpreter | Use venv Python `idf5.5_py3.10_env/Scripts/python.exe` |
| Submodule checkout timeout/failure | Large repo / network | `--depth 1`; delete failed submodule and re-run `git submodule update --init --recursive` |

## Outputs

- ESP-IDF installed at `C:\esp\v5.5.1\esp-idf`.
- Tools under `C:\Users\<user>\.espressif`.
- Build artifacts under `firmware/kidpal/build/`.
