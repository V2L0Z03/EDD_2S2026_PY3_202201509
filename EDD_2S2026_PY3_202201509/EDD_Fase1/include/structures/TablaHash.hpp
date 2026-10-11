#ifndef TABLA_HASH_HPP
#define TABLA_HASH_HPP

#include "../models/Reserva.hpp"
#include "ListaReservas.hpp"
#include <vector>
#include <string>
#include <iostream>

class TablaHash {
private:
    std::vector<ListaReservas> tabla;
    int tamanio;

    int hashFuncion(const std::string& clave) const {
        unsigned int hash = 0;
        for (char c : clave) {
            hash = (hash * 31) + c;
        }
        return hash % tamanio;
    }

public:
    TablaHash(int tamInicial = 5) : tamanio(tamInicial) {
        tabla.resize(tamanio);
    }

    void insertar(const Reserva& r) {
        int index = hashFuncion(r.getCodigoReserva());

        // aca Verifico si la reserva ya existe para no duplicar
        if (tabla[index].buscar(r.getCodigoReserva()) == nullptr) {
            tabla[index].insertar(r);
        }
    }

    bool eliminar(const std::string& codigo) {
        int index = hashFuncion(codigo);
        return tabla[index].eliminar(codigo);
    }

    Reserva* buscar(const std::string& codigo) {
        int index = hashFuncion(codigo);
        NodoReserva* nr = tabla[index].buscar(codigo);
        if (nr) return &nr->reserva;
        return nullptr;
    }

    int getTamanio() const { return tamanio; }


/*
    int getReservasTotales() const {
        int totales = 0;
        for (int i = 0; i < tamanio; i++) {
            totales += tabla[i].getTamanio();
        }
        return totales;
    }*/



    int getReservasTotales()const{
        int totales = 0;
        for(int i=0; i < tamanio; i++){
            totales += tabla[i].getTamanio;
        }
        return totales;
    }

    int getBucketsOcupados() const {
        int ocupados = 0;
        for (int i = 0; i < tamanio; i++) {
            if (!tabla[i].estaVacia()) ocupados++;
        }
        return ocupados;
    }

    int getColisionesActuales() const {
        int colisiones_actuales = 0;
        for (int i = 0; i < tamanio; i++) {
            if (tabla[i].getTamanio() > 1) {
                // aca es s un bucket tiene N elementos (N > 1), ocurrieron N - 1 colisiones allí
                colisiones_actuales += (tabla[i].getTamanio() - 1);
            }
        }
        return colisiones_actuales;
    }

    const std::vector<ListaReservas>& getTabla() const { return tabla; }
};

#endif // TABLA_HASH_HPP
