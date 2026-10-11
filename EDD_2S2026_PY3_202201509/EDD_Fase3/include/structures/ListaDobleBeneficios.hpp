#ifndef LISTA_DOBLE_BENEFICIOS_HPP
#define LISTA_DOBLE_BENEFICIOS_HPP

#include "../models/Beneficio.hpp"
#include <iostream>

struct NodoBeneficio {
    Beneficio beneficio;
    NodoBeneficio* siguiente;
    NodoBeneficio* anterior;

    NodoBeneficio(const Beneficio& b)
        : beneficio(b), siguiente(nullptr), anterior(nullptr) {}
};

class ListaDobleBeneficios {
private:
    NodoBeneficio* cabeza;
    NodoBeneficio* cola;
    int tamanio;

public:
    ListaDobleBeneficios();
    ~ListaDobleBeneficios();

    void agregarAlFinal(const Beneficio& beneficio);
    void mostrarLista() const;
    void mostrarListaInversa() const;
    void vaciar();

    bool estaVacia() const { return cabeza == nullptr; }
    int getTamanio() const { return tamanio; }
    NodoBeneficio* getCabeza() const { return cabeza; }
};

#endif // LISTA_DOBLE_BENEFICIOS_HPP