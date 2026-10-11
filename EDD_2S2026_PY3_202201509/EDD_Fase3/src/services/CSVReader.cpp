#include "../../include/services/CSVReader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool CSVReader::cargarPeliculasDesdeCSV(const std::string& rutaArchivo, ArbolBST& arbol) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << " Error: No se pudo abrir el archivo CSV en la ruta: " << rutaArchivo << "\n";
        return false;
    }

    std::string linea;
    bool primeraLinea = true;
    int cargadas = 0;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        // omito encabezado si existe la palabra 'codigo'
        if (primeraLinea) {
            primeraLinea = false;
            if (linea.find("codigo") != std::string::npos || linea.find("Codigo") != std::string::npos) {
                continue;
            }
        }

        std::stringstream ss(linea);
        std::string codigo, titulo, genero, strDuracion, clasificacion, idioma, fechaEstreno, fechaFin;

        if (std::getline(ss, codigo, ',') &&
            std::getline(ss, titulo, ',') &&
            std::getline(ss, genero, ',') &&
            std::getline(ss, strDuracion, ',') &&
            std::getline(ss, clasificacion, ',') &&
            std::getline(ss, idioma, ',') &&
            std::getline(ss, fechaEstreno, ',') &&
            std::getline(ss, fechaFin, ',')) {

            // Limpieza básica de retornos de carro
            if (!fechaFin.empty() && fechaFin.back() == '\r') {
                fechaFin.pop_back();
            }

            int duracion = 0;
            try {
                duracion = std::stoi(strDuracion);
            } catch (...) {
                duracion = 0;
            }

            Pelicula p(codigo, titulo, genero, duracion, clasificacion, idioma, fechaEstreno, fechaFin);
            arbol.insertar(p);
            cargadas++;
        }
    }

    archivo.close();
    std::cout << " Carga masiva completada exitosamente: " << cargadas << " peliculas registradas en el BST.\n";
    return true;
}