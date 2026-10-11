#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include "../structures/ListaReservas.hpp"

class Cliente {
private:
    std::string id;
    std::string nombre;
    std::string correo;
    std::string password;
    std::string telefono;
    ListaReservas reservas;

public:
    Cliente() : id(""), nombre(""), correo(""), password(""), telefono("") {}

    Cliente(const std::string& _id, const std::string& _nom, const std::string& _correo,
            const std::string& _pass, const std::string& _tel)
        : id(_id), nombre(_nom), correo(_correo), password(_pass), telefono(_tel) {}

    std::string getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getCorreo() const { return correo; }
    std::string getPassword() const { return password; }
    std::string getTelefono() const { return telefono; }

    void setId(const std::string& val) { id = val; }
    void setNombre(const std::string& val) { nombre = val; }
    void setCorreo(const std::string& val) { correo = val; }
    void setPassword(const std::string& val) { password = val; }
    void setTelefono(const std::string& val) { telefono = val; }

    ListaReservas& getReservas() { return reservas; }
    const ListaReservas& getReservas() const { return reservas; }
};

#endif // CLIENTE_HPP
