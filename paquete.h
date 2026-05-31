#ifndef PAQUETE_H_INCLUDED
#define PAQUETE_H_INCLUDED

#include <queue>
#include <mutex>
#include <chrono>
#include "semaforo.h"

struct Paquete {
    int id;
    int prioridad;   // 0 = baja, 1 = alta
    std::chrono::steady_clock::time_point creado_en;
};

// ─── Waiting Queue (estantería, sin límite de capacidad) ─────────────────────
extern std::queue<Paquete> waiting_queue;
extern std::mutex mtx_waiting;

// ─── Processing Queue (cinta, máximo 5 paquetes) ─────────────────────────────
extern std::queue<Paquete> processing_queue;
extern std::mutex mtx_processing;

// ─── Semáforos de la cinta ────────────────────────────────────────────────────
extern Semaforo espacios_cinta;  // inicia en 5 (capacidad máxima)
extern Semaforo paquetes_cinta;  // inicia en 0 (cinta vacía al arrancar)

// ─── Contador global de paquetes producidos (protegido con mutex) ─────────────
extern int total_producidos;
extern std::mutex mtx_contador;


#endif // PAQUETE_H_INCLUDED
