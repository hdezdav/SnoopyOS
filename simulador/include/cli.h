#ifndef CLI_H
#define CLI_H

#include "planificador.h"
#include "memoria.h"
#include "sincronizacion.h"
#include <vector>
#include <string>
#include <memory>

class CLI {
public:
    CLI(std::shared_ptr<Planificador> planificador, std::shared_ptr<GestorMemoria> memoria);
    void iniciar();

private:
    void procesar_comando(const std::string& comando);
    void comando_new();
    void comando_ps();
    void comando_tick();
    void comando_kill(int id);
    void comando_run(int n);
    void comando_mem();
    void comando_stats();
    void comando_access(int id_proceso, int direccion);
    void comando_produce(int id_proceso);
    void comando_consume(int id_proceso);
    void comando_fairness();
    void mostrar_ayuda();
    void calcular_estadisticas();

    std::shared_ptr<Planificador> planificador;
    std::shared_ptr<GestorMemoria> memoria;
    std::shared_ptr<BufferCompartido> buffer_compartido;
    std::vector<std::shared_ptr<PCB>> procesos_totales;
    int proximo_id;
    int tiempo_global;
};

#endif // CLI_H
