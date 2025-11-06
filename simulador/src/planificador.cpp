#include "planificador.h"
#include <iostream>
#include <algorithm>

Planificador::Planificador(int quantum)
    : quantum_fijo(quantum), tiempo_quantum_actual(0), proceso_en_ejecucion(nullptr),
      algoritmo_actual(TipoPlanificador::ROUND_ROBIN) {}

void Planificador::agregar_proceso(std::shared_ptr<PCB> proceso) {
    proceso->estado = EstadoProceso::LISTO;
    cola_listos.push_back(proceso);
    
    // Si es SJF, ordenar la cola
    if (algoritmo_actual == TipoPlanificador::SJF) {
        ordenar_cola_sjf();
    }
}

void Planificador::tick() {
    // Si no hay proceso en ejecución, tomar el siguiente de la cola
    if (!proceso_en_ejecucion && !cola_listos.empty()) {
        proceso_en_ejecucion = cola_listos.front();
        cola_listos.pop_front();
        proceso_en_ejecucion->estado = EstadoProceso::EJECUTANDO;
        tiempo_quantum_actual = 0;
    }

    if (proceso_en_ejecucion) {
        // Incrementar quantum y ejecutar ráfaga
        tiempo_quantum_actual++;
        proceso_en_ejecucion->tiempo_en_rafaga++;
        proceso_en_ejecucion->rafagas[proceso_en_ejecucion->rafaga_actual]--;
        proceso_en_ejecucion->actualizar_tiempo_restante();

        // Si la ráfaga terminó
        if (proceso_en_ejecucion->rafagas[proceso_en_ejecucion->rafaga_actual] <= 0) {
            proceso_en_ejecucion->rafaga_actual++;
            proceso_en_ejecucion->tiempo_en_rafaga = 0;
            
            // Si no hay más ráfagas, el proceso terminó
            if (proceso_en_ejecucion->rafaga_actual >= proceso_en_ejecucion->rafagas.size()) {
                proceso_en_ejecucion->estado = EstadoProceso::TERMINADO;
                std::cout << "Proceso P" << proceso_en_ejecucion->id << " ha terminado." << std::endl;
                proceso_en_ejecucion = nullptr;
            } else { // Si no, el proceso pasa a bloqueado (simulado) y vuelve a listos
                std::cout << "Proceso P" << proceso_en_ejecucion->id << " completo una rafaga." << std::endl;
                proceso_en_ejecucion->estado = EstadoProceso::LISTO;
                cola_listos.push_back(proceso_en_ejecucion);
                
                // Si es SJF, reordenar
                if (algoritmo_actual == TipoPlanificador::SJF) {
                    ordenar_cola_sjf();
                }
                
                proceso_en_ejecucion = nullptr;
            }
        }
        // Si se acabó el quantum (solo para Round Robin)
        else if (algoritmo_actual == TipoPlanificador::ROUND_ROBIN && tiempo_quantum_actual >= quantum_fijo) {
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

void Planificador::suspender_proceso(int id) {
    // Buscar en proceso actual
    if (proceso_en_ejecucion && proceso_en_ejecucion->id == id) {
        proceso_en_ejecucion->estado = EstadoProceso::SUSPENDIDO;
        procesos_suspendidos.push_back(proceso_en_ejecucion);
        std::cout << "Proceso P" << id << " suspendido (estaba ejecutando)." << std::endl;
        proceso_en_ejecucion = nullptr;
        return;
    }
    
    // Buscar en cola de listos
    for (auto it = cola_listos.begin(); it != cola_listos.end(); ++it) {
        if ((*it)->id == id) {
            (*it)->estado = EstadoProceso::SUSPENDIDO;
            procesos_suspendidos.push_back(*it);
            cola_listos.erase(it);
            std::cout << "Proceso P" << id << " suspendido (estaba en cola)." << std::endl;
            return;
        }
    }
    
    std::cout << "Proceso P" << id << " no encontrado." << std::endl;
}

void Planificador::reanudar_proceso(int id) {
    for (auto it = procesos_suspendidos.begin(); it != procesos_suspendidos.end(); ++it) {
        if ((*it)->id == id) {
            (*it)->estado = EstadoProceso::LISTO;
            cola_listos.push_back(*it);
            procesos_suspendidos.erase(it);
            
            if (algoritmo_actual == TipoPlanificador::SJF) {
                ordenar_cola_sjf();
            }
            
            std::cout << "Proceso P" << id << " reanudado." << std::endl;
            return;
        }
    }
    
    std::cout << "Proceso P" << id << " no esta suspendido." << std::endl;
}

void Planificador::cambiar_algoritmo(TipoPlanificador tipo) {
    algoritmo_actual = tipo;
    
    if (tipo == TipoPlanificador::SJF) {
        std::cout << "Algoritmo cambiado a SJF (Shortest Job First)" << std::endl;
        ordenar_cola_sjf();
    } else {
        std::cout << "Algoritmo cambiado a Round Robin (quantum=" << quantum_fijo << ")" << std::endl;
    }
}

void Planificador::ordenar_cola_sjf() {
    // Convertir deque a vector para ordenar
    std::vector<std::shared_ptr<PCB>> vec(cola_listos.begin(), cola_listos.end());
    
    // Ordenar por tiempo restante (menor primero)
    std::sort(vec.begin(), vec.end(), 
        [](const std::shared_ptr<PCB>& a, const std::shared_ptr<PCB>& b) {
            return a->tiempo_restante_total < b->tiempo_restante_total;
        });
    
    // Reconstruir cola
    cola_listos.clear();
    for (auto& p : vec) {
        cola_listos.push_back(p);
    }
}
