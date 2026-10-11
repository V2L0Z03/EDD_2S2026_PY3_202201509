#include "../../include/structures/ArbolB.hpp"
#include <iostream>

// ════════════════════════════════════════════════════════════
//  NodoArbolB
// ════════════════════════════════════════════════════════════
NodoArbolB::NodoArbolB(bool hoja) : numClaves(0), esHoja(hoja) {
    for (int i = 0; i < MAX_CLAVES_B; i++) claves[i] = nullptr;
    for (int i = 0; i < MAX_HIJOS_B;  i++) hijos[i]  = nullptr;
}

// ════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ════════════════════════════════════════════════════════════
ArbolB::ArbolB() : raiz(nullptr), numClientes(0) {}

ArbolB::~ArbolB() { limpiar(); }

// ════════════════════════════════════════════════════════════
//  limpiar — solo destruye NodoArbolB, NO los Cliente*
// ════════════════════════════════════════════════════════════
void ArbolB::limpiarNodos(NodoArbolB* nodo) {
    if (nodo == nullptr) return;
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; i++)
            limpiarNodos(nodo->hijos[i]);
    }
    delete nodo;
}

void ArbolB::limpiar() {
    limpiarNodos(raiz);
    raiz        = nullptr;
    numClientes = 0;
}

// ════════════════════════════════════════════════════════════
//  INSERCIÓN
// ════════════════════════════════════════════════════════════

// Divide hijos[i] (que está lleno con 3 claves) promoviendo la
// clave mediana (índice 1) al padre.
void ArbolB::dividirHijo(NodoArbolB* padre, int i, NodoArbolB* hijoLleno) {
    // hijoLleno tiene 3 claves: [k0, k1, k2]  y  4 hijos: [c0,c1,c2,c3]
    // Mediana = k1 (índice 1)
    // hijoLleno se queda con [k0] y [c0,c1]
    // nuevo    recibe  [k2] y [c2,c3]

    NodoArbolB* nuevo = new NodoArbolB(hijoLleno->esHoja);
    nuevo->claves[0]  = hijoLleno->claves[2];
    nuevo->numClaves  = 1;

    if (!hijoLleno->esHoja) {
        nuevo->hijos[0]       = hijoLleno->hijos[2];
        nuevo->hijos[1]       = hijoLleno->hijos[3];
        hijoLleno->hijos[2]   = nullptr;
        hijoLleno->hijos[3]   = nullptr;
    }

    // Guardar mediana antes de borrarla de hijoLleno
    Cliente* mediana      = hijoLleno->claves[1];
    hijoLleno->claves[1]  = nullptr;
    hijoLleno->claves[2]  = nullptr;
    hijoLleno->numClaves  = 1;

    // Abrir hueco en padre para el nuevo hijo (en posición i+1)
    for (int j = padre->numClaves; j > i; j--)
        padre->hijos[j + 1] = padre->hijos[j];
    padre->hijos[i + 1] = nuevo;

    // Abrir hueco en padre para la mediana (en posición i)
    for (int j = padre->numClaves - 1; j >= i; j--)
        padre->claves[j + 1] = padre->claves[j];
    padre->claves[i] = mediana;
    padre->numClaves++;
}

// Inserta cli en el subárbol enraizado en nodo, que NO está lleno.
void ArbolB::insertarNoLleno(NodoArbolB* nodo, Cliente* cli) {
    int i = nodo->numClaves - 1;

    if (nodo->esHoja) {
        // Desplazar claves mayores hacia la derecha y colocar
        while (i >= 0 && cli->getId() < nodo->claves[i]->getId()) {
            nodo->claves[i + 1] = nodo->claves[i];
            i--;
        }
        nodo->claves[i + 1] = cli;
        nodo->numClaves++;
    } else {
        // Encontrar el hijo correcto
        while (i >= 0 && cli->getId() < nodo->claves[i]->getId())
            i--;
        i++;

        // Si el hijo está lleno, dividirlo primero
        if (nodo->hijos[i]->numClaves == MAX_CLAVES_B) {
            dividirHijo(nodo, i, nodo->hijos[i]);
            // Decidir en cuál de los dos hijos resultantes descender
            if (cli->getId() > nodo->claves[i]->getId())
                i++;
        }
        insertarNoLleno(nodo->hijos[i], cli);
    }
}

bool ArbolB::insertar(Cliente* cli) {
    if (cli == nullptr || cli->getId().empty()) return false;

    // Duplicado
    if (buscarPorId(cli->getId()) != nullptr) return false;

    if (raiz == nullptr) {
        raiz             = new NodoArbolB(true);
        raiz->claves[0]  = cli;
        raiz->numClaves  = 1;
        numClientes++;
        return true;
    }

    if (raiz->numClaves == MAX_CLAVES_B) {
        // Raíz llena: crear nueva raíz y dividir
        NodoArbolB* nuevaRaiz = new NodoArbolB(false);
        nuevaRaiz->hijos[0]   = raiz;
        dividirHijo(nuevaRaiz, 0, raiz);
        raiz = nuevaRaiz;
    }

    insertarNoLleno(raiz, cli);
    numClientes++;
    return true;
}

// ════════════════════════════════════════════════════════════
//  BÚSQUEDAS
// ════════════════════════════════════════════════════════════
Cliente* ArbolB::buscarHelper(NodoArbolB* nodo, const std::string& id) const {
    if (nodo == nullptr) return nullptr;

    int i = 0;
    while (i < nodo->numClaves && id > nodo->claves[i]->getId())
        i++;

    if (i < nodo->numClaves && id == nodo->claves[i]->getId())
        return nodo->claves[i];

    if (nodo->esHoja) return nullptr;
    return buscarHelper(nodo->hijos[i], id);
}

Cliente* ArbolB::buscarPorId(const std::string& id) const {
    return buscarHelper(raiz, id);
}

// Inorden para buscar por correo (O(n))
Cliente* ArbolB::buscarPorCorreoHelper(NodoArbolB* nodo,
                                        const std::string& correo) const {
    if (nodo == nullptr) return nullptr;

    for (int i = 0; i < nodo->numClaves; i++) {
        // Hijo izquierdo del slot i
        if (!nodo->esHoja) {
            Cliente* res = buscarPorCorreoHelper(nodo->hijos[i], correo);
            if (res) return res;
        }
        if (nodo->claves[i]->getCorreo() == correo)
            return nodo->claves[i];
    }
    // Hijo más a la derecha
    if (!nodo->esHoja)
        return buscarPorCorreoHelper(nodo->hijos[nodo->numClaves], correo);
    return nullptr;
}

Cliente* ArbolB::buscarPorCorreo(const std::string& correo) const {
    return buscarPorCorreoHelper(raiz, correo);
}

void ArbolB::buscarCredencialesHelper(NodoArbolB* nodo,
                                       const std::string& userOrEmail,
                                       const std::string& pass,
                                       Cliente*& resultado) const {
    if (nodo == nullptr || resultado != nullptr) return;

    for (int i = 0; i < nodo->numClaves; i++) {
        if (!nodo->esHoja)
            buscarCredencialesHelper(nodo->hijos[i], userOrEmail, pass, resultado);
        if (resultado) return;

        Cliente* c = nodo->claves[i];
        bool matchUser = (c->getCorreo() == userOrEmail || c->getId() == userOrEmail);
        if (matchUser && c->getPassword() == pass) {
            resultado = c;
            return;
        }
    }
    if (!nodo->esHoja)
        buscarCredencialesHelper(nodo->hijos[nodo->numClaves], userOrEmail, pass, resultado);
}

Cliente* ArbolB::buscarPorCredenciales(const std::string& userOrEmail,
                                        const std::string& pass) const {
    Cliente* resultado = nullptr;
    buscarCredencialesHelper(raiz, userOrEmail, pass, resultado);
    return resultado;
}

// ════════════════════════════════════════════════════════════
//  RECORRIDO INORDEN
// ════════════════════════════════════════════════════════════
void ArbolB::inordenHelper(NodoArbolB* nodo,
                            void (*cb)(Cliente*, void*), void* ctx) const {
    if (nodo == nullptr) return;
    for (int i = 0; i < nodo->numClaves; i++) {
        if (!nodo->esHoja)
            inordenHelper(nodo->hijos[i], cb, ctx);
        cb(nodo->claves[i], ctx);
    }
    if (!nodo->esHoja)
        inordenHelper(nodo->hijos[nodo->numClaves], cb, ctx);
}

void ArbolB::recorridoInorden(void (*cb)(Cliente*, void*), void* ctx) const {
    inordenHelper(raiz, cb, ctx);
}

// ════════════════════════════════════════════════════════════
//  ELIMINACIÓN
// ════════════════════════════════════════════════════════════

Cliente* ArbolB::obtenerPredecesor(NodoArbolB* nodo, int idx) const {
    NodoArbolB* curr = nodo->hijos[idx];
    while (!curr->esHoja)
        curr = curr->hijos[curr->numClaves];
    return curr->claves[curr->numClaves - 1];
}

Cliente* ArbolB::obtenerSucesor(NodoArbolB* nodo, int idx) const {
    NodoArbolB* curr = nodo->hijos[idx + 1];
    while (!curr->esHoja)
        curr = curr->hijos[0];
    return curr->claves[0];
}

// Rota desde el hijo izquierdo (idx-1): baja una clave del padre
// a hijos[idx] y sube la clave más a la derecha de hijos[idx-1].
void ArbolB::tomarDelAnterior(NodoArbolB* nodo, int idx) {
    NodoArbolB* hijo     = nodo->hijos[idx];
    NodoArbolB* hermano  = nodo->hijos[idx - 1];

    // Desplazar claves/hijos de hijo hacia la derecha
    for (int j = hijo->numClaves - 1; j >= 0; j--)
        hijo->claves[j + 1] = hijo->claves[j];
    if (!hijo->esHoja) {
        for (int j = hijo->numClaves; j >= 0; j--)
            hijo->hijos[j + 1] = hijo->hijos[j];
    }

    // Bajar clave del padre
    hijo->claves[0] = nodo->claves[idx - 1];
    if (!hijo->esHoja)
        hijo->hijos[0] = hermano->hijos[hermano->numClaves];

    hijo->numClaves++;

    // Subir clave más derecha del hermano al padre
    nodo->claves[idx - 1] = hermano->claves[hermano->numClaves - 1];
    hermano->claves[hermano->numClaves - 1] = nullptr;
    if (!hermano->esHoja)
        hermano->hijos[hermano->numClaves] = nullptr;
    hermano->numClaves--;
}

// Rota desde el hijo derecho (idx+1): baja clave del padre a
// hijos[idx] y sube clave más a la izquierda de hijos[idx+1].
void ArbolB::tomarDelSiguiente(NodoArbolB* nodo, int idx) {
    NodoArbolB* hijo    = nodo->hijos[idx];
    NodoArbolB* hermano = nodo->hijos[idx + 1];

    // Añadir clave del padre al final de hijo
    hijo->claves[hijo->numClaves] = nodo->claves[idx];
    if (!hijo->esHoja)
        hijo->hijos[hijo->numClaves + 1] = hermano->hijos[0];
    hijo->numClaves++;

    // Subir clave más izquierda del hermano al padre
    nodo->claves[idx] = hermano->claves[0];

    // Desplazar claves/hijos del hermano hacia la izquierda
    for (int j = 1; j < hermano->numClaves; j++)
        hermano->claves[j - 1] = hermano->claves[j];
    if (!hermano->esHoja) {
        for (int j = 1; j <= hermano->numClaves; j++)
            hermano->hijos[j - 1] = hermano->hijos[j];
    }
    hermano->claves[hermano->numClaves - 1] = nullptr;
    if (!hermano->esHoja)
        hermano->hijos[hermano->numClaves] = nullptr;
    hermano->numClaves--;
}

// Fusiona hijos[idx] y hijos[idx+1] bajando claves[idx] como mediana.
// Resultado en hijos[idx]; hijos[idx+1] es deleted.
void ArbolB::fusionar(NodoArbolB* nodo, int idx) {
    NodoArbolB* hijo    = nodo->hijos[idx];
    NodoArbolB* hermano = nodo->hijos[idx + 1];

    // Bajar mediana del padre a hijo (posición hijo->numClaves)
    hijo->claves[hijo->numClaves] = nodo->claves[idx];

    // Copiar claves y hijos del hermano a hijo
    for (int j = 0; j < hermano->numClaves; j++)
        hijo->claves[hijo->numClaves + 1 + j] = hermano->claves[j];
    if (!hijo->esHoja) {
        for (int j = 0; j <= hermano->numClaves; j++)
            hijo->hijos[hijo->numClaves + 1 + j] = hermano->hijos[j];
    }
    hijo->numClaves += 1 + hermano->numClaves;

    // Eliminar mediana y puntero a hermano del padre
    for (int j = idx; j < nodo->numClaves - 1; j++)
        nodo->claves[j] = nodo->claves[j + 1];
    for (int j = idx + 1; j < nodo->numClaves; j++)
        nodo->hijos[j] = nodo->hijos[j + 1];

    nodo->claves[nodo->numClaves - 1] = nullptr;
    nodo->hijos[nodo->numClaves]       = nullptr;
    nodo->numClaves--;

    // Borrar el nodo hermano (solo el nodo, no los Cliente*)
    delete hermano;
}

// Garantiza que hijos[idx] tenga al menos 2 claves antes de descender.
void ArbolB::llenar(NodoArbolB* nodo, int idx) {
    if (idx != 0 && nodo->hijos[idx - 1]->numClaves > 1) {
        tomarDelAnterior(nodo, idx);
    } else if (idx != nodo->numClaves && nodo->hijos[idx + 1]->numClaves > 1) {
        tomarDelSiguiente(nodo, idx);
    } else {
        // Fusionar con un hermano
        if (idx != nodo->numClaves)
            fusionar(nodo, idx);
        else
            fusionar(nodo, idx - 1);
    }
}

void ArbolB::eliminarDeHoja(NodoArbolB* nodo, int idx) {
    for (int i = idx; i < nodo->numClaves - 1; i++)
        nodo->claves[i] = nodo->claves[i + 1];
    nodo->claves[nodo->numClaves - 1] = nullptr;
    nodo->numClaves--;
}

void ArbolB::eliminarDeNoHoja(NodoArbolB* nodo, int idx) {
    // La clave a eliminar; no liberar aquí (propietario es EstructuraClientes)
    const std::string idClave = nodo->claves[idx]->getId();

    if (nodo->hijos[idx]->numClaves > 1) {
        // Caso 2a: predecesor
        Cliente* pred      = obtenerPredecesor(nodo, idx);
        nodo->claves[idx]  = pred;
        eliminarHelper(nodo->hijos[idx], pred->getId());
    } else if (nodo->hijos[idx + 1]->numClaves > 1) {
        // Caso 2b: sucesor
        Cliente* suc       = obtenerSucesor(nodo, idx);
        nodo->claves[idx]  = suc;
        eliminarHelper(nodo->hijos[idx + 1], suc->getId());
    } else {
        // Caso 2c: fusionar con hijo derecho, luego eliminar
        fusionar(nodo, idx);
        // Después de fusionar, la clave está en hijos[idx]
        eliminarHelper(nodo->hijos[idx], idClave);
    }
}

bool ArbolB::eliminarHelper(NodoArbolB* nodo, const std::string& id) {
    if (nodo == nullptr) return false;

    // Encontrar primer índice donde claves[i]->getId() >= id
    int i = 0;
    while (i < nodo->numClaves && id > nodo->claves[i]->getId())
        i++;

    if (i < nodo->numClaves && id == nodo->claves[i]->getId()) {
        // Clave encontrada en este nodo
        if (nodo->esHoja)
            eliminarDeHoja(nodo, i);
        else
            eliminarDeNoHoja(nodo, i);
        return true;
    }

    // Clave no está en este nodo
    if (nodo->esHoja) return false;  // No existe

    bool estaEnUltimoHijo = (i == nodo->numClaves);

    // Si el hijo tiene solo 1 clave, rellenarlo antes de descender
    if (nodo->hijos[i]->numClaves == 1)
        llenar(nodo, i);

    // Después de llenar, si era el último hijo y se fusionó, ir a i-1
    if (estaEnUltimoHijo && i > nodo->numClaves)
        return eliminarHelper(nodo->hijos[i - 1], id);
    else
        return eliminarHelper(nodo->hijos[i], id);
}

bool ArbolB::eliminar(const std::string& id) {
    if (raiz == nullptr) return false;

    bool eliminado = eliminarHelper(raiz, id);
    if (!eliminado) return false;

    numClientes--;

    // Si la raíz quedó vacía, bajarla un nivel
    if (raiz->numClaves == 0) {
        NodoArbolB* viejaRaiz = raiz;
        raiz = raiz->esHoja ? nullptr : raiz->hijos[0];
        // Limpiar puntero para no eliminar hijo al borrar viejaRaiz
        viejaRaiz->hijos[0] = nullptr;
        delete viejaRaiz;
    }

    return true;
}
