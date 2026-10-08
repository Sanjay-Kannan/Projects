param(
    [Parameter(Mandatory = $true)]
    [string]$CubeWbProject,
    [ValidateSet('Sensor', 'Light', 'Fan')]
    [string]$Role = 'Sensor'
)

$ErrorActionPreference = 'Stop'
$project = (Resolve-Path -LiteralPath $CubeWbProject).Path
$sourceRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\firmware')).Path
$allowedProjectFiles = @('.project', '.cproject', '.ioc')
$presentProjectFiles = Get-ChildItem -LiteralPath $project -File | Where-Object { $_.Name -in $allowedProjectFiles }
if ($presentProjectFiles.Count -eq 0) {
    throw "The target folder does not look like a STM32CubeIDE/CubeMX project: $project"
}

$targetRoot = Join-Path $project 'Core\Src\room'
New-Item -ItemType Directory -Path $targetRoot -Force | Out-Null
Get-ChildItem -LiteralPath $sourceRoot -Recurse -File -Include '*.c', '*.h' |
    Where-Object { $_.FullName -notmatch '[\\/]coordinator[\\/]' } |
    ForEach-Object {
        $relative = [System.IO.Path]::GetRelativePath($sourceRoot, $_.DirectoryName)
        $destination = Join-Path $targetRoot $relative
        New-Item -ItemType Directory -Path $destination -Force | Out-Null
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $destination $_.Name) -Force
    }

$ioc = Join-Path $sourceRoot 'platform\room_board.ioc'
Copy-Item -LiteralPath $ioc -Destination (Join-Path $project 'room_board.ioc') -Force

$symbol = switch ($Role) {
    'Sensor' { 'ROOM_DEVICE_ROLE=1' }
    'Light'  { 'ROOM_DEVICE_ROLE=2' }
    'Fan'    { 'ROOM_DEVICE_ROLE=3' }
}
$notes = @"
Room firmware sources were copied into Core/Src/room, and room_board.ioc was copied to the project root.
Selected image role: $Role ($symbol).

In STM32CubeIDE, add the following include search paths to the active build configuration:
Core/Src/room
Core/Src/room/common
Core/Src/room/drivers
Core/Src/room/platform
Core/Src/room/zigbee
Core/Src/room/environmental_node
Core/Src/room/device_control

Add $symbol to the compiler preprocessor symbols.
In the CubeWB Zigbee Skeleton application-start sequencer task, after ZbInit has returned its ZigBeeT pointer,
call room_cube_application_start(zigbee_app_info.zb). Do not call this from an interrupt.
Regenerate the board peripheral initialization from room_board.ioc, retaining the Skeleton's CPU2/IPCC/SHCI setup.
"@
Set-Content -LiteralPath (Join-Path $project 'ROOM_FIRMWARE_INTEGRATION.txt') -Value $notes -Encoding utf8
Write-Host "Installed $Role firmware sources into $project"
Write-Host 'Next, follow ROOM_FIRMWARE_INTEGRATION.txt to connect the CubeWB stack startup and compiler settings.'
