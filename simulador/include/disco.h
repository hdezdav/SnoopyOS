#ifndef DISCO_H
#define DISCO_H

#include <vector>
#include <deque>
#include <string>
#include <memory>

enum class AlgoritmoDisco {
    FCFS,   
    SSTF,   
    SCAN   
};

struct SolicitudDisco {
    int id_proceso;
    int cilindro;
    int tiempo_llegada;
    
    SolicitudDisco(int id, int cil, int t) 
        : id_proceso(id), cilindro(cil), tiempo_llegada(t) {}
};

class PlanificadorDisco {
public:
    PlanificadorDisco(int num_cilindros, AlgoritmoDisco alg);
    
    void agregar_solicitud(int id_proceso, int cilindro, int tiempo);
    void procesar_siguiente();
    void mostrar_estado() const;
    void cambiar_algoritmo(AlgoritmoDisco alg);
    
    int obtener_posicion_cabezal() const { return posicion_cabezal; }
    int obtener_solicitudes_pendientes() const { return cola.size(); }
    
private:
    void procesar_fcfs();
    void procesar_sstf();
    void procesar_scan();
    
    int num_cilindros;
    int posicion_cabezal;
    AlgoritmoDisco algoritmo;
    std::deque<std::shared_ptr<SolicitudDisco>> cola;
    int direccion_scan; // 1 = arriba, -1 = abajo
    int solicitudes_atendidas;
    int movimientos_totales;
};

#endif 
