#ifndef LISTA_CIRCULAR_PROMOS_HPP
#define LISTA_CIRCULAR_PROMOS_HPP

#include "../models/Promocion.hpp"
#include <iostream>


// Este es mi nodo que almacena mi Prommocion y apunta al siguiente
struct NodoPromocion {
    Promocion promocion;
    NodoPromocion* siguiente;

    NodoPromocion(const Promocion& p) : promocion(p), siguiente(nullptr) {}

};

class ListaCircularPromos {
    private:
        NodoPromocion* cabeza;
        int tamanio;

    public: 
        ListaCircularPromos();
        ~ListaCircularPromos();


        void agregarPromocion(const Promocion& promo);
        bool eliminarPromocion(const std::string& codigo);
        NodoPromocion* buscarPromocion(const std::string& codigo) const;
        void mostrarPromociones() const;
        void vaciar();

        bool  estaVacia() const { return cabeza == nullptr;}
        int getTamanio() const { return tamanio; }
        NodoPromocion* getCabeza() const { return cabeza; }
};

#endif // LISTA_CIRCULAR_PROMOS_HPP