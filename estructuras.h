#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/* Formatos binarios compartidos por los cuatro programas del TP. */

const float TASA_COMISION = 0.10f;
const int DESPLAZAMIENTO_CLAVE = 5;

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

#endif
