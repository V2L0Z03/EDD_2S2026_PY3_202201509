#ifndef GRAFO_SEDES_HPP
#define GRAFO_SEDES_HPP

#include "../models/Sede.hpp"
#include <string>
#include <iostream>

// ═══════════════════════════════════════════════════════════════
// GRAFO NO DIRIGIDO DE SEDES
//   Implementación 100% manual con punteros. Sin STL.
//
//   Estructura interna:
//     - Lista enlazada simple de Vértices (cada Vértice = una Sede)
//     - Cada Vértice posee su propia lista enlazada de Aristas
//       (lista de adyacencia), que en esta Sección 2 parte vacía.
//
//   Complejidad de búsqueda: O(V)  — lista no ordenada.
// ═══════════════════════════════════════════════════════════════

// ──────────────────────────────────────────────────────────────
// Nodo de la lista de adyacencia (Arista)
//   - destino : puntero al Vértice al otro extremo de la arista
//   - peso    : costo/distancia de la conexión (para Sección 3+)
//   - siguiente: enlace al próximo nodo de arista en la lista
// ──────────────────────────────────────────────────────────────
struct Arista {
    struct Vertice* destino;   // forward declaration
    double          peso;
    Arista*         siguiente;

    Arista(struct Vertice* dest, double p = 0.0)
        : destino(dest), peso(p), siguiente(nullptr) {}
};

// ──────────────────────────────────────────────────────────────
// Nodo del grafo (Vértice)
//   - sede      : datos de la sede almacenados por valor
//   - listaAdy  : cabecera de la lista de adyacencia (aristas)
//   - siguiente : enlace al próximo vértice en la lista de vértices
// ──────────────────────────────────────────────────────────────
struct Vertice {
    Sede    sede;
    Arista* listaAdy;   // primera arista del vértice (nullptr = aislado)
    Vertice* siguiente;

    explicit Vertice(const Sede& s)
        : sede(s), listaAdy(nullptr), siguiente(nullptr) {}
};


// ──────────────────────────────────────────────────────────────
// Clase GrafoSedes
// ──────────────────────────────────────────────────────────────
class GrafoSedes {
private:
    Vertice* cabeza;     // primer vértice de la lista de vértices
    int      numVertices;
    int      numAristas;

    // Limpia recursivamente la lista de aristas de un vértice
    void liberarAristas(Arista* a) {
        while (a != nullptr) {
            Arista* siguiente = a->siguiente;
            delete a;
            a = siguiente;
        }
    }

public:
    // ── Constructor / Destructor ──────────────────────────────
    GrafoSedes()
        : cabeza(nullptr), numVertices(0), numAristas(0) {}

    ~GrafoSedes() {
        Vertice* actual = cabeza;
        while (actual != nullptr) {
            liberarAristas(actual->listaAdy);
            Vertice* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        cabeza = nullptr;
    }

    // Sin copia — grafo es propietario de su memoria
    GrafoSedes(const GrafoSedes&)            = delete;
    GrafoSedes& operator=(const GrafoSedes&) = delete;

    // ── Getters de estadísticas ───────────────────────────────
    int getNumVertices() const { return numVertices; }
    int getNumAristas()  const { return numAristas; }

    // ── insertarSede ─────────────────────────────────────────
    // Inserta una sede como nuevo vértice si su código no existe.
    // Retorna true si se insertó, false si ya existía.
    bool insertarSede(const Sede& s) {
        // Verificar duplicado por código
        if (buscarVertice(s.getCodigo()) != nullptr) {
            std::cerr << "[GrafoSedes] Sede duplicada, ignorada: "
                      << s.getCodigo() << "\n";
            return false;
        }

        Vertice* nuevo = new Vertice(s);

        // Insertar al inicio de la lista (O(1))
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        numVertices++;

        std::cout << "[GrafoSedes] Sede insertada: "
                  << s.getCodigo() << " - " << s.getNombre() << "\n";
        return true;
    }

    // ── buscarVertice ─────────────────────────────────────────
    // Busca un vértice por código de sede.
    // Retorna puntero al Vértice o nullptr si no existe.
    Vertice* buscarVertice(const std::string& codigo) const {
        Vertice* actual = cabeza;
        while (actual != nullptr) {
            if (actual->sede.getCodigo() == codigo) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // ── sedeExiste ────────────────────────────────────────────
    // Conveniencia para validaciones en el parser JSON.
    bool sedeExiste(const std::string& codigo) const {
        return buscarVertice(codigo) != nullptr;
    }

    // ── insertarArista ────────────────────────────────────────
    // (Preparado para Sección 3 — Conexiones entre Sedes)
    // Arista no dirigida: agrega la conexión en ambas listas.
    // Retorna false si alguno de los vértices no existe.
    bool insertarArista(const std::string& codigoOrigen,
                        const std::string& codigoDestino,
                        double peso = 0.0)
    {
        Vertice* origen  = buscarVertice(codigoOrigen);
        Vertice* destino = buscarVertice(codigoDestino);

        if (origen == nullptr || destino == nullptr) {
            std::cerr << "[GrafoSedes] No se pudo agregar arista: "
                      << codigoOrigen << " <-> " << codigoDestino << "\n";
            return false;
        }

        // Verificar si la arista ya existe (no permitir duplicados)
        Arista* a = origen->listaAdy;
        while (a != nullptr) {
            if (a->destino == destino) return false; // ya existe
            a = a->siguiente;
        }

        // Insertar en lista de origen
        Arista* nuevaOD = new Arista(destino, peso);
        nuevaOD->siguiente = origen->listaAdy;
        origen->listaAdy   = nuevaOD;

        // Insertar en lista de destino (grafo NO dirigido)
        Arista* nuevaDO = new Arista(origen, peso);
        nuevaDO->siguiente = destino->listaAdy;
        destino->listaAdy  = nuevaDO;

        numAristas++;
        return true;
    }

    // ── imprimirGrafo ─────────────────────────────────────────
    // Debug: lista vértices y sus adyacencias en stdout.
    void imprimirGrafo() const {
        std::cout << "=== GrafoSedes ("
                  << numVertices << " sedes, "
                  << numAristas  << " conexiones) ===\n";
        Vertice* v = cabeza;
        while (v != nullptr) {
            std::cout << "  [" << v->sede.getCodigo() << "] "
                      << v->sede.getNombre()
                      << " | Salas: " << v->sede.getCantidadSalas()
                      << " | Tel: "   << v->sede.getTelefono() << "\n";

            Arista* a = v->listaAdy;
            if (a == nullptr) {
                std::cout << "      (sin conexiones)\n";
            }
            while (a != nullptr) {
                std::cout << "      -> " << a->destino->sede.getCodigo()
                          << " (peso: " << a->peso << ")\n";
                a = a->siguiente;
            }
            v = v->siguiente;
        }
    }

    // ── getCabeza ─────────────────────────────────────────────
    // Acceso a la lista para iteración externa (reportes, etc.)
    Vertice* getCabeza() const { return cabeza; }
};

#endif // GRAFO_SEDES_HPP
