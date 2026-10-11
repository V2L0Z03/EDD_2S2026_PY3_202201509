#ifndef GRAPHVIZ_REPORTER_HPP
#define GRAPHVIZ_REPORTER_HPP

#include "../structures/ArbolBST.hpp"
#include "../structures/ArbolAVL.hpp"
#include "../structures/ListaCircularPromos.hpp"
#include "../structures/ListaCircularDobleSolicitudes.hpp"
#include "../structures/MatrizDispersa.hpp"
#include "../structures/EstructuraClientes.hpp"   // incluye ArbolB.hpp
#include <string>

class GraphvizReporter {
private:
    static void generarDOT_BST(NodoBST* nodo, std::ofstream& file);
    static void generarDOT_AVL(NodoAVL* nodo, std::ofstream& file);

    // estos son mis Helpers para el reporte del Árbol B
    static void generarDOT_ArbolB(NodoArbolB* nodo,
                                   std::ofstream& file,
                                   bool esRaiz,
                                   int& contador);
    static void generarAristas_ArbolB(NodoArbolB* nodo,
                                       std::ofstream& file,
                                       int& contador);

    static void ejecutarComandoGraphviz(const std::string& rutaDot,
                                        const std::string& rutaPng);

public:
    // estos son los Reportes existentes
    static bool generarReporteBST(const ArbolBST& arbol,
                                   const std::string& nombreArchivo = "reporte_bst");
    static bool generarReporteAVL(const ArbolAVL& arbol,
                                   const std::string& nombreArchivo = "reporte_avl");
    static bool generarReporteSolicitudes(const ListaCircularDobleSolicitudes& lista,
                                           const std::string& nombreArchivo = "reporte_solicitudes");
    static bool generarReportePromociones(const ListaCircularPromos& lista,
                                           const std::string& nombreArchivo = "reporte_promociones");
    static bool generarReporteMatriz(MatrizDispersa& matriz,
                                      const std::string& nombreArchivo = "reporte_matriz");
    static bool generarReporteClientes(const EstructuraClientes& clientes,
                                        const std::string& nombreArchivo = "reporte_clientes");

    // ── N: Este es e reporte del Árbol B con tablas HTML y color
    static bool generarReporteArbolB(const EstructuraClientes& clientes,
                                      const std::string& nombreArchivo = "reporte_arbol_b");
};

#endif // GRAPHVIZ_REPORTER_HPP