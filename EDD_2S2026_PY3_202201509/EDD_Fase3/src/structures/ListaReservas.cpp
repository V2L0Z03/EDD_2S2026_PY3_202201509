#include "../../include/structures/ListaReservas.hpp"

ListaReservas::ListaReservas() : cabeza(nullptr), tamanio(0) {}

ListaReservas::~ListaReservas() {
    limpiar();
}

void ListaReservas::insertar(const Reserva& r) {
    NodoReserva* nuevo = new NodoReserva(r);
    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        NodoReserva* aux = cabeza;
        while (aux->siguiente != nullptr) {
            aux = aux->siguiente;
        }
        aux->siguiente = nuevo;
    }
    tamanio++;
}

bool ListaReservas::eliminar(const std::string& codigoReserva) {
    if (cabeza == nullptr) return false;

    if (cabeza->reserva.getCodigoReserva() == codigoReserva) {
        NodoReserva* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
        tamanio--;
        return true;
    }

    NodoReserva* actual = cabeza;
    while (actual->siguiente != nullptr && actual->siguiente->reserva.getCodigoReserva() != codigoReserva) {
        actual = actual->siguiente;
    }

    if (actual->siguiente != nullptr) {
        NodoReserva* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp;
        tamanio--;
        return true;
    }

    return false;
}

NodoReserva* ListaReservas::buscar(const std::string& codigoReserva) const {
    NodoReserva* aux = cabeza;
    while (aux != nullptr) {
        if (aux->reserva.getCodigoReserva() == codigoReserva) {
            return aux;
        }
        aux = aux->siguiente;
    }
    return nullptr;
}

void ListaReservas::limpiar() {
    NodoReserva* aux = cabeza;
    while (aux != nullptr) {
        NodoReserva* temp = aux;
        aux = aux->siguiente;
        delete temp;
    }
    cabeza = nullptr;
    tamanio = 0;
}
