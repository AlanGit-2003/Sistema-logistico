#include "paquete.h"
#include "ptc.h"
#include <stdlib.h>

static int ultimo_id = 0; // para llevar la cuenta del ultimo id
int cant_paquetes_prioridad_1 = 0;
int cant_paquetes_prioridad_0 = 0;
int suma_espera_prioridad_1 = 0;
int suma_espera_prioridad_0 = 0;

extern std::mutex mtx_tiempo_espera;

Paquete crearPaquete(int prioridad){
    Paquete p;
    p.id = ++ultimo_id;
    if(prioridad == 1){ // si prioridad deseada es alta
        p.prioridad = 1;
    }else{ // sino prioridad aleatoria
        p.prioridad = rand()%2;
    }
    p.fecha = std::chrono::steady_clock::now(); // tiempo en que se crea un paquete
    return p;
}

/*static int prioridadEfectiva(const Paquete& p){
    long long espera = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - p.fecha
    ).count();
    if (p.prioridad == 0 && espera >= 6000){
        return 1; // promovido por aging
    }
    return p.prioridad;
}

bool compararPaquete(const Paquete& a, const Paquete& b){
    int pa = prioridadEfectiva(a);
    int pb = prioridadEfectiva(b);
    if (pa == pb){
        return a.id > b.id; // FIFO entre misma prioridad efectiva
    }
    return pa < pb;
}*/

bool compararPaquete(const Paquete& a,const Paquete& b){ // comparar prioridad de paquetes
    if(a.prioridad == b.prioridad){
        return a.id > b.id; // desempate por FIFO (retorna el primero que llego a la cola)
    }
    return a.prioridad < b.prioridad; // retorna el de mayor prioridad
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
