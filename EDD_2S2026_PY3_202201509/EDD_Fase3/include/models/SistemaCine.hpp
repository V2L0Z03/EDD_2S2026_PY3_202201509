#ifndef SISTEMA_CINE_HPP
#define SISTEMA_CINE_HPP

#include "../structures/ArbolBST.hpp"
#include "../structures/ArbolAVL.hpp"
#include "../structures/MatrizDispersa.hpp"
#include "../structures/ListaCircularPromos.hpp"
#include "../structures/ListaCircularDobleSolicitudes.hpp"
#include "../structures/EstructuraClientes.hpp"
#include "../structures/TablaHash.hpp"
#include "../structures/GrafoSedes.hpp"
#include <string>

class SistemaCine {
private:
    SistemaCine() : clienteLogueado(nullptr), contadorSolicitudes(1) {
        currPelicula = "No configurada";
        currHorario = "-";
        currSala = "-";
    }
    ~SistemaCine() = default;

public:
    static SistemaCine& getInstancia() {
        static SistemaCine instancia;
        return instancia;
    }

    ArbolBST cartelera;
    ArbolAVL funciones;
    ListaCircularPromos promociones;
    ListaCircularDobleSolicitudes solicitudes;
    EstructuraClientes clientes;
    TablaHash reservas;
    GrafoSedes sedes;       // Catálogo de sedes cinematográficas (Fase 3)
    Cliente* clienteLogueado;
    int contadorSolicitudes;

    std::string currPelicula;
    std::string currHorario;
    std::string currSala;

    // Delete copy/move constructors to ensure singleton pattern
    SistemaCine(const SistemaCine&) = delete;
    SistemaCine& operator=(const SistemaCine&) = delete;
};

#endif // SISTEMA_CINE_HPP
