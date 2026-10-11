#include "../../include/structures/ListaCircularDobleSolicitudes.hpp"

ListaCircularDobleSolicitudes::ListaCircularDobleSolicitudes() : cabeza(nullptr), tamanio(0) {}
ListaCircularDobleSolicitudes::~ListaCircularDobleSolicitudes() {
    vaciar();
}

void ListaCircularDobleSolicitudes::agregarSolicitud(const Solicitud& sol) {
    NodoSolicitud* nuevo = new NodoSolicitud(sol);

    if(estaVacia()) {
        cabeza = nuevo;
        cabeza->siguiente = cabeza;
        //cabeza->anterior = cabeza;
    } else {
        NodoSolicitud* cola = cabeza->anterior;
        cola->siguiente = nuevo;
        nuevo->anterior = cola;
        nuevo->siguiente = cabeza;
        //cabeza->anterior = nuevo;
    }
    tamanio++;
}
NodoSolicitud* ListaCircularDobleSolicitudes::buscarPorTelefono(const std::string& telefono) const {
    if (estaVacia()) return nullptr;

    NodoSolicitud* actual = cabeza;
    do {
        if (actual->solicitud.getTelefono() == telefono) {
            return actual;
        }
        actual = actual->siguiente;
    } while (actual != cabeza);

    return nullptr;
}

NodoSolicitud* ListaCircularDobleSolicitudes::buscarPorId(int id) const {
    if (estaVacia()) return nullptr;

    NodoSolicitud* actual = cabeza;
    do {
        if (actual->solicitud.getId() == id) {
            return actual;
        }
        actual = actual->siguiente;
    } while (actual != cabeza);

    return nullptr;
}

bool ListaCircularDobleSolicitudes::eliminarSolicitud(int id) {
    if (estaVacia()) return false;

    NodoSolicitud* actual = buscarPorId(id);
    if (actual == nullptr) return false; // No encontrado

    // sto es si es Único nodo en la lista
    if (actual == cabeza && actual->siguiente == cabeza) {
        delete actual;
        cabeza = nullptr;
    } 
    // stoe es si es Múltiples nodos en la lista
    else {
        actual->anterior->siguiente = actual->siguiente;
        actual->siguiente->anterior = actual->anterior;

        if (actual == cabeza) {
            cabeza = actual->siguiente;
        }
        delete actual;
    }

    tamanio--;
    return true;
}

void ListaCircularDobleSolicitudes::cambiarEstado(int id, const std::string& nuevoEstado) {
    NodoSolicitud* nodo = buscarPorId(id);
    if (nodo != nullptr) {
        nodo->solicitud.setEstado(nuevoEstado);
    }
}

void ListaCircularDobleSolicitudes::mostrarSolicitudes() const {
    if (estaVacia()) {
        std::cout << "[No hay solicitudes especiales pendientes]\n";
        return;
    }

    NodoSolicitud* actual = cabeza;
    int i = 1;
    do {
        std::cout << "--------------------------------------------------------\n";
        std::cout << "Solicitud #" << actual->solicitud.getId() << " (Posicion en cola: " << i << ")\n";
        std::cout << "Cliente: " << actual->solicitud.getCliente() 
                  << " | Tel: " << actual->solicitud.getTelefono() << "\n";
        std::cout << "Tipo: " << actual->solicitud.getTipo() 
                  << " | Estado: [" << actual->solicitud.getEstado() << "]\n";
        std::cout << "Detalle: " << actual->solicitud.getDescripcion() << "\n";

        actual = actual->siguiente;
        i++;
    } while (actual != cabeza);
    std::cout << "--------------------------------------------------------\n";
}

void ListaCircularDobleSolicitudes::vaciar() {
    if (estaVacia()) return;

    NodoSolicitud* actual = cabeza;
    NodoSolicitud* auxiliar = nullptr;

    // Desconecto el ciclo para iterar linealmente
    cabeza->anterior->siguiente = nullptr;

    while (actual != nullptr) {
        auxiliar = actual->siguiente;
        delete actual;
        actual = auxiliar;
    }

    cabeza = nullptr;
    tamanio = 0;
}