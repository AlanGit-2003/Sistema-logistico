#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <thread>
#include <queue>

std::priority_queue<Paquete, std::vector<Paquete>, bool (*)(const Paquete&, const Paquete&)> waiting_queue(compararPaquete); // recurso compartido
std::queue<Paquete> processing_queue;
Semaforo hay_espacio_waiting_queue;
Semaforo hay_espacio_procesing_queue;
Semaforo hay_datos_waiting_queue;
Semaforo hay_datos_procesing_queue;

int cantidad_paquetes = 20;
int prioridad = 0;
int producidos = 0;

int main()
{
    srand(time(0));

    init(hay_espacio_waiting_queue, cantidad_paquetes); //El tamaño límite del buffer
    init(hay_datos_waiting_queue, 0); // No hay datos al principio
    init(hay_espacio_procesing_queue, 5);
    init(hay_datos_procesing_queue, 0);

    std::thread operario1(productor, prioridad);
    std::thread operario2(transportador);
    std::thread repartidor(consumidor);

    operario1.join();
    operario2.join();
    repartidor.join();

    std::cout << "Tiempo promedio de espera de paquetes producidos: ";

    return 0;
}

//prueba git push Leon
