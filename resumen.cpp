// resumen.cpp - Resumen del cierre semanal (El buffet de Alberto)
// Lee:  comandas_semana_sX-mm.dat (ordenado por idMozo)
// Muestra por pantalla: por cada mozo, productos vendidos y comision a pagar,
// y al final el total de productos vendidos por el buffet.
// Tecnica: CORTE DE CONTROL (el archivo esta ordenado por idMozo).

#include <iostream>
#include <cstdio>   
using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    char nombreArchivo[50];
    cout << "Nombre de la planilla semanal (ej: comandas_semana_s1-06.dat): ";
    cin >> nombreArchivo;

    FILE* f = fopen(nombreArchivo, "rb");
    if (f == NULL) {
        cout << "No se pudo abrir el archivo " << nombreArchivo << "." << endl;
        return 1;
    }

    Comanda c;
    int leido = fread(&c, sizeof(Comanda), 1, f); 

    if (leido == 0) {
        cout << "La planilla semanal esta vacia: no hay ventas para resumir." << endl;
        fclose(f);
        return 0;
    }

    int totalBuffet = 0; 

    cout << "===== RESUMEN DEL CIERRE =====" << endl;
    cout << "Mozo\tProductos\tComision" << endl;

    while (leido == 1) {
        int mozoActual = c.idMozo;      
        int productos = 0;              
        float comision = 0;

        while (leido == 1 && c.idMozo == mozoActual) {
            productos += c.cantidad;
            comision += c.comision;
            leido = fread(&c, sizeof(Comanda), 1, f);
        }

        cout << mozoActual << "\t" << productos << "\t\t$" << comision << endl;
        totalBuffet += productos;
    }

    cout << "------------------------------" << endl;
    cout << "Total de productos vendidos por el buffet: " << totalBuffet << endl;

    fclose(f);
    return 0;
}