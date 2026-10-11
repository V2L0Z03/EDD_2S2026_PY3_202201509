#define CROW_MAIN
#define CROW_STATIC_DIR "public/"
#include "../include/vendor/crow_all.h"
#include "../include/models/SistemaCine.hpp"
#include "../include/services/JSONReader.hpp"
#include "../include/services/GrafoReporter.hpp"
#include "../include/vendor/json.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

using namespace std;
using json = nlohmann::json;

// Helper para leer archivos en binario / texto
string leerArchivo(const string& filename) {
    ifstream in(filename, ios::in | ios::binary);
    if (in) {
        ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return contents.str();
    }
    return "";
}

long long obtenerTimestampActual() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

int main() {
    crow::SimpleApp app;

    // Asegurar directorio de reportes al iniciar
    GrafoReporter::asegurarCarpetaReportes();

    // ═══════════════════════════════════════════════════════════════
    // RUTAS ESTÁTICAS (FRONTEND)
    // ═══════════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/")
    ([]() {
        string page = leerArchivo("public/index.html");
        auto response = crow::response(page);
        response.set_header("Content-Type", "text/html; charset=utf-8");
        return response;
    });

    CROW_ROUTE(app, "/style.css")
    ([]() {
        string page = leerArchivo("public/style.css");
        auto response = crow::response(page);
        response.set_header("Content-Type", "text/css; charset=utf-8");
        return response;
    });

    CROW_ROUTE(app, "/script.js")
    ([]() {
        string page = leerArchivo("public/script.js");
        auto response = crow::response(page);
        response.set_header("Content-Type", "application/javascript; charset=utf-8");
        return response;
    });

    // Servir imágenes y archivos DOT generados por Graphviz
    CROW_ROUTE(app, "/reports/<string>")
    ([](const string& nombreArchivo) {
        string rutaCompleta = "public/reports/" + nombreArchivo;
        string contenido = leerArchivo(rutaCompleta);
        if (contenido.empty()) {
            return crow::response(404, "Reporte no encontrado.");
        }

        auto response = crow::response(200, contenido);
        if (nombreArchivo.rfind(".png") != string::npos) {
            response.set_header("Content-Type", "image/png");
        } else if (nombreArchivo.rfind(".dot") != string::npos) {
            response.set_header("Content-Type", "text/plain; charset=utf-8");
        }
        response.set_header("Cache-Control", "no-cache, no-store, must-revalidate");
        return response;
    });

    // ═══════════════════════════════════════════════════════════════
    // ENDPOINTS DE AUTENTICACIÓN Y CARGA MASIVA
    // ═══════════════════════════════════════════════════════════════
    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        auto payload = crow::json::load(req.body);
        if (!payload) {
            return crow::response(400, "{\"error\": \"JSON Inválido\"}");
        }

        string email = payload["email"].s();
        string password = payload["password"].s();

        // 1. Validar Administrador
        if (email == "AdminCine@gmail.com" && password == "admin123Pass") {
            crow::json::wvalue res;
            res["status"]  = "success";
            res["role"]    = "admin";
            res["message"] = "Bienvenido Administrador";
            auto response  = crow::response(200, res);
            response.set_header("Content-Type", "application/json");
            return response;
        }

        // 2. Validar Cliente (Árbol B)
        SistemaCine& sys = SistemaCine::getInstancia();
        NodoCliente* nodoCli = sys.clientes.buscarPorCredenciales(email, password);

        if (nodoCli != nullptr) {
            sys.clienteLogueado = &nodoCli->cliente;
            crow::json::wvalue res;
            res["status"]  = "success";
            res["role"]    = "cliente";
            res["nombre"]  = nodoCli->cliente.getNombre();
            res["message"] = "Bienvenido " + nodoCli->cliente.getNombre();
            auto response  = crow::response(200, res);
            response.set_header("Content-Type", "application/json");
            return response;
        }

        crow::json::wvalue el_err;
        el_err["status"]  = "error";
        el_err["message"] = "Credenciales incorrectas o usuario no encontrado.";
        auto responseError = crow::response(401, el_err);
        responseError.set_header("Content-Type", "application/json");
        return responseError;
    });

    CROW_ROUTE(app, "/api/carga-masiva").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        const std::string& contenidoJSON = req.body;
        if (contenidoJSON.empty()) {
            crow::json::wvalue err;
            err["status"]  = "error";
            err["message"] = "Body vacío. Envía el contenido del archivo JSON.";
            return crow::response(400, err);
        }

        SistemaCine& sys = SistemaCine::getInstancia();

        ResultadoCarga r = JSONReader::cargarCargaMasiva(
            contenidoJSON,
            sys.sedes,
            sys.cartelera,
            sys.funciones);

        // Disparo explícito de seguridad para recálculo de aristas ponderadas
        sys.sedes.recalcularAristas(sys.funciones);

        crow::json::wvalue res;
        res["status"]               = (r.errores.total == 0) ? "success" : "partial";
        res["sedes_cargadas"]       = r.sedesCargadas;
        res["peliculas_cargadas"]   = r.peliculasCargadas;
        res["funciones_cargadas"]   = r.funcionesCargadas;
        res["funciones_rechazadas"] = r.funcionesRechazadas;
        res["aristas_conectadas"]   = sys.sedes.getNumAristas();

        std::vector<crow::json::wvalue> arrErr;
        NodoLog* e = r.errores.cabeza;
        while (e) { arrErr.push_back(crow::json::wvalue(e->mensaje)); e = e->siguiente; }
        res["errores"] = std::move(arrErr);

        std::vector<crow::json::wvalue> arrAdv;
        NodoLog* a = r.advertencias.cabeza;
        while (a) { arrAdv.push_back(crow::json::wvalue(a->mensaje)); a = a->siguiente; }
        res["advertencias"] = std::move(arrAdv);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // ═══════════════════════════════════════════════════════════════
    // SECCIÓN 3: ENDPOINTS DE GRAFO DE SEDES Y ANALÍTICAS
    // ═══════════════════════════════════════════════════════════════

    // 1. Obtener catálogo de sedes disponibles (para dropdowns y listas)
    CROW_ROUTE(app, "/api/sedes")
    ([]() {
        SistemaCine& sys = SistemaCine::getInstancia();
        crow::json::wvalue res;
        res["status"] = "success";
        res["total"]  = sys.sedes.getNumVertices();

        std::vector<crow::json::wvalue> lista;
        Vertice* v = sys.sedes.getCabeza();
        while (v != nullptr) {
            crow::json::wvalue item;
            item["codigo"]    = v->sede.getCodigo();
            item["nombre"]    = v->sede.getNombre();
            item["direccion"] = v->sede.getDireccion();
            item["salas"]     = v->sede.getCantidadSalas();
            item["telefono"]  = v->sede.getTelefono();
            lista.push_back(std::move(item));
            v = v->siguiente;
        }
        res["sedes"] = std::move(lista);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // 2. BFS: Camino Mínimo entre dos Sedes (menor número de saltos)
    CROW_ROUTE(app, "/api/sedes/camino-corto").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        auto payload = crow::json::load(req.body);
        if (!payload || !payload.has("origen") || !payload.has("destino")) {
            crow::json::wvalue err;
            err["status"]  = "error";
            err["message"] = "Debe proporcionar 'origen' y 'destino' en el cuerpo JSON.";
            return crow::response(400, err);
        }

        string origen  = payload["origen"].s();
        string destino = payload["destino"].s();

        SistemaCine& sys = SistemaCine::getInstancia();
        ResultadoCaminoBFS resBFS = sys.sedes.buscarCaminoCortoBFS(origen, destino);

        if (!resBFS.encontrado) {
            crow::json::wvalue err;
            err["status"]  = "error";
            err["message"] = "No existe un camino entre las sedes seleccionadas o alguna de las sedes no fue encontrada.";
            auto response = crow::response(200, err);
            response.set_header("Content-Type", "application/json");
            return response;
        }

        crow::json::wvalue res;
        res["status"]  = "success";
        res["origen"]  = origen;
        res["destino"] = destino;
        res["saltos"]  = resBFS.saltos;

        std::vector<crow::json::wvalue> rutaArray;
        NodoRuta* act = resBFS.camino;
        while (act != nullptr) {
            crow::json::wvalue nodoObj;
            nodoObj["codigo"]    = act->vertice->sede.getCodigo();
            nodoObj["nombre"]    = act->vertice->sede.getNombre();
            nodoObj["direccion"] = act->vertice->sede.getDireccion();
            nodoObj["salas"]     = act->vertice->sede.getCantidadSalas();
            nodoObj["telefono"]  = act->vertice->sede.getTelefono();
            rutaArray.push_back(std::move(nodoObj));
            act = act->siguiente;
        }
        res["ruta"] = std::move(rutaArray);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // 3. Ranking de Aristas (Orden descendente por películas en común)
    CROW_ROUTE(app, "/api/sedes/ranking")
    ([]() {
        SistemaCine& sys = SistemaCine::getInstancia();
        ListaRankingAristas ranking;
        sys.sedes.obtenerRankingAristas(ranking);

        crow::json::wvalue res;
        res["status"] = "success";
        res["total"]  = ranking.total;

        std::vector<crow::json::wvalue> items;
        AristaRanking* act = ranking.cabeza;
        int posicion = 1;
        while (act != nullptr) {
            crow::json::wvalue obj;
            obj["posicion"]        = posicion++;
            obj["origen_codigo"]   = act->origenCodigo;
            obj["origen_nombre"]   = act->origenNombre;
            obj["destino_codigo"]  = act->destinoCodigo;
            obj["destino_nombre"]  = act->destinoNombre;
            obj["peliculas_comun"] = static_cast<int>(act->peso);
            items.push_back(std::move(obj));
            act = act->siguiente;
        }
        res["ranking"] = std::move(items);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // 4. Sedes Similares / Sugeridas (Vecinos ordenados por afinidad)
    CROW_ROUTE(app, "/api/sedes/similares")
    ([](const crow::request& req) {
        char* codParam = req.url_params.get("codigo");
        if (!codParam) {
            crow::json::wvalue err;
            err["status"]  = "error";
            err["message"] = "Parámetro 'codigo' faltante en la URL. Ejemplo: /api/sedes/similares?codigo=S001";
            return crow::response(400, err);
        }

        string codigoSede = string(codParam);
        SistemaCine& sys = SistemaCine::getInstancia();

        Vertice* vCons = sys.sedes.buscarVertice(codigoSede);
        if (!vCons) {
            crow::json::wvalue err;
            err["status"]  = "error";
            err["message"] = "La sede con código '" + codigoSede + "' no existe.";
            return crow::response(404, err);
        }

        ListaSedesSimilares similares;
        sys.sedes.obtenerSedesSimilares(codigoSede, similares);

        crow::json::wvalue res;
        res["status"]      = "success";
        res["sede_codigo"] = vCons->sede.getCodigo();
        res["sede_nombre"] = vCons->sede.getNombre();
        res["total"]       = similares.total;

        std::vector<crow::json::wvalue> items;
        SedeSimilar* act = similares.cabeza;
        while (act != nullptr) {
            crow::json::wvalue obj;
            obj["codigo"]          = act->codigo;
            obj["nombre"]          = act->nombre;
            obj["direccion"]       = act->direccion;
            obj["salas"]           = act->salas;
            obj["telefono"]        = act->telefono;
            obj["peliculas_comun"] = static_cast<int>(act->peso);
            items.push_back(std::move(obj));
            act = act->siguiente;
        }
        res["similares"] = std::move(items);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // 5. Reporte 1 Graphviz: Grafo de Sedes
    CROW_ROUTE(app, "/api/grafo/reporte-sedes")
    ([]() {
        SistemaCine& sys = SistemaCine::getInstancia();
        bool ok = GrafoReporter::generarReporteGrafoSedes(sys.sedes);

        crow::json::wvalue res;
        if (ok) {
            long long t = obtenerTimestampActual();
            res["status"]  = "success";
            res["url"]     = "/reports/grafo_sedes.png?t=" + to_string(t);
            res["dot_url"] = "/reports/grafo_sedes.dot";
            res["mensaje"] = "Grafo de Sedes renderizado exitosamente.";
        } else {
            res["status"]  = "error";
            res["mensaje"] = "Ocurrió un error al compilar el grafo con Graphviz (dot). Verifica que Graphviz esté instalado.";
        }

        auto response = crow::response(ok ? 200 : 500, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // 6. Reporte 2 Graphviz: Lista de Adyacencia
    CROW_ROUTE(app, "/api/grafo/reporte-adyacencia")
    ([]() {
        SistemaCine& sys = SistemaCine::getInstancia();
        bool ok = GrafoReporter::generarReporteListaAdyacencia(sys.sedes);

        crow::json::wvalue res;
        if (ok) {
            long long t = obtenerTimestampActual();
            res["status"]  = "success";
            res["url"]     = "/reports/lista_adyacencia.png?t=" + to_string(t);
            res["dot_url"] = "/reports/lista_adyacencia.dot";
            res["mensaje"] = "Lista de Adyacencia renderizada exitosamente.";
        } else {
            res["status"]  = "error";
            res["mensaje"] = "Ocurrió un error al compilar la lista de adyacencia con Graphviz (dot).";
        }

        auto response = crow::response(ok ? 200 : 500, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // ═══════════════════════════════════════════════════════════════
    // ARRANQUE DEL SERVIDOR
    // ═══════════════════════════════════════════════════════════════
    cout << "===========================================" << endl;
    cout << "      CinemaUSAC Server — Fase 3          " << endl;
    cout << "      Puerto: 18080                        " << endl;
    cout << "===========================================" << endl;

    app.port(18080).multithreaded().run();

    return 0;
}
