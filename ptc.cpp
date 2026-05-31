#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>

// Recursos compartidos
std::priority_queue<Paquete, std::vector<Paquete>, bool (*)(const Paquete&, const Paquete&)> waiting_queue(compararPaquete); // recurso compartido

extern std::priority_queue<Paquete, std::vector<Paquete>, bool (*)(const Paquete&, const Paquete&)> waiting_queue;
extern std::queue<Paquete> processing_queue;
extern Semaforo hay_espacio_waiting_queue;
extern Semaforo hay_espacio_procesing_queue;
extern Semaforo hay_datos_waiting_queue;
extern Semaforo hay_datos_procesing_queue;
std::mutex mtx_waiting_queue; // Para exclusión mutua de la cola
std::mutex mtx_processing_queue;

extern const int cantidad_paquetes;
extern int producidos;

void productor(int prioridad) {
    for (int i = 0; i < cantidad_paquetes; i++) {
        // 1. Espera a que haya un hueco libre
        wait(hay_espacio_waiting_queue);

        // 2. Mutex de la cola
        mtx_waiting_queue.lock();
        Paquete p = crearPaquete(prioridad);
        std::cout << "producido: " << p.id << "-" << p.prioridad << std::endl;
        waiting_queue.push(p);
        producidos++;
        mtx_waiting_queue.unlock();

        // 3. Avisa que hay un nuevo dato disponible
        signal(hay_datos_waiting_queue);
    }
    std::cout << "Cantidad total de paquetes producidos : " << producidos << std::endl;
}

void transportador(){
    for (int i = 0; i < cantidad_paquetes; i++) {
        wait(hay_datos_waiting_queue);
        std::this_thread::sleep_for(std::chrono::milliseconds(90));

        mtx_waiting_queue.lock();
        Paquete p = waiting_queue.top();
        waiting_queue.pop();
        mtx_waiting_queue.unlock();

        signal(hay_espacio_waiting_queue);

        wait(hay_espacio_procesing_queue);

        mtx_processing_queue.lock();
        processing_queue.push(p);
        std::cout << "transportado: " << p.id <<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(420));
        mtx_processing_queue.unlock();

        signal(hay_datos_procesing_queue);
    }
}

void consumidor() {
    for (int i = 0; i < cantidad_paquetes; i++) {
        // 1. Espera a que haya al menos un dato
        wait(hay_datos_procesing_queue);

        // 2. Mutex de la cola
        mtx_processing_queue.lock();
        Paquete p = processing_queue.front();
        processing_queue.pop();
        std::cout << "procesado: " << p.id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(270));
        mtx_processing_queue.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        // 3. Avisa que liberó un espacio
        signal(hay_espacio_procesing_queue);
    }
}
