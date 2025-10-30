#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "pcb.h"
#include <deque>
#include <memory>

class Planificador {
public:
    Planificador(int quantum);
    void agregar_proceso(std::shared_ptr<PCB> proceso);
    void tick();
    std::shared_ptr<PCB> obtener_proceso_actual();
    const std::deque<std::shared_ptr<PCB>>& obtener_cola_listos() const;
    void terminar_proceso_actual();
    void actualizar_tiempos_espera();

private:
    std::deque<std::shared_ptr<PCB>> cola_listos;
    std::shared_ptr<PCB> proceso_en_ejecucion;
    int quantum_fijo;
    int tiempo_quantum_actual;
};

#endif // PLANIFICADOR_H
