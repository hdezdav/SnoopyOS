#include "sincronizacion.h"
#include <iostream>

// Semaforo 
Semaforo::Semaforo(int valor_inicial) : valor(valor_inicial) {}

void Semaforo::wait(std::shared_ptr<PCB> proceso) {
    valor--;
    if (valor < 0) {
        // Bloquear el proceso
        proceso->estado = EstadoProceso::BLOQUEADO;
        cola_bloqueados.push(proceso);
        std::cout << "  -> Proceso P" << proceso->id << " bloqueado en semaforo" << std::endl;
    }
}

std::shared_ptr<PCB> Semaforo::signal() {
    valor++;
    if (!cola_bloqueados.empty()) {
        // Desbloquear un proceso
        auto proceso = cola_bloqueados.front();
        cola_bloqueados.pop();
        proceso->estado = EstadoProceso::LISTO;
        std::cout << "  -> Proceso P" << proceso->id << " desbloqueado" << std::endl;
        return proceso;
    }
    return nullptr;
}

// Mutex 
Mutex::Mutex() : bloqueado(false), proceso_propietario(-1) {}

bool Mutex::lock(std::shared_ptr<PCB> proceso) {
    if (!bloqueado) {
        bloqueado = true;
        proceso_propietario = proceso->id;
        std::cout << "  -> Proceso P" << proceso->id << " adquirio el mutex" << std::endl;
        return true;
    }
    std::cout << "  -> Proceso P" << proceso->id << " bloqueado esperando mutex" << std::endl;
    return false;
}

void Mutex::unlock() {
    if (bloqueado) {
        std::cout << "  -> Proceso P" << proceso_propietario << " libero el mutex" << std::endl;
        bloqueado = false;
        proceso_propietario = -1;
    }
}

// Buffer Compartido
BufferCompartido::BufferCompartido(int capacidad)
    : capacidad_maxima(capacidad),
      mutex(1),           // Mutex binario
      lleno(0),           // Inicialmente 0 items producidos
      vacio(capacidad)    // Inicialmente capacidad espacios vacios
{}

void BufferCompartido::producir(int id_proceso, int item) {
    std::cout << "  [PRODUCTOR P" << id_proceso << "] Intentando producir item " << item << std::endl;
    
    if (esta_lleno()) {
        std::cout << "  -> Buffer lleno, productor debe esperar" << std::endl;
        return;
    }
    
    items.push(item);
    std::cout << "  -> Item " << item << " producido. Buffer: " << items.size() 
              << "/" << capacidad_maxima << std::endl;
}

int BufferCompartido::consumir(int id_proceso) {
    std::cout << "  [CONSUMIDOR P" << id_proceso << "] Intentando consumir" << std::endl;
    
    if (esta_vacio()) {
        std::cout << "  -> Buffer vacio, consumidor debe esperar" << std::endl;
        return -1;
    }
    
    int item = items.front();
    items.pop();
    std::cout << "  -> Item " << item << " consumido. Buffer: " << items.size() 
              << "/" << capacidad_maxima << std::endl;
    return item;
}

void BufferCompartido::mostrar_estado() const {
    std::cout << "\n=== BUFFER COMPARTIDO ===" << std::endl;
    std::cout << "Capacidad: " << capacidad_maxima << std::endl;
    std::cout << "Items actuales: " << items.size() << std::endl;
    std::cout << "Estado: ";
    if (esta_lleno()) std::cout << "LLENO";
    else if (esta_vacio()) std::cout << "VACIO";
    else std::cout << "PARCIAL";
    std::cout << std::endl;
}
