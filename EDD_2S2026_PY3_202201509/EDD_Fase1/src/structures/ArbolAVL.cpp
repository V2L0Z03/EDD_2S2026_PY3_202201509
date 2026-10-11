#include "../../include/structures/ArbolAVL.hpp"

ArbolAVL::ArbolAVL() : raiz(nullptr), contadorNodos(0) {}

ArbolAVL::~ArbolAVL() {
    vaciar();
}

int ArbolAVL::getAltura(NodoAVL* nodo) const {
    if (nodo == nullptr) return 0;
    return nodo->altura;
}

int ArbolAVL::getFactorEquilibrio(NodoAVL* nodo) const {
    if (nodo == nullptr) return 0;
    return getAltura(nodo->izquierdo) - getAltura(nodo->derecho);
}

void ArbolAVL::actualizarAltura(NodoAVL* nodo) {
    if (nodo != nullptr) {
        nodo->altura = 1 + std::max(getAltura(nodo->izquierdo), getAltura(nodo->derecho));
    }
}

// ─────────────────────────────────────────────────────────────
// 4 ROTACIONES MANUALES OBLIGATORIAS
// ─────────────────────────────────────────────────────────────

// 1. Rotación Simple Izquierda (RR) - cuando el subárbol derecho está desbalanceado hacia la derecha
NodoAVL* ArbolAVL::rotacionIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* T2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = T2;

    actualizarAltura(x);
    actualizarAltura(y);

    return y;
}

// 2. Rotación Simple Derecha (LL) - cuando el subárbol izquierdo está desbalanceado hacia la izquierda
NodoAVL* ArbolAVL::rotacionDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* T2 = x->derecho;

    x->derecho = y;
    y->izquierdo = T2;

    actualizarAltura(y);
    actualizarAltura(x);

    return x;
}

// 3. Rotación Doble Izquierda-Derecha (LR) - hijo izquierdo es pesado a la derecha
NodoAVL* ArbolAVL::rotacionIzquierdaDerecha(NodoAVL* n) {
    n->izquierdo = rotacionIzquierda(n->izquierdo);
    return rotacionDerecha(n);
}

// 4. Rotación Doble Derecha-Izquierda (RL) - hijo derecho es pesado a la izquierda
NodoAVL* ArbolAVL::rotacionDerechaIzquierda(NodoAVL* n) {
    n->derecho = rotacionDerecha(n->derecho);
    return rotacionIzquierda(n);
}

// ─────────────────────────────────────────────────────────────
// BALANCEO DE UN NODO
// ─────────────────────────────────────────────────────────────
NodoAVL* ArbolAVL::balancear(NodoAVL* nodo) {
    if (nodo == nullptr) return nullptr;

    actualizarAltura(nodo);
    int fe = getFactorEquilibrio(nodo);

    // Izquierda Pesada (FE > 1)
    if (fe > 1) {
        if (getFactorEquilibrio(nodo->izquierdo) >= 0) {
            return rotacionDerecha(nodo); // LL / Single Right
        } else {
            nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
            return rotacionDerecha(nodo); // LR / Double Left-Right
        }
    }

    // Derecha Pesada (FE < -1)
    if (fe < -1) {
        if (getFactorEquilibrio(nodo->derecho) <= 0) {
            return rotacionIzquierda(nodo); // RR / Single Left
        } else {
            nodo->derecho = rotacionDerecha(nodo->derecho);
            return rotacionIzquierda(nodo); // RL / Double Right-Left
        }
    }

    return nodo;
}

// ─────────────────────────────────────────────────────────────
// INSERCIÓN BALANCEADA
// ─────────────────────────────────────────────────────────────
NodoAVL* ArbolAVL::insertarRec(NodoAVL* nodo, const Funcion& f) {
    if (nodo == nullptr) {
        contadorNodos++;
        return new NodoAVL(f);
    }

    const std::string& codigo = f.getCodigo();

    if (codigo < nodo->funcion.getCodigo()) {
        nodo->izquierdo = insertarRec(nodo->izquierdo, f);
    } else if (codigo > nodo->funcion.getCodigo()) {
        nodo->derecho = insertarRec(nodo->derecho, f);
    } else {
        // Clave existente / duplicada: actualizar los datos de la función
        nodo->funcion = f;
        return nodo;
    }

    // 1. Actualizar la altura del nodo actual
    nodo->altura = 1 + std::max(obtenerAltura(nodo->izquierdo), obtenerAltura(nodo->derecho));

    // 2. Obtener el Factor de Equilibrio (FE = Altura_Izq - Altura_Der)
    int fe = obtenerFactorEquilibrio(nodo);

    // 3. Verificación de las 4 rotaciones al insertar según la clave:

    // CASO LL (Rotación Derecha)
    if (fe > 1 && nodo->izquierdo != nullptr && codigo < nodo->izquierdo->funcion.getCodigo()) {
        return rotacionDerecha(nodo);
    }

    // CASO RR (Rotación Izquierda)
    if (fe < -1 && nodo->derecho != nullptr && codigo > nodo->derecho->funcion.getCodigo()) {
        return rotacionIzquierda(nodo);
    }

    // CASO LR (Doble Izquierda-Derecha)
    if (fe > 1 && nodo->izquierdo != nullptr && codigo > nodo->izquierdo->funcion.getCodigo()) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    // CASO RL (Doble Derecha-Izquierda)
    if (fe < -1 && nodo->derecho != nullptr && codigo < nodo->derecho->funcion.getCodigo()) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

bool ArbolAVL::insertar(const Funcion& funcion) {
    if (funcion.getCodigo().empty()) return false;
    this->raiz = insertarRec(this->raiz, funcion);
    return true;
}

// ─────────────────────────────────────────────────────────────
// ELIMINACIÓN BALANCEADA
// ─────────────────────────────────────────────────────────────
NodoAVL* ArbolAVL::obtenerMinimo(NodoAVL* nodo) const {
    NodoAVL* actual = nodo;
    while (actual != nullptr && actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

NodoAVL* ArbolAVL::eliminarRec(NodoAVL* nodo, const std::string& codigo, bool& eliminado) {
    if (nodo == nullptr) {
        eliminado = false;
        return nullptr;
    }

    if (codigo < nodo->funcion.getCodigo()) {
        nodo->izquierdo = eliminarRec(nodo->izquierdo, codigo, eliminado);
    } else if (codigo > nodo->funcion.getCodigo()) {
        nodo->derecho = eliminarRec(nodo->derecho, codigo, eliminado);
    } else {
        // Encontrado
        eliminado = true;
        contadorNodos--;

        if (nodo->izquierdo == nullptr || nodo->derecho == nullptr) {
            NodoAVL* temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;
            if (temp == nullptr) {
                // Sin hijos
                temp = nodo;
                nodo = nullptr;
            } else {
                // Un hijo
                *nodo = *temp;
            }
            delete temp;
        } else {
            // Dos hijos: obtener el sucesor inorden (mínimo del subárbol derecho)
            NodoAVL* temp = obtenerMinimo(nodo->derecho);
            nodo->funcion = temp->funcion;
            bool dummy = false;
            nodo->derecho = eliminarRec(nodo->derecho, temp->funcion.getCodigo(), dummy);
        }
    }

    if (nodo == nullptr) return nullptr;

    return balancear(nodo);
}

bool ArbolAVL::eliminar(const std::string& codigo) {
    bool eliminado = false;
    raiz = eliminarRec(raiz, codigo, eliminado);
    return eliminado;
}

// ─────────────────────────────────────────────────────────────
// BÚSQUEDA Y EDICIÓN
// ─────────────────────────────────────────────────────────────
NodoAVL* ArbolAVL::buscarRec(NodoAVL* nodo, const std::string& codigo) const {
    if (nodo == nullptr || nodo->funcion.getCodigo() == codigo) {
        return nodo;
    }
    if (codigo < nodo->funcion.getCodigo()) {
        return buscarRec(nodo->izquierdo, codigo);
    }
    return buscarRec(nodo->derecho, codigo);
}

NodoAVL* ArbolAVL::buscar(const std::string& codigo) const {
    return buscarRec(raiz, codigo);
}

bool ArbolAVL::editar(const Funcion& funcion) {
    NodoAVL* nodo = buscar(funcion.getCodigo());
    if (nodo != nullptr) {
        nodo->funcion = funcion;
        return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────
// RECORRIDOS
// ─────────────────────────────────────────────────────────────
void ArbolAVL::preordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const {
    if (nodo == nullptr) return;
    fn(nodo);
    preordenRec(nodo->izquierdo, fn);
    preordenRec(nodo->derecho, fn);
}

void ArbolAVL::inordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const {
    if (nodo == nullptr) return;
    inordenRec(nodo->izquierdo, fn);
    fn(nodo);
    inordenRec(nodo->derecho, fn);
}

void ArbolAVL::postordenRec(NodoAVL* nodo, const std::function<void(NodoAVL*)>& fn) const {
    if (nodo == nullptr) return;
    postordenRec(nodo->izquierdo, fn);
    postordenRec(nodo->derecho, fn);
    fn(nodo);
}

void ArbolAVL::recorridoPreorden(const std::function<void(NodoAVL*)>& fn) const {
    preordenRec(raiz, fn);
}

void ArbolAVL::recorridoInorden(const std::function<void(NodoAVL*)>& fn) const {
    inordenRec(raiz, fn);
}

void ArbolAVL::recorridoPostorden(const std::function<void(NodoAVL*)>& fn) const {
    postordenRec(raiz, fn);
}

void ArbolAVL::preordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const {
    if (nodo == nullptr) return;
    resultado.push_back(nodo);
    preordenVectorRec(nodo->izquierdo, resultado);
    preordenVectorRec(nodo->derecho, resultado);
}

void ArbolAVL::inordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const {
    if (nodo == nullptr) return;
    inordenVectorRec(nodo->izquierdo, resultado);
    resultado.push_back(nodo);
    inordenVectorRec(nodo->derecho, resultado);
}

void ArbolAVL::postordenVectorRec(NodoAVL* nodo, std::vector<NodoAVL*>& resultado) const {
    if (nodo == nullptr) return;
    postordenVectorRec(nodo->izquierdo, resultado);
    postordenVectorRec(nodo->derecho, resultado);
    resultado.push_back(nodo);
}

void ArbolAVL::obtenerPreorden(std::vector<NodoAVL*>& resultado) const {
    preordenVectorRec(raiz, resultado);
}

void ArbolAVL::obtenerInorden(std::vector<NodoAVL*>& resultado) const {
    inordenVectorRec(raiz, resultado);
}

void ArbolAVL::obtenerPostorden(std::vector<NodoAVL*>& resultado) const {
    postordenVectorRec(raiz, resultado);
}

// ─────────────────────────────────────────────────────────────
// VACIAR Y DESTRUCTOR
// ─────────────────────────────────────────────────────────────
void ArbolAVL::destruirRec(NodoAVL* nodo) {
    if (nodo == nullptr) return;
    destruirRec(nodo->izquierdo);
    destruirRec(nodo->derecho);
    delete nodo;
}

void ArbolAVL::vaciar() {
    destruirRec(raiz);
    raiz = nullptr;
    contadorNodos = 0;
}
