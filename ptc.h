#ifndef PTC_H_INCLUDED
#define PTC_H_INCLUDED

#include <mutex>

void productor(int id, int cant_producir, int prioridad);
void transportador(int cant_paquetes);
void consumidor(int id, int cant_consumir);

#endif // PTC_H_INCLUDED
