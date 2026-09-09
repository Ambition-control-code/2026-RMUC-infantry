param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug"
)

$ErrorActionPreference = "Stop"

# ---- 定位 cmake ----
$cmake = $null
$pathHit = Get-Command cmake -ErrorAction SilentlyContinue
if ($pathHit) {
    $cmake = $pathHit.Source
} else {
    $bundleRoot = Join-Path $env:LOCALAPPDATA "stm32cube\bundles\cmake"
    $cmake = Get-ChildItem -Path $bundleRoot -Filter cmake.exe -Recurse -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
}
if (-not $cmake) {
    throw "cmake.exe not found."
}

# ---- 定位 ninja ----
$ninjaDir = $null
$pathHit = Get-Command ninja -ErrorAction SilentlyContinue
if ($pathHit) {
    $ninjaDir = Split-Path $pathHit.Source -Parent
} else {
    $bundleRoot = Join-Path $env:LOCALAPPDATA "stm32cube\bundles\ninja"
    $ninjaExe = Get-ChildItem -Path $bundleRoot -Filter ninja.exe -Recurse -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
    if ($ninjaExe) { $ninjaDir = Split-Path $ninjaExe -Parent }
}

# ---- 定位 arm-none-eabi-gcc ----
$gccDir = $null
$pathHit = Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue
if ($pathHit) {
    $gccDir = Split-Path $pathHit.Source -Parent
} else {
    $bundleRoot = Join-Path $env:LOCALAPPDATA "stm32cube\bundles\gnu-tools-for-stm32"
    $gccExe = Get-ChildItem -Path $bundleRoot -Filter arm-none-eabi-gcc.exe -Recurse -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending |
        Select-Object -First 1 -ExpandProperty FullName
    if ($gccExe) { $gccDir = Split-Path $gccExe -Parent }
}

# ---- 组装 PATH ----
$envPath = $env:PATH
try {
    $paths = @()
    if ($ninjaDir) { $paths += $ninjaDir }
    if ($gccDir)  { $paths += $gccDir }
    if ($paths.Count -gt 0) {
        $env:PATH = ($paths -join ';') + ';' + $envPath
    }

    # ---- 自动配置 ----
    $buildDir = Join-Path $PSScriptRoot "build\$Config"
    $cacheFile = Join-Path $buildDir "CMakeCache.txt"
    $needConfigure = $false

    if (-not (Test-Path $cacheFile)) {
        Write-Host "CMakeCache.txt not found, configuring..."
        $needConfigure = $true
    } else {
        $cacheContent = Get-Content $cacheFile -Raw
        $sourceDir = Join-Path $PSScriptRoot ""
        if ($cacheContent -notmatch [regex]::Escape($sourceDir)) {
            Write-Host "CMakeCache source directory mismatch, reconfiguring..."
            Remove-Item -Path $buildDir -Recurse -Force -ErrorAction Stop
            $needConfigure = $true
        }
    }

    if ($needConfigure) {
        & $cmake --preset $Config
        if ($LASTEXITCODE -ne 0) {
            exit $LASTEXITCODE
        }
    }

    # ---- 编译 ----
    & $cmake --build --preset $Config
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
} finally {
    $env:PATH = $envPath
}

# ---- 烧录 ----
& (Join-Path $PSScriptRoot "flash.ps1") -Config $Config
