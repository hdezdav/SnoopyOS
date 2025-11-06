#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "pcb.h"
#include <deque>
#include <memory>
#include <vector>

enum class TipoPlanificador {
    ROUND_ROBIN,
    SJF  
};

class Planificador {
public:
    Planificador(int quantum);
    void agregar_proceso(std::shared_ptr<PCB> proceso);
    void tick();
    std::shared_ptr<PCB> obtener_proceso_actual();
    const std::deque<std::shared_ptr<PCB>>& obtener_cola_listos() const;
    void terminar_proceso_actual();
    void actualizar_tiempos_espera();
    
    // Nuevos métodos
    void suspender_proceso(int id);
    void reanudar_proceso(int id);
    void cambiar_algoritmo(TipoPlanificador tipo);
    TipoPlanificador obtener_algoritmo() const { return algoritmo_actual; }
    const std::vector<std::shared_ptr<PCB>>& obtener_suspendidos() const { return procesos_suspendidos; }

private:
    void ordenar_cola_sjf();  // Ordena la cola según tiempo restante
    
    std::deque<std::shared_ptr<PCB>> cola_listos;
    std::vector<std::shared_ptr<PCB>> procesos_suspendidos;
    std::shared_ptr<PCB> proceso_en_ejecucion;
    int quantum_fijo;
    int tiempo_quantum_actual;
    TipoPlanificador algoritmo_actual;
};

#endif 
