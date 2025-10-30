#include "memoria.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// ============ TablaPaginas ============
TablaPaginas::TablaPaginas(int id, int num_paginas) 
    : id_proceso(id) {
    entradas.resize(num_paginas);
    for (auto& entrada : entradas) {
        entrada.numero_marco = -1;
        entrada.presente = false;
        entrada.referenciado = false;
        entrada.tiempo_carga = -1;
    }
}

int TablaPaginas::obtener_marco(int num_pagina) {
    if (num_pagina >= 0 && num_pagina < entradas.size() && entradas[num_pagina].presente) {
        return entradas[num_pagina].numero_marco;
    }
    return -1;
}

void TablaPaginas::cargar_pagina(int num_pagina, int num_marco, int tiempo) {
    if (num_pagina >= 0 && num_pagina < entradas.size()) {
        entradas[num_pagina].numero_marco = num_marco;
        entradas[num_pagina].presente = true;
        entradas[num_pagina].tiempo_carga = tiempo;
        entradas[num_pagina].referenciado = true;
    }
}

void TablaPaginas::invalidar_pagina(int num_pagina) {
    if (num_pagina >= 0 && num_pagina < entradas.size()) {
        entradas[num_pagina].presente = false;
        entradas[num_pagina].numero_marco = -1;
    }
}

void TablaPaginas::marcar_referenciado(int num_pagina) {
    if (num_pagina >= 0 && num_pagina < entradas.size()) {
        entradas[num_pagina].referenciado = true;
    }
}

// ============ GestorMemoria ============
GestorMemoria::GestorMemoria(int num_marcos, int tam_pagina, AlgoritmoReemplazo alg)
    : tamanio_pagina(tam_pagina), algoritmo(alg), 
      fallos_pagina(0), accesos_totales(0), tiempo_actual(0), ventana_pff(10) {
    marcos.reserve(num_marcos);
    for (int i = 0; i < num_marcos; ++i) {
        marcos.emplace_back(i);
    }
}

void GestorMemoria::crear_tabla_paginas(int id_proceso, int num_paginas) {
    auto tabla = std::make_shared<TablaPaginas>(id_proceso, num_paginas);
    tablas_paginas[id_proceso] = tabla;
    std::cout << "Tabla de paginas creada para proceso " << id_proceso 
              << " con " << num_paginas << " paginas." << std::endl;
}

bool GestorMemoria::acceder_memoria(int id_proceso, int direccion_virtual) {
    accesos_totales++;
    tiempo_actual++;
    
    // Verificar que el proceso tenga tabla de páginas
    if (tablas_paginas.find(id_proceso) == tablas_paginas.end()) {
        std::cout << "ERROR: Proceso " << id_proceso << " no tiene tabla de paginas." << std::endl;
        return false;
    }
    
    auto tabla = tablas_paginas[id_proceso];
    int num_pagina = direccion_virtual / tamanio_pagina;
    
    // Verificar que la página esté en rango
    if (num_pagina >= tabla->entradas.size()) {
        std::cout << "ERROR: Direccion fuera de rango." << std::endl;
        return false;
    }
    
    // Verificar si la página está en memoria (hit)
    if (tabla->entradas[num_pagina].presente) {
        tabla->marcar_referenciado(num_pagina);
        std::cout << "[HIT] P" << id_proceso << " accede a pagina " << num_pagina 
                  << " en marco " << tabla->entradas[num_pagina].numero_marco << std::endl;
        return true;
    }
    
    // Page fault (miss)
    fallos_pagina++;
    fallos_recientes.push_back(tiempo_actual);
    std::cout << "[FALLO DE PAGINA] P" << id_proceso << " pagina " << num_pagina << std::endl;
    
    // Buscar marco libre o hacer reemplazo
    int marco_libre = buscar_marco_libre();
    if (marco_libre == -1) {
        // No hay marco libre, seleccionar víctima
        marco_libre = seleccionar_victima();
        
        // Invalidar la página anterior
        Marco& marco = marcos[marco_libre];
        if (marco.ocupado) {
            auto tabla_victima = tablas_paginas[marco.id_proceso];
            tabla_victima->invalidar_pagina(marco.num_pagina);
            std::cout << "  -> Reemplazando: P" << marco.id_proceso 
                      << " pagina " << marco.num_pagina << std::endl;
        }
    }
    
    // Cargar la página en el marco
    Marco& marco = marcos[marco_libre];
    marco.ocupado = true;
    marco.id_proceso = id_proceso;
    marco.num_pagina = num_pagina;
    tabla->cargar_pagina(num_pagina, marco_libre, tiempo_actual);
    
    if (algoritmo == AlgoritmoReemplazo::FIFO) {
        cola_fifo.push(marco_libre);
    }
    
    std::cout << "  -> Cargada en marco " << marco_libre << std::endl;
    return false; // Retorna false porque hubo fallo
}

int GestorMemoria::buscar_marco_libre() {
    for (auto& marco : marcos) {
        if (!marco.ocupado) {
            return marco.numero;
        }
    }
    return -1;
}

int GestorMemoria::seleccionar_victima() {
    if (algoritmo == AlgoritmoReemplazo::FIFO) {
        // FIFO: seleccionar el marco más antiguo
        if (!cola_fifo.empty()) {
            int marco_victima = cola_fifo.front();
            cola_fifo.pop();
            return marco_victima;
        }
        return 0; // Fallback
    } else {
        // LRU: seleccionar el marco menos recientemente usado
        int marco_lru = 0;
        int tiempo_min = tiempo_actual + 1;
        
        for (auto& marco : marcos) {
            if (marco.ocupado) {
                auto tabla = tablas_paginas[marco.id_proceso];
                int tiempo = tabla->entradas[marco.num_pagina].tiempo_carga;
                if (tiempo < tiempo_min) {
                    tiempo_min = tiempo;
                    marco_lru = marco.numero;
                }
            }
        }
        return marco_lru;
    }
}

float GestorMemoria::obtener_tasa_aciertos() const {
    if (accesos_totales == 0) return 0.0f;
    int aciertos = accesos_totales - fallos_pagina;
    return (float)aciertos / accesos_totales * 100.0f;
}

float GestorMemoria::obtener_pff_puntual() const {
    // PFF = fallos en ventana / ventana de tiempo
    int fallos_en_ventana = 0;
    for (int tiempo_fallo : fallos_recientes) {
        if (tiempo_actual - tiempo_fallo <= ventana_pff) {
            fallos_en_ventana++;
        }
    }
    
    if (ventana_pff == 0) return 0.0f;
    return (float)fallos_en_ventana / ventana_pff;
}

void GestorMemoria::mostrar_estado() const {
    std::cout << "\n=== ESTADO DE LA MEMORIA ===" << std::endl;
    
    // Tabla detallada
    std::cout << "Marco | Ocupado | Proceso | Pagina" << std::endl;
    std::cout << "------|---------|---------|-------" << std::endl;
    
    for (const auto& marco : marcos) {
        std::cout << std::setw(5) << marco.numero << " | ";
        std::cout << std::setw(7) << (marco.ocupado ? "SI" : "NO") << " | ";
        std::cout << std::setw(7) << (marco.ocupado ? std::to_string(marco.id_proceso) : "-") << " | ";
        std::cout << std::setw(6) << (marco.ocupado ? std::to_string(marco.num_pagina) : "-") << std::endl;
    }
    
    // Visualización ASCII
    std::cout << "\n=== VISUALIZACION ASCII ===" << std::endl;
    std::cout << "┌";
    for (size_t i = 0; i < marcos.size(); ++i) {
        std::cout << "────┬";
    }
    std::cout << "\b┐" << std::endl;
    
    // Números de marco
    std::cout << "│";
    for (const auto& marco : marcos) {
        std::cout << " " << std::setw(2) << marco.numero << " │";
    }
    std::cout << std::endl;
    
    std::cout << "├";
    for (size_t i = 0; i < marcos.size(); ++i) {
        std::cout << "────┼";
    }
    std::cout << "\b┤" << std::endl;
    
    // Estado de ocupación (P = Proceso, vacío = libre)
    std::cout << "│";
    for (const auto& marco : marcos) {
        if (marco.ocupado) {
            std::cout << " P" << marco.id_proceso << " │";
        } else {
            std::cout << " -- │";
        }
    }
    std::cout << std::endl;
    
    std::cout << "└";
    for (size_t i = 0; i < marcos.size(); ++i) {
        std::cout << "────┴";
    }
    std::cout << "\b┘" << std::endl;
    
    // Estadísticas rápidas
    int marcos_ocupados = 0;
    for (const auto& marco : marcos) {
        if (marco.ocupado) marcos_ocupados++;
    }
    float porcentaje_uso = (float)marcos_ocupados / marcos.size() * 100.0f;
    std::cout << "\nUso de memoria: " << marcos_ocupados << "/" << marcos.size() 
              << " marcos (" << std::fixed << std::setprecision(1) << porcentaje_uso << "%)" << std::endl;
}

void GestorMemoria::mostrar_estadisticas() const {
    std::cout << "\n=== ESTADISTICAS DE MEMORIA ===" << std::endl;
    std::cout << "Algoritmo: " << (algoritmo == AlgoritmoReemplazo::FIFO ? "FIFO" : "LRU") << std::endl;
    std::cout << "Accesos totales: " << accesos_totales << std::endl;
    std::cout << "Fallos de pagina: " << fallos_pagina << std::endl;
    std::cout << "Tasa de aciertos: " << std::fixed << std::setprecision(2) 
              << obtener_tasa_aciertos() << "%" << std::endl;
    std::cout << "PFF puntual: " << std::fixed << std::setprecision(3) 
              << obtener_pff_puntual() << std::endl;
    std::cout << "Tiempo promedio por acceso: ~" 
              << (fallos_pagina > 0 ? "100ns (con fallos)" : "10ns (sin fallos)") << std::endl;
}

