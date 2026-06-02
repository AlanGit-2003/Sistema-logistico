#ifndef PTC_H_INCLUDED
#define PTC_H_INCLUDED

#include <mutex>

void productor(int cant_productores, int prioridad);
void transportador();
void consumidor(int cant_consumidores);

#endif // PTC_H_INCLUDED
