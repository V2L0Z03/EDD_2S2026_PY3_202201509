#include "../../include/structures/MatrizDispersa.hpp"
#include "../../include/vendor/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// ============================================================
//  IMPLEMENTACIÓN DE ListaCabeceras
// ============================================================
ListaCabeceras::~ListaCabeceras() {
    vaciar();
}

void ListaCabeceras::insertarCabecera(int id) {
    NodoCabecera* nuevo = new NodoCabecera(id);

    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else if (id < cabeza->id) {
        nuevo->siguiente   = cabeza;
        cabeza->anterior   = nuevo;
        cabeza             = nuevo;
    } else {
        NodoCabecera* actual = cabeza;
        while (actual->siguiente != nullptr && actual->siguiente->id < id) {
            actual = actual->siguiente;
        }
        if (actual->id == id) {
            delete nuevo;
            return;
        }
        nuevo->siguiente  = actual->siguiente;
        if (actual->siguiente != nullptr) {
            actual->siguiente->anterior = nuevo;
        }
        actual->siguiente = nuevo;
        nuevo->anterior   = actual;
    }
}

NodoCabecera* ListaCabeceras::buscarCabecera(int id) const {
    NodoCabecera* actual = cabeza;
    while (actual != nullptr) {
        if (actual->id == id) return actual;
        actual = actual->siguiente;
    }
    return nullptr;
}

void ListaCabeceras::vaciar() {
    NodoCabecera* actual = cabeza;
    while (actual != nullptr) {
        NodoCabecera* sig = actual->siguiente;
        delete actual;
        actual = sig;
    }
    cabeza = nullptr;
}

// ============================================================
//  IMPLEMENTACIÓN DE MatrizDispersa
// ============================================================
MatrizDispersa::MatrizDispersa()
    : maxFilas(0), maxColumnas(0), asientosReservadosCount(0) {}

MatrizDispersa::MatrizDispersa(int filas, int columnas)
    : maxFilas(filas), maxColumnas(columnas), asientosReservadosCount(0) {}

MatrizDispersa::~MatrizDispersa() {
    vaciar();
}

void MatrizDispersa::inicializarDimensiones(int filas, int columnas) {
    vaciar();
    this->maxFilas    = filas;
    this->maxColumnas = columnas;
}

NodoMatriz* MatrizDispersa::buscarNodo(int fila, int columna) const {
    NodoCabecera* cabFila = cabecerasFilas.buscarCabecera(fila);
    if (cabFila == nullptr) return nullptr;

    NodoMatriz* actual = cabFila->acceso;
    while (actual != nullptr) {
        if (actual->asiento.getColumna() == columna) return actual;
        actual = actual->derecha;
    }
    return nullptr;
}

bool MatrizDispersa::reservarAsiento(int fila, int columna,
                                      const std::string& cliente,
                                      const std::string& codigoReserva) {
    if (fila < 1 || columna < 1) {
        std::cout << " Error: Asiento invalido (1-"
                  << maxFilas << ", 1-" << maxColumnas << ")\n";
        return false;
    }

    // Autoexpandir dinámicamente si el asiento excede los límites
    if (fila > maxFilas) maxFilas = fila;
    if (columna > maxColumnas) maxColumnas = columna;

    NodoMatriz* existente = buscarNodo(fila, columna);
    if (existente != nullptr && existente->asiento.isOcupado()) {
        std::cout << " Error: El asiento F" << fila << ",C" << columna
                  << " ya está ocupado por " << existente->asiento.getCliente() << ".\n";
        return false;
    }

    cabecerasFilas.insertarCabecera(fila);
    cabecerasColumnas.insertarCabecera(columna);

    NodoCabecera* cabFila    = cabecerasFilas.buscarCabecera(fila);
    NodoCabecera* cabColumna = cabecerasColumnas.buscarCabecera(columna);

    Asiento nuevoAsiento(fila, columna, true, cliente, codigoReserva);
    NodoMatriz* nuevoNodo = new NodoMatriz(nuevoAsiento);

    // Inserción ordenada en la fila
    if (cabFila->acceso == nullptr) {
        cabFila->acceso = nuevoNodo;
    } else if (columna < cabFila->acceso->asiento.getColumna()) {
        nuevoNodo->derecha        = cabFila->acceso;
        cabFila->acceso->izquierda = nuevoNodo;
        cabFila->acceso           = nuevoNodo;
    } else {
        NodoMatriz* actual = cabFila->acceso;
        while (actual->derecha != nullptr && actual->derecha->asiento.getColumna() < columna)
            actual = actual->derecha;
        nuevoNodo->derecha = actual->derecha;
        if (actual->derecha != nullptr)
            actual->derecha->izquierda = nuevoNodo;
        actual->derecha    = nuevoNodo;
        nuevoNodo->izquierda = actual;
    }

    // Inserción ordenada en la columna
    if (cabColumna->acceso == nullptr) {
        cabColumna->acceso = nuevoNodo;
    } else if (fila < cabColumna->acceso->asiento.getFila()) {
        nuevoNodo->abajo          = cabColumna->acceso;
        cabColumna->acceso->arriba = nuevoNodo;
        cabColumna->acceso        = nuevoNodo;
    } else {
        NodoMatriz* actual = cabColumna->acceso;
        while (actual->abajo != nullptr && actual->abajo->asiento.getFila() < fila)
            actual = actual->abajo;
        nuevoNodo->abajo = actual->abajo;
        if (actual->abajo != nullptr)
            actual->abajo->arriba = nuevoNodo;
        actual->abajo    = nuevoNodo;
        nuevoNodo->arriba = actual;
    }

    asientosReservadosCount++;
    return true;
}

bool MatrizDispersa::cancelarReserva(int fila, int columna) {
    NodoMatriz* nodo = buscarNodo(fila, columna);
    if (nodo == nullptr || !nodo->asiento.isOcupado()) {
        std::cout << " Error: El asiento no está ocupado o no existe.\n";
        return false;
    }

    // Desconectar horizontalmente
    if (nodo->izquierda != nullptr) {
        nodo->izquierda->derecha = nodo->derecha;
    } else {
        NodoCabecera* cabFila = cabecerasFilas.buscarCabecera(fila);
        if (cabFila) cabFila->acceso = nodo->derecha;
    }
    if (nodo->derecha != nullptr) {
        nodo->derecha->izquierda = nodo->izquierda;
    }

    // Desconectar verticalmente
    if (nodo->arriba != nullptr) {
        nodo->arriba->abajo = nodo->abajo;
    } else {
        NodoCabecera* cabColumna = cabecerasColumnas.buscarCabecera(columna);
        if (cabColumna) cabColumna->acceso = nodo->abajo;
    }
    if (nodo->abajo != nullptr) {
        nodo->abajo->arriba = nodo->arriba;
    }

    delete nodo;
    asientosReservadosCount--;
    return true;
}

void MatrizDispersa::mostrarMapaAsientos() const {
    if (maxFilas == 0 || maxColumnas == 0) {
        std::cout << "[La función no tiene dimensiones asignadas]\n";
        return;
    }
    std::cout << "\n=== MAPA DE ASIENTOS (" << maxFilas << "x" << maxColumnas << ") ===\n";
    for (int f = 1; f <= maxFilas; f++) {
        for (int c = 1; c <= maxColumnas; c++) {
            NodoMatriz* nodo = buscarNodo(f, c);
            std::cout << (nodo && nodo->asiento.isOcupado() ? "[X]" : "[L]");
        }
        std::cout << "\n";
    }
}

void MatrizDispersa::mostrarEstadisticas() const {
    int total = maxFilas * maxColumnas;
    std::cout << "Total: " << total << " | Reservados: " << asientosReservadosCount
              << " | Libres: " << (total - asientosReservadosCount) << "\n";
}

void MatrizDispersa::vaciar() {
    NodoCabecera* cabFila = cabecerasFilas.getCabeza();
    while (cabFila != nullptr) {
        NodoMatriz* actual = cabFila->acceso;
        while (actual != nullptr) {
            NodoMatriz* sig = actual->derecha;
            delete actual;
            actual = sig;
        }
        cabFila = cabFila->siguiente;
    }
    cabecerasFilas.vaciar();
    cabecerasColumnas.vaciar();
    asientosReservadosCount = 0;
}

// ── Auxiliares GUI
bool MatrizDispersa::estaOcupado(int fila, int columna) const {
    NodoMatriz* nodo = buscarNodo(fila, columna);
    return (nodo != nullptr && nodo->asiento.isOcupado());
}

std::string MatrizDispersa::getClienteAsiento(int fila, int columna) const {
    NodoMatriz* nodo = buscarNodo(fila, columna);
    if (nodo != nullptr && nodo->asiento.isOcupado())
        return nodo->asiento.getCliente();
    return "";
}

std::string MatrizDispersa::getCodigoReservaAsiento(int fila, int columna) const {
    NodoMatriz* nodo = buscarNodo(fila, columna);
    if (nodo != nullptr && nodo->asiento.isOcupado())
        return nodo->asiento.getCodigoReserva();
    return "";
}

// ============================================================
//  PERSISTENCIA JSON (Fase 2)
// ============================================================
bool MatrizDispersa::serializarJSON(const std::string& rutaArchivo) const {
    json root;
    root["filas"]    = maxFilas;
    root["columnas"] = maxColumnas;
    root["asientos"] = json::array();

    NodoCabecera* cabFila = cabecerasFilas.getCabeza();
    while (cabFila != nullptr) {
        NodoMatriz* actual = cabFila->acceso;
        while (actual != nullptr) {
            if (actual->asiento.isOcupado()) {
                json seat;
                seat["fila"]          = actual->asiento.getFila();
                seat["columna"]       = actual->asiento.getColumna();
                seat["cliente"]       = actual->asiento.getCliente();
                seat["codigo_reserva"]= actual->asiento.getCodigoReserva();
                root["asientos"].push_back(seat);
            }
            actual = actual->derecha;
        }
        cabFila = cabFila->siguiente;
    }

    std::ofstream file(rutaArchivo);
    if (!file.is_open()) {
        std::cerr << "[Matriz] Error: No se pudo escribir: " << rutaArchivo << "\n";
        return false;
    }
    file << root.dump(4);
    file.close();
    std::cout << "[Matriz] Persistencia guardada: " << rutaArchivo << "\n";
    return true;
}

bool MatrizDispersa::deserializarJSON(const std::string& rutaArchivo) {
    std::ifstream file(rutaArchivo);
    if (!file.is_open()) {
        // No existe archivo todavía: es válido (sala sin reservas previas)
        return true;
    }

    json root;
    try {
        file >> root;
    } catch (...) {
        std::cerr << "[Matriz] Error al parsear: " << rutaArchivo << "\n";
        return false;
    }
    file.close();

    // Restaurar dimensiones si están en el JSON
    if (root.contains("filas") && root.contains("columnas")) {
        int f = root["filas"].get<int>();
        int c = root["columnas"].get<int>();
        if (f > 0 && c > 0) {
            vaciar();
            maxFilas    = f;
            maxColumnas = c;
        }
    }

    // Reconstruir asientos ocupados
    if (root.contains("asientos") && root["asientos"].is_array()) {
        for (const auto& seat : root["asientos"]) {
            int         fila    = seat.value("fila",           0);
            int         col     = seat.value("columna",        0);
            std::string cliente = seat.value("cliente",        "");
            std::string codRes  = seat.value("codigo_reserva", "");
            if (fila > 0 && col > 0)
                reservarAsiento(fila, col, cliente, codRes);
        }
    }

    std::cout << "[Matriz] Estado cargado desde: " << rutaArchivo << "\n";
    return true;
}