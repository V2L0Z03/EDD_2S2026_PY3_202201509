#include "../../include/ui/MenuController.hpp"
#include "../../include/services/CSVReader.hpp"
#include "../../include/services/GraphvizReporter.hpp"
#include <iostream>
#include <limits>

MenuController::MenuController() : salaCine(5, 5), contadorSolicitudes(1) {}

void MenuController::pausarPantalla() {
    std::cout << "\nPresione ENTER para continuar...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void MenuController::iniciar() {
    mostrarMenuPrincipal();
}

void MenuController::mostrarMenuPrincipal() {
    int opcion = 0;
    do {
        std::cout << "\n========================================================\n";
        std::cout << "        SISTEMA DE GESTION EDD CINEMA USAC              \n";
        std::cout << "========================================================\n";
        std::cout << "  1. Panel de Administrador\n";
        std::cout << "  2. Panel de Cliente\n";
        std::cout << "  3. Salir\n";
        std::cout << "========================================================\n";
        std::cout << "Seleccione una opcion: ";
        
        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (opcion) {
            case 1:
                menuAdministrador();
                break;
            case 2:
                menuCliente();
                break;
            case 3:
                std::cout << "\nGracias por utilizar EDD CinemaUSAC. ¡Hasta pronto!\n";
                break;
            default:
                std::cout << "\nOpcion no valida. Intente de nuevo.\n";
        }
    } while (opcion != 3);
}

// ========================================================
// PANEL DE ADMINISTRADOR
// ========================================================
void MenuController::menuAdministrador() {
    int opcion = 0;
    do {
        std::cout << "\n--------------------------------------------------------\n";
        std::cout << "              PANEL DE ADMINISTRACION                   \n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  1. Carga Masiva de Peliculas (CSV)\n";
        std::cout << "  2. Gestion de Peliculas (BST)\n";
        std::cout << "  3. Gestion de Promociones y Beneficios\n";
        std::cout << "  4. Gestion de Solicitudes Especiales\n";
        std::cout << "  5. Gestion de Sala (Matriz Dispersa)\n";
        std::cout << "  6. Generar Reportes Visuales (Graphviz)\n";
        std::cout << "  7. Volver al Menú Principal\n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: subMenuCargaMasiva(); break;
            case 2: subMenuGestionPeliculas(); break;
            case 3: subMenuGestionPromociones(); break;
            case 4: subMenuGestionSolicitudes(); break;
            case 5: subMenuGestionSala(); break;
            case 6: subMenuReportesGraphviz(); break;
            case 7: break;
            default: std::cout << "\nOpcion invalida.\n";
        }
    } while (opcion != 7);
}

void MenuController::subMenuCargaMasiva() {
    std::string ruta;
    std::cout << "\n--- CARGA MASIVA CSV ---\n";
    std::cout << "Ingrese la ruta del archivo CSV (Ej: test_data/peliculas_ejemplo.csv): ";
    std::cin >> ruta;

    CSVReader::cargarPeliculasDesdeCSV(ruta, cartelera);
    pausarPantalla();
}

void MenuController::subMenuGestionPeliculas() {
    int subOp = 0;
    std::cout << "\n--- GESTION DE PELICULAS (BST) ---\n";
    std::cout << "1. Agregar Nueva Pelicula\n";
    std::cout << "2. Buscar Pelicula por Codigo\n";
    std::cout << "3. Eliminar Pelicula\n";
    std::cout << "4. Ver Cartelera Ordenada\n";
    std::cout << "Seleccione: ";
    std::cin >> subOp;

    if (subOp == 1) {
        std::string cod, tit, gen, clasif, idm, fEst, fFin;
        int dur;
        std::cout << "Codigo: "; std::cin >> cod;
        std::cout << "Titulo: "; std::cin.ignore(); std::getline(std::cin, tit);
        std::cout << "Genero: "; std::getline(std::cin, gen);
        std::cout << "Duracion (min): "; std::cin >> dur;
        std::cout << "Clasificacion: "; std::cin >> clasif;
        std::cout << "Idioma: "; std::cin.ignore(); std::getline(std::cin, idm);
        std::cout << "Fecha Estreno (YYYY-MM-DD): "; std::cin >> fEst;
        std::cout << "Fecha Retiro (YYYY-MM-DD): "; std::cin >> fFin;

        cartelera.insertar(Pelicula(cod, tit, gen, dur, clasif, idm, fEst, fFin));
        std::cout << " Pelicula agregada al BST exitosamente.\n";
    } else if (subOp == 2) {
        std::string cod;
        std::cout << "Ingrese Codigo a buscar: "; std::cin >> cod;
        NodoBST* res = cartelera.buscar(cod);
        if (res) {
            std::cout << " Encontrada: " << res->pelicula.getTitulo() 
                      << " | Genero: " << res->pelicula.getGenero() << "\n";
        } else {
            std::cout << " Pelicula no registrada.\n";
        }
    } else if (subOp == 3) {
        std::string cod;
        std::cout << "Ingrese Codigo a eliminar: "; std::cin >> cod;
        if (cartelera.eliminar(cod)) {
            std::cout << " Pelicula eliminada del BST.\n";
        } else {
            std::cout << " No se encontro la pelicula.\n";
        }
    } else if (subOp == 4) {
        cartelera.mostrarCartelera();
    }
    pausarPantalla();
}

void MenuController::subMenuGestionPromociones() {
    int subOp = 0;
    std::cout << "\n--- GESTION DE PROMOCIONES Y BENEFICIOS ---\n";
    std::cout << "1. Crear Promocion\n";
    std::cout << "2. Agregar Beneficio a Promocion Existente\n";
    std::cout << "3. Ver Promociones y Beneficios\n";
    std::cout << "Seleccione: ";
    std::cin >> subOp;

    if (subOp == 1) {
        std::string cod, nom, fVal, dia;
        std::cout << "Codigo Promo: "; std::cin >> cod;
        std::cout << "Nombre: "; std::cin.ignore(); std::getline(std::cin, nom);
        std::cout << "Fecha Validez: "; std::cin >> fVal;
        std::cout << "Dia Aplica: "; std::cin >> dia;

        promociones.agregarPromocion(Promocion(cod, nom, fVal, dia));
        std::cout << " Promocion agregada a la lista circular.\n";
    } else if (subOp == 2) {
        std::string cod, tipo, desc, descVal;
        std::cout << "Codigo de Promocion a modificar: "; std::cin >> cod;
        NodoPromocion* promo = promociones.buscarPromocion(cod);
        if (promo) {
            std::cout << "Tipo de Beneficio (ej. Descuento / Combo): "; std::cin.ignore(); std::getline(std::cin, tipo);
            std::cout << "Descripcion: "; std::getline(std::cin, desc);
            std::cout << "Descuento / Valor: "; std::getline(std::cin, descVal);

            promo->promocion.agregarBeneficio(Beneficio(tipo, desc, descVal));
            std::cout << " Beneficio agregado a la sublista de la promocion " << cod << ".\n";
        } else {
            std::cout << " Promocion no encontrada.\n";
        }
    } else if (subOp == 3) {
        promociones.mostrarPromociones();
    }
    pausarPantalla();
}

void MenuController::subMenuGestionSolicitudes() {
    std::cout << "\n--- GESTION DE SOLICITUDES ESPECIALES ---\n";
    solicitudes.mostrarSolicitudes();

    if (!solicitudes.estaVacia()) {
        char resp;
        std::cout << "\n¿Desea atender la solicitud actual en cabeza? (s/n): ";
        std::cin >> resp;
        if (resp == 's' || resp == 'S') {
            int idAtender = solicitudes.getCabeza()->solicitud.getId();
            //solicitudes.atenderSolicitud(idAtender);
            std::cout << " Solicitud ID " << idAtender << " atendida y removida del sistema.\n";
        }
    }
    pausarPantalla();
}

void MenuController::subMenuGestionSala() {
    int subOp = 0;
    std::cout << "\n--- GESTION DE SALA (MATRIZ DISPERSA) ---\n";
    std::cout << "1. Ver Mapa de Asientos\n";
    std::cout << "2. Reconfigurar Dimensiones de la Sala\n";
    std::cout << "Seleccione: ";
    std::cin >> subOp;

    if (subOp == 1) {
        salaCine.mostrarMapaAsientos();
        salaCine.mostrarEstadisticas();
    } else if (subOp == 2) {
        int f, c;
        std::cout << "Nuevas Filas: "; std::cin >> f;
        std::cout << "Nuevas Columnas: "; std::cin >> c;
        salaCine.inicializarDimensiones(f, c);
        std::cout << " Sala reconfigurada a " << f << "x" << c << ".\n";
    }
    pausarPantalla();
}

void MenuController::subMenuReportesGraphviz() {
    std::cout << "\n--- GENERACION DE REPORTES VISUALES (GRAPHVIZ) ---\n";
    GraphvizReporter::generarReporteBST(cartelera, "reporte_bst");
    GraphvizReporter::generarReportePromociones(promociones, "reporte_promociones");
    GraphvizReporter::generarReporteSolicitudes(solicitudes, "reporte_solicitudes");
    GraphvizReporter::generarReporteMatriz(salaCine, "reporte_matriz");
    std::cout << "\n Todos los reportes han sido actualizados en la carpeta 'reports_output/'.\n";
    pausarPantalla();
}

// ========================================================
// PANEL DE CLIENTE
// ========================================================
void MenuController::menuCliente() {
    int opcion = 0;
    do {
        std::cout << "\n--------------------------------------------------------\n";
        std::cout << "                 PANEL DE CLIENTE                       \n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << "  1. Ver Cartelera de Peliculas\n";
        std::cout << "  2. Ver Promociones y Beneficios Activos\n";
        std::cout << "  3. Reservar Asiento\n";
        std::cout << "  4. Cancelar Reserva de Asiento\n";
        std::cout << "  5. Registrar Solicitud Especial\n";
        std::cout << "  6. Volver al Menú Principal\n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: clienteVerCartelera(); break;
            case 2: clienteVerPromociones(); break;
            case 3: clienteReservarAsiento(); break;
            case 4: clienteCancelarReserva(); break;
            case 5: clienteCrearSolicitud(); break;
            case 6: break;
            default: std::cout << "\nOpcion invalida.\n";
        }
    } while (opcion != 6);
}

void MenuController::clienteVerCartelera() {
    std::cout << "\n--- CARTELERA DE PELICULAS DISPONIBLES ---\n";
    cartelera.mostrarCartelera();
    pausarPantalla();
}

void MenuController::clienteVerPromociones() {
    std::cout << "\n--- PROMOCIONES Y BENEFICIOS DISPONIBLES ---\n";
    promociones.mostrarPromociones();
    pausarPantalla();
}

void MenuController::clienteReservarAsiento() {
    std::cout << "\n--- RESERVA DE ASIENTOS ---\n";
    salaCine.mostrarMapaAsientos();

    int f, c;
    std::string cliente;
    std::cout << "Ingrese Número de Fila: "; std::cin >> f;
    std::cout << "Ingrese Número de Columna: "; std::cin >> c;
    std::cout << "Ingrese Nombre del Cliente: "; std::cin.ignore(); std::getline(std::cin, cliente);

    if (salaCine.reservarAsiento(f, c, cliente)) {
        std::cout << " ¡Asiento reservado con éxito para " << cliente << "!\n";
    }
    pausarPantalla();
}

void MenuController::clienteCancelarReserva() {
    std::cout << "\n--- CANCELACION DE RESERVA ---\n";
    int f, c;
    std::cout << "Ingrese Fila del asiento a cancelar: "; std::cin >> f;
    std::cout << "Ingrese Columna del asiento a cancelar: "; std::cin >> c;

    if (salaCine.cancelarReserva(f, c)) {
        std::cout << " Reserva cancelada exitosamente.\n";
    }
    pausarPantalla();
}

void MenuController::clienteCrearSolicitud() {
    std::cout << "\n--- REGISTRAR SOLICITUD ESPECIAL ---\n";
    std::string cliente, tel, tipo, desc;
    std::cout << "Nombre Cliente: "; std::cin.ignore(); std::getline(std::cin, cliente);
    std::cout << "Telefono: "; std::getline(std::cin, tel);
    std::cout << "Tipo de Solicitud (ej. Accesibilidad, Cumpleaños, Queja): "; std::getline(std::cin, tipo);
    std::cout << "Detalle: "; std::getline(std::cin, desc);

    Solicitud sol(contadorSolicitudes++, cliente, tel, tipo, desc);
    solicitudes.agregarSolicitud(sol);
    std::cout << " Solicitud registrada con éxito. Su ID asignado es: " << sol.getId() << "\n";
    pausarPantalla();
}