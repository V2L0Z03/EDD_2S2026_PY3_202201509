#ifndef LISTA_CIRCULAR_DOBLE_SOLICITUDES_HPP
#define LISTA_CIRCULAR_DOBLE_SOLICITUDES_HPP

#include "../models/Solicitud.hpp"
#include <iostream>

//Mi estructura
struct NodoSolicitud {
    Solicitud solicitud;
    NodoSolicitud* siguiente; //pa la direccion en memoriaam
    NodoSolicitud* anterior;

    NodoSolicitud(const Solicitud& s) 
    : solicitud(s), siguiente(nullptr), anterior(nullptr) {}
};

//my class
class ListaCircularDobleSolicitudes {
    private:
        NodoSolicitud* cabeza;
        int tamanio;

    public:
    ListaCircularDobleSolicitudes();
    ~ListaCircularDobleSolicitudes();

    void agregarSolicitud(const Solicitud& sol);
    bool eliminarSolicitud(int id);
    NodoSolicitud* buscarPorTelefono(const std::string& telefono) const;
    NodoSolicitud* buscarPorId(int id) const;
    void mostrarSolicitudes() const;
    void cambiarEstado(int id, const std::string& nuevoEstado);
    void vaciar();


    bool estaVacia() const { return cabeza == nullptr; }
    int getTamanio() const { return tamanio; }
    NodoSolicitud* getCabeza() const { return cabeza;}
};
#endif // LISTA_CIRCULAR_DOBLE_SOLICITUDES_HPP