#include "heap.h"
#include <iostream>
#include <iomanip>
#include <cmath>

BuddyAllocator::BuddyAllocator(int tamano)
    : tamano_heap(tamano), proximo_ptr(1000) {
    // Iniciar con un bloque grande libre
    bloques.push_back(Bloque(0, tamano_heap, false, -1));
}

int BuddyAllocator::proximo_potencia_2(int n) const {
    if (n <= 0) return 1;
    int potencia = 1;
    while (potencia < n) {
        potencia *= 2;
    }
    return potencia;
}

int BuddyAllocator::malloc(int tamano, int id_proceso) {
    if (tamano <= 0) {
        std::cout << "Error: tamano invalido" << std::endl;
        return -1;
    }
    int tamano_real = proximo_potencia_2(tamano);
    int mejor_indice = -1, menor_tamano = tamano_heap + 1;
    for (size_t i = 0; i < bloques.size(); ++i)
        if (!bloques[i].ocupado && bloques[i].tamano >= tamano_real && bloques[i].tamano < menor_tamano) {
            menor_tamano = bloques[i].tamano;
            mejor_indice = i;
        }
    if (mejor_indice == -1) {
        std::cout << "Error: No hay memoria suficiente (fragmentacion o memoria llena)" << std::endl;
        return -1;
    }
    while (bloques[mejor_indice].tamano > tamano_real && bloques[mejor_indice].tamano > 1)
        dividir_bloque(mejor_indice);
    bloques[mejor_indice].ocupado = true;
    bloques[mejor_indice].id_proceso = id_proceso;
    int ptr = proximo_ptr++;
    asignaciones[ptr] = mejor_indice;
    std::cout << "malloc(" << tamano << ") -> ptr=" << ptr 
              << " (bloque de " << bloques[mejor_indice].tamano << " bytes en pos " 
              << bloques[mejor_indice].inicio << ")" << std::endl;
    return ptr;
}

void BuddyAllocator::dividir_bloque(int indice) {
    if (bloques[indice].tamano <= 1) return;
    
    int mitad = bloques[indice].tamano / 2;
    int inicio = bloques[indice].inicio;
    
    // Modificar bloque existente
    bloques[indice].tamano = mitad;
    
    // Crear buddy
    bloques.insert(bloques.begin() + indice + 1, 
                   Bloque(inicio + mitad, mitad, false, -1));
}

bool BuddyAllocator::free(int ptr) {
    auto it = asignaciones.find(ptr);
    if (it == asignaciones.end()) {
        std::cout << "Error: ptr " << ptr << " no encontrado" << std::endl;
        return false;
    }
    
    int indice = it->second;
    bloques[indice].ocupado = false;
    bloques[indice].id_proceso = -1;
    
    std::cout << "free(ptr=" << ptr << ") -> liberado bloque de " 
              << bloques[indice].tamano << " bytes" << std::endl;
    
    asignaciones.erase(it);
    
    // Intentar combinar buddies
    combinar_buddies();
    
    return true;
}

void BuddyAllocator::combinar_buddies() {
    bool combinado = true;
    
    while (combinado) {
        combinado = false;
        
        for (size_t i = 0; i < bloques.size() - 1; ++i) {
            // Verificar si i y i+1 son buddies
            if (!bloques[i].ocupado && !bloques[i+1].ocupado &&
                bloques[i].tamano == bloques[i+1].tamano &&
                bloques[i].inicio + bloques[i].tamano == bloques[i+1].inicio) {
                
                // Combinar
                bloques[i].tamano *= 2;
                bloques.erase(bloques.begin() + i + 1);
                combinado = true;
                break;
            }
        }
    }
}

void BuddyAllocator::mostrar_estado() const {
    std::cout << "\n=== ESTADO DEL HEAP (Buddy System) ===\n"
              << "Tamano total: " << tamano_heap << " bytes\n\nBloques:" << std::endl;
    int ocupado_total = 0;
    for (size_t i = 0; i < bloques.size(); ++i) {
        std::cout << "  [" << std::setw(4) << bloques[i].inicio << "-" 
                  << std::setw(4) << (bloques[i].inicio + bloques[i].tamano - 1) 
                  << "] " << std::setw(4) << bloques[i].tamano << " bytes - ";
        if (bloques[i].ocupado) {
            std::cout << "OCUPADO (P" << bloques[i].id_proceso << ")";
            ocupado_total += bloques[i].tamano;
        } else
            std::cout << "LIBRE";
        std::cout << std::endl;
    }
    
    // Visualización gráfica
    std::cout << "\nVisualizacion:" << std::endl;
    std::cout << "[";
    int pos = 0;
    for (const auto& bloque : bloques) {
        while (pos < bloque.inicio) {
            std::cout << " ";
            pos++;
        }
        int chars = bloque.tamano / 8;
        if (chars < 1) chars = 1;
        for (int j = 0; j < chars; ++j) {
            std::cout << (bloque.ocupado ? "█" : "░");
        }
        pos += bloque.tamano;
    }
    std::cout << "]" << std::endl;
    
    float uso = (float)ocupado_total / tamano_heap * 100;
    std::cout << "\nUso de memoria: " << ocupado_total << "/" << tamano_heap 
              << " (" << std::fixed << std::setprecision(1) << uso << "%)" << std::endl;
    std::cout << "Fragmentacion: " << std::setprecision(1) 
              << calcular_fragmentacion() << "%" << std::endl;
}

float BuddyAllocator::calcular_fragmentacion() const {
    int libre_total = 0, bloque_libre_mayor = 0;
    for (const auto& bloque : bloques)
        if (!bloque.ocupado) {
            libre_total += bloque.tamano;
            if (bloque.tamano > bloque_libre_mayor)
                bloque_libre_mayor = bloque.tamano;
        }
    return (libre_total == 0) ? 0.0f : ((float)(libre_total - bloque_libre_mayor) / libre_total) * 100.0f;
}
