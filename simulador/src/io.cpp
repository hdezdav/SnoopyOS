#include "io.h"
#include <iostream>
#include <iomanip>

// Impresora

Impresora::Impresora() : ocupada(false), trabajo_actual(nullptr), tiempo_restante(0) {}

void Impresora::agregar_trabajo(int id_proceso, const std::string& texto, int prioridad, int tiempo) {
    auto solicitud = std::make_shared<SolicitudIO>(id_proceso, "print", texto, prioridad, tiempo);
    cola_impresion.push(solicitud);
    std::cout << "Trabajo de impresion agregado: P" << id_proceso 
              << " (prioridad " << prioridad << ")" << std::endl;
}

void Impresora::imprimir_siguiente() {
    if (ocupada && --tiempo_restante <= 0) {
        std::cout << "\n=== IMPRESION COMPLETADA ===\nProceso P" << trabajo_actual->id_proceso 
                  << "\nTexto: \"" << trabajo_actual->datos << "\"\n===========================\n" << std::endl;
        historial.push_back("P" + std::to_string(trabajo_actual->id_proceso) + ": " + trabajo_actual->datos);
        ocupada = false;
        trabajo_actual = nullptr;
    }
    if (!ocupada && !cola_impresion.empty()) {
        trabajo_actual = cola_impresion.top();
        cola_impresion.pop();
        tiempo_restante = 3;
        ocupada = true;
        std::cout << "Impresora: Iniciando trabajo de P" << trabajo_actual->id_proceso << std::endl;
    }
}

void Impresora::mostrar_cola() const {
    std::cout << "\n=== COLA DE IMPRESION ===" << std::endl;
    std::cout << "Estado: " << (ocupada ? "OCUPADA" : "LIBRE") << std::endl;
    
    if (ocupada) {
        std::cout << "Imprimiendo: P" << trabajo_actual->id_proceso 
                  << " (" << tiempo_restante << " ticks restantes)" << std::endl;
    }
    
    std::cout << "Trabajos en cola: " << cola_impresion.size() << std::endl;
    
    if (!historial.empty()) {
        std::cout << "\nHistorial (ultimas 5):" << std::endl;
        int inicio = std::max(0, (int)historial.size() - 5);
        for (int i = inicio; i < (int)historial.size(); ++i) {
            std::cout << "  " << i+1 << ". " << historial[i] << std::endl;
        }
    }
}

// GestorIO 

GestorIO::GestorIO() 
    : impresora(std::make_shared<Impresora>()), solicitudes_completadas(0),
      tiempo_espera_total(0), solicitudes_totales(0) {}

void GestorIO::solicitar_impresion(int id_proceso, const std::string& texto, int prioridad, int tiempo) {
    impresora->agregar_trabajo(id_proceso, texto, prioridad, tiempo);
    solicitudes_totales++;
}

void GestorIO::solicitar_lectura(int id_proceso, const std::string& archivo, int prioridad, int tiempo) {
    cola_lectura.push(std::make_shared<SolicitudIO>(id_proceso, "read", archivo, prioridad, tiempo));
    std::cout << "Solicitud de lectura agregada: P" << id_proceso << " (" << archivo << ")" << std::endl;
    solicitudes_totales++;
}

void GestorIO::solicitar_escritura(int id_proceso, const std::string& archivo, 
                                   const std::string& datos, int prioridad, int tiempo) {
    cola_escritura.push(std::make_shared<SolicitudIO>(id_proceso, "write", archivo + ":" + datos, prioridad, tiempo));
    std::cout << "Solicitud de escritura agregada: P" << id_proceso << " (" << archivo << ")" << std::endl;
    solicitudes_totales++;
}

void GestorIO::tick() {
    impresora->imprimir_siguiente();
    if (!cola_lectura.empty()) {
        auto solicitud = cola_lectura.top();
        cola_lectura.pop();
        std::cout << "Lectura completada: P" << solicitud->id_proceso << " leyo " << solicitud->datos << std::endl;
        solicitudes_completadas++;
    }
    if (!cola_escritura.empty()) {
        auto solicitud = cola_escritura.top();
        cola_escritura.pop();
        std::cout << "Escritura completada: P" << solicitud->id_proceso << std::endl;
        solicitudes_completadas++;
    }
}

void GestorIO::mostrar_estado() const {
    std::cout << "\n=== ESTADO DE I/O ===\n";
    impresora->mostrar_cola();
    std::cout << "\nCola de lectura: " << cola_lectura.size() << " pendientes\n"
              << "Cola de escritura: " << cola_escritura.size() << " pendientes" << std::endl;
}

void GestorIO::mostrar_estadisticas() const {
    std::cout << "\n=== ESTADISTICAS DE I/O ===\n"
              << "Solicitudes totales: " << solicitudes_totales << "\n"
              << "Completadas: " << solicitudes_completadas << "\n"
              << "Pendientes: " << (solicitudes_totales - solicitudes_completadas) << std::endl;
    if (solicitudes_completadas > 0) {
        float promedio = (float)tiempo_espera_total / solicitudes_completadas;
        std::cout << "Tiempo espera promedio: " << std::fixed << std::setprecision(2) 
                  << promedio << " ticks" << std::endl;
    }
}
