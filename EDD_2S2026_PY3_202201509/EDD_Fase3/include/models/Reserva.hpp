#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <string>

class Reserva {
private:
    std::string codigoReserva;
    std::string codigoFuncion;
    std::string idCliente;
    int fila;
    int columna;
    std::string fechaReserva;

public:
    Reserva() : codigoReserva(""), codigoFuncion(""), idCliente(""), fila(0), columna(0), fechaReserva("") {}

    Reserva(const std::string& codRes, const std::string& codFunc, const std::string& idCli, int f, int c, const std::string& fecha)
        : codigoReserva(codRes), codigoFuncion(codFunc), idCliente(idCli), fila(f), columna(c), fechaReserva(fecha) {}

    std::string getCodigoReserva() const { return codigoReserva; }
    std::string getCodigoFuncion() const { return codigoFuncion; }
    std::string getIdCliente() const { return idCliente; }
    int getFila() const { return fila; }
    int getColumna() const { return columna; }
    std::string getFechaReserva() const { return fechaReserva; }

    void setCodigoReserva(const std::string& val) { codigoReserva = val; }
    void setCodigoFuncion(const std::string& val) { codigoFuncion = val; }
    void setIdCliente(const std::string& val) { idCliente = val; }
    void setFila(int val) { fila = val; }
    void setColumna(int val) { columna = val; }
    void setFechaReserva(const std::string& val) { fechaReserva = val; }
};

#endif // RESERVA_HPP
