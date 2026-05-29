#include "pc.h"
#include "semaforo.h"
#include <iostream>
#include <queue>
#include <mutex>

// Recursos compartidos
extern std::queue<int> buffer;
extern Semaforo hay_espacio;
extern Semaforo hay_datos;
std::mutex mtx_buffer; // Para exclusión mutua de la cola

const int tam = 100000;

void productor() {
    int producidos = 0;
    for (int i = 0; i < tam; i++) {
        // 1. Espera a que haya un hueco libre
        wait(hay_espacio);

        // 2. Mutex de la cola
        mtx_buffer.lock();
        buffer.push(i);
        producidos++;
        mtx_buffer.unlock();

        // 3. Avisa que hay un nuevo dato disponible
        signal(hay_datos);
    }
    std::cout << "Producidos: " << producidos << std::endl;
}

void consumidor() {
    for (int i = 0; i < tam; i++) {
        // 1. Espera a que haya al menos un dato
        wait(hay_datos);

        // 2. Mutex de la cola
        mtx_buffer.lock();
        int val = buffer.front();
        buffer.pop();
        mtx_buffer.unlock();

        // 3. Avisa que liberó un espacio
        signal(hay_espacio);
    }
}
