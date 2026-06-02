#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>

extern std::priority_queue<Paquete, std::vector<Paquete>, bool (*)(const Paquete&, const Paquete&)> waiting_queue; // recurso compartido
extern std::queue<Paquete> processing_queue;
extern Semaforo hay_espacio_procesing_queue;
extern Semaforo hay_datos_waiting_queue;
extern Semaforo hay_datos_procesing_queue;
std::mutex mtx_waiting_queue; // Para exclusión mutua de la estanteria
std::mutex mtx_processing_queue; // Para exclusión mutua de la cinta transportadora
std::mutex mtx_total_producidos; // Para exclusión mutua del contador global de producidos
std::mutex mtx_tiempo_espera; // Para exclusión mutua del sumador de tiempos de espera
std::mutex mtx_consola; // Para exclusión mutua de la consola

extern const int cant_paquetes;
int total_producidos = 0;

void productor(int cant_productores, int prioridad) {
    int producidos = 0;
    for (int i = 0; i < cant_paquetes / cant_productores; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(90)); // espera de 90ms
        // 1. Creacion de un paquete
        Paquete p = crearPaquete(prioridad);

        // 2. Mutex del contador global de producidos
        mtx_total_producidos.lock();
        total_producidos++;
        mtx_total_producidos.unlock();

        // 3. Mutex de la estanteria
        mtx_waiting_queue.lock();
        waiting_queue.push(p); // inserta el paquete en la estanteria
        producidos++;
        mtx_waiting_queue.unlock();

        // 3. Avisa que hay un nuevo paquete disponible en la estanteria
        signal(hay_datos_waiting_queue);

        mtx_consola.lock();
        std::cout << "Paquete producido: " << p.id << "-" << p.prioridad <<  std::endl;
        mtx_consola.unlock();
    }
}

void transportador(){
    for (int i = 0; i < cant_paquetes; i++) {
        // 1. Espera que haya paquetes disponibles en la estanteria
        wait(hay_datos_waiting_queue);

        // 2. Mutex de la estanteria
        mtx_waiting_queue.lock();
        Paquete p = waiting_queue.top();
        waiting_queue.pop(); // extrae un paquete de la estanteria
        mtx_waiting_queue.unlock();

        wait(hay_espacio_procesing_queue);

        // 3. Espera que haya espacio en la cinta
        mtx_processing_queue.lock();
        processing_queue.push(p); // inserta el paquete en la cinta
        mtx_processing_queue.unlock();

        // 4. Avisa que hay un nuevo paquete en la cinta
        signal(hay_datos_procesing_queue);
        std::this_thread::sleep_for(std::chrono::milliseconds(420)); // espera de 420ms

        mtx_consola.lock();
        std::cout << "Paquete transportado: " << p.id << "-" << p.prioridad <<  std::endl;
        mtx_consola.unlock();
    }
}

void consumidor(int cant_consumidores) {
    for (int i = 0; i < cant_paquetes / cant_consumidores; i++) {
        // 1. Espera a que haya al menos un dato en la cinta
        wait(hay_datos_procesing_queue);

        // 2. Mutex de la cinta transportadora
        mtx_processing_queue.lock();
        Paquete p = processing_queue.front();
        processing_queue.pop(); // extrae un paquete de la cinta
        mtx_processing_queue.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(550)); // espera de 550ms

        // 3. Avisa que liberó un espacio en la cinta
        signal(hay_espacio_procesing_queue);
        std::this_thread::sleep_for(std::chrono::milliseconds(270)); // espera de 270ms

        calcularTiempoEspera(p); // calcular el tiempo de espera de un paquete

        mtx_consola.lock();
        std::cout << "Paquete procesado: " << p.id << "-" << p.prioridad << std::endl;
        mtx_consola.unlock();
    }
}
