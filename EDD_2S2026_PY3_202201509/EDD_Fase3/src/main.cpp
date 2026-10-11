#define CROW_MAIN
#define CROW_STATIC_DIR "public/"
#include "../include/vendor/crow_all.h"
#include "../include/models/SistemaCine.hpp"
#include "../include/services/JSONReader.hpp"
#include "../include/vendor/json.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using json = nlohmann::json;

// Helper para leer archivos
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

int main() {
    crow::SimpleApp app;

    // ----- RUTAS ESTÁTICAS (FRONTEND) -----
    CROW_ROUTE(app, "/")
    ([]() {
        string page = leerArchivo("public/index.html");
        auto response = crow::response(page);
        response.set_header("Content-Type", "text/html");
        return response;
    });

    CROW_ROUTE(app, "/style.css")
    ([]() {
        string page = leerArchivo("public/style.css");
        auto response = crow::response(page);
        response.set_header("Content-Type", "text/css");
        return response;
    });

    CROW_ROUTE(app, "/script.js")
    ([]() {
        string page = leerArchivo("public/script.js");
        auto response = crow::response(page);
        response.set_header("Content-Type", "application/javascript");
        return response;
    });

    // ----- ENDPOINTS (BACKEND REST API) -----
    CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        // Usar parser nativo de Crow
        auto payload = crow::json::load(req.body);

        if (!payload) {
            return crow::response(400, "{\"error\": \"JSON Inválido\"}");
        }

        string email = payload["email"].s();
        string password = payload["password"].s();

        // 1. Validar Administrador
        if (email == "AdminCine@gmail.com" && password == "admin123Pass") {
            crow::json::wvalue res;
            res["status"] = "success";
            res["role"] = "admin";
            res["message"] = "Bienvenido Administrador";
            auto response = crow::response(200, res);
            response.set_header("Content-Type", "application/json");
            return response;
        }

        // 2. Validar Cliente (Buscando en el Árbol B Heredado)
        SistemaCine& sys = SistemaCine::getInstancia();
        NodoCliente* nodoCli = sys.clientes.buscarPorCredenciales(email, password);

        if (nodoCli != nullptr) {
            // Seteamos estado en Singleton
            sys.clienteLogueado = &nodoCli->cliente;

            crow::json::wvalue res;
            res["status"] = "success";
            res["role"] = "cliente";
            res["message"] = "Bienvenido " + nodoCli->cliente.getNombre();
            auto response = crow::response(200, res);
            response.set_header("Content-Type", "application/json");
            return response;
        }

        // 3. Respuesta de Error
        crow::json::wvalue el_err;
        el_err["status"] = "error";
        el_err["message"] = "Credenciales incorrectas o usuario no encontrado.";
        auto responseError = crow::response(401, el_err);
        responseError.set_header("Content-Type", "application/json");
        return responseError;
    });

    // ----- POST /api/carga-masiva --------------------------------
    CROW_ROUTE(app, "/api/carga-masiva").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        // El frontend envía el contenido JSON como texto plano en el body
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

        // Construir respuesta JSON con Crow
        crow::json::wvalue res;
        res["status"]              = (r.errores.total == 0) ? "success" : "partial";
        res["sedes_cargadas"]      = r.sedesCargadas;
        res["peliculas_cargadas"]  = r.peliculasCargadas;
        res["funciones_cargadas"]  = r.funcionesCargadas;
        res["funciones_rechazadas"]= r.funcionesRechazadas;

        // Serializar bitácora de errores
        std::vector<crow::json::wvalue> arrErr;
        NodoLog* e = r.errores.cabeza;
        while (e) { arrErr.push_back(crow::json::wvalue(e->mensaje)); e = e->siguiente; }
        res["errores"] = std::move(arrErr);

        // Serializar advertencias
        std::vector<crow::json::wvalue> arrAdv;
        NodoLog* a = r.advertencias.cabeza;
        while (a) { arrAdv.push_back(crow::json::wvalue(a->mensaje)); a = a->siguiente; }
        res["advertencias"] = std::move(arrAdv);

        auto response = crow::response(200, res);
        response.set_header("Content-Type", "application/json");
        return response;
    });

    // Iniciar servidor
    cout << "===========================================" << endl;
    cout << "      CinemaUSAC Server Iniciando...      " << endl;
    cout << "      Puerto: 18080                        " << endl;
    cout << "===========================================" << endl;
    
    app.port(18080).multithreaded().run();

    return 0;
}
