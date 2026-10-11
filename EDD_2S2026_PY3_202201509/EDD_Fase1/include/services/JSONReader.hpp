#ifndef JSON_READER_HPP
#define JSON_READER_HPP

#include <string>
#include "../structures/ArbolBST.hpp"
#include "../structures/ArbolAVL.hpp"
#include "../structures/EstructuraClientes.hpp"

class JSONReader {
public:
    // esto es pa mi Carga películas + funciones desde un JSON con estructura anidada
    static bool cargarPeliculasDesdeJSON(const std::string& rutaArchivo, ArbolBST& arbolCartelera, ArbolAVL& arbolFunciones);

    // esto es pa mi Carga clientes + reservas desde un JSON
    static bool cargarClientesDesdeJSON(const std::string& rutaArchivo, EstructuraClientes& clientes, ArbolAVL& arbolFunciones, ArbolBST& arbolCartelera);

    // esto es pa mi Carga unificada de películas, funciones, clientes y reservas desde JSON
    static bool cargarTodoDesdeJSON(const std::string& rutaArchivo, ArbolBST& arbolCartelera, ArbolAVL& arbolFunciones, EstructuraClientes& clientes);
};

#endif // JSON_READER_HPP
