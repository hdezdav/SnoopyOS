#ifndef SINCRONIZACION_H
#define SINCRONIZACION_H

#include <queue>
#include <memory>
#include "pcb.h"

// Semáforo para sincronización
class Semaforo {
public:
    Semaforo(int valor_inicial);
    
    void wait(std::shared_ptr<PCB> proceso);
    std::shared_ptr<PCB> signal();
    
    int obtener_valor() const { return valor; }
    bool hay_procesos_bloqueados() const { return !cola_bloqueados.empty(); }
    int procesos_bloqueados() const { return cola_bloqueados.size(); }
    
private:
    int valor;
    std::queue<std::shared_ptr<PCB>> cola_bloqueados;
};

// Mutex para exclusión mutua
class Mutex {
public:
    Mutex();
    
    bool lock(std::shared_ptr<PCB> proceso);
    void unlock();
    
    bool esta_bloqueado() const { return bloqueado; }
    int obtener_propietario() const { return proceso_propietario; }
    
private:
    bool bloqueado;
    int proceso_propietario;
};

// Buffer compartido para problema productor, consumidor
class BufferCompartido {
public:
    BufferCompartido(int capacidad);
    
    void producir(int id_proceso, int item);
    int consumir(int id_proceso);
    
    bool esta_lleno() const { return items.size() >= capacidad_maxima; }
    bool esta_vacio() const { return items.empty(); }
    int obtener_size() const { return items.size(); }
    
    void mostrar_estado() const;
    
private:
    std::queue<int> items;
    size_t capacidad_maxima;
    Semaforo mutex;
    Semaforo lleno;
    Semaforo vacio;
};

#endif 
