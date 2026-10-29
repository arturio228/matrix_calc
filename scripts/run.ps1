param([switch]$Asan, [switch]$Msvc, [switch]$NoBuild, [string]$BuildDirectory)
. "$PSScriptRoot\common.ps1"
if (-not $NoBuild) { & "$PSScriptRoot\build.ps1" -Asan:$Asan -Msvc:$Msvc -BuildDirectory $BuildDirectory }
$buildPath = Get-LabBuildDirectory $BuildDirectory -Asan:$Asan -Msvc:$Msvc
if ($Msvc) { $buildPath = Join-Path $buildPath 'Debug' }
if ($Asan) {
    Set-LabRuntimePath (Get-LabCompiler '')
    $env:ASAN_OPTIONS = 'detect_leaks=0:halt_on_error=1:strict_string_checks=1'
}
& "$env:SystemRoot\System32\chcp.com" 65001 | Out-Null
Invoke-LabNative (Join-Path $buildPath 'matrix_calculator.exe') @()
