#include "cli.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>

CLI::CLI(std::shared_ptr<Planificador> planificador, std::shared_ptr<GestorMemoria> memoria)
    : planificador(planificador), memoria(memoria), proximo_id(1), tiempo_global(0),
      buffer_compartido(std::make_shared<BufferCompartido>(5)) {
    std::srand(std::time(nullptr)); // Inicializar semilla para números aleatorios
}

void CLI::iniciar() {
    std::string linea;
    std::cout << "==================================" << std::endl;
    std::cout << "  SnoopyOS - Simulador de SO" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Escribe 'help' para ver comandos" << std::endl;
    std::cout << "==================================" << std::endl;
    
    while (true) {
        std::cout << "\nSnoopyOS> ";
        std::getline(std::cin, linea);
        if (linea == "exit" || linea == "quit") {
            calcular_estadisticas();
            std::cout << "Saliendo del simulador..." << std::endl;
            break;
        }
        procesar_comando(linea);
    }
}

void CLI::procesar_comando(const std::string& comando) {
    std::stringstream ss(comando);
    std::string cmd;
    ss >> cmd;

    if (cmd == "new") {
        comando_new();
    } else if (cmd == "ps") {
        comando_ps();
    } else if (cmd == "tick") {
        comando_tick();
    } else if (cmd == "kill") {
        int id;
        if (ss >> id) {
            comando_kill(id);
        } else {
            std::cout << "Uso: kill <id>" << std::endl;
        }
    } else if (cmd == "run") {
        int n;
        if (ss >> n) {
            comando_run(n);
        } else {
            std::cout << "Uso: run <n>" << std::endl;
        }
    } else if (cmd == "mem") {
        comando_mem();
    } else if (cmd == "stats") {
        comando_stats();
    } else if (cmd == "access") {
        int id, dir;
        if (ss >> id >> dir) {
            comando_access(id, dir);
        } else {
            std::cout << "Uso: access <id_proceso> <direccion>" << std::endl;
        }
    } else if (cmd == "produce") {
        int id;
        if (ss >> id) {
            comando_produce(id);
        } else {
            std::cout << "Uso: produce <id_proceso>" << std::endl;
        }
    } else if (cmd == "consume") {
        int id;
        if (ss >> id) {
            comando_consume(id);
        } else {
            std::cout << "Uso: consume <id_proceso>" << std::endl;
        }
    } else if (cmd == "buffer") {
        buffer_compartido->mostrar_estado();
    } else if (cmd == "fairness") {
        comando_fairness();
    } else if (cmd == "help") {
        mostrar_ayuda();
    } else if (cmd != "") {
        std::cout << "Comando desconocido: " << cmd << std::endl;
        std::cout << "Escribe 'help' para ver los comandos disponibles." << std::endl;
    }
}

void CLI::comando_new() {
    // Crea un proceso con 1 a 3 ráfagas de 2 a 5 ticks cada una.
    int num_rafagas = std::rand() % 3 + 1;
    std::vector<int> rafagas;
    for (int i = 0; i < num_rafagas; ++i) {
        rafagas.push_back(std::rand() % 4 + 2);
    }
    
    int num_paginas = std::rand() % 5 + 3; // 3-7 páginas
    auto pcb = std::make_shared<PCB>(proximo_id, rafagas, tiempo_global, num_paginas);
    procesos_totales.push_back(pcb);
    planificador->agregar_proceso(pcb);
    memoria->crear_tabla_paginas(proximo_id, num_paginas);
    
    std::cout << "Proceso P" << pcb->id << " creado con " << num_rafagas 
              << " rafagas y " << num_paginas << " paginas." << std::endl;
    proximo_id++;
}

void CLI::comando_ps() {
    std::cout << "\n=== ESTADO DE PROCESOS ===" << std::endl;
    std::cout << "ID | Estado      | Rafaga | T.Llegada | T.Espera" << std::endl;
    std::cout << "---|-------------|--------|-----------|----------" << std::endl;
    
    auto actual = planificador->obtener_proceso_actual();
    if (actual) {
        std::cout << std::setw(2) << actual->id << " | EJECUTANDO  | "
                  << std::setw(6) << actual->rafaga_actual + 1 << " | "
                  << std::setw(9) << actual->tiempo_llegada << " | "
                  << std::setw(8) << actual->tiempo_espera << std::endl;
    }

    const auto& cola_listos = planificador->obtener_cola_listos();
    for (const auto& pcb : cola_listos) {
        std::cout << std::setw(2) << pcb->id << " | LISTO       | "
                  << std::setw(6) << pcb->rafaga_actual + 1 << " | "
                  << std::setw(9) << pcb->tiempo_llegada << " | "
                  << std::setw(8) << pcb->tiempo_espera << std::endl;
    }
    
    // Mostrar procesos bloqueados
    for (const auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::BLOQUEADO) {
            std::cout << std::setw(2) << pcb->id << " | BLOQUEADO   | "
                      << std::setw(6) << pcb->rafaga_actual + 1 << " | "
                      << std::setw(9) << pcb->tiempo_llegada << " | "
                      << std::setw(8) << pcb->tiempo_espera << std::endl;
        }
    }
    
    // Mostrar procesos terminados
    for (const auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::TERMINADO) {
            std::cout << std::setw(2) << pcb->id << " | TERMINADO   | "
                      << std::setw(6) << "-" << " | "
                      << std::setw(9) << pcb->tiempo_llegada << " | "
                      << std::setw(8) << pcb->tiempo_espera << std::endl;
        }
    }
}

void CLI::comando_tick() {
    std::cout << "--- Tick " << ++tiempo_global << " ---" << std::endl;
    planificador->tick();
}

void CLI::comando_kill(int id) {
    bool encontrado = false;
    
    // Buscar en la cola de listos
    auto& cola = const_cast<std::deque<std::shared_ptr<PCB>>&>(planificador->obtener_cola_listos());
    for (auto it = cola.begin(); it != cola.end(); ++it) {
        if ((*it)->id == id) {
            (*it)->estado = EstadoProceso::TERMINADO;
            (*it)->tiempo_finalizacion = tiempo_global;
            cola.erase(it);
            std::cout << "Proceso P" << id << " terminado (estaba en cola de listos)." << std::endl;
            encontrado = true;
            break;
        }
    }
    
    // Verificar si está en ejecución
    if (!encontrado) {
        auto actual = planificador->obtener_proceso_actual();
        if (actual && actual->id == id) {
            planificador->terminar_proceso_actual();
            std::cout << "Proceso P" << id << " terminado (estaba en ejecucion)." << std::endl;
            encontrado = true;
        }
    }
    
    if (!encontrado) {
        std::cout << "Proceso P" << id << " no encontrado o ya terminado." << std::endl;
    }
}

void CLI::comando_run(int n) {
    std::cout << "\n=== Ejecutando " << n << " ticks ===" << std::endl;
    for (int i = 0; i < n; ++i) {
        comando_tick();
    }
    std::cout << "=== Finalizados " << n << " ticks ===" << std::endl;
}

void CLI::comando_mem() {
    memoria->mostrar_estado();
}

void CLI::comando_stats() {
    std::cout << "\n=== ESTADISTICAS GLOBALES ===" << std::endl;
    std::cout << "Tiempo global: " << tiempo_global << std::endl;
    std::cout << "Procesos totales: " << procesos_totales.size() << std::endl;
    
    int terminados = 0;
    float t_espera_total = 0, t_retorno_total = 0;
    
    for (const auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::TERMINADO) {
            terminados++;
            t_espera_total += pcb->tiempo_espera;
            t_retorno_total += pcb->tiempo_retorno;
        }
    }
    
    std::cout << "Procesos terminados: " << terminados << std::endl;
    if (terminados > 0) {
        std::cout << "Tiempo espera promedio: " << std::fixed << std::setprecision(2) 
                  << (t_espera_total / terminados) << std::endl;
        std::cout << "Tiempo retorno promedio: " << std::fixed << std::setprecision(2) 
                  << (t_retorno_total / terminados) << std::endl;
    }
    
    memoria->mostrar_estadisticas();
}

void CLI::comando_access(int id_proceso, int direccion) {
    memoria->acceder_memoria(id_proceso, direccion);
}

void CLI::comando_produce(int id_proceso) {
    int item = std::rand() % 100;
    buffer_compartido->producir(id_proceso, item);
}

void CLI::comando_consume(int id_proceso) {
    buffer_compartido->consumir(id_proceso);
}

void CLI::comando_fairness() {
    std::cout << "\n=== ANALISIS DE FAIRNESS ===" << std::endl;
    
    if (procesos_totales.empty()) {
        std::cout << "No hay procesos para analizar." << std::endl;
        return;
    }
    
    // Calcular tiempos de espera
    std::vector<int> tiempos_espera;
    int total_espera = 0;
    int procesos_analizados = 0;
    
    for (const auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::TERMINADO || 
            pcb->estado == EstadoProceso::EJECUTANDO ||
            pcb->estado == EstadoProceso::LISTO) {
            tiempos_espera.push_back(pcb->tiempo_espera);
            total_espera += pcb->tiempo_espera;
            procesos_analizados++;
        }
    }
    
    if (procesos_analizados == 0) {
        std::cout << "No hay procesos completados o en ejecucion." << std::endl;
        return;
    }
    
    float promedio = (float)total_espera / procesos_analizados;
    
    // Calcular desviación estándar
    float varianza = 0;
    for (int tiempo : tiempos_espera) {
        varianza += std::pow(tiempo - promedio, 2);
    }
    varianza /= procesos_analizados;
    float desv_std = std::sqrt(varianza);
    
    // Calcular coeficiente de variación (CV)
    float cv = (promedio > 0) ? (desv_std / promedio) : 0;
    
    std::cout << "Procesos analizados: " << procesos_analizados << std::endl;
    std::cout << "Tiempo espera promedio: " << std::fixed << std::setprecision(2) << promedio << std::endl;
    std::cout << "Desviacion estandar: " << std::fixed << std::setprecision(2) << desv_std << std::endl;
    std::cout << "Coeficiente de variacion: " << std::fixed << std::setprecision(3) << cv << std::endl;
    
    // Interpretar fairness
    std::cout << "\nInterpretacion de Fairness: ";
    if (cv < 0.3) {
        std::cout << "EXCELENTE (muy justo)" << std::endl;
    } else if (cv < 0.5) {
        std::cout << "BUENO (justo)" << std::endl;
    } else if (cv < 0.8) {
        std::cout << "MODERADO (algo de variacion)" << std::endl;
    } else {
        std::cout << "POBRE (alta variacion)" << std::endl;
    }
    
    // Mostrar distribución
    std::cout << "\nDistribucion de tiempos de espera:" << std::endl;
    for (const auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::TERMINADO || 
            pcb->estado == EstadoProceso::EJECUTANDO ||
            pcb->estado == EstadoProceso::LISTO) {
            std::cout << "  P" << pcb->id << ": " << pcb->tiempo_espera << " ticks ";
            
            // Barra visual
            int barras = (int)((float)pcb->tiempo_espera / promedio * 10);
            std::cout << "[";
            for (int i = 0; i < barras && i < 20; ++i) {
                std::cout << "=";
            }
            std::cout << "]" << std::endl;
        }
    }
}

void CLI::mostrar_ayuda() {
    std::cout << "\n=== COMANDOS DISPONIBLES ===" << std::endl;
    std::cout << "\n[Gestion de Procesos]" << std::endl;
    std::cout << "  new              - Crea un nuevo proceso" << std::endl;
    std::cout << "  ps               - Muestra estado de procesos" << std::endl;
    std::cout << "  tick             - Avanza un tick de reloj" << std::endl;
    std::cout << "  run <n>          - Ejecuta n ticks" << std::endl;
    std::cout << "  kill <id>        - Termina el proceso con ID" << std::endl;
    
    std::cout << "\n[Gestion de Memoria]" << std::endl;
    std::cout << "  mem              - Muestra estado de memoria" << std::endl;
    std::cout << "  access <id> <dir> - Accede a memoria virtual" << std::endl;
    
    std::cout << "\n[Sincronizacion]" << std::endl;
    std::cout << "  produce <id>     - Produce un item (problema prod-cons)" << std::endl;
    std::cout << "  consume <id>     - Consume un item (problema prod-cons)" << std::endl;
    std::cout << "  buffer           - Muestra estado del buffer compartido" << std::endl;
    
    std::cout << "\n[Estadisticas]" << std::endl;
    std::cout << "  stats            - Muestra estadisticas completas" << std::endl;
    std::cout << "  fairness         - Analiza fairness del planificador" << std::endl;
    
    std::cout << "\n[Sistema]" << std::endl;
    std::cout << "  help             - Muestra esta ayuda" << std::endl;
    std::cout << "  exit             - Sale del simulador" << std::endl;
}

void CLI::calcular_estadisticas() {
    // Actualizar tiempos de espera y retorno
    for (auto& pcb : procesos_totales) {
        if (pcb->estado == EstadoProceso::TERMINADO && pcb->tiempo_finalizacion > 0) {
            pcb->tiempo_retorno = pcb->tiempo_finalizacion - pcb->tiempo_llegada;
        }
    }
}
