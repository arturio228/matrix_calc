param([switch]$Asan, [switch]$Msvc, [string]$BuildDirectory,
      [string]$Compiler, [string]$CMake, [string]$MakeProgram,
      [string]$GoogleTestSource, [int]$Jobs = 2)
. "$PSScriptRoot\common.ps1"
$cmakePath = Get-LabCMake $CMake
$buildPath = Get-LabBuildDirectory $BuildDirectory -Asan:$Asan -Msvc:$Msvc
$configureArgs = @('-S', $script:LabProjectRoot, '-B', $buildPath,
    '-DBUILD_TESTING=ON', '-DMATRIX_USE_GTEST=ON', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON')
if ($Msvc) {
    $configureArgs += @('-G', 'Visual Studio 17 2022', '-A', 'x64')
} else {
    $compilerPath = Get-LabCompiler $Compiler
    $compilerBin = Split-Path -Parent $compilerPath
    $makePath = Find-LabTool 'mingw32-make.exe' @($MakeProgram, (Join-Path $compilerBin 'mingw32-make.exe'))
    Set-LabRuntimePath $compilerPath
    $configureArgs += @('-G', 'MinGW Makefiles', '-DCMAKE_BUILD_TYPE=Debug',
        ('-DCMAKE_CXX_COMPILER=' + (Get-LabShortPath $compilerPath)),
        ('-DCMAKE_C_COMPILER=' + (Get-LabShortPath (Join-Path $compilerBin 'clang.exe'))),
        ('-DCMAKE_MAKE_PROGRAM=' + (Get-LabShortPath $makePath)))
}
if (-not $GoogleTestSource) {
    $offlineSource = Join-Path $script:LabToolRoot 'googletest'
    if (Test-Path -LiteralPath (Join-Path $offlineSource 'CMakeLists.txt')) { $GoogleTestSource = $offlineSource }
}
if ($GoogleTestSource) { $configureArgs += '-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=' + $GoogleTestSource }
if ($Asan) {
    $configureArgs += @('-DMATRIX_ENABLE_ASAN=ON', '-DMATRIX_STATIC_RUNTIME=OFF')
    $env:ASAN_OPTIONS = 'detect_leaks=0:halt_on_error=1:strict_string_checks=1'
} else {
    $configureArgs += @('-DMATRIX_ENABLE_ASAN=OFF', '-DMATRIX_STATIC_RUNTIME=ON')
}
Invoke-LabNative $cmakePath $configureArgs
Invoke-LabNative $cmakePath @('--build', $buildPath, '--config', 'Debug', '--parallel', "$Jobs")
