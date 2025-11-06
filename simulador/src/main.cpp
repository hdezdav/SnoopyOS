#include "planificador.h"
#include "memoria.h"
#include "cli.h"
#include <iostream>
#include <memory>

const int QUANTUM_FIJO = 3;       // Quantum de 3 ticks
const int NUM_MARCOS = 8;         // 8 marcos de memoria física
const int TAM_PAGINA = 256;       // 256 bytes por página

int main() {
    std::cout << "Iniciando SnoopyOS..." << std::endl;
    
    // Crear el planificador
    auto planificador = std::make_shared<Planificador>(QUANTUM_FIJO);
    
    // Crear el gestor de memoria 
    auto memoria = std::make_shared<GestorMemoria>(
        NUM_MARCOS, 
        TAM_PAGINA, 
        GestorMemoria::AlgoritmoReemplazo::FIFO
    );

    // Crear e iniciar la CLI
    CLI cli(planificador, memoria);
    cli.iniciar();

    return 0;
}
