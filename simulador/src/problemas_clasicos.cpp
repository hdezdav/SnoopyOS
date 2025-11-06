#include "problemas_clasicos.h"
#include <iostream>
#include <algorithm>

// ============ Filósofos Comensales ============

FilosofosComenales::FilosofosComenales(int num_filosofos)
    : num_filosofos_(num_filosofos), filosofos_comiendo(0) {
    for (int i = 0; i < num_filosofos_; ++i) {
        tenedores.push_back(std::make_shared<Mutex>());
        estado_filosofos.push_back("PENSANDO");
    }
    std::cout << "Mesa de " << num_filosofos_ << " filosofos creada." << std::endl;
}

void FilosofosComenales::filosofo_piensa(int id) {
    if (id < 0 || id >= num_filosofos_) return;
    mutex_mesa.lock(nullptr);
    estado_filosofos[id] = "PENSANDO";
    mutex_mesa.unlock();
    std::cout << "Filosofo " << id << " esta PENSANDO" << std::endl;
}

void FilosofosComenales::filosofo_intenta_comer(int id) {
    if (id < 0 || id >= num_filosofos_) {
        std::cout << "ID de filosofo invalido: " << id << std::endl;
        return;
    }
    
    mutex_mesa.lock(nullptr);
    estado_filosofos[id] = "HAMBRIENTO";
    mutex_mesa.unlock();
    
    std::cout << "Filosofo " << id << " tiene HAMBRE" << std::endl;
    
    // Intentar tomar tenedores
    tomar_tenedores(id);
}

void FilosofosComenales::tomar_tenedores(int id) {
    int izq = tenedor_izquierdo(id);
    int der = tenedor_derecho(id);
    
    int primero = (id % 2 == 0) ? izq : der;
    int segundo = (id % 2 == 0) ? der : izq;
    
    if (tenedores[primero]->lock(nullptr)) {
        std::cout << "  Filosofo " << id << " tomo tenedor " << primero << std::endl;
        if (tenedores[segundo]->lock(nullptr)) {
            std::cout << "  Filosofo " << id << " tomo tenedor " << segundo << std::endl;
            mutex_mesa.lock(nullptr);
            estado_filosofos[id] = "COMIENDO";
            filosofos_comiendo++;
            mutex_mesa.unlock();
            std::cout << "  -> Filosofo " << id << " esta COMIENDO" << std::endl;
        } else {
            tenedores[primero]->unlock();
            estado_filosofos[id] = "HAMBRIENTO";
        }
    }
}

void FilosofosComenales::filosofo_termina_comer(int id) {
    if (id < 0 || id >= num_filosofos_) return;
    
    if (estado_filosofos[id] != "COMIENDO") {
        std::cout << "Filosofo " << id << " no esta comiendo." << std::endl;
        return;
    }
    
    soltar_tenedores(id);
}

void FilosofosComenales::soltar_tenedores(int id) {
    int izq = tenedor_izquierdo(id);
    int der = tenedor_derecho(id);
    
    tenedores[izq]->unlock();
    tenedores[der]->unlock();
    
    mutex_mesa.lock(nullptr);
    estado_filosofos[id] = "PENSANDO";
    filosofos_comiendo--;
    mutex_mesa.unlock();
    
    std::cout << "Filosofo " << id << " termino de comer y solto tenedores " 
              << izq << " y " << der << std::endl;
}

void FilosofosComenales::mostrar_estado() const {
    std::cout << "\n=== ESTADO DE LOS FILOSOFOS ===" << std::endl;
    std::cout << "Filosofos comiendo: " << filosofos_comiendo << "/" << num_filosofos_ << "\n" << std::endl;
    
    // Visualización de la mesa
    std::cout << "     Mesa Redonda" << std::endl;
    std::cout << "   ";
    for (int i = 0; i < num_filosofos_; ++i) {
        std::cout << "F" << i << "  ";
    }
    std::cout << "\n   ";
    for (int i = 0; i < num_filosofos_; ++i) {
        std::cout << "[";
        if (estado_filosofos[i] == "PENSANDO") std::cout << "😌";
        else if (estado_filosofos[i] == "HAMBRIENTO") std::cout << "🍽️";
        else std::cout << "🍝";
        std::cout << "] ";
    }
    std::cout << "\n" << std::endl;
    
    // Estado detallado
    for (int i = 0; i < num_filosofos_; ++i) {
        std::cout << "Filosofo " << i << ": " << estado_filosofos[i];
        if (estado_filosofos[i] == "COMIENDO") {
            std::cout << " (tenedores " << tenedor_izquierdo(i) 
                      << " y " << tenedor_derecho(i) << ")";
        }
        std::cout << std::endl;
    }
    
    // Estado de tenedores
    std::cout << "\nTenedores:" << std::endl;
    for (int i = 0; i < num_filosofos_; ++i) {
        std::cout << "  Tenedor " << i << ": " 
                  << (tenedores[i]->esta_bloqueado() ? "OCUPADO" : "LIBRE") << std::endl;
    }
}

bool FilosofosComenales::hay_deadlock() const {
    // Detectar deadlock: todos los filósofos están hambriados y ninguno puede comer
    int hambrientos = 0;
    for (const auto& estado : estado_filosofos) {
        if (estado == "HAMBRIENTO") hambrientos++;
    }
    
    return (hambrientos == num_filosofos_ && filosofos_comiendo == 0);
}

//Lectores-Escritores

LectoresEscritores::LectoresEscritores()
    : lectores_activos(0), escritores_activos(0), escritores_esperando(0),
      escritor_actual_id(-1), sem_recurso(1) {
    std::cout << "Sistema Lectores-Escritores inicializado." << std::endl;
}

void LectoresEscritores::lector_comienza(int id) {
    mutex_lectura.lock(nullptr);
    
    lectores_activos++;
    if (lectores_activos == 1) {
        // Primer lector bloquea a escritores
        sem_recurso.wait(nullptr);
    }
    
    lectores_ids.push_back(id);
    mutex_lectura.unlock();
    
    std::cout << "Lector " << id << " comienza a LEER (total lectores: " 
              << lectores_activos << ")" << std::endl;
}

void LectoresEscritores::lector_termina(int id) {
    mutex_lectura.lock(nullptr);
    
    auto it = std::find(lectores_ids.begin(), lectores_ids.end(), id);
    if (it != lectores_ids.end()) {
        lectores_ids.erase(it);
    }
    
    lectores_activos--;
    if (lectores_activos == 0) {
        // Último lector desbloquea a escritores
        sem_recurso.signal();
    }
    
    mutex_lectura.unlock();
    
    std::cout << "Lector " << id << " termino de leer (lectores restantes: " 
              << lectores_activos << ")" << std::endl;
}

void LectoresEscritores::escritor_comienza(int id) {
    escritores_esperando++;
    std::cout << "Escritor " << id << " esperando acceso..." << std::endl;
    
    mutex_escritura.lock(nullptr);
    sem_recurso.wait(nullptr);
    
    escritores_esperando--;
    escritores_activos = 1;
    escritor_actual_id = id;
    
    std::cout << "Escritor " << id << " comienza a ESCRIBIR (acceso exclusivo)" << std::endl;
}

void LectoresEscritores::escritor_termina(int id) {
    if (escritor_actual_id != id) {
        std::cout << "Escritor " << id << " no tiene el recurso." << std::endl;
        return;
    }
    
    escritores_activos = 0;
    escritor_actual_id = -1;
    
    sem_recurso.signal();
    mutex_escritura.unlock();
    
    std::cout << "Escritor " << id << " termino de escribir" << std::endl;
}

void LectoresEscritores::mostrar_estado() const {
    std::cout << "\n=== ESTADO LECTORES-ESCRITORES ===" << std::endl;
    std::cout << "Lectores activos: " << lectores_activos << std::endl;
    
    if (lectores_activos > 0) {
        std::cout << "  IDs leyendo: ";
        for (int id : lectores_ids) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Escritores activos: " << escritores_activos << std::endl;
    if (escritores_activos > 0) {
        std::cout << "  ID escribiendo: " << escritor_actual_id << std::endl;
    }
    
    std::cout << "Escritores esperando: " << escritores_esperando << std::endl;
    
    std::cout << "\nEstado del recurso: ";
    if (escritores_activos > 0) {
        std::cout << "ESCRIBIENDO (exclusivo)" << std::endl;
    } else if (lectores_activos > 0) {
        std::cout << "LEYENDO (compartido)" << std::endl;
    } else {
        std::cout << "LIBRE" << std::endl;
    }
}
