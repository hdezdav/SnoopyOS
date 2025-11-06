# Script de para Windows con PowerShell
# Tener g++ o MinGW instalado :(

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  Compilando SnoopyOS Simulador" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan

# Agregar compilador al PATH si es necesario
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    if (Test-Path "C:\msys64\ucrt64\bin\g++.exe") {
        $env:Path += ";C:\msys64\ucrt64\bin"
        Write-Host "Compilador agregado al PATH de esta sesion" -ForegroundColor Yellow
    } elseif (Test-Path "C:\msys64\mingw64\bin\g++.exe") {
        $env:Path += ";C:\msys64\mingw64\bin"
        Write-Host "Compilador agregado al PATH de esta sesion" -ForegroundColor Yellow
    }
}

# Crear directorios si no existen
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}
if (-not (Test-Path "bin")) {
    New-Item -ItemType Directory -Path "bin" | Out-Null
}

# Compilar archivos fuente
Write-Host "`nCompilando archivos fuente..." -ForegroundColor Yellow

$archivos = @(
    "src/planificador.cpp",
    "src/memoria.cpp",
    "src/sincronizacion.cpp",
    "src/problemas_clasicos.cpp",
    "src/io.cpp",
    "src/disco.cpp",
    "src/heap.cpp",
    "src/cli.cpp",
    "src/main.cpp"
)

$objetos = @()
$errores = 0

foreach ($archivo in $archivos) {
    $nombre = [System.IO.Path]::GetFileNameWithoutExtension($archivo)
    $objeto = "build/$nombre.o"
    
    Write-Host "  Compilando $archivo..." -ForegroundColor Gray
    
    try {
        & g++ -std=c++17 -Iinclude -Wall -c $archivo -o $objeto 2>&1 | Out-String | Write-Host
        
        if ($LASTEXITCODE -eq 0) {
            $objetos += $objeto
            Write-Host "    OK: $nombre.o" -ForegroundColor Green
        } else {
            $errores++
            Write-Host "    ERROR al compilar $archivo" -ForegroundColor Red
        }
    } catch {
        Write-Host "    ERROR: No se encontró el compilador g++" -ForegroundColor Red
        Write-Host "    Por favor instala MinGW-w64 o MSYS2" -ForegroundColor Red
        exit 1
    }
}

if ($errores -gt 0) {
    Write-Host "`nCompilacion fallida con $errores errores" -ForegroundColor Red
    exit 1
}

# Enlazar
Write-Host "`nEnlazando ejecutable..." -ForegroundColor Yellow
& g++ -std=c++17 -Iinclude -o bin/simulador.exe $objetos

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilacion exitosa!" -ForegroundColor Green
    Write-Host "`nEjecutable creado en: bin/simulador.exe" -ForegroundColor Cyan
    Write-Host "Para ejecutar: .\bin\simulador.exe" -ForegroundColor Cyan
} else {
    Write-Host "Error al enlazar el ejecutable" -ForegroundColor Red
    exit 1
}
