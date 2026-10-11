#ifndef LISTA_RESERVAS_HPP
#define LISTA_RESERVAS_HPP

#include "../models/Reserva.hpp"

struct NodoReserva {
    Reserva reserva;
    NodoReserva* siguiente;

    NodoReserva(const Reserva& r) : reserva(r), siguiente(nullptr) {}
};

class ListaReservas {
private:
    NodoReserva* cabeza;
    int tamanio;

    // Helper: copia profunda desde otra lista
    void copiarDesde(const ListaReservas& otra) {
        cabeza = nullptr;
        tamanio = 0;
        NodoReserva* aux = otra.cabeza;
        while (aux != nullptr) {
            insertar(aux->reserva);
            aux = aux->siguiente;
        }
    }

public:
    ListaReservas();
    ~ListaReservas();

    // Rule of Three: copia profunda para evitar doble-free
    ListaReservas(const ListaReservas& otra) : cabeza(nullptr), tamanio(0) {
        copiarDesde(otra);
    }
    ListaReservas& operator=(const ListaReservas& otra) {
        if (this != &otra) {
            limpiar();
            copiarDesde(otra);
        }
        return *this;
    }

    void insertar(const Reserva& r);
    bool eliminar(const std::string& codigoReserva);
    NodoReserva* buscar(const std::string& codigoReserva) const;
    void limpiar();

    int getTamanio() const { return tamanio; }
    bool estaVacia() const { return cabeza == nullptr; }
    NodoReserva* getCabeza() const { return cabeza; }
};

#endif // LISTA_RESERVAS_HPP
