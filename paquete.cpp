#include "paquete.h"
#include <stdlib.h>

static int ultimo_id = 0;

Paquete crearPaquete(int prioridad){
    Paquete p;
    p.id = ++ultimo_id;
    if(prioridad == 1){
        p.prioridad = 1;
    }else{
        p.prioridad = rand()%2;
    }
    //p.fecha = ;
    return p;
}

bool compararPaquete(const Paquete& a,const Paquete& b){
    if(a.prioridad == b.prioridad){
        return a.id > b.id;
    }
    return a.prioridad < b.prioridad;
}
