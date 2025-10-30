# SnoopyOS
Simulador de un sistema operativo simple en C++.

## Estructura del Proyecto

- **simulador/** - Contiene todo el código fuente del simulador.
  - **include/** - Archivos de cabecera (.h).
  - **src/** - Archivos de implementación (.cpp).
  - **Makefile** - Para compilar el proyecto.

## Cómo Compilar y Ejecutar

1.  Navega a la carpeta `simulador`:
    ```bash
    cd simulador
    ```

2.  Usa `make` para compilar el proyecto. Esto creará un ejecutable en `simulador/bin/`.
    ```bash
    make
    ```

3.  Ejecuta el simulador:
    ```bash
    ./bin/simulador
    ```

## Comandos de la CLI

- `new`: Crea un nuevo proceso con ráfagas aleatorias y lo añade a la cola de listos.
- `ps`: Muestra el proceso en ejecución y los procesos en la cola de listos.
- `tick`: Avanza un tick de tiempo en el simulador.
- `run n`: Ejecuta `n` ticks de tiempo.
- `kill <id>`: (No implementado) Termina un proceso.
- `exit`: Cierra el simulador.
