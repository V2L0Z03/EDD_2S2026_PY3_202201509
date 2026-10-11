#ifndef MENU_CONTROLLER_HPP
#define MENU_CONTROLLER_HPP

#include "../structures/ArbolBST.hpp"
#include "../structures/ListaCircularPromos.hpp"
#include "../structures/ListaCircularDobleSolicitudes.hpp"
#include "../structures/MatrizDispersa.hpp"

class MenuController {
private:
    ArbolBST cartelera;
    ListaCircularPromos promociones;
    ListaCircularDobleSolicitudes solicitudes;
    MatrizDispersa salaCine;
    int contadorSolicitudes;

    // Estos son mis metodos auxiliares de menú
    void mostrarMenuPrincipal();
    void menuAdministrador();
    void menuCliente();

    // Submenús del Admin xd
    void subMenuCargaMasiva();
    void subMenuGestionPeliculas();
    void subMenuGestionPromociones();
    void subMenuGestionSolicitudes();
    void subMenuGestionSala();
    void subMenuReportesGraphviz();

    // Submenús del Cliente
    void clienteVerCartelera();
    void clienteVerPromociones();
    void clienteReservarAsiento();
    void clienteCancelarReserva();
    void clienteCrearSolicitud();

    // Helpers
    void pausarPantalla();

public:
    MenuController();
    void iniciar();
};

#endif // MENU_CONTROLLER_HPP