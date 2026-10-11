#ifndef GRAFO_REPORTER_HPP
#define GRAFO_REPORTER_HPP

#include "../structures/GrafoSedes.hpp"
#include <string>

class GrafoReporter {
public:
    // Asegura que la carpeta public/reports/ exista
    static void asegurarCarpetaReportes();

    // Reporte 1: Grafo No Dirigido con pesos y datos completos de cada Sede
    static bool generarReporteGrafoSedes(
        const GrafoSedes& grafo,
        const std::string& rutaDot = "public/reports/grafo_sedes.dot",
        const std::string& rutaPng = "public/reports/grafo_sedes.png");

    // Reporte 2: Representación visual clásica de la Lista de Adyacencia
    static bool generarReporteListaAdyacencia(
        const GrafoSedes& grafo,
        const std::string& rutaDot = "public/reports/lista_adyacencia.dot",
        const std::string& rutaPng = "public/reports/lista_adyacencia.png");

    // Ejecuta el comando dot del sistema
    static bool ejecutarComandoGraphviz(
        const std::string& rutaDot,
        const std::string& rutaPng);
};

#endif // GRAFO_REPORTER_HPP
