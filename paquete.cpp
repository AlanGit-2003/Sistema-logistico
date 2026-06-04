#include "paquete.h"
#include "ptc.h"
#include <stdlib.h>

std::mutex mtx_id; // Para exclusión mutua del ultimo id
std::mutex mtx_tiempo_espera; // Para exclusión mutua del sumador de tiempos de espera

static int ultimo_id = 0; // para llevar la cuenta del ultimo id
int cant_paquetes_prioridad_1 = 0;
int cant_paquetes_prioridad_0 = 0;
int suma_espera_prioridad_1 = 0;
int suma_espera_prioridad_0 = 0;

Paquete crearPaquete(int prioridad){
    Paquete p;
    mtx_id.lock();
    p.id = ++ultimo_id;
    mtx_id.unlock();
    if(prioridad == 0) p.prioridad = 0; // si prioridad deseada es baja
    if(prioridad == 1) p.prioridad = 1; // si prioridad deseada es alta
    if(prioridad == 2) p.prioridad = rand()%2; // si prioridad deseada es aleatoria
    p.fecha = std::chrono::steady_clock::now(); // tiempo en que se crea un paquete
    return p;
}

void calcularTiempoEspera(Paquete p){
        long long espera = std::chrono::duration_cast<std::chrono::milliseconds>( // tiempo desde que se creo el paquete hasta que se proceso
            std::chrono::steady_clock::now() - p.fecha
        ).count();
        mtx_tiempo_espera.lock();
         if(p.prioridad == 1){
            suma_espera_prioridad_1 += espera;
            cant_paquetes_prioridad_1++;
        }
        else{
            suma_espera_prioridad_0 += espera;
            cant_paquetes_prioridad_0++;
        }
        mtx_tiempo_espera.unlock();
}
