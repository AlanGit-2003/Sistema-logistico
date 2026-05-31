#include "pc2.h"
#include "paquete.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

#include <mutex>

using namespace std;
using namespace chrono;

const int tam = 1550;

// Mutex para el sobrante del consumidor (igual que el profesor en pcv3)
mutex mtx_buffer_consumo;
int consumidos = 0;

// ID unico por paquete, protegido con mutex (race condition controlada)
static int siguiente_id = 0;
static mutex mtx_id;

void productor(string id, int cantidadProductores) {
    int producidos = 0;
    int miParte = tam / cantidadProductores;

    for (int i = 0; i < miParte; i++) {

        // Armar el paquete
        Paquete p;
        mtx_id.lock();
        p.id = siguiente_id++;
        mtx_id.unlock();
        p.prioridad = rand() % 2;   // 0 o 1 al azar
        p.creado_en = steady_clock::now();

        // Contador global de producidos (requisito race condition)
        mtx_contador.lock();
        total_producidos++;
        mtx_contador.unlock();

        // Retardo de ingreso a la estantería
        this_thread::sleep_for(milliseconds(90));

        // Esperar espacio en la cinta antes de meter en estantería
        wait(espacios_cinta);

        // Meter en waiting queue
        mtx_waiting.lock();
        waiting_queue.push(p);
        producidos++;
        mtx_waiting.unlock();

        // Avisar que hay un paquete nuevo
        signal(paquetes_cinta);

        cout << "[Operario " << id << "] Paquete #" << p.id
             << " | prioridad: " << p.prioridad
             << " | en estanteria" << endl;
    }

    cout << "[Operario " << id << "] Finalizado. Produjo: " << producidos << endl;
}

void consumidor(string id, int cantidadConsumidores) {
    int miParte = tam / cantidadConsumidores;

    for (int i = 0; i < miParte; i++) {

        // 1. Esperar a que haya un paquete en la estantería
        wait(paquetes_cinta);

        // 2. Elegir paquete por prioridad: alta primero, baja si no hay alta
        mtx_waiting.lock();

        Paquete elegido;
        queue<Paquete> temp;
        bool encontrado = false;

        // Buscar uno de alta prioridad
        while (!waiting_queue.empty()) {
            Paquete p = waiting_queue.front();
            waiting_queue.pop();
            if (p.prioridad == 1 && !encontrado) {
                elegido = p;
                encontrado = true;
            } else {
                temp.push(p);
            }
        }

        // Si no habia de alta, tomar el primero de baja que quedó en temp
        if (!encontrado) {
            elegido = temp.front();
            temp.pop();
        }

        // Devolver el resto a la waiting queue
        while (!temp.empty()) {
            waiting_queue.push(temp.front());
            temp.pop();
        }

        mtx_waiting.unlock();

        // 3. Retardo de pase a la cinta (420ms)
        this_thread::sleep_for(milliseconds(10));

        // 4. Meter en la processing queue
        mtx_processing.lock();
        processing_queue.push(elegido);
        mtx_processing.unlock();

        cout << "[Repartidor " << id << "] Paquete #" << elegido.id
             << " | prioridad: " << elegido.prioridad
             << " | en cinta" << endl;

        // 5. Tiempo minimo en la cinta (550ms)
        this_thread::sleep_for(milliseconds(10));

        // 6. Retirar de la cinta
        mtx_processing.lock();
        processing_queue.pop();
        mtx_processing.unlock();

        // 7. Retardo de liberacion (270ms)
        this_thread::sleep_for(milliseconds(10));

        // 8. Liberar espacio en la cinta
        signal(espacios_cinta);

        // Calcular tiempo de espera total del paquete
        long long espera_ms = duration_cast<milliseconds>(
            steady_clock::now() - elegido.creado_en
        ).count();

        cout << "[Repartidor " << id << "] Paquete #" << elegido.id
             << " | prioridad: " << elegido.prioridad
             << " | procesado | espera: " << espera_ms << "ms" << endl;
    }

    // Manejo del sobrante (igual que el profesor en pcv3)
    // Si tam no divide exacto, el ultimo consumidor en llegar agarra lo que sobra
    mtx_buffer_consumo.lock();
    while (consumidos < tam) {
        if (waiting_queue.empty()) break;

        wait(paquetes_cinta);

        mtx_waiting.lock();
        Paquete p = waiting_queue.front();
        waiting_queue.pop();
        consumidos++;
        mtx_waiting.unlock();

        signal(espacios_cinta);

        cout << "[Repartidor " << id << "] Paquete #" << p.id
             << " | sobrante | procesado" << endl;
    }
    mtx_buffer_consumo.unlock();

    cout << "[Repartidor " << id << "] Finalizado." << endl;
}
