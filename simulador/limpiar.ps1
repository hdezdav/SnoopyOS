# Limpiar archivos compilados

Write-Host "Limpiando archivos generados..." -ForegroundColor Yellow

if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
    Write-Host "  Eliminado: build/" -ForegroundColor Gray
}

if (Test-Path "bin") {
    Remove-Item -Recurse -Force "bin"
    Write-Host "  Eliminado: bin/" -ForegroundColor Gray
}

Write-Host "Limpieza completada!" -ForegroundColor Green
