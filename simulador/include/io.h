#ifndef IO_H
#define IO_H

#include <string>
#include <queue>
#include <vector>
#include <memory>
#include "sincronizacion.h"

// Cola de prioridad para solicitudes de I/O
struct SolicitudIO {
    int id_proceso;
    std::string tipo;      // "print", "read", "write"
    std::string datos;
    int prioridad;         // 1 = alta, 2 = media, 3 = baja
    int tiempo_llegada;
    
    SolicitudIO(int id, std::string t, std::string d, int p, int tllegada)
        : id_proceso(id), tipo(t), datos(d), prioridad(p), tiempo_llegada(tllegada) {}
};

// Comparador: menor prioridad = más urgente
struct ComparadorPrioridad {
    bool operator()(const std::shared_ptr<SolicitudIO>& a, 
                   const std::shared_ptr<SolicitudIO>& b) const {
        if (a->prioridad != b->prioridad) {
            return a->prioridad > b->prioridad; // Menor número = mayor prioridad
        }
        return a->tiempo_llegada > b->tiempo_llegada; // FCFS si misma prioridad
    }
};

// Impresora virtual
class Impresora {
public:
    Impresora();
    void agregar_trabajo(int id_proceso, const std::string& texto, int prioridad, int tiempo);
    void imprimir_siguiente();
    void mostrar_cola() const;
    bool esta_ocupada() const { return ocupada; }
    int trabajos_pendientes() const { return cola_impresion.size(); }
    
private:
    std::priority_queue<std::shared_ptr<SolicitudIO>, 
                       std::vector<std::shared_ptr<SolicitudIO>>,
                       ComparadorPrioridad> cola_impresion;
    bool ocupada;
    std::shared_ptr<SolicitudIO> trabajo_actual;
    int tiempo_restante;
    std::vector<std::string> historial;
};

// Gestor de I/O
class GestorIO {
public:
    GestorIO();
    void solicitar_impresion(int id_proceso, const std::string& texto, int prioridad, int tiempo);
    void solicitar_lectura(int id_proceso, const std::string& archivo, int prioridad, int tiempo);
    void solicitar_escritura(int id_proceso, const std::string& archivo, 
                            const std::string& datos, int prioridad, int tiempo);
    void tick();
    void mostrar_estado() const;
    void mostrar_estadisticas() const;
    
private:
    std::shared_ptr<Impresora> impresora;
    std::priority_queue<std::shared_ptr<SolicitudIO>,
                       std::vector<std::shared_ptr<SolicitudIO>>,
                       ComparadorPrioridad> cola_lectura;
    std::priority_queue<std::shared_ptr<SolicitudIO>,
                       std::vector<std::shared_ptr<SolicitudIO>>,
                       ComparadorPrioridad> cola_escritura;
    
    int solicitudes_completadas;
    int tiempo_espera_total;
    int solicitudes_totales;
};

#endif 
