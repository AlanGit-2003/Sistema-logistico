#include "pc2.h"
#include "paquete.h"
#include <iostream>
#include <thread>

using namespace std;

int main() {

    // Inicializar semaforos
    init(espacios_cinta, 5); // capacidad maxima de la cinta
    init(paquetes_cinta, 0); // cinta vacia al arrancar

    // ── Configuracion A: 1 productor, 2 consumidores ──────────────────────────
    //thread op1(productor, "1", 1);
    //thread rep1(consumidor, "1", 2);
    //thread rep2(consumidor, "2", 2);

    //op1.join();
    //rep1.join();
    //rep2.join();

    // ── Configuracion B: descomentar esto y comentar la A ─────────────────────
     //thread op1(productor, "1", 3);
     //thread op2(productor, "2", 3);
     //thread op3(productor, "3", 3);
     //thread rep1(consumidor, "1", 1);
     //op1.join(); op2.join(); op3.join();
     //rep1.join();

    // ── Configuracion C: descomentar esto y comentar la A ─────────────────────
     thread op1(productor, "1", 3);
     thread op2(productor, "2", 3);
     thread op3(productor, "3", 3);
     thread rep1(consumidor, "1", 3);
     thread rep2(consumidor, "2", 3);
     thread rep3(consumidor, "3", 3);
     op1.join(); op2.join(); op3.join();
     rep1.join(); rep2.join(); rep3.join();

    cout << "\nTotal paquetes producidos: " << total_producidos << endl;

    return 0;
}
