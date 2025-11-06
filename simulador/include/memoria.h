#ifndef MEMORIA_H
#define MEMORIA_H

#include <vector>
#include <map>
#include <queue>
#include <memory>

// Entrada de la tabla de páginas
struct EntradaTabla {
    int numero_marco;    // Número de marco físico 
    bool presente;       // Bit de presencia
    bool referenciado;   // LRU
    int tiempo_carga;    // FIFO
};

// Tabla de páginas de un proceso
class TablaPaginas {
public:
    TablaPaginas(int id_proceso, int num_paginas);
    
    int obtener_marco(int num_pagina);
    void cargar_pagina(int num_pagina, int num_marco, int tiempo);
    void invalidar_pagina(int num_pagina);
    void marcar_referenciado(int num_pagina);
    
    int id_proceso;
    std::vector<EntradaTabla> entradas;
};

// Marco de memoria física
struct Marco {
    int numero;
    bool ocupado;
    int id_proceso;    // Proceso que lo ocupa
    int num_pagina;    // Número de página que contiene
    
    Marco(int num) : numero(num), ocupado(false), id_proceso(-1), num_pagina(-1) {}
};

// Gestor de Memoria
class GestorMemoria {
public:
    enum class AlgoritmoReemplazo {
        FIFO,
        LRU,
        PFF   
    };
    
    GestorMemoria(int num_marcos, int tam_pagina, AlgoritmoReemplazo algoritmo);
    
    // Crear tabla de páginas para un proceso
    void crear_tabla_paginas(int id_proceso, int num_paginas);
    
    // Acceder a una dirección virtual
    bool acceder_memoria(int id_proceso, int direccion_virtual);
    
    // Obtener estadísticas
    int obtener_fallos_pagina() const { return fallos_pagina; }
    int obtener_accesos_totales() const { return accesos_totales; }
    float obtener_tasa_aciertos() const;
    float obtener_pff_puntual() const; 
    
    // Visualización
    void mostrar_estado() const;
    void mostrar_estado_color() const;  // con colores
    void mostrar_estadisticas() const;
    
    // Cambiar algoritmo
    void cambiar_algoritmo(AlgoritmoReemplazo alg);
    AlgoritmoReemplazo obtener_algoritmo() const { return algoritmo; }
    
private:
    int buscar_marco_libre();
    int seleccionar_victima(); // Para reemplazo de página
    void reemplazar_pagina(int id_proceso, int num_pagina);
    void ajustar_marcos_pff();  // Ajuste dinámico 
    
    std::vector<Marco> marcos;
    std::map<int, std::shared_ptr<TablaPaginas>> tablas_paginas; // ID proceso > tabla
    
    int tamanio_pagina;
    AlgoritmoReemplazo algoritmo;
    
    // Estadísticas
    int fallos_pagina;
    int accesos_totales;
    int tiempo_actual;
    int ultimo_acceso_hit;  // Colores
    std::vector<int> marcos_hits;  // Marcos con hit reciente
    std::vector<int> marcos_miss;  // Marcos con miss reciente
    std::queue<int> cola_fifo; // índices de marcos
    
    // Para PFF
    int ventana_pff; // Ventana de tiempo para calcular PFF
    std::vector<int> fallos_recientes; // Fallos en ventana de tiempo
    float umbral_pff_superior;  // Umbral para aumentar
    float umbral_pff_inferior;  // Umbral para reducir
    std::map<int, int> marcos_por_proceso;  // Marcos asignados por proceso
};

#endif 
