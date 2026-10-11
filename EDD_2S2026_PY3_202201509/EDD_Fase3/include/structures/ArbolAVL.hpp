#ifndef ARBOL_AVL_HPP
#define ARBOL_AVL_HPP

#include "../models/Funcion.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>

struct NodoAVL {
    Funcion funcion;
    int altura;
    NodoAVL* izquierdo;
    NodoAVL* derecho;

    NodoAVL(const Funcion& f)
        : funcion(f), altura(1), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolAVL {
private:
    NodoAVL* raiz;
    int contadorNodos;

    // estas son mis Rotaciones manuales obligatorias
    NodoAVL* rotacionIzquierda(NodoAVL* x);         // Simple Izquierda (RR)
    NodoAVL* rotacionDerecha(NodoAVL* y);           // Simple Derecha (LL)
    NodoAVL* rotacionIzquierdaDerecha(NodoAVL* n);  // Doble Izquierda-Derecha (LR)
    NodoAVL* rotacionDerechaIzquierda(NodoAVL* n);  // Doble Derecha-Izquierda (RL)

    // estos son mismétodos recursivos
    NodoAVL* balancear(NodoAVL* nodo);
    NodoAVL* insertarRec(NodoAVL* nodo, const Funcion& f);
    NodoAVL* eliminarRec(NodoAVL* nodo, const std::string& codigo, bool& eliminado);
    NodoAVL* buscarRec(NodoAVL* nodo, const std::string& codigo) const;
    NodoAVL* obtenerMinimo(NodoAVL* nodo) const;

    void preordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const;
    void inordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const;
    void postordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const;
    
    void preordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const;
    void inordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const;
    void postordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const;

    void destruirRec(NodoAVL* nodo);

public:
    ArbolAVL();
    ~ArbolAVL();

    // Auxiliares de altura y factor de equilibrio
    int getAltura(NodoAVL* nodo) const;
    int getFactorEquilibrio(NodoAVL* nodo) const;
    int obtenerAltura(NodoAVL* nodo) const { return getAltura(nodo); }
    int obtenerFactorEquilibrio(NodoAVL* nodo) const { return getFactorEquilibrio(nodo); }
    void actualizarAltura(NodoAVL* nodo);

    // Operaciones principales
    bool insertar(const Funcion& funcion);
    bool eliminar(const std::string& codigo);
    NodoAVL* buscar(const std::string& codigo) const;
    bool editar(const Funcion& funcion);
    void vaciar();

    // Recorridos
    void recorridoPreorden(const std::function<void(NodoAVL*)>& fn) const;
    void recorridoInorden(const std::function<void(NodoAVL*)>& fn) const;
    void recorridoPostorden(const std::function<void(NodoAVL*)>& fn) const;

    void obtenerPreorden(std::vector<NodoAVL*>& resultado) const;
    void obtenerInorden(std::vector<NodoAVL*>& resultado) const;
    void obtenerPostorden(std::vector<NodoAVL*>& resultado) const;

    // Métodos de estado
    bool estaVacio() const { return raiz == nullptr; }
    int getTamanio() const { return contadorNodos; }
    NodoAVL* getRaiz() const { return raiz; }
    int getAlturaArbol() const { return getAltura(raiz); }
};

#endif // ARBOL_AVL_HPP
