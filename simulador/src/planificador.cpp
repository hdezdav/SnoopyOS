#include "planificador.h"
#include <iostream>

Planificador::Planificador(int quantum)
    : quantum_fijo(quantum), tiempo_quantum_actual(0), proceso_en_ejecucion(nullptr) {}

void Planificador::agregar_proceso(std::shared_ptr<PCB> proceso) {
    proceso->estado = EstadoProceso::LISTO;
    cola_listos.push_back(proceso);
}

void Planificador::tick() {
    // Si no hay proceso en ejecución, tomar el siguiente de la cola de listos
    if (!proceso_en_ejecucion && !cola_listos.empty()) {
        proceso_en_ejecucion = cola_listos.front();
        cola_listos.pop_front();
        proceso_en_ejecucion->estado = EstadoProceso::EJECUTANDO;
        tiempo_quantum_actual = 0;
    }

    if (proceso_en_ejecucion) {
        // Incrementar quantum y ejecutar ráfaga
        tiempo_quantum_actual++;
        proceso_en_ejecucion->rafagas[proceso_en_ejecucion->rafaga_actual]--;

        // Si la ráfaga terminó
        if (proceso_en_ejecucion->rafagas[proceso_en_ejecucion->rafaga_actual] <= 0) {
            proceso_en_ejecucion->rafaga_actual++;
            // Si no hay más ráfagas, el proceso terminó
            if (proceso_en_ejecucion->rafaga_actual >= proceso_en_ejecucion->rafagas.size()) {
                proceso_en_ejecucion->estado = EstadoProceso::TERMINADO;
                std::cout << "Proceso P" << proceso_en_ejecucion->id << " ha terminado." << std::endl;
                proceso_en_ejecucion = nullptr;
            } else { // Si no, el proceso pasa a bloqueado (simulado) y vuelve a listos
                 std::cout << "Proceso P" << proceso_en_ejecucion->id << " completo una rafaga." << std::endl;
                proceso_en_ejecucion->estado = EstadoProceso::LISTO;
                cola_listos.push_back(proceso_en_ejecucion);
                proceso_en_ejecucion = nullptr;
            }
        }
        // Si se acabó el quantum
        else if (tiempo_quantum_actual >= quantum_fijo) {
            std::cout << "Quantum agotado para proceso P" << proceso_en_ejecucion->id << std::endl;
            proceso_en_ejecucion->estado = EstadoProceso::LISTO;
            cola_listos.push_back(proceso_en_ejecucion);
            proceso_en_ejecucion = nullptr;
        }
    } else {
        std::cout << "CPU inactiva." << std::endl;
    }
    
    // Actualizar tiempos de espera
    actualizar_tiempos_espera();
}

void Planificador::terminar_proceso_actual() {
    if (proceso_en_ejecucion) {
        proceso_en_ejecucion->estado = EstadoProceso::TERMINADO;
        proceso_en_ejecucion = nullptr;
    }
}

void Planificador::actualizar_tiempos_espera() {
    // Incrementar tiempo de espera para procesos en cola de listos
    for (auto& pcb : cola_listos) {
        pcb->tiempo_espera++;
    }
}

std::shared_ptr<PCB> Planificador::obtener_proceso_actual() {
    return proceso_en_ejecucion;
}

const std::deque<std::shared_ptr<PCB>>& Planificador::obtener_cola_listos() const {
    return cola_listos;
}
