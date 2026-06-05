#include "ptc.h"
#include "semaforo.h"
#include "paquete.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>

extern std::queue<Paquete> waiting_queue; // recurso compartido
extern std::queue<Paquete> processing_queue;
extern Semaforo hay_espacio_procesing_queue;
extern Semaforo hay_datos_waiting_queue;
extern Semaforo hay_datos_procesing_queue;
std::mutex mtx_waiting_queue; // Para exclusión mutua de la estanteria
std::mutex mtx_processing_queue; // Para exclusión mutua de la cinta transportadora
std::mutex mtx_total_producidos; // Para exclusión mutua del contador global de producidos
std::mutex mtx_consola; // Para exclusión mutua de la consola

int total_producidos = 0;

void productor(int id, int cant_producir, int prioridad) {
    int producidos = 0;
    for (int i = 0; i < cant_producir; i++) {
        // 1. Creacion de un paquete
        Paquete p = crearPaquete(prioridad);
        //if(p.id == 5) p.prioridad = 0; // para Prueba de equidad (Anti-Starvation)

        // 2. Mutex del contador global de producidos
        mtx_total_producidos.lock();
        total_producidos++;
        mtx_total_producidos.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(90)); // espera de 90ms
        // 3. Mutex de la estanteria
        mtx_waiting_queue.lock();
        waiting_queue.push(p); // inserta el paquete en la estanteria
        producidos++;
        mtx_waiting_queue.unlock();

        // 3. Avisa que hay un nuevo paquete disponible en la estanteria
        signal(hay_datos_waiting_queue);

        mtx_consola.lock();
        std::cout << "[Operario " << id << "] Paquete #" << p.id
             << " | Prioridad: " << p.prioridad
             << " | En estanteria" << std::endl;
        mtx_consola.unlock();
    }
    mtx_consola.lock();
    std::cout << "[Operario " << id << "] Finalizado. Produjo: " << producidos << std::endl;
    mtx_consola.unlock();
}

void transportador(int cant_paquetes){
    for (int i = 0; i < cant_paquetes; i++) {
        // 1. Espera que haya paquetes disponibles en la estanteria
        wait(hay_datos_waiting_queue);

        // 2. Mutex de la estanteria
        mtx_waiting_queue.lock();
        // Buscar si hay algún paquete de baja prioridad envejecido (>= 6000ms)
        Paquete elegido;
        std::queue<Paquete> temp;
        bool encontrado = false;

        while (!waiting_queue.empty()) {
            Paquete p = waiting_queue.front();
            waiting_queue.pop();
            if (!encontrado) {
                long long espera = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - p.fecha
                ).count();
                if (p.prioridad == 0 && espera >= 6000) {
                    elegido = p; // promovido por aging
                    encontrado = true;
                    continue; // no lo mete en temp, lo sacamos
                }
            }
            temp.push(p);
        }

        if (!encontrado) {
            // Sin aging: buscar el de mayor prioridad en temp
            std::queue<Paquete> resto;
            while (!temp.empty()) {
                Paquete p = temp.front();
                temp.pop();
                if (!encontrado && p.prioridad == 1) {
                    elegido = p;
                    encontrado = true;
                } else {
                    resto.push(p);
                }
            }
            // Si no habia ninguno de prioridad alta, tomar el primero de baja
            if (!encontrado) {
                elegido = resto.front();
                resto.pop();
                temp = resto;
            } else {
                temp = resto;
            }
        }

        waiting_queue = temp; // restaurar la cola sin el paquete seleccionado
        mtx_waiting_queue.unlock();

        wait(hay_espacio_procesing_queue);

        // 3. Espera que haya espacio en la cinta
        mtx_processing_queue.lock();
        processing_queue.push(elegido); // inserta el paquete en la cinta
        mtx_processing_queue.unlock();

        // 4. Avisa que hay un nuevo paquete en la cinta
        signal(hay_datos_procesing_queue);
        std::this_thread::sleep_for(std::chrono::milliseconds(420)); // espera de 420ms

        mtx_consola.lock();
        std::cout << "[Paquete #" << elegido.id << "]"
             << " | prioridad: " << elegido.prioridad
             << " | en cinta" << std::endl;
        mtx_consola.unlock();
    }
}

void consumidor(int id, int cant_consumir) {
    int consumidos = 0;
    for (int i = 0; i < cant_consumir; i++) {
        // 1. Espera a que haya al menos un dato en la cinta
        wait(hay_datos_procesing_queue);

        // 2. Mutex de la cinta transportadora
        mtx_processing_queue.lock();
        Paquete p = processing_queue.front();
        processing_queue.pop(); // extrae un paquete de la cinta
        consumidos ++;
        mtx_processing_queue.unlock();

        calcularTiempoEspera(p); // calcular el tiempo de espera de un paquete
        std::this_thread::sleep_for(std::chrono::milliseconds(550)); // espera de 550ms

        // 3. Avisa que liberó un espacio en la cinta
        signal(hay_espacio_procesing_queue);
        std::this_thread::sleep_for(std::chrono::milliseconds(270)); // espera de 270ms

        mtx_consola.lock();
        std::cout << "[Repartidor " << id << "] Paquete #" << p.id
             << " | Prioridad: " << p.prioridad
             << " | Procesado" << std::endl;
        mtx_consola.unlock();
    }
    mtx_consola.lock();
    std::cout << "[Repartidor " << id << "] Finalizado. Repartio: " << consumidos << std::endl;
    mtx_consola.unlock();
}
