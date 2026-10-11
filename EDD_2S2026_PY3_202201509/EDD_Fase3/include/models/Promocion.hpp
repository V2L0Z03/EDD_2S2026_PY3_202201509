#ifndef PROMOCION_HPP
#define PROMOCION_HPP

#include <string>
#include "../structures/ListaDobleBeneficios.hpp"

class Promocion {
private:
    std::string codigo;         // Ej. "Promo_01"
    std::string nombre;         // Ej. "Martes de Locura"
    std::string vigencia;       // Ej. "2026-01-01 a 2026-12-31"
    std::string diasAplicables; // Ej. "Martes"
    ListaDobleBeneficios beneficios; // Sublista enlazada

public:
    Promocion() : codigo(""), nombre(""), vigencia(""), diasAplicables("") {}
    Promocion(const std::string& cod, const std::string& nom, 
              const std::string& vig, const std::string& dias)
        : codigo(cod), nombre(nom), vigencia(vig), diasAplicables(dias) {}

    // Getters
    std::string getCodigo() const { return codigo; }
    std::string getNombre() const { return nombre; }
    std::string getVigencia() const { return vigencia; }
    std::string getDiasAplicables() const { return diasAplicables; }
    ListaDobleBeneficios& getBeneficios() { return beneficios; }
    const ListaDobleBeneficios& getBeneficios() const { return beneficios; }

    // Métodos delegados para manipular los beneficios internos
    void agregarBeneficio(const Beneficio& b) {
        beneficios.agregarAlFinal(b);
    }
};

#endif // PROMOCION_HPP