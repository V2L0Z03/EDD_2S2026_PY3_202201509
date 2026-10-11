#ifndef PELICULA_HPP
#define PELICULA_HPP

#include <string>
//My classss
class Pelicula {
    private:
        std::string codigo;
        std::string titulo;
        std::string genero;
        int duracionMinutos; // Duración en minutos
        std::string clasificacion;
        std::string idioma;
        std::string fechaEstreno;
        std::string fechaFin;

    public:
        Pelicula() 
        : codigo(""), titulo(""), genero(""), duracionMinutos(0), 
          clasificacion(""), idioma(""), fechaEstreno(""), fechaFin("") {}

        Pelicula(const std::string& cod, const std::string& tit, const std::string& gen,
                int dur, const std::string& clasif, const std::string& idm,
                const std::string& fEstreno, const std::string& fFin)
                    : codigo(cod), titulo(tit), genero(gen), duracionMinutos(dur),
                    clasificacion(clasif), idioma(idm), fechaEstreno(fEstreno), fechaFin(fFin) {}



    //Mis Geters
    std::string getCodigo() const { return codigo; }
    std::string getTitulo() const { return titulo; }
    std::string getGenero() const { return genero; }
    int getDuracionMinutos() const { return duracionMinutos; }
    std::string getClasificacion() const { return clasificacion; }
    std::string getIdioma() const { return idioma; }
    std::string getFechaEstreno() const { return fechaEstreno; }
    std::string getFechaFin() const { return fechaFin; }


    //seteers
    void setTitulo(const std::string& t) { titulo = t; }
    void setGenero(const std::string& g) { genero = g; }
    void setDuracionMinutos(int d) { duracionMinutos = d; }
    void setClasificacion(const std::string& c) { clasificacion = c; }
    void setIdioma(const std::string& i) { idioma = i; }
    void setFechaEstreno(const std::string& fe) { fechaEstreno = fe; }
    void setFechaFin(const std::string& ff) { fechaFin = ff; }
};

#endif // PELICULA_HPP      