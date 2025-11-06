# Agregar compilador al PATH si es necesario (para las DLLs)
if (Test-Path "C:\msys64\ucrt64\bin") {
    $env:Path += ";C:\msys64\ucrt64\bin"
} elseif (Test-Path "C:\msys64\mingw64\bin") {
    $env:Path += ";C:\msys64\mingw64\bin"
}

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  SnoopyOS - Simulador de SO" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

if (-not (Test-Path "bin\simulador.exe")) {
    Write-Host "ERROR: No se encontro el ejecutable." -ForegroundColor Red
    Write-Host "Por favor compila primero con: .\compilar.ps1" -ForegroundColor Yellow
    exit 1
}

# Ejecutar el simulador
& ".\bin\simulador.exe"

Write-Host ""
Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  Simulador finalizado" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
