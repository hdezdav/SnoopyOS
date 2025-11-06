#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <map>
#include <string>
#include <memory>

// Buddy System, potencias de 2
class BuddyAllocator {
public:
    BuddyAllocator(int tamano_total);
    
    int malloc(int tamano, int id_proceso);
    bool free(int ptr);
    void mostrar_estado() const;
    float calcular_fragmentacion() const;
    
private:
    struct Bloque {
        int inicio;
        int tamano;
        bool ocupado;
        int id_proceso;
        
        Bloque(int i, int t, bool o, int id) 
            : inicio(i), tamano(t), ocupado(o), id_proceso(id) {}
    };
    
    int proximo_potencia_2(int n) const;
    void dividir_bloque(int indice);
    void combinar_buddies();
    
    int tamano_heap;
    std::vector<Bloque> bloques;
    std::map<int, int> asignaciones; // ptr > índice
    int proximo_ptr;
};

#endif 
