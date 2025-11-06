#ifndef PCB_H
#define PCB_H

#include <vector>

// Enumeración para los estados de un proceso
enum class EstadoProceso {
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    SUSPENDIDO,  // Nuevo: Estado suspendido manualmente
    TERMINADO
};

// Bloque de Control de Proceso 
struct PCB {
    int id;
    EstadoProceso estado;
    std::vector<int> rafagas; // Ráfagas de CPU requeridas
    int rafaga_actual;       // Índice de la ráfaga actual
    int tiempo_llegada;
    int tiempo_espera;
    int tiempo_retorno;
    int tiempo_finalizacion;
    int num_paginas;         // Número de páginas del proceso
    
    // Nuevos campos para SJF
    int tiempo_restante_total;  // Suma de todas las ráfagas restantes
    int tiempo_en_rafaga;       // Tiempo ejecutado en la ráfaga actual
    
    PCB(int _id, std::vector<int> _rafagas, int _tiempo_llegada, int _num_paginas = 4)
        : id(_id), estado(EstadoProceso::NUEVO), rafagas(_rafagas),
          rafaga_actual(0), tiempo_llegada(_tiempo_llegada),
          tiempo_espera(0), tiempo_retorno(0), tiempo_finalizacion(-1),
          num_paginas(_num_paginas), tiempo_en_rafaga(0) {
        // Calcular tiempo restante total
        tiempo_restante_total = 0;
        for (int r : rafagas) {
            tiempo_restante_total += r;
        }
    }
    
    void actualizar_tiempo_restante() {
        tiempo_restante_total = 0;
        for (size_t i = rafaga_actual; i < rafagas.size(); i++) {
            if (i == rafaga_actual) {
                tiempo_restante_total += (rafagas[i] - tiempo_en_rafaga);
            } else {
                tiempo_restante_total += rafagas[i];
            }
        }
    }
};

#endif 
