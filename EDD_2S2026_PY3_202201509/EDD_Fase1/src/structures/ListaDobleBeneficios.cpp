#include "../../include/structures/ListaDobleBeneficios.hpp"

ListaDobleBeneficios::ListaDobleBeneficios() : cabeza(nullptr), cola(nullptr), tamanio(0) {}

ListaDobleBeneficios::~ListaDobleBeneficios() {
    vaciar();
}

void ListaDobleBeneficios::agregarAlFinal(const Beneficio& beneficio) {
    NodoBeneficio* nuevo = new NodoBeneficio(beneficio);

    if (estaVacia()) {
        cabeza = nuevo;
        cola = nuevo;
    } else {
        cola->siguiente = nuevo;
        nuevo->anterior = cola;
        cola = nuevo;
    }
    tamanio++;
}

void ListaDobleBeneficios::mostrarLista() const {
    if (estaVacia()) {
        std::cout << "  [Lista de beneficios vacia]\n";
        return;
    }

    NodoBeneficio* actual = cabeza;
    int i = 1;
    while (actual != nullptr) {
        std::cout << "  " << i << ". Tipo: " << actual->beneficio.getTipo()
                  << " | Desc: " << actual->beneficio.getDescripcion()
                  << " | Valor: " << actual->beneficio.getValor() << "\n";
        actual = actual->siguiente;
        i++;
    }
}

void ListaDobleBeneficios::mostrarListaInversa() const {
    if (estaVacia()) {
        std::cout << "  [Lista de beneficios vacia]\n";
        return;
    }

    NodoBeneficio* actual = cola;
    int i = tamanio;
    while (actual != nullptr) {
        std::cout << "  " << i << ". Tipo: " << actual->beneficio.getTipo()
                  << " | Desc: " << actual->beneficio.getDescripcion()
                  << " | Valor: " << actual->beneficio.getValor() << "\n";
        actual = actual->anterior;
        i--;
    }
}

void ListaDobleBeneficios::vaciar() {
    NodoBeneficio* actual = cabeza;
    while (actual != nullptr) {
        NodoBeneficio* siguiente = actual->siguiente;
        delete actual; // Liberacion de memoria individual por nodo
        actual = siguiente;
    }
    cabeza = nullptr;
    cola = nullptr;
    tamanio = 0;
}