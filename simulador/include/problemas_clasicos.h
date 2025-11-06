#ifndef PROBLEMAS_CLASICOS_H
#define PROBLEMAS_CLASICOS_H

#include "sincronizacion.h"
#include <vector>
#include <memory>
#include <string>

// Problema de los Filósofos Comensales
class FilosofosComenales {
public:
    FilosofosComenales(int num_filosofos);
    
    void filosofo_intenta_comer(int id);
    void filosofo_termina_comer(int id);
    void filosofo_piensa(int id);
    void mostrar_estado() const;
    bool hay_deadlock() const;
    
private:
    void tomar_tenedores(int id);
    void soltar_tenedores(int id);
    int tenedor_izquierdo(int id) const { return id; }
    int tenedor_derecho(int id) const { return (id + 1) % num_filosofos_; }
    
    int num_filosofos_;
    std::vector<std::shared_ptr<Mutex>> tenedores;
    std::vector<std::string> estado_filosofos; // "PENSANDO", "HAMBRIADO", "COMIENDO"
    Mutex mutex_mesa;  // Para proteger el estado
    int filosofos_comiendo;
};

// Problema de Lectores, Escritores
class LectoresEscritores {
public:
    LectoresEscritores();
    
    void lector_comienza(int id);
    void lector_termina(int id);
    void escritor_comienza(int id);
    void escritor_termina(int id);
    void mostrar_estado() const;
    
private:
    int lectores_activos;
    int escritores_activos;
    int escritores_esperando;
    std::vector<int> lectores_ids;
    int escritor_actual_id;
    
    Mutex mutex_lectura;
    Mutex mutex_escritura;
    Semaforo sem_recurso;
};

#endif 
