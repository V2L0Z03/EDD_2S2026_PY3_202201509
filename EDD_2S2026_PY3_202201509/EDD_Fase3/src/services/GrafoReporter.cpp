#include "../../include/services/GrafoReporter.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>

static std::string sanitizeLabel(const std::string& str) {
    std::string clean = "";
    for (char c : str) {
        if (c == '"') clean += "\\\"";
        else if (c == '\\') clean += "\\\\";
        else if (c == '\n') clean += "\\n";
        else clean += c;
    }
    return clean;
}

static std::string sanitizeNodeId(const std::string& str) {
    std::string clean = "";
    for (char c : str) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            clean += c;
        } else {
            clean += '_';
        }
    }
    return clean.empty() ? "node" : clean;
}

void GrafoReporter::asegurarCarpetaReportes() {
    try {
        std::filesystem::create_directories("public/reports");
    } catch (...) {
        std::system("if not exist \"public\\reports\" mkdir \"public\\reports\"");
    }
}

bool GrafoReporter::ejecutarComandoGraphviz(const std::string& rutaDot, const std::string& rutaPng) {
    std::string comando = "dot -Tpng \"" + rutaDot + "\" -o \"" + rutaPng + "\"";
    int res = std::system(comando.c_str());
    if (res == 0) {
        std::cout << "[GrafoReporter] PNG generado exitosamente: " << rutaPng << "\n";
        return true;
    } else {
        std::cerr << "[GrafoReporter] Error ejecutando comando dot. Asegúrate de tener Graphviz en PATH.\n";
        return false;
    }
}

// ─────────────────────────────────────────────────────────────
// REPORTE 1: Grafo No Dirigido de Sedes
// ─────────────────────────────────────────────────────────────
bool GrafoReporter::generarReporteGrafoSedes(
    const GrafoSedes& grafo,
    const std::string& rutaDot,
    const std::string& rutaPng)
{
    asegurarCarpetaReportes();

    std::ofstream file(rutaDot);
    if (!file.is_open()) {
        std::cerr << "[GrafoReporter] Error al abrir archivo DOT: " << rutaDot << "\n";
        return false;
    }

    file << "graph GrafoSedes {\n";
    file << "    graph [label=\"RED CINEMATOGRÁFICA — GRAFO DE SEDES\\nCinemaUSAC (Total Sedes: "
         << grafo.getNumVertices() << ", Conexiones: " << grafo.getNumAristas() << ")\","
         << " labelloc=t, fontsize=18, fontname=\"Helvetica-Bold\","
         << " bgcolor=\"#0A0F1E\", fontcolor=\"#F0F4FF\", pad=\"0.5\", nodesep=\"0.8\"];\n";
    file << "    node [shape=box, style=\"filled,rounded\", fillcolor=\"#1E293B\", fontcolor=\"#F8FAFC\","
         << " fontname=\"Helvetica\", fontsize=11, color=\"#6366F1\", penwidth=2.0];\n";
    file << "    edge [color=\"#818CF8\", fontcolor=\"#F59E0B\", fontname=\"Helvetica-Bold\", fontsize=10, penwidth=2.0];\n\n";

    if (grafo.getNumVertices() == 0) {
        file << "    vacio [label=\"(Sin Sedes Registradas)\", shape=plaintext, fontcolor=\"#94A3B8\"];\n";
        file << "}\n";
        file.close();
        ejecutarComandoGraphviz(rutaDot, rutaPng);
        return true;
    }

    // 1. Declarar todos los Vértices con sus datos completos
    Vertice* v = grafo.getCabeza();
    while (v != nullptr) {
        std::string nId = "sede_" + sanitizeNodeId(v->sede.getCodigo());
        file << "    " << nId << " [label=\"Código: " << sanitizeLabel(v->sede.getCodigo())
             << "\\nNombre: " << sanitizeLabel(v->sede.getNombre())
             << "\\nSalas: "  << v->sede.getCantidadSalas()
             << "\\nTel: "    << sanitizeLabel(v->sede.getTelefono())
             << "\"];\n";
        v = v->siguiente;
    }

    file << "\n    // Aristas Ponderadas (Películas en Común)\n";

    // 2. Declarar las Aristas Ponderadas sin duplicar (origen < destino)
    v = grafo.getCabeza();
    while (v != nullptr) {
        Arista* a = v->listaAdy;
        while (a != nullptr) {
            if (v->sede.getCodigo() < a->destino->sede.getCodigo()) {
                std::string idOrigen  = "sede_" + sanitizeNodeId(v->sede.getCodigo());
                std::string idDestino = "sede_" + sanitizeNodeId(a->destino->sede.getCodigo());
                int pesoEntero = static_cast<int>(a->peso);

                file << "    " << idOrigen << " -- " << idDestino
                     << " [label=\" " << pesoEntero << " "
                     << (pesoEntero == 1 ? "película" : "películas") << " \"];\n";
            }
            a = a->siguiente;
        }
        v = v->siguiente;
    }

    file << "}\n";
    file.close();

    return ejecutarComandoGraphviz(rutaDot, rutaPng);
}

// ─────────────────────────────────────────────────────────────
// REPORTE 2: Representación Visual Clásica de la Lista de Adyacencia
// ─────────────────────────────────────────────────────────────
bool GrafoReporter::generarReporteListaAdyacencia(
    const GrafoSedes& grafo,
    const std::string& rutaDot,
    const std::string& rutaPng)
{
    asegurarCarpetaReportes();

    std::ofstream file(rutaDot);
    if (!file.is_open()) {
        std::cerr << "[GrafoReporter] Error al abrir archivo DOT: " << rutaDot << "\n";
        return false;
    }

    file << "digraph ListaAdyacencia {\n";
    file << "    graph [rankdir=LR, label=\"ESTRUCTURA DE DATOS — LISTA DE ADYACENCIA\\nCinemaUSAC\","
         << " labelloc=t, fontsize=18, fontname=\"Helvetica-Bold\","
         << " bgcolor=\"#0A0F1E\", fontcolor=\"#F0F4FF\", pad=\"0.5\"];\n";
    file << "    node [fontname=\"Helvetica\", fontsize=11, style=filled];\n";
    file << "    edge [color=\"#6366F1\", fontcolor=\"#E2E8F0\", fontname=\"Helvetica\", fontsize=10, penwidth=1.5];\n\n";

    if (grafo.getNumVertices() == 0) {
        file << "    vacio [label=\"(Grafo Vacío)\", shape=plaintext, fontcolor=\"#94A3B8\"];\n";
        file << "}\n";
        file.close();
        ejecutarComandoGraphviz(rutaDot, rutaPng);
        return true;
    }

    // 1. Declarar columna vertical de vértices cabecera
    file << "    // Nodos Principales de Vértices\n";
    Vertice* v = grafo.getCabeza();
    while (v != nullptr) {
        std::string vId = "v_" + sanitizeNodeId(v->sede.getCodigo());
        file << "    " << vId << " [label=\"Vértice: " << sanitizeLabel(v->sede.getCodigo())
             << "\\n" << sanitizeLabel(v->sede.getNombre())
             << "\", shape=box, fillcolor=\"#4F46E5\", fontcolor=\"#FFFFFF\", color=\"#818CF8\", penwidth=2.0];\n";
        v = v->siguiente;
    }

    file << "\n    // Alineación vertical de cabeceras\n";
    v = grafo.getCabeza();
    while (v != nullptr && v->siguiente != nullptr) {
        file << "    v_" << sanitizeNodeId(v->sede.getCodigo())
             << " -> v_" << sanitizeNodeId(v->siguiente->sede.getCodigo())
             << " [style=invis];\n";
        v = v->siguiente;
    }

    file << "\n    // Listas enlazadas de aristas adyacentes\n";
    v = grafo.getCabeza();
    while (v != nullptr) {
        std::string codV = sanitizeNodeId(v->sede.getCodigo());
        std::string prevNode = "v_" + codV;

        Arista* a = v->listaAdy;
        int aristaIdx = 0;

        if (a == nullptr) {
            std::string nullId = "null_" + codV;
            file << "    " << nullId << " [label=\"NULL\", shape=plaintext, fontcolor=\"#94A3B8\", fontsize=10];\n";
            file << "    " << prevNode << " -> " << nullId << " [color=\"#94A3B8\", style=dashed];\n";
        } else {
            while (a != nullptr) {
                std::string aId = "ady_" + codV + "_" + std::to_string(aristaIdx);
                int pesoEntero = static_cast<int>(a->peso);

                file << "    " << aId << " [label=\"Dest: " << sanitizeLabel(a->destino->sede.getCodigo())
                     << "\\n" << sanitizeLabel(a->destino->sede.getNombre())
                     << "\\nPeso: " << pesoEntero
                     << "\", shape=ellipse, fillcolor=\"#059669\", fontcolor=\"#FFFFFF\", color=\"#34D399\", penwidth=1.5];\n";

                file << "    " << prevNode << " -> " << aId << " [label=\"sig\"];\n";

                prevNode = aId;
                a = a->siguiente;
                aristaIdx++;
            }

            // Terminador NULL al final de la lista
            std::string nullId = "null_" + codV;
            file << "    " << nullId << " [label=\"NULL\", shape=plaintext, fontcolor=\"#94A3B8\", fontsize=10];\n";
            file << "    " << prevNode << " -> " << nullId << " [color=\"#94A3B8\", style=dashed];\n";
        }

        v = v->siguiente;
    }

    file << "}\n";
    file.close();

    return ejecutarComandoGraphviz(rutaDot, rutaPng);
}
