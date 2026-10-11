#ifndef FUNCION_HPP
#define FUNCION_HPP

#include <string>

class Funcion {
private:
    std::string codigo;         // Ej. "F001"
    std::string codigoPelicula; // Ej. "P001"
    std::string fecha;          // Ej. "2026-09-10"
    std::string hora;           // Ej. "18:00"
    std::string sala;           // Ej. "Sala 1"
    double precio;              // Ej. 45.0
    std::string codigoSede;     // Ej. "S001" (Fase 3)

public:
    Funcion() 
        : codigo(""), codigoPelicula(""), fecha(""), hora(""), sala(""), precio(0.0), codigoSede("") {}

    Funcion(const std::string& cod, const std::string& codPeli,
            const std::string& fec, const std::string& hor,
            const std::string& sal, double prec,
            const std::string& codSede = "")
        : codigo(cod), codigoPelicula(codPeli), fecha(fec), hora(hor), sala(sal), precio(prec), codigoSede(codSede) {}

    // Getters
    std::string getCodigo() const { return codigo; }
    std::string getCodigoPelicula() const { return codigoPelicula; }
    std::string getFecha() const { return fecha; }
    std::string getHora() const { return hora; }
    std::string getSala() const { return sala; }
    double getPrecio() const { return precio; }
    std::string getCodigoSede() const { return codigoSede; }

    // Setters
    void setCodigo(const std::string& c) { codigo = c; }
    void setCodigoPelicula(const std::string& cp) { codigoPelicula = cp; }
    void setFecha(const std::string& f) { fecha = f; }
    void setHora(const std::string& h) { hora = h; }
    void setSala(const std::string& s) { sala = s; }
    void setPrecio(double p) { precio = p; }
    void setCodigoSede(const std::string& cs) { codigoSede = cs; }
};

#endif // FUNCION_HPP

