#ifndef ARBOL_BST_HPP
#define ARBOL_BST_HPP

#include "../models/Pelicula.hpp"
#include "MatrizDispersa.hpp"
#include <iostream>
#include <vector>

struct NodoBST {
    Pelicula pelicula;
    MatrizDispersa salaCine;
    NodoBST* izquierdo;
    NodoBST* derecho;

    NodoBST(const Pelicula& p)
        : pelicula(p), izquierdo(nullptr), derecho(nullptr) {
            salaCine.inicializarDimensiones(5, 5); // Por defecto 5x5
        }
};

class ArbolBST {
private:
    NodoBST* raiz;
    int contadorNodos;

    // Métodos auxiliares recursivos para recorridos con vector (ordenamientos)
    void preordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const;
    void inordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const;
    void postordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const;


    // Métodos auxiliares recursivos
    NodoBST* insertarRec(NodoBST* nodo, const Pelicula& p);
    NodoBST* buscarRec(NodoBST* nodo, const std::string& codigo) const;
    NodoBST* eliminarRec(NodoBST* nodo, const std::string& codigo, bool& eliminado);
    NodoBST* obtenerMinimo(NodoBST* nodo) const;
    void mostrarInordenRec(NodoBST* nodo) const;
    void destruirRec(NodoBST* nodo);

public:
    ArbolBST();
    ~ArbolBST();

    void insertar(const Pelicula& pelicula);
    bool eliminar(const std::string& codigo);
    NodoBST* buscar(const std::string& codigo) const;
    void mostrarCartelera() const;
    void vaciar();

    // Métodos públicos para recolección en vector (ordenamientos)
    void recorridoPreorden(std::vector<NodoBST*>& resultado) const;
    void recorridoInorden(std::vector<NodoBST*>& resultado) const;
    void recorridoPostorden(std::vector<NodoBST*>& resultado) const;

    bool estaVacio() const { return raiz == nullptr; }
    int getTamanio() const { return contadorNodos; }
    NodoBST* getRaiz() const { return raiz; }
};

#endif // ARBOL_BST_HPP