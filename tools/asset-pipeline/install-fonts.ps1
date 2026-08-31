# Per-user font install (no admin): ZCOOL KuaiLe + Baloo 2 (ExtraBold instance)
$ErrorActionPreference = 'Stop'
$fontsDir = Join-Path $env:LOCALAPPDATA 'Microsoft\Windows\Fonts'
New-Item -ItemType Directory -Force -Path $fontsDir | Out-Null
$reg = 'HKCU:\Software\Microsoft\Windows NT\CurrentVersion\Fonts'
foreach ($entry in @(
  @{ Name = 'ZCOOL KuaiLe (TrueType)'; File = 'ZCOOLKuaiLe-Regular.ttf' },
  @{ Name = 'Baloo 2 ExtraBold (TrueType)'; File = 'Baloo2-800.ttf' }
)) {
  $src = Join-Path $PWD ('fonts\' + $entry.File)
  $dst = Join-Path $fontsDir $entry.File
  Copy-Item -Force $src $dst
  New-ItemProperty -Path $reg -Name $entry.Name -Value $entry.File -PropertyType String -Force | Out-Null
  Write-Host "installed $($entry.Name) -> $dst"
}
