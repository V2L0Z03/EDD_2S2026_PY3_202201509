#include "../../include/structures/ListaCircularPromos.hpp"

ListaCircularPromos::ListaCircularPromos() : cabeza(nullptr), tamanio(0) {}

ListaCircularPromos::~ListaCircularPromos() {
    vaciar();
}
// ESTO ES PA AGREGAR
void ListaCircularPromos::agregarPromocion(const Promocion& promo) {
    NodoPromocion* nuevo = new NodoPromocion(promo);

    if (estaVacia()) {
        cabeza = nuevo;
        nuevo->siguiente = cabeza; // Enlace circular inicial
    } else {
        NodoPromocion* actual = cabeza;
        while (actual->siguiente != cabeza) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
        nuevo->siguiente = cabeza; // Cierra el ciclo
    }
    tamanio++;
}

//ESTA ES MI FUNCION PARA BUSCAR PROMOCION
NodoPromocion* ListaCircularPromos::buscarPromocion(const std::string& codigo) const {
    if (estaVacia()) return nullptr;

    NodoPromocion* actual = cabeza; // esto sera para la primer busqueda en el inicio
    do {
        if (actual->promocion.getCodigo() == codigo) { //si lo encunetro entonces toma el valor Actual
            return actual;
        }
        actual = actual->siguiente; // es para avanzar al siguiente nodo en la lista circular
    } while (actual != cabeza);  //si actual es distnto el ciclo se repite con el if

    return nullptr; //si es igual a la cabeza entonces termino el ciclo y se devuelve null
}

bool ListaCircularPromos::eliminarPromocion(const std::string& codigo) {
    if (estaVacia()) return false;

    NodoPromocion* actual = cabeza;
    NodoPromocion* anterior = nullptr;

    // Buscar el nodo a eliminar
    do {
        if (actual->promocion.getCodigo() == codigo) {
            break;
        }
        anterior = actual;
        actual = actual->siguiente;
    } while (actual != cabeza);

    // Si no se encontró
    if (actual->promocion.getCodigo() != codigo) return false;

    // Caso 1: Único nodo en la lista
    if (actual == cabeza && actual->siguiente == cabeza) {
        delete actual;
        cabeza = nullptr;
    } 
    // Caso 2: Se elimina la cabeza (habiendo más nodos)
    else if (actual == cabeza) {
        NodoPromocion* ultimo = cabeza;
        while (ultimo->siguiente != cabeza) {
            ultimo = ultimo->siguiente;
        }
        cabeza = cabeza->siguiente;
        ultimo->siguiente = cabeza;
        delete actual;
    } 
    // Caso 3: Se elimina un nodo intermedio o el último
    else {
        anterior->siguiente = actual->siguiente;
        delete actual;
    }

    tamanio--;
    return true;
}

void ListaCircularPromos::mostrarPromociones() const {
    if (estaVacia()) {
        std::cout << "[No hay promociones registradas]\n";
        return;
    }

    NodoPromocion* actual = cabeza;
    int i = 1;
    do {
        std::cout << "========================================\n";
        std::cout << "Promocion #" << i << ": [" << actual->promocion.getCodigo() << "] " 
                  << actual->promocion.getNombre() << "\n";
        std::cout << "Vigencia: " << actual->promocion.getVigencia() 
                  << " | Dias: " << actual->promocion.getDiasAplicables() << "\n";
        std::cout << "Beneficios Asociados:\n";
        
        actual->promocion.getBeneficios().mostrarLista();
        
        actual = actual->siguiente;
        i++;
    } while (actual != cabeza);
    std::cout << "========================================\n";
}

void ListaCircularPromos::vaciar() {
    if (estaVacia()) return;

    NodoPromocion* actual = cabeza;
    NodoPromocion* siguienteNodo = nullptr;

    // Rompo la circularidad para la eliminación limpia
    NodoPromocion* ultimo = cabeza;
    while (ultimo->siguiente != cabeza) {
        ultimo = ultimo->siguiente;
    }
    ultimo->siguiente = nullptr; // Convierto temporalmente en lista lineal

    while (actual != nullptr) {
        siguienteNodo = actual->siguiente;
        delete actual; // Inivoco destructor de Promocion y de ListaDobleBeneficios
        actual = siguienteNodo;
    }

    cabeza = nullptr;
    tamanio = 0;
}