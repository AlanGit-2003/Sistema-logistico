#ifndef PAQUETE_H_INCLUDED
#define PAQUETE_H_INCLUDED

struct Paquete{
    int id;
    int prioridad;
    // fecha;
};

Paquete crearPaquete(int prioridad);
bool compararPaquete(const Paquete& a,const Paquete& b);

#endif // PAQUETE_H_INCLUDED
