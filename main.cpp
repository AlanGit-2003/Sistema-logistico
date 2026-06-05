#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <thread>
#include <queue>

std::queue<Paquete> waiting_queue; // cola de paquetes para la estanteria
std::queue<Paquete> processing_queue; // cola de paquetes para la cinta transportadora
Semaforo hay_espacio_procesing_queue;
Semaforo hay_datos_waiting_queue;
Semaforo hay_datos_procesing_queue;

int cant_paquetes = 550;
int prioridad = 2; // prioridad de los paquetes a producir (1=alta, 0=baja, 2=aleatoria)
const int cant_productores = 3;
const int cant_consumidores = 3;

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
    for(int i=1; i<=cant_productores; i++){ // for para crear n hilos
        int cant_producir = cant_paquetes / cant_productores; // cantidad a producir por cada hilo
        int extra = 0;
        if (i <= cant_paquetes % cant_productores){ // si el paquete n <= sobrante le corresponde 1 extra
            extra = 1;
        }
        operarios.emplace_back(productor, i, cant_producir + extra, prioridad);
    }

    std::thread operario(transportador, cant_paquetes);

    std::vector<std::thread> repartidores;
    for(int i=1; i<=cant_consumidores; i++){ // for para crear n hilos consumidores
        int cant_consumir = cant_paquetes / cant_consumidores; // cantidad a consumir por cada hilo
        int extra = 0;
        if (i <= cant_paquetes % cant_consumidores){ // si el paquete n <= sobrante le corresponde 1 extra
            extra = 1;
        }
        repartidores.emplace_back(consumidor, i, cant_consumir + extra);
    }

    for(auto& t : operarios) t.join(); // for para join de los n productores
    operario.join();
    for(auto& t : repartidores) t.join(); // for para join de los n consumidores

    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Total de paquetes producidos : " << total_producidos << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    if (cant_paquetes_prioridad_1 > 0){
        std::cout << "Promedio de espera de paquetes con prioridad alta: " << suma_espera_prioridad_1 / cant_paquetes_prioridad_1 << "ms" << std::endl;
    }else{
        std::cout << "Promedio de espera de paquetes con prioridad alta: sin datos" << std::endl;
    }
    std::cout << "------------------------------------------------------------" << std::endl;
    if (cant_paquetes_prioridad_0 > 0){
        std::cout << "Promedio de espera de paquetes con prioridad baja: " << suma_espera_prioridad_0 / cant_paquetes_prioridad_0 << "ms" << std::endl;}
    else{
        std::cout << "Promedio de espera de paquetes con prioridad baja: sin datos" << std::endl;
    }
    std::cout << "------------------------------------------------------------" << std::endl;

    return 0;
}
