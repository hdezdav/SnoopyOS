#include "disco.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>

PlanificadorDisco::PlanificadorDisco(int num_cil, AlgoritmoDisco alg)
    : num_cilindros(num_cil), posicion_cabezal(0), algoritmo(alg),
      direccion_scan(1), solicitudes_atendidas(0), movimientos_totales(0) {}

void PlanificadorDisco::agregar_solicitud(int id_proceso, int cilindro, int tiempo) {
    if (cilindro < 0 || cilindro >= num_cilindros) {
        std::cout << "Error: cilindro fuera de rango (0-" << num_cilindros-1 << ")" << std::endl;
        return;
    }
    cola.push_back(std::make_shared<SolicitudDisco>(id_proceso, cilindro, tiempo));
    std::cout << "Solicitud agregada: P" << id_proceso << " -> cilindro " << cilindro << std::endl;
}

void PlanificadorDisco::procesar_siguiente() {
    if (cola.empty()) {
        std::cout << "No hay solicitudes pendientes" << std::endl;
        return;
    }
    
    switch (algoritmo) {
        case AlgoritmoDisco::FCFS:
            procesar_fcfs();
            break;
        case AlgoritmoDisco::SSTF:
            procesar_sstf();
            break;
        case AlgoritmoDisco::SCAN:
            procesar_scan();
            break;
    }
}

void PlanificadorDisco::procesar_fcfs() {
    auto solicitud = cola.front();
    cola.pop_front();
    
    int movimiento = std::abs(solicitud->cilindro - posicion_cabezal);
    movimientos_totales += movimiento;
    posicion_cabezal = solicitud->cilindro;
    solicitudes_atendidas++;
    
    std::cout << "FCFS: Atendiendo P" << solicitud->id_proceso 
              << " en cilindro " << solicitud->cilindro 
              << " (movimiento: " << movimiento << ")" << std::endl;
}

void PlanificadorDisco::procesar_sstf() {
    // Buscar la solicitud más cercana
    auto mas_cercana = cola.begin();
    int distancia_min = std::abs((*mas_cercana)->cilindro - posicion_cabezal);
    
    for (auto it = cola.begin(); it != cola.end(); ++it) {
        int distancia = std::abs((*it)->cilindro - posicion_cabezal);
        if (distancia < distancia_min) {
            distancia_min = distancia;
            mas_cercana = it;
        }
    }
    
    auto solicitud = *mas_cercana;
    cola.erase(mas_cercana);
    
    movimientos_totales += distancia_min;
    posicion_cabezal = solicitud->cilindro;
    solicitudes_atendidas++;
    
    std::cout << "SSTF: Atendiendo P" << solicitud->id_proceso 
              << " en cilindro " << solicitud->cilindro 
              << " (movimiento: " << distancia_min << ")" << std::endl;
}

void PlanificadorDisco::procesar_scan() {
    // Buscar solicitudes en la dirección actual
    std::shared_ptr<SolicitudDisco> siguiente = nullptr;
    auto it_siguiente = cola.end();
    
    for (auto it = cola.begin(); it != cola.end(); ++it) {
        if (direccion_scan == 1) {
            // Hacia arriba
            if ((*it)->cilindro >= posicion_cabezal) {
                if (!siguiente || (*it)->cilindro < siguiente->cilindro) {
                    siguiente = *it;
                    it_siguiente = it;
                }
            }
        } else {
            // Hacia abajo
            if ((*it)->cilindro <= posicion_cabezal) {
                if (!siguiente || (*it)->cilindro > siguiente->cilindro) {
                    siguiente = *it;
                    it_siguiente = it;
                }
            }
        }
    }
    
    // Si no hay en la dirección actual, cambiar dirección
    if (!siguiente) {
        direccion_scan *= -1;
        std::cout << "SCAN: Cambiando direccion a " 
                  << (direccion_scan == 1 ? "arriba" : "abajo") << std::endl;
        
        for (auto it = cola.begin(); it != cola.end(); ++it) {
            if (direccion_scan == 1) {
                if ((*it)->cilindro >= posicion_cabezal) {
                    if (!siguiente || (*it)->cilindro < siguiente->cilindro) {
                        siguiente = *it;
                        it_siguiente = it;
                    }
                }
            } else {
                if ((*it)->cilindro <= posicion_cabezal) {
                    if (!siguiente || (*it)->cilindro > siguiente->cilindro) {
                        siguiente = *it;
                        it_siguiente = it;
                    }
                }
            }
        }
    }
    
    if (siguiente && it_siguiente != cola.end()) {
        int movimiento = std::abs(siguiente->cilindro - posicion_cabezal);
        movimientos_totales += movimiento;
        posicion_cabezal = siguiente->cilindro;
        solicitudes_atendidas++;
        
        std::cout << "SCAN: Atendiendo P" << siguiente->id_proceso 
                  << " en cilindro " << siguiente->cilindro 
                  << " (movimiento: " << movimiento << ")" << std::endl;
        
        cola.erase(it_siguiente);
    }
}

void PlanificadorDisco::mostrar_estado() const {
    std::string alg_nombre = (algoritmo == AlgoritmoDisco::FCFS) ? "FCFS" : 
                             (algoritmo == AlgoritmoDisco::SSTF) ? "SSTF" : "SCAN";
    std::cout << "\n=== ESTADO DEL DISCO ===\n"
              << "Algoritmo: " << alg_nombre << "\n"
              << "Posicion cabezal: " << posicion_cabezal << "\n"
              << "Solicitudes pendientes: " << cola.size() << std::endl;
    
    if (!cola.empty()) {
        std::cout << "\nCola de solicitudes:" << std::endl;
        for (const auto& sol : cola) {
            std::cout << "  P" << sol->id_proceso << " -> cilindro " << sol->cilindro << std::endl;
        }
    }
    
    // Visualización del disco
    std::cout << "\nVisualizacion del disco:" << std::endl;
    std::cout << "Cil |";
    for (int i = 0; i < num_cilindros; i += 10) {
        std::cout << std::setw(3) << i << " ";
    }
    std::cout << "\n    |";
    for (int i = 0; i < num_cilindros; ++i) {
        if (i == posicion_cabezal) {
            std::cout << "H";
        } else {
            bool hay_solicitud = false;
            for (const auto& sol : cola) {
                if (sol->cilindro == i) {
                    hay_solicitud = true;
                    break;
                }
            }
            std::cout << (hay_solicitud ? "*" : "-");
        }
    }
    std::cout << std::endl;
    
    // Estadísticas
    std::cout << "\nEstadisticas:" << std::endl;
    std::cout << "  Solicitudes atendidas: " << solicitudes_atendidas << std::endl;
    std::cout << "  Movimientos totales: " << movimientos_totales << std::endl;
    if (solicitudes_atendidas > 0) {
        float promedio = (float)movimientos_totales / solicitudes_atendidas;
        std::cout << "  Promedio por solicitud: " << std::fixed << std::setprecision(2) 
                  << promedio << " cilindros" << std::endl;
    }
}

void PlanificadorDisco::cambiar_algoritmo(AlgoritmoDisco alg) {
    algoritmo = alg;
    std::string nombre = (alg == AlgoritmoDisco::FCFS) ? "FCFS" : 
                         (alg == AlgoritmoDisco::SSTF) ? "SSTF" : "SCAN";
    std::cout << "Algoritmo de disco cambiado a: " << nombre << std::endl;
}
