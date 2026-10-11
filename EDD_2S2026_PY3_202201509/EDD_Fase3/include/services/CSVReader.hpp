#ifndef CSV_READER_HPP
#define CSV_READER_HPP

#include <string>
#include "../structures/ArbolBST.hpp"

class CSVReader {
public:
    static bool cargarPeliculasDesdeCSV(const std::string& rutaArchivo, ArbolBST& arbol);
};

#endif // CSV_READER_HPP