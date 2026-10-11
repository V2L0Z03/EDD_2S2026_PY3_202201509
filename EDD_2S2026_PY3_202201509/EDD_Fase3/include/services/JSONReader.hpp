#ifndef JSON_READER_HPP
#define JSON_READER_HPP

#include <string>
#include "../structures/ArbolBST.hpp"
#include "../structures/ArbolAVL.hpp"
#include "../structures/EstructuraClientes.hpp"
#include "../structures/GrafoSedes.hpp"

// ─────────────────────────────────────────────────────────────────
// ResultadoCarga: bitácora devuelta al endpoint /api/carga-masiva.
// Usamos una lista enlazada manual para los mensajes de error
// (sin std::vector, cumpliendo las reglas de la Fase 3).
// ─────────────────────────────────────────────────────────────────
struct NodoLog {
    std::string mensaje;
    NodoLog*    siguiente;
    explicit NodoLog(const std::string& m) : mensaje(m), siguiente(nullptr) {}
};

struct BitacoraLog {
    NodoLog* cabeza  = nullptr;
    NodoLog* cola    = nullptr;
    int      total   = 0;

    void agregar(const std::string& msg) {
        NodoLog* n = new NodoLog(msg);
        if (!cabeza) { cabeza = cola = n; }
        else         { cola->siguiente = n; cola = n; }
        total++;
    }

    ~BitacoraLog() {
        NodoLog* a = cabeza;
        while (a) { NodoLog* s = a->siguiente; delete a; a = s; }
    }
};

struct ResultadoCarga {
    int           peliculasCargadas  = 0;
    int           funcionesCargadas  = 0;
    int           sedesCargadas      = 0;
    int           funcionesRechazadas = 0;
    BitacoraLog   errores;            // funciones rechazadas y motivos
    BitacoraLog   advertencias;       // avisos no críticos
};

// ─────────────────────────────────────────────────────────────────
class JSONReader {
public:
    // ── Carga masiva Fase 3 (sedes + películas/funciones validadas)
    static ResultadoCarga cargarCargaMasiva(
        const std::string& contenidoJSON,   // texto JSON crudo (no ruta)
        GrafoSedes&        grafoSedes,
        ArbolBST&          arbolCartelera,
        ArbolAVL&          arbolFunciones);

    // ── Métodos heredados de Fase 2 (intactos) ────────────────────
    static bool cargarPeliculasDesdeJSON(const std::string& rutaArchivo,
                                         ArbolBST& arbolCartelera,
                                         ArbolAVL& arbolFunciones);

    static bool cargarClientesDesdeJSON(const std::string& rutaArchivo,
                                        EstructuraClientes& clientes,
                                        ArbolAVL& arbolFunciones,
                                        ArbolBST& arbolCartelera);

    static bool cargarTodoDesdeJSON(const std::string& rutaArchivo,
                                    ArbolBST& arbolCartelera,
                                    ArbolAVL& arbolFunciones,
                                    EstructuraClientes& clientes);
};

#endif // JSON_READER_HPP
