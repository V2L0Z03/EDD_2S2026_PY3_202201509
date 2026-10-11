#ifndef ESTRUCTURA_CLIENTES_HPP
#define ESTRUCTURA_CLIENTES_HPP

#include "../models/Cliente.hpp"
#include "ArbolB.hpp"

//  NodoCliente — nodo de la lista sombra doblemente enlazada.
//  La lista sombra permite getCabeza() / getCola() en O(1)
//  y es la propietaria real de los objetos Cliente.
//  El ArbolB almacena punteros NO propietarios a esos objetos.
struct NodoCliente {
    Cliente      cliente;
    NodoCliente* siguiente;
    NodoCliente* anterior;

    explicit NodoCliente(const Cliente& c)
        : cliente(c), siguiente(nullptr), anterior(nullptr) {}
};


class EstructuraClientes {
private:
    ArbolB       arbol;    // índice B-Treepunteros no propietarios
    NodoCliente* cabeza;
    NodoCliente* cola;
    int          tamanio;

    // acaElimina un NodoCliente de la lista sombra sin tocar el árbol.
    void quitarDeListaSombra(NodoCliente* nodo);

public:
    EstructuraClientes();
    ~EstructuraClientes();

    // Sin copia (el árbol no es copiable).
    EstructuraClientes(const EstructuraClientes&)            = delete;
    EstructuraClientes& operator=(const EstructuraClientes&) = delete;

    // Mutadores
    bool insertar(const Cliente& c);
    bool editar(const Cliente& c);       // por ID; no modifica el ID
    bool eliminar(const std::string& id);
    void limpiar();

    //  Búsquedas (delegadas al ArbolB)
    NodoCliente* buscarPorId(const std::string& id) const;
    NodoCliente* buscarPorCorreo(const std::string& correo) const;
    NodoCliente* buscarPorCredenciales(const std::string& usuarioOCorreo,
                                       const std::string& password) const;

    //  Acceso a la lista sombra (para iteración en UI / reportes)
    int          getTamanio() const { return tamanio; }
    bool         estaVacia()  const { return cabeza == nullptr; }
    NodoCliente* getCabeza()  const { return cabeza; }
    NodoCliente* getCola()    const { return cola; }

    // Acceso al ArbolB (para el reporte Graphviz)
    const ArbolB& getArbol() const { return arbol; }
};

#endif // ESTRUCTURA_CLIENTES_HPP
