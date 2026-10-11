#ifndef BENEFICIO_HPP
#define BENEFICIO_HPP

#include <string>

class Beneficio {
private:
    std::string tipo;        // "descuento", "combo", "2x1", etc.
    std::string descripcion; // sonlas descripciones de los beneficios
    std::string valor;       // "5%", "10%", "NA"

public:
    Beneficio() : tipo(""), descripcion(""), valor("NA") {}
    Beneficio(const std::string& tipo, const std::string& descripcion, const std::string& valor)
        : tipo(tipo), descripcion(descripcion), valor(valor) {}

    // Getters
    std::string getTipo() const { return tipo; }
    std::string getDescripcion() const { return descripcion; }
    std::string getValor() const { return valor; }

    // Setters
    void setTipo(const std::string& t) { tipo = t; }
    void setDescripcion(const std::string& d) { descripcion = d; }
    void setValor(const std::string& v) { valor = v; }
};

#endif // BENEFICIO_HPP