#ifndef ARBOL_B_HPP
#define ARBOL_B_HPP

#include <string>
#include "../models/Cliente.hpp"


// ────────────────────────────────────────────────────────────
static const int ORDEN_B      = 4;
static const int MAX_CLAVES_B = 3;  // M - 1
static const int MAX_HIJOS_B  = 4;  // M

// Nodo del Árbol B 
// Las claves son punteros NO propietarios (la memoria del
// Cliente la gestiona EstructuraClientes a través de su
// lista sombra de NodoCliente
struct NodoArbolB {
    Cliente*    claves[MAX_CLAVES_B];
    NodoArbolB* hijos[MAX_HIJOS_B];
    int         numClaves;
    bool        esHoja;

    explicit NodoArbolB(bool hoja = true);
};

//  Clase ArbolB 
class ArbolB {
private:
    NodoArbolB* raiz;
    int         numClientes;

    // helpers inserción 
    void insertarNoLleno(NodoArbolB* nodo, Cliente* cli);
    void dividirHijo(NodoArbolB* padre, int i, NodoArbolB* hijoLleno);

    //helpers búsqueda 
    Cliente* buscarHelper(NodoArbolB* nodo, const std::string& id) const;
    Cliente* buscarPorCorreoHelper(NodoArbolB* nodo, const std::string& correo) const;
    void     buscarCredencialesHelper(NodoArbolB* nodo,
                                      const std::string& userOrEmail,
                                      const std::string& pass,
                                      Cliente*& resultado) const;

    // helpers eliminación
    bool eliminarHelper(NodoArbolB* nodo, const std::string& id);
    void eliminarDeHoja(NodoArbolB* nodo, int idx);
    void eliminarDeNoHoja(NodoArbolB* nodo, int idx);
    Cliente* obtenerPredecesor(NodoArbolB* nodo, int idx) const;
    Cliente* obtenerSucesor(NodoArbolB* nodo, int idx) const;
    void llenar(NodoArbolB* nodo, int idx);
    void tomarDelAnterior(NodoArbolB* nodo, int idx);
    void tomarDelSiguiente(NodoArbolB* nodo, int idx);
    void fusionar(NodoArbolB* nodo, int idx);

    // recorrido 
    void inordenHelper(NodoArbolB* nodo,
                       void (*cb)(Cliente*, void*), void* ctx) const;

    // ── limpiar (Solo borra NodoArbolB, NO los Cliente*)
    void limpiarNodos(NodoArbolB* nodo);

public:
    ArbolB();
    ~ArbolB();

    // No copiar (el árbol no es propietario de los Cliente*)
    ArbolB(const ArbolB&)            = delete;
    ArbolB& operator=(const ArbolB&) = delete;

    // aca inserto puntero no propietario. false si el ID ya existe.
    bool insertar(Cliente* cli);

    // Búsquedas
    Cliente* buscarPorId(const std::string& id) const;
    Cliente* buscarPorCorreo(const std::string& correo) const;
    Cliente* buscarPorCredenciales(const std::string& userOrEmail,
                                   const std::string& pass) const;

    // Elimina la clave del árbol (NO libera el Cliente*).
    // Retorna true si se encontró y eliminó.
    bool eliminar(const std::string& id);

    // Recorrido inorden: llama cb(cliente, ctx) para cada cliente.
    void recorridoInorden(void (*cb)(Cliente*, void*), void* ctx) const;

    // pa Liberar todos los NodoArbolB (NO los objetos Cliente*).
    void limpiar();

    bool        estaVacio()    const { return raiz == nullptr; }
    int         getTamano()    const { return numClientes; }
    NodoArbolB* getRaiz()      const { return raiz; }
};

#endif // ARBOL_B_HPP
