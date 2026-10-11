#ifndef SEDE_HPP
#define SEDE_HPP

#include <string>

// ─────────────────────────────────────────────────────────────
// Modelo: Sede
//   Almacena los datos de una sede cinematográfica que actuará
//   como vértice dentro del GrafoSedes (no dirigido).
// ─────────────────────────────────────────────────────────────
class Sede {
private:
    std::string codigo;
    std::string nombre;
    std::string direccion;
    int         cantidadSalas;
    std::string telefono;

public:
    // Constructor por defecto (necesario para instancias en estructuras)
    Sede()
        : codigo(""), nombre(""), direccion(""),
          cantidadSalas(0), telefono("") {}

    // Constructor principal
    Sede(const std::string& codigo,
         const std::string& nombre,
         const std::string& direccion,
         int                cantidadSalas,
         const std::string& telefono)
        : codigo(codigo), nombre(nombre), direccion(direccion),
          cantidadSalas(cantidadSalas), telefono(telefono) {}

    // ── Getters ──────────────────────────────────────────────
    const std::string& getCodigo()        const { return codigo; }
    const std::string& getNombre()        const { return nombre; }
    const std::string& getDireccion()     const { return direccion; }
    int                getCantidadSalas() const { return cantidadSalas; }
    const std::string& getTelefono()      const { return telefono; }

    // ── Setters (necesarios para actualizar datos en el grafo) ─
    void setCodigo(const std::string& v)        { codigo        = v; }
    void setNombre(const std::string& v)        { nombre        = v; }
    void setDireccion(const std::string& v)     { direccion     = v; }
    void setCantidadSalas(int v)                { cantidadSalas = v; }
    void setTelefono(const std::string& v)      { telefono      = v; }
};

#endif // SEDE_HPP
