# SnoopyOS - Simulador de Sistema Operativo
Simulador completo de un núcleo de sistema operativo en C++ con gestión de procesos, memoria virtual, sincronización y más.

## 📋 Características Principales

- **Planificación de Procesos**: Round Robin y SJF (Shortest Job First)
- **Memoria Virtual**: Paginación con algoritmos FIFO, LRU y PFF
- **Sincronización**: Semáforos, Mutex, Productor-Consumidor
- **Problemas Clásicos**: Filósofos Comensales y Lectores-Escritores
- **Gestión de Estados**: NUEVO, LISTO, EJECUTANDO, BLOQUEADO, SUSPENDIDO, TERMINADO
- **Análisis de Fairness**: Métricas de equidad del planificador
- **Visualización Avanzada**: Estado de memoria con colores (hits/miss)
- **I/O Avanzado**: Impresora virtual con colas de prioridad
- **Disco**: FCFS, SSTF, SCAN con visualización del cabezal
- **Heap Allocator**: Buddy System con malloc/free
- **CLI Interactiva**: 35+ comandos para control completo
- **Scripts**: 6 experimentos listos para ejecutar

## 📁 Estructura del Proyecto

```
SnoopyOS/
├── simulador/
│   ├── include/          # Archivos de cabecera (.h)
│   │   ├── pcb.h
│   │   ├── planificador.h
│   │   ├── memoria.h
│   │   ├── sincronizacion.h
│   │   ├── problemas_clasicos.h
│   │   └── cli.h
│   ├── src/              # Implementaciones (.cpp)
│   │   ├── main.cpp
│   │   ├── planificador.cpp
│   │   ├── memoria.cpp
│   │   ├── sincronizacion.cpp
│   │   ├── problemas_clasicos.cpp
│   │   └── cli.cpp
│   ├── bin/              # Ejecutables
│   ├── build/            # Archivos objeto
│   ├── Makefile
│   ├── compilar.ps1      # Script de compilación Windows
│   └── ejecutar.ps1      # Script de ejecución Windows
└── README.md
```

## 🚀 Compilación y Ejecución

### Windows (PowerShell)
```powershell
cd simulador
.\compilar.ps1
.\ejecutar.ps1
```

### Linux/macOS
```bash
cd simulador
make
./bin/simulador
```

## 🎮 Comandos Disponibles

### Gestión de Procesos
| Comando | Descripción |
|---------|-------------|
| `new` | Crea un nuevo proceso con ráfagas aleatorias |
| `ps` | Muestra estado de todos los procesos |
| `tick` | Avanza un tick de reloj |
| `run <n>` | Ejecuta n ticks automáticamente |
| `kill <id>` | Termina el proceso especificado |
| `suspend <id>` | Suspende un proceso manualmente |
| `resume <id>` | Reanuda un proceso suspendido |

### Planificación
| Comando | Descripción |
|---------|-------------|
| `sched rr` | Cambia a Round Robin |
| `sched sjf` | Cambia a Shortest Job First |

### Gestión de Memoria
| Comando | Descripción |
|---------|-------------|
| `mem` | Muestra estado de memoria con visualización ASCII |
| `memc` | Muestra estado con colores (verde=hit, rojo=miss) |
| `memalg <tipo>` | Cambia algoritmo (fifo\|lru\|pff) |
| `access <id> <dir>` | Accede a dirección virtual del proceso |

### Sincronización
| Comando | Descripción |
|---------|-------------|
| `produce <id>` | Produce un elemento (problema productor-consumidor) |
| `consume <id>` | Consume un elemento del buffer |
| `buffer` | Muestra estado del buffer compartido |

### Problemas Clásicos
| Comando | Descripción |
|---------|-------------|
| `filosofos crear <n>` | Crea mesa con N filósofos (3-10) |
| `filosofos piensa <id>` | Filósofo ID piensa |
| `filosofos come <id>` | Filósofo ID intenta comer |
| `filosofos termina <id>` | Filósofo ID termina de comer |
| `filosofos estado` | Muestra estado de la mesa |
| `lectores lee <id>` | Lector ID comienza a leer |
| `lectores termina_leer <id>` | Lector ID termina |
| `lectores escribe <id>` | Escritor ID comienza |
| `lectores termina_escribir <id>` | Escritor ID termina |
| `lectores estado` | Muestra estado del recurso |

### Estadísticas
| Comando | Descripción |
|---------|-------------|
| `stats` | Muestra estadísticas completas del sistema |
| `fairness` | Análisis de equidad del planificador |

### Sistema
| Comando | Descripción |
|---------|-------------|
| `help` | Muestra todos los comandos |
| `exit` | Sale del simulador |

## 📊 Ejemplo de Uso

```
SnoopyOS> new
Proceso P1 creado con 3 rafagas y 5 paginas.

SnoopyOS> new
Proceso P2 creado con 4 rafagas y 4 paginas.

SnoopyOS> ps
Planificador: Round Robin (quantum=3)

ID | Estado      | Rafaga | T.Llegada | T.Espera | T.Rest
---|-------------|--------|-----------|----------|-------
 1 | EJECUTANDO  |      1 |         0 |        0 |     12
 2 | LISTO       |      1 |         0 |        0 |     15

SnoopyOS> sched sjf
Algoritmo cambiado a SJF (Shortest Job First)

SnoopyOS> run 10
[Ejecutando 10 ticks...]

SnoopyOS> mem
=== ESTADO DE LA MEMORIA ===
┌────┬────┬────┬────┬────┬────┬────┬────┐
│  0 │  1 │  2 │  3 │  4 │  5 │  6 │  7 │
├────┼────┼────┼────┼────┼────┼────┼────┤
│ P1 │ P1 │ P2 │ -- │ -- │ -- │ -- │ -- │
└────┴────┴────┴────┴────┴────┴────┴────┘
Uso de memoria: 3/8 marcos (37.5%)

SnoopyOS> stats
=== ESTADISTICAS DEL SISTEMA ===
Tiempo global: 15
Procesos terminados: 1
Tasa de aciertos: 68.0%
...

SnoopyOS> exit
```

## 🔧 Componentes Implementados

### 1. Gestión de Procesos
- PCB completo con 6 estados
- Round Robin (quantum = 3)
- SJF (Shortest Job First)
- Suspensión/Reanudación manual

### 2. Memoria Virtual
- 8 marcos de memoria física
- Páginas de 256 bytes
- Algoritmos: FIFO, LRU y PFF (Page Fault Frequency)
- Visualización con colores (verde=hit, rojo=miss)
- Estadísticas: fallos, hits, tasa de aciertos

### 3. Sincronización
- Semáforos con wait/signal
- Mutex con lock/unlock
- Problema Productor-Consumidor
- Buffer compartido (capacidad 5)

### 4. Problemas Clásicos
- **Filósofos Comensales**: Prevención de deadlock con orden de tenedores
- **Lectores-Escritores**: Múltiples lectores, escritor exclusivo
- Detección de deadlock
- Visualización de estados con emojis

### 5. Entrada/Salida
- Impresora virtual con cola de prioridad
- Historial de impresiones
- 3 ticks por trabajo

### 6. Disco
- 100 cilindros simulados
- FCFS, SSTF, SCAN (elevator)
- Visualización ASCII del cabezal
- Estadísticas de movimientos

### 7. Heap Allocator
- Buddy System (potencias de 2)
- malloc/free dinámico
- Coalescencia automática
- Medición de fragmentación

### 8. Estadísticas
- Tiempo de espera/retorno
- Análisis de fairness (CV)
- Tasa de aciertos de memoria
- PFF (Page Fault Frequency)

### 9. Scripts de Experimentos
- 6 scripts predefinidos
- Comparativas de algoritmos
- Demostración de conceptos

## 👥 Autores

- **Sofía Vélez**
- **David Hernández**

**Universidad EAFIT**  
**Sistemas Operativos - 2025**
