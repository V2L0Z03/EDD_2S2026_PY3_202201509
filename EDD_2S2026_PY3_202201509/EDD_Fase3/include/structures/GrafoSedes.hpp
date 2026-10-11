#ifndef GRAFO_SEDES_HPP
#define GRAFO_SEDES_HPP

#include "../models/Sede.hpp"
#include "ArbolAVL.hpp"
#include <string>
#include <iostream>

// ═══════════════════════════════════════════════════════════════
// GRAFO NO DIRIGIDO DE SEDES — SECCIÓN 3
//   Implementación 100% manual con punteros. Sin STL.
// ═══════════════════════════════════════════════════════════════

// ──────────────────────────────────────────────────────────────
// Nodo de la lista de adyacencia (Arista)
// ──────────────────────────────────────────────────────────────
struct Arista {
    struct Vertice* destino;   // puntero al vértice vecino
    double          peso;      // cantidad de películas en común
    Arista*         siguiente;

    Arista(struct Vertice* dest, double p = 0.0)
        : destino(dest), peso(p), siguiente(nullptr) {}
};

// ──────────────────────────────────────────────────────────────
// Nodo del grafo (Vértice)
// ──────────────────────────────────────────────────────────────
struct Vertice {
    Sede     sede;
    Arista*  listaAdy;   // primera arista en la lista de adyacencia
    Vertice* siguiente;  // enlace al próximo vértice del grafo

    explicit Vertice(const Sede& s)
        : sede(s), listaAdy(nullptr), siguiente(nullptr) {}
};

// ──────────────────────────────────────────────────────────────
// Estructura Auxiliar Manual: Conjunto de Cadenas (Sin STL)
// Para almacenar y calcular intersección de películas por sede
// ──────────────────────────────────────────────────────────────
struct NodoString {
    std::string valor;
    NodoString* siguiente;
    explicit NodoString(const std::string& v) : valor(v), siguiente(nullptr) {}
};

class ConjuntoManualStrings {
private:
    NodoString* cabeza;
    int         tam;

public:
    ConjuntoManualStrings() : cabeza(nullptr), tam(0) {}
    ~ConjuntoManualStrings() { vaciar(); }

    void vaciar() {
        NodoString* act = cabeza;
        while (act != nullptr) {
            NodoString* sig = act->siguiente;
            delete act;
            act = sig;
        }
        cabeza = nullptr;
        tam = 0;
    }

    bool contiene(const std::string& v) const {
        NodoString* act = cabeza;
        while (act != nullptr) {
            if (act->valor == v) return true;
            act = act->siguiente;
        }
        return false;
    }

    bool agregarSiNoExiste(const std::string& v) {
        if (v.empty() || contiene(v)) return false;
        NodoString* nuevo = new NodoString(v);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        tam++;
        return true;
    }

    int getTamanio() const { return tam; }
    NodoString* getCabeza() const { return cabeza; }

    int contarInterseccion(const ConjuntoManualStrings& otro) const {
        int count = 0;
        NodoString* act = cabeza;
        while (act != nullptr) {
            if (otro.contiene(act->valor)) {
                count++;
            }
            act = act->siguiente;
        }
        return count;
    }
};

// ──────────────────────────────────────────────────────────────
// Estructura Auxiliar Manual: Cola FIFO para BFS (Sin STL)
// ──────────────────────────────────────────────────────────────
struct NodoColaBFS {
    Vertice*     vertice;
    NodoColaBFS* siguiente;
    explicit NodoColaBFS(Vertice* v) : vertice(v), siguiente(nullptr) {}
};

class ColaBFS {
private:
    NodoColaBFS* frente;
    NodoColaBFS* fin;
    int          cantidad;

public:
    ColaBFS() : frente(nullptr), fin(nullptr), cantidad(0) {}
    ~ColaBFS() { vaciar(); }

    bool estaVacia() const { return frente == nullptr; }
    int getCantidad() const { return cantidad; }

    void encolar(Vertice* v) {
        NodoColaBFS* nuevo = new NodoColaBFS(v);
        if (fin == nullptr) {
            frente = fin = nuevo;
        } else {
            fin->siguiente = nuevo;
            fin = nuevo;
        }
        cantidad++;
    }

    Vertice* desencolar() {
        if (estaVacia()) return nullptr;
        NodoColaBFS* temp = frente;
        Vertice* v = temp->vertice;
        frente = frente->siguiente;
        if (frente == nullptr) fin = nullptr;
        delete temp;
        cantidad--;
        return v;
    }

    Vertice* verFrente() const {
        return frente != nullptr ? frente->vertice : nullptr;
    }

    void vaciar() {
        while (!estaVacia()) {
            desencolar();
        }
    }
};

// ──────────────────────────────────────────────────────────────
// Estructura Auxiliar Manual: Registro de Visitados y Padres para BFS
// ──────────────────────────────────────────────────────────────
struct NodoVisitadoBFS {
    Vertice*         vertice;
    Vertice*         padre;
    NodoVisitadoBFS* siguiente;
    NodoVisitadoBFS(Vertice* v, Vertice* p)
        : vertice(v), padre(p), siguiente(nullptr) {}
};

class ListaVisitadosBFS {
private:
    NodoVisitadoBFS* cabeza;

public:
    ListaVisitadosBFS() : cabeza(nullptr) {}
    ~ListaVisitadosBFS() { vaciar(); }

    void vaciar() {
        NodoVisitadoBFS* act = cabeza;
        while (act != nullptr) {
            NodoVisitadoBFS* sig = act->siguiente;
            delete act;
            act = sig;
        }
        cabeza = nullptr;
    }

    bool estaVisitado(Vertice* v) const {
        NodoVisitadoBFS* act = cabeza;
        while (act != nullptr) {
            if (act->vertice == v) return true;
            act = act->siguiente;
        }
        return false;
    }

    void registrar(Vertice* v, Vertice* padre) {
        NodoVisitadoBFS* nuevo = new NodoVisitadoBFS(v, padre);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
    }

    Vertice* obtenerPadre(Vertice* v) const {
        NodoVisitadoBFS* act = cabeza;
        while (act != nullptr) {
            if (act->vertice == v) return act->padre;
            act = act->siguiente;
        }
        return nullptr;
    }
};

// ──────────────────────────────────────────────────────────────
// Estructura de Resultado del Camino Mínimo (BFS)
// ──────────────────────────────────────────────────────────────
struct NodoRuta {
    Vertice*  vertice;
    NodoRuta* siguiente;
    explicit NodoRuta(Vertice* v) : vertice(v), siguiente(nullptr) {}
};

struct ResultadoCaminoBFS {
    bool      encontrado;
    int       saltos;
    NodoRuta* camino;

    ResultadoCaminoBFS() : encontrado(false), saltos(0), camino(nullptr) {}
    ~ResultadoCaminoBFS() { vaciar(); }

    void vaciar() {
        NodoRuta* act = camino;
        while (act != nullptr) {
            NodoRuta* sig = act->siguiente;
            delete act;
            act = sig;
        }
        camino = nullptr;
        saltos = 0;
        encontrado = false;
    }
};

// ──────────────────────────────────────────────────────────────
// Estructura y Lista de Ranking de Aristas
// ──────────────────────────────────────────────────────────────
struct AristaRanking {
    std::string    origenCodigo;
    std::string    origenNombre;
    std::string    destinoCodigo;
    std::string    destinoNombre;
    double         peso;
    AristaRanking* siguiente;

    AristaRanking(const std::string& oC, const std::string& oN,
                  const std::string& dC, const std::string& dN,
                  double p)
        : origenCodigo(oC), origenNombre(oN),
          destinoCodigo(dC), destinoNombre(dN),
          peso(p), siguiente(nullptr) {}
};

class ListaRankingAristas {
public:
    AristaRanking* cabeza;
    int            total;

    ListaRankingAristas() : cabeza(nullptr), total(0) {}
    ~ListaRankingAristas() { vaciar(); }

    void vaciar() {
        AristaRanking* act = cabeza;
        while (act != nullptr) {
            AristaRanking* sig = act->siguiente;
            delete act;
            act = sig;
        }
        cabeza = nullptr;
        total = 0;
    }

    void insertarOrdenadoDesc(const std::string& oC, const std::string& oN,
                             const std::string& dC, const std::string& dN,
                             double peso)
    {
        AristaRanking* nuevo = new AristaRanking(oC, oN, dC, dN, peso);
        if (cabeza == nullptr || peso > cabeza->peso) {
            nuevo->siguiente = cabeza;
            cabeza = nuevo;
        } else {
            AristaRanking* act = cabeza;
            while (act->siguiente != nullptr && act->siguiente->peso >= peso) {
                act = act->siguiente;
            }
            nuevo->siguiente = act->siguiente;
            act->siguiente = nuevo;
        }
        total++;
    }
};

// ──────────────────────────────────────────────────────────────
// Estructura y Lista de Sedes Similares / Sugeridas
// ──────────────────────────────────────────────────────────────
struct SedeSimilar {
    std::string  codigo;
    std::string  nombre;
    std::string  direccion;
    int          salas;
    std::string  telefono;
    double       peso;
    SedeSimilar* siguiente;

    SedeSimilar(const std::string& c, const std::string& n,
                const std::string& dir, int s, const std::string& tel, double p)
        : codigo(c), nombre(n), direccion(dir), salas(s), telefono(tel),
          peso(p), siguiente(nullptr) {}
};

class ListaSedesSimilares {
public:
    SedeSimilar* cabeza;
    int          total;

    ListaSedesSimilares() : cabeza(nullptr), total(0) {}
    ~ListaSedesSimilares() { vaciar(); }

    void vaciar() {
        SedeSimilar* act = cabeza;
        while (act != nullptr) {
            SedeSimilar* sig = act->siguiente;
            delete act;
            act = sig;
        }
        cabeza = nullptr;
        total = 0;
    }

    void insertarOrdenadoDesc(const Sede& s, double peso) {
        SedeSimilar* nuevo = new SedeSimilar(s.getCodigo(), s.getNombre(),
                                             s.getDireccion(), s.getCantidadSalas(),
                                             s.getTelefono(), peso);
        if (cabeza == nullptr || peso > cabeza->peso) {
            nuevo->siguiente = cabeza;
            cabeza = nuevo;
        } else {
            SedeSimilar* act = cabeza;
            while (act->siguiente != nullptr && act->siguiente->peso >= peso) {
                act = act->siguiente;
            }
            nuevo->siguiente = act->siguiente;
            act->siguiente = nuevo;
        }
        total++;
    }
};

// ──────────────────────────────────────────────────────────────
// Clase Principal: GrafoSedes
// ──────────────────────────────────────────────────────────────
class GrafoSedes {
private:
    Vertice* cabeza;     // primer vértice de la lista de sedes
    int      numVertices;
    int      numAristas; // total de conexiones no dirigidas

    void liberarAristas(Arista* a) {
        while (a != nullptr) {
            Arista* sig = a->siguiente;
            delete a;
            a = sig;
        }
    }

public:
    GrafoSedes()
        : cabeza(nullptr), numVertices(0), numAristas(0) {}

    ~GrafoSedes() {
        vaciar();
    }

    GrafoSedes(const GrafoSedes&)            = delete;
    GrafoSedes& operator=(const GrafoSedes&) = delete;

    void vaciar() {
        Vertice* act = cabeza;
        while (act != nullptr) {
            liberarAristas(act->listaAdy);
            Vertice* sig = act->siguiente;
            delete act;
            act = sig;
        }
        cabeza = nullptr;
        numVertices = 0;
        numAristas = 0;
    }

    void limpiarSoloAristas() {
        Vertice* act = cabeza;
        while (act != nullptr) {
            liberarAristas(act->listaAdy);
            act->listaAdy = nullptr;
            act = act->siguiente;
        }
        numAristas = 0;
    }

    int getNumVertices() const { return numVertices; }
    int getNumAristas()  const { return numAristas; }
    Vertice* getCabeza() const { return cabeza; }

    bool insertarSede(const Sede& s) {
        if (buscarVertice(s.getCodigo()) != nullptr) {
            return false;
        }

        Vertice* nuevo = new Vertice(s);
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        numVertices++;
        return true;
    }

    Vertice* buscarVertice(const std::string& codigo) const {
        Vertice* act = cabeza;
        while (act != nullptr) {
            if (act->sede.getCodigo() == codigo) {
                return act;
            }
            act = act->siguiente;
        }
        return nullptr;
    }

    bool sedeExiste(const std::string& codigo) const {
        return buscarVertice(codigo) != nullptr;
    }

    bool insertarArista(const std::string& codigoOrigen,
                        const std::string& codigoDestino,
                        double peso = 0.0)
    {
        if (codigoOrigen == codigoDestino) return false;

        Vertice* origen  = buscarVertice(codigoOrigen);
        Vertice* destino = buscarVertice(codigoDestino);

        if (origen == nullptr || destino == nullptr) {
            return false;
        }

        // Si ya existe la arista, actualizamos su peso en ambos extremos
        Arista* aO = origen->listaAdy;
        while (aO != nullptr) {
            if (aO->destino == destino) {
                aO->peso = peso;
                // Actualizar simétrico
                Arista* aD = destino->listaAdy;
                while (aD != nullptr) {
                    if (aD->destino == origen) { aD->peso = peso; break; }
                    aD = aD->siguiente;
                }
                return true;
            }
            aO = aO->siguiente;
        }

        // Insertar en lista de origen
        Arista* nuevaOD = new Arista(destino, peso);
        nuevaOD->siguiente = origen->listaAdy;
        origen->listaAdy   = nuevaOD;

        // Insertar en lista de destino (Grafo No Dirigido)
        Arista* nuevaDO = new Arista(origen, peso);
        nuevaDO->siguiente = destino->listaAdy;
        destino->listaAdy  = nuevaDO;

        numAristas++;
        return true;
    }

    // ── Recálculo Dinámico de Aristas Ponderadas ───────────────
    // Peso = número de películas distintas en común entre ambas sedes
    void recalcularAristas(const ArbolAVL& arbolFunciones) {
        limpiarSoloAristas();
        if (numVertices < 2) return;

        // Mapeo dinámico manual (arreglo de punteros y conjuntos manuales)
        struct SedePelisInfo {
            Vertice*              vertice;
            ConjuntoManualStrings peliculas;
        };

        SedePelisInfo* mapeo = new SedePelisInfo[numVertices];
        Vertice* temp = cabeza;
        int idx = 0;
        while (temp != nullptr && idx < numVertices) {
            mapeo[idx].vertice = temp;
            temp = temp->siguiente;
            idx++;
        }

        // Recolectar películas por sede a partir del árbol AVL
        arbolFunciones.recorridoInorden([&](NodoAVL* nodo) {
            const std::string& codSede = nodo->funcion.getCodigoSede();
            const std::string& codPeli = nodo->funcion.getCodigoPelicula();
            if (!codSede.empty() && !codPeli.empty()) {
                for (int i = 0; i < numVertices; ++i) {
                    if (mapeo[i].vertice->sede.getCodigo() == codSede) {
                        mapeo[i].peliculas.agregarSiNoExiste(codPeli);
                        break;
                    }
                }
            }
        });

        // Crear aristas ponderadas para pares con películas en común > 0
        for (int i = 0; i < numVertices; ++i) {
            for (int j = i + 1; j < numVertices; ++j) {
                int enComun = mapeo[i].peliculas.contarInterseccion(mapeo[j].peliculas);
                if (enComun > 0) {
                    insertarArista(mapeo[i].vertice->sede.getCodigo(),
                                   mapeo[j].vertice->sede.getCodigo(),
                                   static_cast<double>(enComun));
                }
            }
        }

        delete[] mapeo;
        std::cout << "[GrafoSedes] Recálculo completado: "
                  << numVertices << " sedes, "
                  << numAristas  << " conexiones creadas.\n";
    }

    // ── Búsqueda de Camino Mínimo por BFS (Menor cantidad de saltos) ──
    ResultadoCaminoBFS buscarCaminoCortoBFS(const std::string& codOrigen,
                                            const std::string& codDestino) const
    {
        ResultadoCaminoBFS res;
        Vertice* origen  = buscarVertice(codOrigen);
        Vertice* destino = buscarVertice(codDestino);

        if (origen == nullptr || destino == nullptr) {
            return res; // encontrado = false
        }

        // Caso trivial: Origen es el mismo Destino
        if (origen == destino) {
            res.encontrado = true;
            res.saltos = 0;
            res.camino = new NodoRuta(origen);
            return res;
        }

        ColaBFS           cola;
        ListaVisitadosBFS visitados;

        cola.encolar(origen);
        visitados.registrar(origen, nullptr);

        bool alcanzado = false;

        while (!cola.estaVacia()) {
            Vertice* actual = cola.desencolar();

            if (actual == destino) {
                alcanzado = true;
                break;
            }

            Arista* ar = actual->listaAdy;
            while (ar != nullptr) {
                Vertice* vecino = ar->destino;
                if (!visitados.estaVisitado(vecino)) {
                    visitados.registrar(vecino, actual);
                    cola.encolar(vecino);
                }
                ar = ar->siguiente;
            }
        }

        if (alcanzado) {
            // Reconstruir camino desde destino hacia origen
            Vertice* curr = destino;
            int totalNodos = 0;

            while (curr != nullptr) {
                NodoRuta* nr = new NodoRuta(curr);
                nr->siguiente = res.camino;
                res.camino = nr;
                totalNodos++;
                curr = visitados.obtenerPadre(curr);
            }

            res.saltos = (totalNodos > 0) ? (totalNodos - 1) : 0;
            res.encontrado = true;
        }

        return res;
    }

    // ── Ranking de Aristas (Ordenado Descendentemente por Peso) ──
    void obtenerRankingAristas(ListaRankingAristas& outRanking) const {
        outRanking.vaciar();

        Vertice* v = cabeza;
        while (v != nullptr) {
            Arista* a = v->listaAdy;
            while (a != nullptr) {
                // Para no duplicar aristas no dirigidas (A-B vs B-A)
                if (v->sede.getCodigo() < a->destino->sede.getCodigo()) {
                    outRanking.insertarOrdenadoDesc(
                        v->sede.getCodigo(),
                        v->sede.getNombre(),
                        a->destino->sede.getCodigo(),
                        a->destino->sede.getNombre(),
                        a->peso
                    );
                }
                a = a->siguiente;
            }
            v = v->siguiente;
        }
    }

    // ── Sedes Similares / Sugeridas para una Sede Consultada ──
    void obtenerSedesSimilares(const std::string& codigoSede,
                               ListaSedesSimilares& outSimilares) const
    {
        outSimilares.vaciar();
        Vertice* v = buscarVertice(codigoSede);
        if (v == nullptr) return;

        Arista* a = v->listaAdy;
        while (a != nullptr) {
            outSimilares.insertarOrdenadoDesc(a->destino->sede, a->peso);
            a = a->siguiente;
        }
    }
};

#endif // GRAFO_SEDES_HPP
