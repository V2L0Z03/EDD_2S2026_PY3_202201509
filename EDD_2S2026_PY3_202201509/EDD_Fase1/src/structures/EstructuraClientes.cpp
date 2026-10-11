#include "../../include/structures/EstructuraClientes.hpp"
#include <iostream>

// ════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ════════════════════════════════════════════════════════════
EstructuraClientes::EstructuraClientes()
    : cabeza(nullptr), cola(nullptr), tamanio(0) {}

EstructuraClientes::~EstructuraClientes() {
    limpiar();
}

// ════════════════════════════════════════════════════════════
//  limpiar
//  Primero vacía el árbol (solo borra nodos del árbol, NO
//  los clientes), luego destruye los NodoCliente que son los
//  verdaderos propietarios de los objetos Cliente.
// ════════════════════════════════════════════════════════════
void EstructuraClientes::limpiar() {
    // 1. Limpiar árbol (solo NodoArbolB, los Cliente* son de la lista)
    arbol.limpiar();

    // 2. Destruir lista sombra (aquí se liberan los Cliente por valor)
    NodoCliente* aux = cabeza;
    while (aux != nullptr) {
        NodoCliente* temp = aux;
        aux = aux->siguiente;
        delete temp;
    }
    cabeza   = nullptr;
    cola     = nullptr;
    tamanio  = 0;
}

// ════════════════════════════════════════════════════════════
//  Quitar un NodoCliente de la lista sombra (sin tocar árbol)
// ════════════════════════════════════════════════════════════
void EstructuraClientes::quitarDeListaSombra(NodoCliente* nodo) {
    if (nodo == nullptr) return;

    if (nodo == cabeza) {
        cabeza = cabeza->siguiente;
        if (cabeza) cabeza->anterior = nullptr;
        else        cola = nullptr;
    } else if (nodo == cola) {
        cola = cola->anterior;
        if (cola) cola->siguiente = nullptr;
        else      cabeza = nullptr;
    } else {
        nodo->anterior->siguiente = nodo->siguiente;
        nodo->siguiente->anterior = nodo->anterior;
    }
    tamanio--;
}

// ════════════════════════════════════════════════════════════
//  insertar
// ════════════════════════════════════════════════════════════
bool EstructuraClientes::insertar(const Cliente& c) {
    if (c.getId().empty()) return false;

    // Duplicado de ID
    if (arbol.buscarPorId(c.getId()) != nullptr) return false;

    // 1. Crear NodoCliente en heap (propietario del objeto Cliente)
    NodoCliente* nuevo = new NodoCliente(c);

    // 2. Insertar puntero NO propietario en el árbol
    if (!arbol.insertar(&nuevo->cliente)) {
        delete nuevo;
        return false;
    }

    // 3. Añadir al final de la lista sombra
    if (cabeza == nullptr) {
        cabeza = nuevo;
        cola   = nuevo;
    } else {
        cola->siguiente   = nuevo;
        nuevo->anterior   = cola;
        cola              = nuevo;
    }
    tamanio++;
    return true;
}

// ════════════════════════════════════════════════════════════
//  editar — actualiza nombre/correo/password/teléfono por ID.
//  El ID no cambia, por lo que el árbol mantiene su orden.
// ════════════════════════════════════════════════════════════
bool EstructuraClientes::editar(const Cliente& c) {
    // buscarPorId retorna el Cliente* dentro del NodoCliente de la lista,
    // al que el árbol también apunta → actualizar in-place afecta a ambos.
    Cliente* ptr = arbol.buscarPorId(c.getId());
    if (ptr == nullptr) return false;

    ptr->setNombre(c.getNombre());
    ptr->setCorreo(c.getCorreo());
    ptr->setPassword(c.getPassword());
    ptr->setTelefono(c.getTelefono());
    return true;
}

// ════════════════════════════════════════════════════════════
//  eliminar
// ════════════════════════════════════════════════════════════
bool EstructuraClientes::eliminar(const std::string& id) {
    // Primero encontrar el Cliente* para ubicar el NodoCliente en la lista
    Cliente* ptr = arbol.buscarPorId(id);
    if (ptr == nullptr) return false;

    // El NodoCliente cuyo campo 'cliente' es *ptr:
    // ptr == &(nodoCliente->cliente), así que el NodoCliente está a un
    // offset fijo usando offsetof. Sin embargo, es más seguro buscarlo
    // linealmente (la lista sombra es pequeña en práctica).
    NodoCliente* nodo = cabeza;
    while (nodo != nullptr) {
        if (&nodo->cliente == ptr) break;
        nodo = nodo->siguiente;
    }
    if (nodo == nullptr) return false;  // inconsistencia

    // 1. Eliminar del árbol (NO libera el Cliente*)
    arbol.eliminar(id);

    // 2. Quitar de la lista sombra (reduce tamanio)
    quitarDeListaSombra(nodo);

    // 3. Liberar el NodoCliente (y con él el objeto Cliente)
    delete nodo;
    return true;
}

// ════════════════════════════════════════════════════════════
//  Búsquedas
//  Se delegan al ArbolB (O(log n)) y luego se envuelven en
//  un NodoCliente* para mantener compatibilidad con la UI.
// ════════════════════════════════════════════════════════════

// Helper: dado un Cliente* del árbol, devolver el NodoCliente
// de la lista sombra que lo contiene.
static NodoCliente* nodoDesde(NodoCliente* cabeza, const Cliente* ptr) {
    NodoCliente* aux = cabeza;
    while (aux != nullptr) {
        if (&aux->cliente == ptr) return aux;
        aux = aux->siguiente;
    }
    return nullptr;
}

NodoCliente* EstructuraClientes::buscarPorId(const std::string& id) const {
    Cliente* ptr = arbol.buscarPorId(id);
    if (ptr == nullptr) return nullptr;
    return nodoDesde(cabeza, ptr);
}

NodoCliente* EstructuraClientes::buscarPorCorreo(const std::string& correo) const {
    Cliente* ptr = arbol.buscarPorCorreo(correo);
    if (ptr == nullptr) return nullptr;
    return nodoDesde(cabeza, ptr);
}

NodoCliente* EstructuraClientes::buscarPorCredenciales(
        const std::string& usuarioOCorreo,
        const std::string& password) const {
    Cliente* ptr = arbol.buscarPorCredenciales(usuarioOCorreo, password);
    if (ptr == nullptr) return nullptr;
    return nodoDesde(cabeza, ptr);
}
