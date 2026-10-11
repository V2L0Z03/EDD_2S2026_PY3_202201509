#ifndef MATRIZ_DISPERSA_HPP
#define MATRIZ_DISPERSA_HPP

#include "..\models\Asiento.hpp"
#include <iostream>
#include <string>

struct NodoMatriz {
    Asiento asiento;
    NodoMatriz* arriba;
    NodoMatriz* abajo;
    NodoMatriz* izquierda;
    NodoMatriz* derecha;

    NodoMatriz(const Asiento& a)
        : asiento(a), arriba(nullptr), abajo(nullptr), izquierda(nullptr), derecha(nullptr) {}
};

struct NodoCabecera {
    int id;
    NodoMatriz* acceso;
    NodoCabecera* siguiente;
    NodoCabecera* anterior;

    NodoCabecera(int id): id(id), acceso(nullptr), siguiente(nullptr), anterior(nullptr) {}
};

// Lista doblemente enlazada de cabeceras (filas o columnas)
class ListaCabeceras {
    private:
        NodoCabecera* cabeza;

public:
    ListaCabeceras() : cabeza(nullptr) {}
    ~ListaCabeceras();

    void insertarCabecera(int id);
    NodoCabecera* buscarCabecera(int id) const;
    NodoCabecera* getCabeza() const { return cabeza; }
    void vaciar();
};

class MatrizDispersa {
    private:
        ListaCabeceras cabecerasFilas;
        ListaCabeceras cabecerasColumnas;

        int maxFilas;
        int maxColumnas;
        int asientosReservadosCount;

        NodoMatriz* buscarNodo(int fila, int columna) const;

    public:
        MatrizDispersa();
        MatrizDispersa(int filas, int columnas);
        ~MatrizDispersa();

        void inicializarDimensiones(int filas, int columnas);
        bool reservarAsiento(int fila, int columna, const std::string& cliente,
                             const std::string& codigoReserva = "");
        bool cancelarReserva(int fila, int columna);
        void mostrarMapaAsientos() const;
        void mostrarEstadisticas() const;
        void vaciar();

        int getMaxFilas()               const { return maxFilas; }
        int getMaxColumnas()            const { return maxColumnas; }
        int getAsientosReservadosCount()const { return asientosReservadosCount; }
        ListaCabeceras& getCabecerasFilas()    { return cabecerasFilas; }
        ListaCabeceras& getCabecerasColumnas() { return cabecerasColumnas; }

        // Métodos auxiliares para la GUI
        bool        estaOcupado(int fila, int columna)      const;
        std::string getClienteAsiento(int fila, int columna) const;
        std::string getCodigoReservaAsiento(int fila, int columna) const;

        //esto es mi Persistencia JSON (Fase 2)
        // aca Serializo los asientos ocupados al archivo indicado.
        bool serializarJSON(const std::string& rutaArchivo) const;
        // Limpia la matriz y reconstruye desde el JSON de la función.
        bool deserializarJSON(const std::string& rutaArchivo);
};

#endif // MATRIZ_DISPERSA_HPP