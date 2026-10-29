$ErrorActionPreference = 'Stop'
$script:LabProjectRoot = Split-Path -Parent $PSScriptRoot
$script:LabPackageRoot = Split-Path -Parent $script:LabProjectRoot
$script:LabWorkspaceRoot = Split-Path -Parent $script:LabPackageRoot
$script:LabToolRoot = Join-Path $script:LabPackageRoot '_work\tools'
$script:LabPreviousTools = Join-Path $script:LabWorkspaceRoot 'Лабораторная-3\_work\tools'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding
$OutputEncoding = [Console]::OutputEncoding

function Find-LabTool {
    param([string]$Name, [string[]]$Candidates)
    foreach ($candidate in $Candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    $tool = Get-Command $Name -CommandType Application -ErrorAction SilentlyContinue
    if ($tool) { return $tool.Source }
    throw "Tool not found: $Name. Install it or pass its path to build.ps1."
}

function Get-LabShortPath {
    param([string]$Path)
    if (-not ('Lab3NativePath' -as [type])) {
        Add-Type -TypeDefinition @'
using System.Text;
using System.Runtime.InteropServices;
public static class Lab3NativePath {
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
    public static extern uint GetShortPathName(string path, StringBuilder result, uint size);
}
'@
    }
    $result = New-Object System.Text.StringBuilder 4096
    $length = [Lab3NativePath]::GetShortPathName($Path, $result, 4096)
    if ($length -gt 0 -and $length -lt 4096) { return $result.ToString() }
    return $Path
}

function Get-LabCMake {
    param([string]$Override)
    return Find-LabTool 'cmake.exe' @($Override,
        (Join-Path $script:LabToolRoot 'cmake\bin\cmake.exe'),
        (Join-Path $script:LabPreviousTools 'cmake-3.31.6-windows-x86_64\bin\cmake.exe'))
}

function Get-LabCompiler {
    param([string]$Override)
    return Find-LabTool 'clang++.exe' @($Override,
        (Join-Path $script:LabProjectRoot 'tools\llvm-mingw\bin\clang++.exe'),
        (Join-Path $script:LabWorkspaceRoot 'Семинар-7\_work\llvm-mingw-20260602-ucrt-x86_64\bin\clang++.exe'))
}

function Set-LabRuntimePath {
    param([string]$Compiler)
    $compilerBin = Split-Path -Parent $Compiler
    $compilerRoot = Split-Path -Parent $compilerBin
    $env:PATH = $compilerBin + ';' + $env:PATH
    $clangRoot = Join-Path $compilerRoot 'lib\clang'
    if (Test-Path -LiteralPath $clangRoot) {
        foreach ($version in Get-ChildItem -LiteralPath $clangRoot -Directory) {
            $runtime = Join-Path $version.FullName 'lib\windows'
            if (Test-Path -LiteralPath $runtime) { $env:PATH = $runtime + ';' + $env:PATH }
        }
    }
}

function Get-LabBuildDirectory {
    param([string]$BuildDirectory, [switch]$Asan, [switch]$Msvc)
    if ($BuildDirectory) { return [System.IO.Path]::GetFullPath($BuildDirectory) }
    $name = 'build'
    if ($Asan) { $name = 'build-asan' }
    if ($Msvc) { $name = 'build-msvc' }
    return Join-Path $script:LabProjectRoot $name
}

function Invoke-LabNative {
    param([string]$Executable, [string[]]$Arguments)
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Executable failed with exit code $LASTEXITCODE" }
}
