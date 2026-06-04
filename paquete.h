#ifndef PAQUETE_H_INCLUDED
#define PAQUETE_H_INCLUDED

#include <chrono>

struct Paquete{
    int id;
    int prioridad;
    std::chrono::steady_clock::time_point fecha;
};

Paquete crearPaquete(int prioridad);
void calcularTiempoEspera(Paquete p);

#endif // PAQUETE_H_INCLUDED
