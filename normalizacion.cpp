#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
const int K = 5;
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
    float totalcomision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};
struct ComandaProcesada {
    char fecha[11]; 
    Comanda comanda;
};
void encriptar(char* pass, int k) {
    // Recorre el arreglo de caracteres hasta encontrar el final '\0'
    for (int i = 0; pass[i] != '\0'; i++) {
        pass[i] = pass[i] + k; // Suma K al valor ASCII del carácter
    }
}

int buscarMozoPorNombre(Mozo mozos[], int cantMozos, const char* nombre) {
    for (int i = 0; i < cantMozos; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0){
            return i;
        }
    }
    return -1;
}


int main() {
FILE* fHistoricas = fopen("comandas_historicas.dat", "rb");
    if (!fHistoricas) { // Si falla (porque no pusiste el archivo ahí), tira error y sale
        cout << "Error al abrir comandas_historicas.dat\n";
        return 1;
    }
    FILE* fInventario = fopen("inventario.dat", "r+b");
    if (!fInventario) {
        cout << "Error al abrir inventario.dat\n";
        fclose(fHistoricas); // Hay que cerrar el historial abierto antes de salir
        return 1;
    }
    Mozo mozos[100];
    int cantMozos = 0;
    ComandaProcesada comandas[10000];
    int cantComandas = 0;
ComandaHistorica com;
while(fread(&com, sizeof(ComandaHistorica), 1, fHistoricas)==1){
 int buscarMozo = buscarMozoPorNombre(mozos, cantMozos, com.nombreMozo);
 if(buscarMozo==-1){
    mozos[cantMozos].idMozo=cantMozos++;
    mozos
 };
};

};






































void crearArchivo(const char* nombre) {
 FILE* f = fopen(nombre, "wb");
 if (f == NULL) { cout << "No se pudo crear." << endl; return; }
 Registro r;
 cout << "Clave (0 para terminar): ";
 cin >> r.clave;
 while (r.clave > 0) {
 cout << "Descripcion: "; cin >> r.descripcion; // char[]: una palabra
 cout << "Valor: "; cin >> r.valor;
 fwrite(&r, sizeof(Registro), 1, f);
 cout << "Clave (0 para terminar): ";
 cin >> r.clave;
 }
 fclose(f);
 cout << "Archivo creado." << endl;
}