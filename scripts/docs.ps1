param([switch]$Open)
. "$PSScriptRoot\common.ps1"
$doxygenPath = Find-LabTool 'doxygen.exe' @((Join-Path $script:LabToolRoot 'doxygen\doxygen.exe'),
    (Join-Path $script:LabPreviousTools 'doxygen\doxygen.exe'))
$dotPath = Find-LabTool 'dot.exe' @((Join-Path $script:LabToolRoot 'graphviz\bin\dot.exe'),
    (Join-Path $script:LabPreviousTools 'graphviz\Graphviz-12.2.1-win64\bin\dot.exe'))
$env:PATH = (Split-Path -Parent $dotPath) + ';' + $env:PATH
Push-Location $script:LabProjectRoot
try { Invoke-LabNative $doxygenPath @('Doxyfile') } finally { Pop-Location }
if ($Open) { Invoke-Item -LiteralPath (Join-Path $script:LabProjectRoot 'docs\html\index.html') }
