#ifndef CLI_H
#define CLI_H

#include "planificador.h"
#include "memoria.h"
#include "sincronizacion.h"
#include "problemas_clasicos.h"
#include "io.h"
#include "disco.h"
#include "heap.h"
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
    void comando_mem_color();
    void comando_memalg(const std::string& alg);
    void comando_stats();
    void comando_access(int id_proceso, int direccion);
    void comando_produce(int id_proceso);
    void comando_consume(int id_proceso);
    void comando_suspend(int id);
    void comando_resume(int id);
    void comando_sched(const std::string& alg);
    void comando_fairness();
    void comando_filosofos(const std::string& sub_cmd);
    void comando_lectores(const std::string& sub_cmd);
    void comando_print(int id, const std::string& texto, int prioridad);
    void comando_io_status();
    void comando_disk_request(int id, int cilindro);
    void comando_disk_run();
    void comando_disk_show();
    void comando_disk_alg(const std::string& alg);
    void comando_alloc(int id, int tamano);
    void comando_free(int ptr);
    void comando_heap_status();
    void mostrar_ayuda();
    void calcular_estadisticas();

    std::shared_ptr<Planificador> planificador;
    std::shared_ptr<GestorMemoria> memoria;
    std::shared_ptr<BufferCompartido> buffer_compartido;
    std::shared_ptr<FilosofosComenales> filosofos;
    std::shared_ptr<LectoresEscritores> lectores_escritores;
    std::shared_ptr<GestorIO> gestor_io;
    std::shared_ptr<PlanificadorDisco> disco;
    std::shared_ptr<BuddyAllocator> heap;
    std::vector<std::shared_ptr<PCB>> procesos_totales;
    int proximo_id;
    int tiempo_global;
};

#endif 
