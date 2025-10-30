#ifndef PCB_H
#define PCB_H

#include <vector>

// Enumeración para los estados de un proceso
enum class EstadoProceso {
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    TERMINADO
};

// Bloque de Control de Proceso (PCB)
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
    
    PCB(int _id, std::vector<int> _rafagas, int _tiempo_llegada, int _num_paginas = 4)
        : id(_id), estado(EstadoProceso::NUEVO), rafagas(_rafagas),
          rafaga_actual(0), tiempo_llegada(_tiempo_llegada),
          tiempo_espera(0), tiempo_retorno(0), tiempo_finalizacion(-1),
          num_paginas(_num_paginas) {}
};

#endif // PCB_H
