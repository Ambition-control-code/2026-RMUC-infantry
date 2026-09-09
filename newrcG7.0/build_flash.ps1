param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug"
)

$ErrorActionPreference = "Stop"

if ($env:CUBE_BUNDLE_PATH) {
    $cubeToolPaths = @(
        (Join-Path $env:CUBE_BUNDLE_PATH "cmake\bin"),
        (Join-Path $env:CUBE_BUNDLE_PATH "ninja\bin"),
        (Join-Path $env:CUBE_BUNDLE_PATH "gnu-tools-for-stm32\14.3.1+st.2\bin")
    ) | Where-Object { Test-Path $_ }

    if ($cubeToolPaths) {
        $env:Path = ($cubeToolPaths -join [IO.Path]::PathSeparator) + [IO.Path]::PathSeparator + $env:Path
    }
}

if ($env:LOCALAPPDATA) {
    $localCubeRoot = Join-Path $env:LOCALAPPDATA "stm32cube\bundles"
    $localToolPaths = @(
        (Join-Path $localCubeRoot "cmake\4.2.3+st.1\bin"),
        (Join-Path $localCubeRoot "ninja\1.13.2+st.1\bin"),
        (Join-Path $localCubeRoot "gnu-tools-for-stm32\14.3.1+st.2\bin")
    ) | Where-Object { Test-Path $_ }

    if ($localToolPaths) {
        $env:Path = ($localToolPaths -join [IO.Path]::PathSeparator) + [IO.Path]::PathSeparator + $env:Path
    }
}

$cmake = $null
$pathHit = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $pathHit) {
    $pathHit = Get-Command cube-cmake -ErrorAction SilentlyContinue
}

if ($pathHit) {
    $cmake = $pathHit.Source
} else {
    $bundleRoots = @()
    if ($env:CUBE_BUNDLE_PATH) {
        $bundleRoots += @(
            (Join-Path $env:CUBE_BUNDLE_PATH "cmake"),
            (Join-Path $env:CUBE_BUNDLE_PATH "CMake")
        )
    }
    if ($env:LOCALAPPDATA) {
        $bundleRoots += (Join-Path $env:LOCALAPPDATA "stm32cube\bundles\cmake")
    }

    $cmake = $bundleRoots |
        Where-Object { Test-Path $_ } |
        ForEach-Object { Get-ChildItem -Path $_ -Filter cmake.exe -Recurse -ErrorAction SilentlyContinue } |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
}

if (-not $cmake) {
    throw "cmake.exe not found. Add CMake to PATH, or install/configure STM32Cube CMake tools."
}

& $cmake --preset $Config
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $cmake --build --preset $Config
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& (Join-Path $PSScriptRoot "flash.ps1") -Config $Config
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
