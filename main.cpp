#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <thread>
#include <queue>

std::priority_queue<Paquete, std::vector<Paquete>, bool (*)(const Paquete&, const Paquete&)> waiting_queue(compararPaquete); // estanteria de cola de paquetes por prioridades
std::queue<Paquete> processing_queue; // cola de paquetes para la cinta transportadora
Semaforo hay_espacio_procesing_queue;
Semaforo hay_datos_waiting_queue;
Semaforo hay_datos_procesing_queue;

int cant_paquetes = 20;
int prioridad = 0; // prioridad de los paquetes a producir (1=alta, 0=aleatoria)
const int cant_productores = 5;
const int cant_consumidores = 5;

extern int total_producidos;
extern int cant_paquetes_prioridad_1;
extern int cant_paquetes_prioridad_0;
extern int suma_espera_prioridad_1;
extern int suma_espera_prioridad_0;

int main()
{
    srand(time(0)); // semilla para numeros aleatorios

    init(hay_datos_waiting_queue, 0); // No hay datos al principio
    init(hay_espacio_procesing_queue, 5); // limite de paquetes en simultaneo de la cinta transportadora
    init(hay_datos_procesing_queue, 0);

    std::vector<std::thread> operarios;
    for(int i=0; i<cant_productores; i++){ // for para crear n hilos productores
        operarios.emplace_back(productor, cant_productores, prioridad);
    }

    std::thread operario(transportador);

    std::vector<std::thread> repartidores;
    for(int i=0; i<cant_consumidores; i++){ // for para crear n hilos consumidores
        repartidores.emplace_back(consumidor, cant_consumidores);
    }

    for(auto& t : operarios) t.join(); // for para join de los n productores
    operario.join();
    for(auto& t : repartidores) t.join(); // for para join de los n consumidores

    std::cout << "Total de paquetes producidos : " << total_producidos << std::endl;
    std::cout << "Promedio de espera de paquetes con prioridad alta: " << suma_espera_prioridad_1 / cant_paquetes_prioridad_1 << "ms" << std::endl;
    std::cout << "Promedio de espera de paquetes con prioridad baja: " << suma_espera_prioridad_0 / cant_paquetes_prioridad_0 << "ms" << std::endl;

    return 0;
}
