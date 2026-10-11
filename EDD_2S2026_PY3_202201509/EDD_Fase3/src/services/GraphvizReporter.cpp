
#include "../../include/services/GraphvizReporter.hpp"
#include "../../include/models/SistemaCine.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <QDate>
#include <QString>
#include <QDir>


// HELPER: Resolver ruta de salida segura

static std::string getOutputPath(const std::string& nombreArchivo, const std::string& ext) {
    QString dir = "reports_output";
    if (!QDir(dir).exists()) {
        dir = "../reports_output";
    }
    return QString("%1/%2.%3").arg(dir).arg(QString::fromStdString(nombreArchivo)).arg(QString::fromStdString(ext)).toStdString();
}

// HELPER: ejecutar dot y convertir a PNG

void GraphvizReporter::ejecutarComandoGraphviz(
    const std::string& rutaDot,
    const std::string& rutaPng
) {
    std::string comando = "dot -Tpng " + rutaDot + " -o " + rutaPng;
    int resultado = std::system(comando.c_str());

    if (resultado == 0) {
        std::cout << "[Graphviz] Reporte generado: " << rutaPng << "\n";
    } else {
        std::cout << "[Graphviz] .dot escrito en: " << rutaDot
                  << "  (Instala Graphviz y agrega 'dot' al PATH para generar PNG)\n";
    }
}

// ─────────────────────────────────────────────────────────────
// REPORTE 1 – BST CARTELERA
// ─────────────────────────────────────────────────────────────
void GraphvizReporter::generarDOT_BST(NodoBST* nodo, std::ofstream& file) {
    if (nodo == nullptr) return;

    QDate hoy         = QDate::currentDate();
    QDate fechaEstreno = QDate::fromString(
        QString::fromStdString(nodo->pelicula.getFechaEstreno()), "yyyy-MM-dd");
    QDate fechaFin    = QDate::fromString(
        QString::fromStdString(nodo->pelicula.getFechaFin()), "yyyy-MM-dd");

    bool esProximo  = fechaEstreno.isValid() && fechaEstreno > hoy;
    bool prontoFin  = fechaFin.isValid() && fechaFin <= hoy.addDays(7) && !esProximo;

    std::string fillColor   = "\"#2ECC71\"";  // verde  – en cartelera
    std::string fontColor   = "\"#0D1117\"";
    if (esProximo) {
        fillColor = "\"#60A5FA\"";            // azul   – próximo estreno
    } else if (prontoFin) {
        fillColor = "\"#F59E0B\"";            // dorado – próximo a retirar
    }

    std::string nodeId = "node_" + nodo->pelicula.getCodigo();

    file << "    " << nodeId
         << " [label=\"Código: " << nodo->pelicula.getCodigo()
         << "\\nTítulo: " << nodo->pelicula.getTitulo()
         << "\\nDuración: " << nodo->pelicula.getDuracionMinutos() << " min"
         << "\\nClasif.: " << nodo->pelicula.getClasificacion()
         << "\", fillcolor=" << fillColor
         << ", fontcolor=" << fontColor
         << "];\n";

    if (nodo->izquierdo != nullptr) {
        file << "    " << nodeId << " -> node_"
             << nodo->izquierdo->pelicula.getCodigo() << ";\n";
        generarDOT_BST(nodo->izquierdo, file);
    }

    if (nodo->derecho != nullptr) {
        file << "    " << nodeId << " -> node_"
             << nodo->derecho->pelicula.getCodigo() << ";\n";
        generarDOT_BST(nodo->derecho, file);
    }
}

bool GraphvizReporter::generarReporteBST(
    const ArbolBST& arbol,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    file << "digraph ArbolBST {\n";
    file << "    graph [label=\"Cartelera de Peliculas (Arbol BST)\","
         << " labelloc=t, fontsize=16, fontname=\"Helvetica\"];\n";
    file << "    node [shape=box, style=filled, fontname=\"Helvetica\","
         << " fontsize=11];\n";
    file << "    edge [color=\"#555555\"];\n";

    if (!arbol.estaVacio()) {
        file << "    leyenda [shape=plaintext, label=\""
             << "Verde = En cartelera | Azul = Proximo estreno | Dorado = Retiro en 7 dias"
             << "\", fontsize=10];\n";
        generarDOT_BST(arbol.getRaiz(), file);
    } else {
        file << "    empty [label=\"Arbol Vacio\", shape=plaintext];\n";
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE AVL – FUNCIONES (Fase 2 - Paso 2)
//   Nodos circulares. Verde = Programada (>= hoy), Rojo = Realizada (< hoy)
// ─────────────────────────────────────────────────────────────
void GraphvizReporter::generarDOT_AVL(NodoAVL* nodo, std::ofstream& file) {
    if (nodo == nullptr) return;

    QDate hoy = QDate::currentDate();
    QDate fechaFunc = QDate::fromString(
        QString::fromStdString(nodo->funcion.getFecha()), "yyyy-MM-dd");

    bool programada = !fechaFunc.isValid() || fechaFunc >= hoy;

    std::string fillColor = programada ? "\"#2ECC71\"" : "\"#EF4444\""; // verde o rojo
    std::string fontColor = programada ? "\"#0D1117\"" : "\"#FFFFFF\"";

    std::string nodeId = "avl_node_" + nodo->funcion.getCodigo();

    int altIzq = nodo->izquierdo ? nodo->izquierdo->altura : 0;
    int altDer = nodo->derecho ? nodo->derecho->altura : 0;
    int fe = altIzq - altDer;

    file << "    " << nodeId
         << " [label=\"Cod: " << nodo->funcion.getCodigo()
         << "\\nPeli: " << nodo->funcion.getCodigoPelicula()
         << "\\nFecha: " << nodo->funcion.getFecha() << " " << nodo->funcion.getHora()
         << "\\nSala: " << nodo->funcion.getSala() << " | $" << nodo->funcion.getPrecio()
         << "\\nAlt: " << nodo->altura << " | FE: " << fe
         << "\", shape=circle, fillcolor=" << fillColor
         << ", fontcolor=" << fontColor << "];\n";

    if (nodo->izquierdo != nullptr) {
        file << "    " << nodeId << " -> avl_node_"
             << nodo->izquierdo->funcion.getCodigo() << " [label=\"I\"];\n";
        generarDOT_AVL(nodo->izquierdo, file);
    }

    if (nodo->derecho != nullptr) {
        file << "    " << nodeId << " -> avl_node_"
             << nodo->derecho->funcion.getCodigo() << " [label=\"D\"];\n";
        generarDOT_AVL(nodo->derecho, file);
    }
}

bool GraphvizReporter::generarReporteAVL(
    const ArbolAVL& arbol,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    file << "digraph ArbolAVL {\n";
    file << "    graph [label=\"Catálogo de Funciones (Arbol AVL)\","
         << " labelloc=t, fontsize=16, fontname=\"Helvetica\"];\n";
    file << "    node [style=filled, fontname=\"Helvetica\", fontsize=10];\n";
    file << "    edge [color=\"#555555\"];\n";

    if (!arbol.estaVacio()) {
        file << "    leyenda [shape=plaintext, label=\""
             << "Verde = Programada | Rojo = Realizada"
             << "\", fontsize=10];\n";
        generarDOT_AVL(arbol.getRaiz(), file);
    } else {
        file << "    empty [label=\"Arbol AVL Vacio\", shape=plaintext];\n";
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE 2 – LISTA CIRCULAR DOBLE – SOLICITUDES
// ─────────────────────────────────────────────────────────────
bool GraphvizReporter::generarReporteSolicitudes(
    const ListaCircularDobleSolicitudes& lista,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    file << "digraph ListaCircularDoble {\n";
    file << "    graph [rankdir=LR, label=\"Solicitudes Especiales — Lista Circular Doble"
         << "\\nTotal: " << lista.getTamanio()
         << "\", labelloc=t, fontsize=14, fontname=\"Helvetica\"];\n";
    file << "    node [shape=circle, style=filled, fontname=\"Helvetica\", fontsize=10];\n";
    file << "    edge [fontsize=9];\n";

    if (!lista.estaVacia()) {
        NodoSolicitud* actual = lista.getCabeza();

        do {
            std::string est   = actual->solicitud.getEstado();
            std::string fill  = "\"#F59E0B\"";
            std::string fcol  = "\"#0D1117\"";
            if (est == "Aprobada") {
                fill = "\"#2ECC71\"";
            } else if (est == "Rechazada" || est == "Atendida") {
                fill  = "\"#EF4444\"";
                fcol  = "\"#FFFFFF\"";
            }

            file << "    sol_" << actual->solicitud.getId()
                 << " [label=\"ID: " << actual->solicitud.getId()
                 << "\\nCliente: " << actual->solicitud.getCliente()
                 << "\\nTipo: " << actual->solicitud.getTipo()
                 << "\\nEstado: " << actual->solicitud.getEstado()
                 << "\", fillcolor=" << fill
                 << ", fontcolor=" << fcol << "];\n";

            actual = actual->siguiente;
        } while (actual != lista.getCabeza());

        actual = lista.getCabeza();
        do {
            NodoSolicitud* sig = actual->siguiente;

            file << "    sol_" << actual->solicitud.getId()
                 << " -> sol_" << sig->solicitud.getId()
                 << " [label=\"sig\", color=blue, fontcolor=blue];\n";

            file << "    sol_" << sig->solicitud.getId()
                 << " -> sol_" << actual->solicitud.getId()
                 << " [label=\"ant\", color=red, fontcolor=red, style=dashed];\n";

            actual = actual->siguiente;
        } while (actual != lista.getCabeza());

    } else {
        file << "    empty [label=\"Lista Vacia\", shape=plaintext];\n";
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE 3 – LISTA CIRCULAR DE LISTAS – PROMOCIONES
// ─────────────────────────────────────────────────────────────
bool GraphvizReporter::generarReportePromociones(
    const ListaCircularPromos& lista,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    file << "digraph ListaPromociones {\n";
    file << "    graph [label=\"Promociones y Beneficios — Lista Circular de Listas\","
         << " labelloc=t, fontsize=14, fontname=\"Helvetica\", rankdir=TB];\n";
    file << "    node [fontname=\"Helvetica\", fontsize=10, style=filled];\n";

    if (!lista.estaVacia()) {
        NodoPromocion* actualPromo = lista.getCabeza();

        file << "    { rank=same;\n";
        do {
            file << "        promo_" << actualPromo->promocion.getCodigo()
                 << " [shape=box, fillcolor=\"#7C3AED\", fontcolor=white,"
                 << " label=\"Promo: " << actualPromo->promocion.getCodigo()
                 << "\\n" << actualPromo->promocion.getNombre()
                 << "\\nVigencia: " << actualPromo->promocion.getVigencia()
                 << "\\nDías: " << actualPromo->promocion.getDiasAplicables()
                 << "\"];\n";
            actualPromo = actualPromo->siguiente;
        } while (actualPromo != lista.getCabeza());
        file << "    }\n";

        actualPromo = lista.getCabeza();
        do {
            file << "    promo_" << actualPromo->promocion.getCodigo()
                 << " -> promo_" << actualPromo->siguiente->promocion.getCodigo()
                 << " [color=\"#7C3AED\", label=\"sig\"];\n";

            NodoBeneficio* actualBen = actualPromo->promocion.getBeneficios().getCabeza();
            std::string prevNode = "promo_" + actualPromo->promocion.getCodigo();
            int bIdx = 1;

            while (actualBen != nullptr) {
                std::string currentBenNode =
                    "ben_" + actualPromo->promocion.getCodigo() + "_" + std::to_string(bIdx);

                file << "    " << currentBenNode
                     << " [shape=box, fillcolor=\"#F59E0B\", fontcolor=\"#0D1117\","
                     << " label=\"Beneficio " << bIdx
                     << "\\nTipo: " << actualBen->beneficio.getTipo()
                     << "\\n" << actualBen->beneficio.getDescripcion()
                     << "\\nValor: " << actualBen->beneficio.getValor()
                     << "\"];\n";

                file << "    " << prevNode << " -> " << currentBenNode
                     << " [color=\"#F59E0B\"];\n";
                file << "    " << currentBenNode << " -> " << prevNode
                     << " [color=\"#F59E0B\", style=dashed];\n";

                prevNode = currentBenNode;
                actualBen = actualBen->siguiente;
                bIdx++;
            }

            actualPromo = actualPromo->siguiente;
        } while (actualPromo != lista.getCabeza());

    } else {
        file << "    empty [label=\"Sin Promociones\", shape=plaintext];\n";
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE 4 – MATRIZ DISPERSA – MAPA DE ASIENTOS
// ─────────────────────────────────────────────────────────────
bool GraphvizReporter::generarReporteMatriz(
    MatrizDispersa& matriz,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    int totalAsientos    = matriz.getMaxFilas() * matriz.getMaxColumnas();
    int reservados       = matriz.getAsientosReservadosCount();
    int libres           = totalAsientos - reservados;

    file << "digraph MatrizDispersa {\n";
    file << "    graph [label=\"Mapa de Asientos — Matriz Dispersa"
         << "\\nTotal: " << totalAsientos
         << " | Libres: " << libres
         << " | Reservados: " << reservados
         << "\", labelloc=t, fontsize=14, fontname=\"Helvetica\"];\n";
    file << "    node [fontname=\"Helvetica\", fontsize=10, style=filled];\n";
    file << "    rankdir=LR;\n";

    file << "    root [label=\"Sala\\n" << matriz.getMaxFilas()
         << " x " << matriz.getMaxColumnas()
         << "\", shape=box, fillcolor=\"#21262D\", fontcolor=white];\n";

    file << "    stats [label=\"Estadísticas\\n"
         << "Total: " << totalAsientos
         << "\\nLibres: " << libres
         << "\\nReservados: " << reservados
         << "\", shape=note, fillcolor=\"#3B82F6\", fontcolor=white];\n";
    file << "    root -> stats [style=dotted, color=gray];\n";

    if (matriz.getMaxColumnas() > 0) {
        file << "    { rank=same;\n";
        file << "        col_header_0 [label=\"F\\\\C\", shape=box,"
             << " fillcolor=\"#30363D\", fontcolor=white];\n";
        for (int c = 1; c <= matriz.getMaxColumnas(); c++) {
            file << "        col_" << c
                 << " [label=\"Col " << c
                 << "\", shape=box, fillcolor=\"#30363D\", fontcolor=white];\n";
        }
        file << "    }\n";

        file << "    col_header_0 -> col_1 [style=invis];\n";
        for (int c = 1; c < matriz.getMaxColumnas(); c++) {
            file << "    col_" << c << " -> col_" << (c+1) << " [style=invis];\n";
        }
    }

    NodoCabecera* cabFila = matriz.getCabecerasFilas().getCabeza();
    std::string prevFila  = "root";

    while (cabFila != nullptr) {
        std::string filaId = "fila_" + std::to_string(cabFila->id);

        file << "    " << filaId
             << " [label=\"Fila " << cabFila->id
             << "\", shape=box, fillcolor=\"#161B22\", fontcolor=\"#E2E8F0\"];\n";
        file << "    " << prevFila << " -> " << filaId
             << " [color=\"#555555\"];\n";

        NodoMatriz* nodoInt = cabFila->acceso;
        std::string prevNode = filaId;

        while (nodoInt != nullptr) {
            std::string nodeName =
                "pos_" +
                std::to_string(nodoInt->asiento.getFila()) +
                "_" +
                std::to_string(nodoInt->asiento.getColumna());

            bool ocupado = nodoInt->asiento.isOcupado();
            std::string fillCol = ocupado ? "\"#E74C3C\"" : "\"#2ECC71\"";
            std::string fcol    = "white";
            std::string estadoStr = ocupado
                ? ("Reservado\\n" + nodoInt->asiento.getCliente())
                : "Libre";

            file << "    " << nodeName
                 << " [label=\"F" << nodoInt->asiento.getFila()
                 << ",C" << nodoInt->asiento.getColumna()
                 << "\\n" << estadoStr
                 << "\", shape=box, fillcolor=" << fillCol
                 << ", fontcolor=" << fcol << "];\n";

            file << "    " << prevNode << " -> " << nodeName
                 << " [dir=both, color=\"#555555\"];\n";

            prevNode = nodeName;
            nodoInt  = nodoInt->derecha;
        }

        prevFila = filaId;
        cabFila  = cabFila->siguiente;
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE 5 – ESTRUCTURA DE CLIENTES Y RESERVAS
// ─────────────────────────────────────────────────────────────
bool GraphvizReporter::generarReporteClientes(
    const EstructuraClientes& clientes,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    file << "digraph EstructuraClientes {\n";
    file << "    graph [label=\"Clientes y Reservas — Lista Doblemente Enlazada"
         << "\\nTotal Clientes: " << clientes.getTamanio()
         << "\", labelloc=t, fontsize=14, fontname=\"Helvetica\", rankdir=TB];\n";
    file << "    node [fontname=\"Helvetica\", fontsize=10, style=filled];\n";

    auto sanitizeDotId = [](const std::string& str) -> std::string {
        std::string clean = "";
        for (char c : str) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                clean += c;
            } else {
                clean += '_';
            }
        }
        return clean.empty() ? "node" : clean;
    };

    auto sanitizeLabel = [](const std::string& str) -> std::string {
        std::string clean = "";
        for (char c : str) {
            if (c == '"') clean += "\\\"";
            else if (c == '\\') clean += "\\\\";
            else if (c == '\n') clean += "\\n";
            else clean += c;
        }
        return clean;
    };

    if (!clientes.estaVacia() && clientes.getCabeza() != nullptr) {
        NodoCliente* actual = clientes.getCabeza();
        int cIdx = 1;
        std::cerr << "[Graphviz] Total de clientes: " << clientes.getTamanio() << "\n";

        // 1. Declarar la fila principal de clientes
        file << "    { rank=same;\n";
        while (actual != nullptr) {
            std::cerr << "[Graphviz] Pasada 1 - Cliente #" << cIdx
                      << " ID='" << actual->cliente.getId() << "'"
                      << " Reservas=" << actual->cliente.getReservas().getTamanio()
                      << " CabezaReservas=" << (void*)actual->cliente.getReservas().getCabeza() << "\n";
            std::string rawId = actual->cliente.getId().empty() ? ("cli_idx_" + std::to_string(cIdx)) : actual->cliente.getId();
            std::string cliId = "cli_" + sanitizeDotId(rawId);
            
            file << "        " << cliId
                 << " [shape=box, fillcolor=\"#3B82F6\", fontcolor=white,"
                 << " label=\"Cliente ID: " << sanitizeLabel(actual->cliente.getId())
                 << "\\nNombre: " << sanitizeLabel(actual->cliente.getNombre())
                 << "\\nCorreo: " << sanitizeLabel(actual->cliente.getCorreo())
                 << "\\nTel: " << sanitizeLabel(actual->cliente.getTelefono())
                 << "\\nReservas: " << actual->cliente.getReservas().getTamanio()
                 << "\"];\n";
            actual = actual->siguiente;
            cIdx++;
        }
        file << "    }\n";

        // 2. Enlazar punteros siguiente/anterior de la lista doblemente enlazada
        actual = clientes.getCabeza();
        cIdx = 1;
        while (actual != nullptr) {
            std::string rawId = actual->cliente.getId().empty() ? ("cli_idx_" + std::to_string(cIdx)) : actual->cliente.getId();
            std::string currentId = "cli_" + sanitizeDotId(rawId);

            if (actual->siguiente != nullptr) {
                std::string rawNextId = actual->siguiente->cliente.getId().empty() ? ("cli_idx_" + std::to_string(cIdx + 1)) : actual->siguiente->cliente.getId();
                std::string sigId = "cli_" + sanitizeDotId(rawNextId);
                
                file << "    " << currentId << " -> " << sigId
                     << " [color=\"#3B82F6\", label=\"sig\"];\n";
                file << "    " << sigId << " -> " << currentId
                     << " [color=\"#EF4444\", label=\"ant\", style=dashed];\n";
            }

            // 3. Dibujar la lista de reservas enlazada verticalmente a cada cliente
            NodoReserva* actualRes = actual->cliente.getReservas().getCabeza();
            std::string prevNode = currentId;
            int rIdx = 1;

            while (actualRes != nullptr) {
                std::cerr << "[Graphviz] Pasada 2 - Reserva #" << rIdx
                          << " Nodo=" << (void*)actualRes
                          << " Cod='" << actualRes->reserva.getCodigoReserva() << "'\n";
                std::string resNode = "res_" + sanitizeDotId(rawId) + "_" + std::to_string(rIdx);

                file << "    " << resNode
                     << " [shape=box, fillcolor=\"#F59E0B\", fontcolor=\"#0D1117\","
                     << " label=\"Reserva: " << sanitizeLabel(actualRes->reserva.getCodigoReserva())
                     << "\\nFunción: " << sanitizeLabel(actualRes->reserva.getCodigoFuncion())
                     << "\\nAsiento: F" << actualRes->reserva.getFila() << ", C" << actualRes->reserva.getColumna()
                     << "\\nFecha: " << sanitizeLabel(actualRes->reserva.getFechaReserva())
                     << "\"];\n";

                file << "    " << prevNode << " -> " << resNode
                     << " [color=\"#F59E0B\"];\n";

                prevNode = resNode;
                actualRes = actualRes->siguiente;
                rIdx++;
            }

            actual = actual->siguiente;
            cIdx++;
        }

    } else {
        file << "    empty [label=\"Sin Clientes Registrados\", shape=plaintext];\n";
    }

    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}

// ─────────────────────────────────────────────────────────────
// REPORTE 6 – ÁRBOL B ORDEN 4 – CLIENTES
//   Nodos con tablas HTML. Código de colores:
//     Raíz     → #A3C4F3 (azul claro)
//     Internos → #F4A261 (naranja)
//     Hojas    → #FFF2CC (amarillo suave)
// ─────────────────────────────────────────────────────────────

// Asigna un ID único a cada nodo durante la generación
// (usamos la dirección del puntero convertida a entero).
static std::string nodeIdB(const NodoArbolB* n) {
    // Usar dirección como ID único y reproducible dentro de un DOT
    unsigned long long addr = reinterpret_cast<unsigned long long>(n);
    return "b_" + std::to_string(addr);
}

static std::string sanitizeDotB(const std::string& s) {
    std::string r;
    for (char c : s) {
        if (c == '"')  r += "\\\"";
        else if (c == '<') r += "&lt;";
        else if (c == '>') r += "&gt;";
        else if (c == '&') r += "&amp;";
        else if (c == '\n') r += " ";
        else r += c;
    }
    return r;
}

// Genera la declaración DOT de un nodo con tabla HTML
void GraphvizReporter::generarDOT_ArbolB(NodoArbolB* nodo,
                                          std::ofstream& file,
                                          bool esRaiz,
                                          int& contador) {
    if (nodo == nullptr) return;
    (void)contador;

    // Determinar colores
    std::string bgColor, borderColor, fontColor;
    if (esRaiz && !nodo->esHoja) {
        bgColor     = "#A3C4F3";  // raíz interna – azul claro
        borderColor = "#2563EB";
        fontColor   = "#1E3A5F";
    } else if (!nodo->esHoja) {
        bgColor     = "#F4A261";  // nodo interno – naranja
        borderColor = "#C05621";
        fontColor   = "#7C2D12";
    } else {
        bgColor     = "#FFF2CC";  // hoja – amarillo suave
        borderColor = "#D4A017";
        fontColor   = "#5A4502";
    }

    std::string tipo = esRaiz ? "RAIZ" : (nodo->esHoja ? "HOJA" : "INTERNO");
    std::string nid  = nodeIdB(nodo);

    // Tabla HTML: puertos p0…p_numClaves entre las claves
    file << "    " << nid << " [shape=none, margin=0, label=<\n";
    file << "      <TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\""
         << " BGCOLOR=\"" << bgColor << "\""
         << " COLOR=\""   << borderColor << "\">\n";

    // Fila de tipo de nodo
    file << "        <TR><TD COLSPAN=\"" << (nodo->numClaves * 2 + 1)
         << "\" BGCOLOR=\"" << borderColor
         << "\"><FONT COLOR=\"white\"><B>" << tipo << " | "
         << nodo->numClaves << " clave(s)</B></FONT></TD></TR>\n";

    // Fila de datos: [puerto] [clave] [puerto] [clave] … [puerto]
    file << "        <TR>\n";
    for (int i = 0; i < nodo->numClaves; i++) {
        // Puerto para hijo i
        file << "          <TD PORT=\"p" << i
             << "\" WIDTH=\"12\" HEIGHT=\"20\" BGCOLOR=\""
             << borderColor << "\"> </TD>\n";

        // Celda con datos del cliente
        std::string id_cli  = sanitizeDotB(nodo->claves[i]->getId());
        std::string nom_cli  = sanitizeDotB(nodo->claves[i]->getNombre());
        std::string cor_cli  = sanitizeDotB(nodo->claves[i]->getCorreo());

        file << "          <TD ALIGN=\"LEFT\" BGCOLOR=\"" << bgColor
             << "\"><FONT COLOR=\"" << fontColor << "\">"
             << "<B>ID: " << id_cli << "</B><BR/>"
             << nom_cli << "<BR/>"
             << "<FONT POINT-SIZE=\"8\">" << cor_cli << "</FONT>"
             << "</FONT></TD>\n";
    }
    // Puerto final (hijo más a la derecha)
    file << "          <TD PORT=\"p" << nodo->numClaves
         << "\" WIDTH=\"12\" HEIGHT=\"20\" BGCOLOR=\""
         << borderColor << "\"> </TD>\n";
    file << "        </TR>\n";
    file << "      </TABLE>>];\n";

    // Recursión sobre hijos
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; i++) {
            if (nodo->hijos[i] != nullptr) {
                generarDOT_ArbolB(nodo->hijos[i], file, false, contador);
            }
        }
    }
}

// Genera las aristas desde nodo padre a sus hijos
void GraphvizReporter::generarAristas_ArbolB(NodoArbolB* nodo,
                                              std::ofstream& file,
                                              int& contador) {
    if (nodo == nullptr || nodo->esHoja) return;
    (void)contador;

    std::string parentId = nodeIdB(nodo);
    for (int i = 0; i <= nodo->numClaves; i++) {
        if (nodo->hijos[i] != nullptr) {
            std::string childId = nodeIdB(nodo->hijos[i]);
            file << "    " << parentId << ":p" << i
                 << " -> " << childId
                 << " [color=\"#555555\"];\n";
            generarAristas_ArbolB(nodo->hijos[i], file, contador);
        }
    }
}

bool GraphvizReporter::generarReporteArbolB(
    const EstructuraClientes& clientes,
    const std::string& nombreArchivo
) {
    std::string dotPath = getOutputPath(nombreArchivo, "dot");
    std::string pngPath = getOutputPath(nombreArchivo, "png");

    std::ofstream file(dotPath);
    if (!file.is_open()) return false;

    const ArbolB& arbol = clientes.getArbol();

    file << "digraph ArbolB {\n";
    file << "    graph [label=\"Gestión de Clientes — Árbol B Orden 4"
         << "\\nTotal Clientes: " << clientes.getTamanio()
         << "\", labelloc=t, fontsize=15, fontname=\"Helvetica\"];\n";
    file << "    node  [fontname=\"Helvetica\", fontsize=10];\n";
    file << "    edge  [arrowhead=vee, arrowsize=0.8];\n";
    file << "    rankdir=TB;\n";

    if (!arbol.estaVacio()) {
        // Leyenda
        file << "    leyenda [shape=plaintext, fontsize=10, label=<"
             << "<TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"4\">"
             << "<TR><TD BGCOLOR=\"#A3C4F3\"> Raíz </TD>"
             << "<TD BGCOLOR=\"#F4A261\"> Interno </TD>"
             << "<TD BGCOLOR=\"#FFF2CC\"> Hoja </TD></TR>"
             << "</TABLE>>];\n\n";

        int contador = 0;
        generarDOT_ArbolB(arbol.getRaiz(), file, true, contador);
        file << "\n";
        generarAristas_ArbolB(arbol.getRaiz(), file, contador);
    } else {
        file << "    empty [label=\"Árbol B Vacío\", shape=plaintext];\n";
    }

    // Estadísticas
    auto& hashRef = SistemaCine::getInstancia().reservas;
    file << "\n    // Estadísticas\n";
    file << "    stats_node [shape=plaintext, margin=0, label=<\n";
    file << "      <TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    file << "        <TR><TD COLSPAN=\"2\" BGCOLOR=\"#FCA5A5\"><B>Estadísticas Tabla Hash</B></TD></TR>\n";
    file << "        <TR><TD>Tamaño de Tabla</TD><TD>" << hashRef.getTamanio() << "</TD></TR>\n";
    file << "        <TR><TD>Total Reservas</TD><TD>" << hashRef.getReservasTotales() << "</TD></TR>\n";
    file << "        <TR><TD>Buckets Ocupados</TD><TD>" << hashRef.getBucketsOcupados() << "</TD></TR>\n";
    file << "        <TR><TD>Colisiones</TD><TD>" << hashRef.getColisionesActuales() << "</TD></TR>\n";
    file << "      </TABLE>>];\n\n";

    // Dibujar Tabla Hash de Reservas (Buckets)
    file << "    // Buckets de la Tabla Hash\n";
    file << "    node [shape=record, style=filled, fillcolor=\"#D1FAE5\", fontcolor=\"black\"];\n";
    
    auto& tablaH = hashRef.getTabla();
    for (int i = 0; i < (int)tablaH.size(); ++i) {
        file << "    bucket_" << i << " [label=\"<f0> Índice " << i << "\"];\n";
        
        // Dibujar y enlazar la lista encadenada en este índice
        NodoReserva* nr = tablaH[i].getCabeza();
        std::string origin = "bucket_" + std::to_string(i) + ":f0";
        int j = 0;
        while(nr != nullptr) {
            std::string nodeId = "hashnode_" + std::to_string(i) + "_" + std::to_string(j);
            file << "    " << nodeId << " [fillcolor=\"#FEF08A\", label=\"Reserva: " << sanitizeDotB(nr->reserva.getCodigoReserva()) 
                 << "\\n" << sanitizeDotB(nr->reserva.getIdCliente()) << " -\> " << sanitizeDotB(nr->reserva.getCodigoFuncion()) << "\"];\n";
            file << "    " << origin << " -> " << nodeId << ";\n";
            origin = nodeId;
            nr = nr->siguiente;
            j++;
        }
    }

    // Alinear todos los buckets horizontal o verticalmente (opcional, los dejamos sueltos por ahora o agregamos un arreglo oculto)
    file << "    { rank=same;";
    for (int i = 0; i < (int)tablaH.size(); ++i) {
        file << " bucket_" << i << ";";
    }
    file << " }\n";



    file << "}\n";
    file.close();

    ejecutarComandoGraphviz(dotPath, pngPath);
    return true;
}