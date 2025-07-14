# Paths
$source = "F:\GitHub\dostest\build\DOSTest.exe"
$destination = "D:\APPS\DOSTest.exe"
$driveLetter = "D"

Write-Host "Building"
cmake --build build --config Debug --target all

Write-Host "Copying $source to $destination..."
Copy-Item -Path $source -Destination $destination -Force

Start-Sleep -Seconds 0.5

Write-Host "Ejecting drive $driveLetter..."

$driveEject = New-Object -comObject Shell.Application
$driveEject.Namespace(17).ParseName("${driveLetter}:\").InvokeVerb("Eject")
