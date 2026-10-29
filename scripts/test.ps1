param([switch]$Asan, [switch]$Msvc, [switch]$NoBuild, [string]$BuildDirectory)
. "$PSScriptRoot\common.ps1"
if (-not $NoBuild) { & "$PSScriptRoot\build.ps1" -Asan:$Asan -Msvc:$Msvc -BuildDirectory $BuildDirectory }
$cmakePath = Get-LabCMake ''
$ctestPath = Join-Path (Split-Path -Parent $cmakePath) 'ctest.exe'
$buildPath = Get-LabBuildDirectory $BuildDirectory -Asan:$Asan -Msvc:$Msvc
if ($Asan) {
    Set-LabRuntimePath (Get-LabCompiler '')
    $env:ASAN_OPTIONS = 'detect_leaks=0:halt_on_error=1:strict_string_checks=1'
}
Invoke-LabNative $ctestPath @('--test-dir', $buildPath, '-C', 'Debug', '--verbose', '--output-on-failure')
