#include "paquete.h"

// ─── Definición de variables globales ────────────────────────────────────────
std::queue<Paquete> waiting_queue;
std::mutex mtx_waiting;

std::queue<Paquete> processing_queue;
std::mutex mtx_processing;

Semaforo espacios_cinta;
Semaforo paquetes_cinta;

int total_producidos = 0;
std::mutex mtx_contador;
