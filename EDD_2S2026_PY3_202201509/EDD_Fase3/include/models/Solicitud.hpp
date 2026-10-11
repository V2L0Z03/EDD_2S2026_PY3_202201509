

#ifndef SOLICITUD_HPP

#include <string>
#define SOLICITUD_HPP

class Solicitud {
    private:
        int id;
        std::string cliente;
        std::string telefono;
        std::string tipo; // Cumpleaños requerimiento especial etc coo¿mo en el PDF
        std::string descripcion;  //pa la descripcion
        std::string estado; //Esto es lo pendiente rechazo etc


    public:
        Solicitud() : id(0), cliente(""), telefono(""), tipo(""), descripcion(""), estado("Pendiente"){}
        Solicitud( int id, const std::string& cli, const std::string& tel, const std::string& tipo, const std::string& desc)
            : id(id), cliente(cli), telefono(tel), tipo(tipo), descripcion(desc), estado("Pendiente"){}

    //Gettes
    int getId() const { return id; }
    std:: string getCliente() const { return cliente; }
    std::string getTelefono() const { return telefono; }
    std::string getTipo() const { return tipo; }
    std::string getDescripcion() const { return descripcion; }
    std::string getEstado() const { return estado; }

    //Setters
    void setEstado(const std::string& nuevoEstado) { estado = nuevoEstado;}
            };
                
#endif // SOLICITUD_HPP
