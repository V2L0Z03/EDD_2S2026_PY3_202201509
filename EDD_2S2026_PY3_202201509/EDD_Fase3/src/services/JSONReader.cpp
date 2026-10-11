#include "../../include/services/JSONReader.hpp"
#include "../../include/vendor/json.hpp"
#include "../../include/models/SistemaCine.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using json = nlohmann::json;

static std::string getStringValue(const json& j, const std::vector<std::string>& keys, const std::string& defVal = "") {
    for (const auto& k : keys) {
        if (j.contains(k) && !j[k].is_null()) {
            if (j[k].is_string()) return j[k].get<std::string>();
            if (j[k].is_number()) return std::to_string(j[k].get<int>());
        }
    }
    return defVal;
}

static int getIntValue(const json& j, const std::vector<std::string>& keys, int defVal = 0) {
    for (const auto& k : keys) {
        if (j.contains(k) && !j[k].is_null()) {
            if (j[k].is_number_integer() || j[k].is_number_unsigned()) {
                return j[k].get<int>();
            }
            if (j[k].is_string()) {
                std::string s = j[k].get<std::string>();
                std::string digits = "";
                for (char c : s) {
                    if (std::isdigit(static_cast<unsigned char>(c))) digits += c;
                    else if (!digits.empty()) break;
                }
                if (!digits.empty()) {
                    try { return std::stoi(digits); } catch(...) {}
                }
            }
        }
    }
    return defVal;
}

static double getDoubleValue(const json& j, const std::vector<std::string>& keys, double defVal = 0.0) {
    for (const auto& k : keys) {
        if (j.contains(k) && !j[k].is_null()) {
            if (j[k].is_number()) return j[k].get<double>();
            if (j[k].is_string()) {
                try { return std::stod(j[k].get<std::string>()); } catch(...) {}
            }
        }
    }
    return defVal;
}

bool JSONReader::cargarPeliculasDesdeJSON(const std::string& rutaArchivo, ArbolBST& arbolCartelera, ArbolAVL& arbolFunciones) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[JSONReader] Error: No se pudo abrir: " << rutaArchivo << "\n";
        return false;
    }

    json root;
    try {
        archivo >> root;
    } catch (const json::parse_error& e) {
        std::cerr << "[JSONReader] Error de parseo JSON: " << e.what() << "\n";
        return false;
    }
    archivo.close();

    json peliculasArray;
    if (root.is_array()) {
        peliculasArray = root;
    } else if (root.contains("Peliculas") && root["Peliculas"].is_array()) {
        peliculasArray = root["Peliculas"];
    } else if (root.contains("peliculas") && root["peliculas"].is_array()) {
        peliculasArray = root["peliculas"];
    } else {
        std::cerr << "[JSONReader] Sin clave 'Peliculas' o 'peliculas' en el JSON.\n";
        return false;
    }

    int cargadasPelis = 0;
    int cargadasFuncs = 0;

    for (const auto& item : peliculasArray) {
        try {
            std::string codigo        = getStringValue(item, {"Codigo", "codigo"});
            std::string titulo        = getStringValue(item, {"Nombre", "titulo", "nombre"});
            std::string genero        = getStringValue(item, {"Genero", "genero"});
            int         duracion      = getIntValue(item,     {"Duracion", "duracion_minutos", "duracion"});
            std::string clasificacion = getStringValue(item, {"Clasificacion", "clasificacion"});
            std::string idioma        = getStringValue(item, {"Idioma", "idioma"});
            std::string fechaEstreno  = getStringValue(item, {"Estreno", "fecha_estreno", "estreno"});
            std::string fechaFin      = getStringValue(item, {"Retiro", "fecha_fin", "retiro"});

            if (codigo.empty() || titulo.empty()) {
                continue;
            }

            Pelicula p(codigo, titulo, genero, duracion,
                       clasificacion, idioma, fechaEstreno, fechaFin);
            arbolCartelera.insertar(p);
            cargadasPelis++;

            json funcionesArray;
            if (item.contains("Funciones") && item["Funciones"].is_array()) {
                funcionesArray = item["Funciones"];
            } else if (item.contains("funciones") && item["funciones"].is_array()) {
                funcionesArray = item["funciones"];
            }

            for (const auto& funcItem : funcionesArray) {
                std::string codFunc = getStringValue(funcItem, {"Codigo", "codigo", "codigo_funcion"});
                std::string fecha   = getStringValue(funcItem, {"Fecha", "fecha"});
                std::string hora    = getStringValue(funcItem, {"Hora", "hora", "horario"});
                std::string sala    = getStringValue(funcItem, {"Sala", "sala"});
                double precio       = getDoubleValue(funcItem, {"Precio", "precio"});

                if (!codFunc.empty()) {
                    Funcion f(codFunc, codigo, fecha, hora, sala, precio);
                    if (arbolFunciones.insertar(f)) {
                        cargadasFuncs++;
                    }
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "[JSONReader] Error procesando película/función: " << e.what() << "\n";
        }
    }

    std::cout << "[JSONReader] Carga completada: " << cargadasPelis
              << " películas en BST y " << cargadasFuncs << " funciones en AVL.\n";
    return cargadasPelis > 0 || cargadasFuncs > 0;
}

bool JSONReader::cargarClientesDesdeJSON(const std::string& rutaArchivo, EstructuraClientes& clientes, ArbolAVL& arbolFunciones, ArbolBST& arbolCartelera) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[JSONReader] Error: No se pudo abrir: " << rutaArchivo << "\n";
        return false;
    }

    json root;
    try {
        archivo >> root;
    } catch (const json::parse_error& e) {
        std::cerr << "[JSONReader] Error de parseo JSON: " << e.what() << "\n";
        return false;
    }
    archivo.close();

    json clientesArray;
    if (root.contains("Clientes") && root["Clientes"].is_array()) {
        clientesArray = root["Clientes"];
    } else if (root.contains("clientes") && root["clientes"].is_array()) {
        clientesArray = root["clientes"];
    } else if (root.is_array()) {
        clientesArray = root;
    } else {
        std::cerr << "[JSONReader] Sin clave 'Clientes' en el JSON.\n";
        return false;
    }

    int cargadosCli = 0;
    int cargadasRes = 0;

    for (const auto& item : clientesArray) {
        try {
            std::string id       = getStringValue(item, {"Id", "ID", "id", "id_cliente", "CTI", "cti", "DPI", "dpi", "dpi_cliente"});
            std::string nombre   = getStringValue(item, {"Nombre", "nombre", "Name", "name"});
            std::string correo   = getStringValue(item, {"Correo", "correo", "email", "Email", "Mail"});
            std::string pass     = getStringValue(item, {"Password", "password", "contrasena", "Contrasena", "Clave", "pass"});
            std::string telefono = getStringValue(item, {"Telefono", "telefono", "phone", "Phone", "Tel"});

            if (id.empty() && correo.empty()) continue;
            if (id.empty()) id = correo;
            if (correo.empty()) correo = id;

            Cliente cli(id, nombre, correo, pass, telefono);

            json reservasArray;
            if (item.contains("Reservas") && item["Reservas"].is_array()) {
                reservasArray = item["Reservas"];
            } else if (item.contains("reservas") && item["reservas"].is_array()) {
                reservasArray = item["reservas"];
            }

            for (const auto& resItem : reservasArray) {
                std::string codRes  = getStringValue(resItem, {"Codigo", "codigo", "codigo_reserva"});
                std::string codFunc = getStringValue(resItem, {"CodigoFuncion", "codigo_funcion", "funcion"});
                int fila            = getIntValue(resItem,     {"Fila", "fila"});
                int col             = getIntValue(resItem,     {"Columna", "columna"});
                std::string fecha   = getStringValue(resItem, {"FechaReserva", "fecha_reserva", "fecha"});

                if (!codFunc.empty() && fila > 0 && col > 0) {
                    if (codRes.empty()) {
                        codRes = "R" + std::to_string(cargadasRes + 1);
                    }
                    Reserva r(codRes, codFunc, id, fila, col, fecha); // Pass 'id' (clienteId)
                    cli.getReservas().insertar(r);
                    SistemaCine::getInstancia().reservas.insertar(r); // Insertar en la Tabla Hash del sistema
                    cargadasRes++;

                    NodoAVL* nodoF = arbolFunciones.buscar(codFunc);
                    if (nodoF != nullptr) {
                        NodoBST* nodoP = arbolCartelera.buscar(nodoF->funcion.getCodigoPelicula());
                        if (nodoP != nullptr) {
                            nodoP->salaCine.reservarAsiento(fila, col, nombre.empty() ? correo : nombre);
                        }
                    }
                }
            }

            if (clientes.insertar(cli)) {
                cargadosCli++;
            } else {
                NodoCliente* existente = clientes.buscarPorId(id);
                if (existente != nullptr) {
                    NodoReserva* nr = cli.getReservas().getCabeza();
                    while (nr != nullptr) {
                        if (existente->cliente.getReservas().buscar(nr->reserva.getCodigoReserva()) == nullptr) {
                            existente->cliente.getReservas().insertar(nr->reserva);
                        }
                        nr = nr->siguiente;
                    }
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "[JSONReader] Error procesando cliente: " << e.what() << "\n";
        }
    }

    std::cout << "[JSONReader] Carga clientes completada: " << cargadosCli << " clientes y " << cargadasRes << " reservas.\n";
    return cargadosCli > 0;
}

bool JSONReader::cargarTodoDesdeJSON(const std::string& rutaArchivo, ArbolBST& arbolCartelera, ArbolAVL& arbolFunciones, EstructuraClientes& clientes) {
    bool okPelis = cargarPeliculasDesdeJSON(rutaArchivo, arbolCartelera, arbolFunciones);
    bool okClis  = cargarClientesDesdeJSON(rutaArchivo, clientes, arbolFunciones, arbolCartelera);
    return okPelis || okClis;
}

// ─────────────────────────────────────────────────────────────────
// cargarCargaMasiva (Fase 3)
//   Recibe el contenido JSON como string (enviado desde el frontend).
//   Pipeline:
//     1. Parsear y cargar sedes → GrafoSedes
//     2. Cargar películas → BST
//     3. Cargar funciones con validación relacional de sede → AVL
//   Devuelve ResultadoCarga con bitácora de errores.
// ─────────────────────────────────────────────────────────────────
ResultadoCarga JSONReader::cargarCargaMasiva(
    const std::string& contenidoJSON,
    GrafoSedes&        grafoSedes,
    ArbolBST&          arbolCartelera,
    ArbolAVL&          arbolFunciones)
{
    ResultadoCarga resultado;

    json root;
    try {
        root = json::parse(contenidoJSON);
    } catch (const json::parse_error& e) {
        resultado.errores.agregar(
            std::string("[FATAL] JSON inválido: ") + e.what());
        return resultado;
    }

    // ── 1. CARGAR SEDES ──────────────────────────────────────────
    json sedesArray;
    if (root.contains("sedes") && root["sedes"].is_array()) {
        sedesArray = root["sedes"];
    }

    for (const auto& s : sedesArray) {
        try {
            std::string cod  = getStringValue(s, {"codigo", "Codigo"});
            std::string nom  = getStringValue(s, {"nombre", "Nombre"});
            std::string dir  = getStringValue(s, {"direccion", "Direccion"});
            int         sal  = getIntValue(s,    {"cantidad_salas", "salas", "Salas"});
            std::string tel  = getStringValue(s, {"telefono", "Telefono"});

            if (cod.empty()) {
                resultado.advertencias.agregar(
                    "[SEDE IGNORADA] No tiene código definido.");
                continue;
            }

            Sede sede(cod, nom, dir, sal, tel);
            if (grafoSedes.insertarSede(sede)) {
                resultado.sedesCargadas++;
            } else {
                resultado.advertencias.agregar(
                    "[SEDE DUPLICADA] " + cod + " - " + nom);
            }
        } catch (const std::exception& e) {
            resultado.errores.agregar(
                std::string("[SEDE ERROR] ") + e.what());
        }
    }

    // ── 2. CARGAR PELÍCULAS + FUNCIONES CON VALIDACIÓN ───────────
    json peliculasArray;
    if (root.contains("peliculas") && root["peliculas"].is_array()) {
        peliculasArray = root["peliculas"];
    } else if (root.contains("Peliculas") && root["Peliculas"].is_array()) {
        peliculasArray = root["Peliculas"];
    } else if (root.is_array()) {
        peliculasArray = root;
    }

    for (const auto& item : peliculasArray) {
        try {
            std::string codigo        = getStringValue(item, {"Codigo", "codigo"});
            std::string titulo        = getStringValue(item, {"Nombre", "titulo", "nombre"});
            std::string genero        = getStringValue(item, {"Genero", "genero"});
            int         duracion      = getIntValue(item,    {"Duracion", "duracion_minutos", "duracion"});
            std::string clasificacion = getStringValue(item, {"Clasificacion", "clasificacion"});
            std::string idioma        = getStringValue(item, {"Idioma", "idioma"});
            std::string fechaEstreno  = getStringValue(item, {"Estreno", "fecha_estreno"});
            std::string fechaFin      = getStringValue(item, {"Retiro", "fecha_fin"});

            if (codigo.empty() || titulo.empty()) continue;

            Pelicula p(codigo, titulo, genero, duracion,
                       clasificacion, idioma, fechaEstreno, fechaFin);
            arbolCartelera.insertar(p);
            resultado.peliculasCargadas++;

            // ── 3. FUNCIONES con validación de sede ──────────────
            json funcionesArray;
            if (item.contains("funciones") && item["funciones"].is_array()) {
                funcionesArray = item["funciones"];
            } else if (item.contains("Funciones") && item["Funciones"].is_array()) {
                funcionesArray = item["Funciones"];
            }

            for (const auto& f : funcionesArray) {
                std::string codFunc = getStringValue(f, {"codigo_funcion", "Codigo", "codigo"});
                std::string codSede = getStringValue(f, {"sede", "Sede"});
                std::string fecha   = getStringValue(f, {"fecha",   "Fecha"});
                std::string hora    = getStringValue(f, {"horario", "hora", "Hora"});
                std::string sala    = getStringValue(f, {"sala",    "Sala"});
                double      precio  = getDoubleValue(f, {"precio",  "Precio"});

                if (codFunc.empty()) continue;

                // VALIDACIÓN RELACIONAL: la sede debe existir en el grafo
                if (!codSede.empty() && !grafoSedes.sedeExiste(codSede)) {
                    resultado.funcionesRechazadas++;
                    resultado.errores.agregar(
                        "[FUNCIÓN RECHAZADA] Código: " + codFunc +
                        " (Película: " + codigo + ")" +
                        " — Sede inexistente: \"" + codSede + "\"");
                    continue;   // no insertar en el AVL
                }

                Funcion func(codFunc, codigo, fecha, hora, sala, precio);
                if (arbolFunciones.insertar(func)) {
                    resultado.funcionesCargadas++;
                } else {
                    resultado.advertencias.agregar(
                        "[FUNCIÓN DUPLICADA] " + codFunc);
                }
            }

        } catch (const std::exception& e) {
            resultado.errores.agregar(
                std::string("[PELÍCULA ERROR] ") + e.what());
        }
    }

    std::cout << "[CargaMasiva] Sedes=" << resultado.sedesCargadas
              << " Peliculas=" << resultado.peliculasCargadas
              << " Funciones=" << resultado.funcionesCargadas
              << " Rechazadas=" << resultado.funcionesRechazadas << "\n";

    return resultado;
}

