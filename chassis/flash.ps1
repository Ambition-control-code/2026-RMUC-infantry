param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Debug"
)

$ErrorActionPreference = "Stop"

$elf = Join-Path $PSScriptRoot "build\$Config\MAN-WCIS.elf"
if (-not (Test-Path $elf)) {
    throw "ELF not found: $elf. Build the $Config preset first."
}

$jlinkCandidates = @(
    "C:\Program Files\SEGGER\JLink\JLink.exe",
    "C:\Program Files\SEGGER\JLink_V926\JLink.exe"
)

$jlink = $jlinkCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $jlink) {
    $pathHit = Get-Command JLink.exe -ErrorAction SilentlyContinue
    if ($pathHit) {
        $jlink = $pathHit.Source
    }
}

if (-not $jlink) {
    throw "JLink.exe not found. Add SEGGER J-Link to PATH, or install SEGGER J-Link."
}

$script = New-TemporaryFile
try {
    @"
r
loadfile "$elf"
r
g
qc
"@ | Set-Content -Path $script -Encoding ASCII

    & $jlink -device STM32F407IG -if SWD -speed 4000 -autoconnect 1 -CommanderScript $script
} finally {
    Remove-Item $script -ErrorAction SilentlyContinue
}
