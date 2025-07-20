$command = { cmake --build build --target clean; cmake --build build --config Release --target all }

while ($true) {
    # Run the command and capture all output
    $output = & $command 2>&1

    # Filter lines that match the phrase
    $matchedLines = $output | Where-Object { $_ -match "Binary size exceeds limit by|Binary size:" }

    if ($matchedLines) {
        $matchedLines | ForEach-Object { Write-Host $_ }
    }
}
