#include "pc.h"
#include "semaforo.h"
#include <iostream>
#include <thread>
#include <queue>

std::queue<int> buffer; // recurso compartido
Semaforo hay_espacio;
Semaforo hay_datos;

int main()
{
    init(hay_espacio, 5); //El tamaño límite del buffer
    init(hay_datos, 0); // No hay datos al principio

    std::thread operario(productor);
    std::thread repartidor(consumidor);
    operario.join();
    repartidor.join();

    std::cout << "Estado final del buffer: ";
    if (buffer.empty()) {
        std::cout << "El buffer esta vacio (el consumidor proceso todo)." << std::endl;
    } else {
        std::cout << "Quedaron " << buffer.size() << " elementos." << std::endl;
        while (!buffer.empty()) {
            std::cout << "[" << buffer.front() << "] ";
            buffer.pop();
        }
        std::cout << std::endl;
    }

    return 0;
}
