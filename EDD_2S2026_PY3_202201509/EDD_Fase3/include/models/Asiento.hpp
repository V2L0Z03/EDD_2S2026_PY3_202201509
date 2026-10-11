#ifndef ASIENTO_HPP
#define ASIENTO_HPP

#include <string>

class Asiento {
    private:
        int fila;
        int columna;
        bool ocupado;
        std::string cliente;       // nombre del cliente que reservó
        std::string codigoReserva; // código único de reserva F2)

    public:
        Asiento(): fila(0), columna(0), ocupado(false), cliente(""), codigoReserva("") {}
        Asiento(int f, int c): fila(f), columna(c), ocupado(false), cliente(""), codigoReserva("") {}
        Asiento(int f, int c, bool o, std::string cli)
            : fila(f), columna(c), ocupado(o), cliente(cli), codigoReserva("") {}
        Asiento(int f, int c, bool o, std::string cli, std::string codRes)
            : fila(f), columna(c), ocupado(o), cliente(cli), codigoReserva(codRes) {}

        // GETTERS
        int getFila()               const { return fila; }
        int getColumna()            const { return columna; }
        bool isOcupado()            const { return ocupado; }
        std::string getCliente()    const { return cliente; }
        std::string getCodigoReserva() const { return codigoReserva; }

        // SETTERS
        void setOcupado(bool oc)                      { ocupado = oc; }
        void setCliente(const std::string& cli)        { cliente = cli; }
        void setCodigoReserva(const std::string& cod)  { codigoReserva = cod; }
};

#endif // ASIENTO_HPP