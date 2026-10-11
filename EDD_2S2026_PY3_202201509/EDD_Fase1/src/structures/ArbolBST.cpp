#include "../../include/structures/ArbolBST.hpp"

ArbolBST::ArbolBST() : raiz(nullptr), contadorNodos(0) {}

ArbolBST::~ArbolBST() {
    vaciar();
}

// INSERCIÓN RECURSIVA
NodoBST* ArbolBST::insertarRec(NodoBST* nodo, const Pelicula& p) {
    if (nodo == nullptr) {
        contadorNodos++;
        return new NodoBST(p);
    }

    if (p.getCodigo() < nodo->pelicula.getCodigo()) {
        nodo->izquierdo = insertarRec(nodo->izquierdo, p);
    } else if (p.getCodigo() > nodo->pelicula.getCodigo()) {
        nodo->derecho = insertarRec(nodo->derecho, p);
    } else {
        // Código duplicado: se actualiza la información existente
        nodo->pelicula = p;
    }

    return nodo;
}

void ArbolBST::insertar(const Pelicula& pelicula) {
    raiz = insertarRec(raiz, pelicula);
}

// BÚSQUEDA RECURSIVA
NodoBST* ArbolBST::buscarRec(NodoBST* nodo, const std::string& codigo) const {
    if (nodo == nullptr || nodo->pelicula.getCodigo() == codigo) {
        return nodo;
    }

    if (codigo < nodo->pelicula.getCodigo()) {
        return buscarRec(nodo->izquierdo, codigo);
    }
    return buscarRec(nodo->derecho, codigo);
}

NodoBST* ArbolBST::buscar(const std::string& codigo) const {
    return buscarRec(raiz, codigo);
}

//  SUCESOR INORDEN (MÍNIMO DE SUBÁRBOL DERECHO) 
NodoBST* ArbolBST::obtenerMinimo(NodoBST* nodo) const {
    NodoBST* actual = nodo;
    while (actual && actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

//  ELIMINACIÓN RECURSIVA
NodoBST* ArbolBST::eliminarRec(NodoBST* nodo, const std::string& codigo, bool& eliminado) {
    if (nodo == nullptr) return nullptr;

    if (codigo < nodo->pelicula.getCodigo()) {
        nodo->izquierdo = eliminarRec(nodo->izquierdo, codigo, eliminado);
    } else if (codigo > nodo->pelicula.getCodigo()) {
        nodo->derecho = eliminarRec(nodo->derecho, codigo, eliminado);
    } else {
        // Encontrado
        eliminado = true;

        // Caso 1: Sin hijos o 1 solo hijo
        if (nodo->izquierdo == nullptr) {
            NodoBST* temp = nodo->derecho;
            delete nodo;
            contadorNodos--;
            return temp;
        } else if (nodo->derecho == nullptr) {
            NodoBST* temp = nodo->izquierdo;
            delete nodo;
            contadorNodos--;
            return temp;
        }

        // Caso 2: Con 2 hijos
        NodoBST* temp = obtenerMinimo(nodo->derecho);
        nodo->pelicula = temp->pelicula; // Copia datos del sucesor
        nodo->derecho = eliminarRec(nodo->derecho, temp->pelicula.getCodigo(), eliminado);
    }
    return nodo;
}

bool ArbolBST::eliminar(const std::string& codigo) {
    bool eliminado = false;
    raiz = eliminarRec(raiz, codigo, eliminado);
    return eliminado;
}

//  RECORRIDO INORDEN
void ArbolBST::mostrarInordenRec(NodoBST* nodo) const {
    if (nodo != nullptr) {
        mostrarInordenRec(nodo->izquierdo);

        std::cout << "  [" << nodo->pelicula.getCodigo() << "] " 
                  << nodo->pelicula.getTitulo() << " | Genero: " 
                  << nodo->pelicula.getGenero() << " | Duracion: " 
                  << nodo->pelicula.getDuracionMinutos() << " min | Clasif: " 
                  << nodo->pelicula.getClasificacion() << " | Idioma: " 
                  << nodo->pelicula.getIdioma() << "\n"
                  << "        Estreno: " << nodo->pelicula.getFechaEstreno() 
                  << " | Retiro: " << nodo->pelicula.getFechaFin() << "\n"
                  << "  --------------------------------------------------------\n";

        mostrarInordenRec(nodo->derecho);
    }
}

void ArbolBST::mostrarCartelera() const {
    if (estaVacio()) {
        std::cout << "[No hay peliculas registradas en la cartelera]\n";
        return;
    }

    std::cout << "========================================================\n";
    std::cout << "          CARTELERA DE PELICULAS (BST INORDER)         \n";
    std::cout << "========================================================\n";
    mostrarInordenRec(raiz);
    std::cout << "Total de peliculas registradas: " << contadorNodos << "\n";
    std::cout << "========================================================\n";
}

// --- ORDENAMIENTOS ---
void ArbolBST::preordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const {
    if (nodo != nullptr) {
        resultado.push_back(nodo);
        preordenRec(nodo->izquierdo, resultado);
        preordenRec(nodo->derecho, resultado);
    }
}

void ArbolBST::inordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const {
    if (nodo != nullptr) {
        inordenRec(nodo->izquierdo, resultado);
        resultado.push_back(nodo);
        inordenRec(nodo->derecho, resultado);
    }
}

void ArbolBST::postordenRec(NodoBST* nodo, std::vector<NodoBST*>& resultado) const {
    if (nodo != nullptr) {
        postordenRec(nodo->izquierdo, resultado);
        postordenRec(nodo->derecho, resultado);
        resultado.push_back(nodo);
    }
}

void ArbolBST::recorridoPreorden(std::vector<NodoBST*>& resultado) const {
    preordenRec(raiz, resultado);
}

void ArbolBST::recorridoInorden(std::vector<NodoBST*>& resultado) const {
    inordenRec(raiz, resultado);
}

void ArbolBST::recorridoPostorden(std::vector<NodoBST*>& resultado) const {
    postordenRec(raiz, resultado);
}

// --- VACIEDAD Y LIBERACIÓN DE MEMORIA ---
void ArbolBST::destruirRec(NodoBST* nodo) {
    if (nodo != nullptr) {
        destruirRec(nodo->izquierdo);
        destruirRec(nodo->derecho);
        delete nodo;
    }
}

void ArbolBST::vaciar() {
    destruirRec(raiz);
    raiz = nullptr;
    contadorNodos = 0;
}