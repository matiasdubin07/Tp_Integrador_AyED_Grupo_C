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
    for (int i = 0; pass[i] != '\0'; i++) {
        pass[i] = pass[i] + k;
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
    if (!fHistoricas) {
        cout << "Error al abrir comandas_historicas.dat\n";
        return 1;
    }
    FILE* fInventario = fopen("inventario.dat", "r+b");
    if (!fInventario) {
        cout << "Error al abrir inventario.dat\n";
        fclose(fHistoricas); 
        return 1;
    }
    Mozo mozos[100];
    int cantMozos = 0;
    ComandaProcesada comandas[10000];
    int cantComandas = 0;
ComandaHistorica com;
while (fread(&com, sizeof(ComandaHistorica), 1, fHistoricas) == 1) {

    // --- MOZOS ---
    int buscarMozo = buscarMozoPorNombre(mozos, cantMozos, com.nombreMozo);
    int idActual;
    if (buscarMozo == -1) {
        idActual = cantMozos + 1;
        mozos[cantMozos].idMozo = idActual;
        strcpy(mozos[cantMozos].nombre, com.nombreMozo);
        mozos[cantMozos].totalcomision = com.comision;
        sprintf(mozos[cantMozos].password, "%d", idActual);
        encriptar(mozos[cantMozos].password, K);
        cantMozos++;
    } else {
        idActual = mozos[buscarMozo].idMozo;
        mozos[buscarMozo].totalcomision += com.comision;
    }

    // --- GUARDAR COMANDA EN MEMORIA ---
    strcpy(comandas[cantComandas].fecha, com.fecha);
    comandas[cantComandas].comanda.idMozo = idActual;
    comandas[cantComandas].comanda.codigoProducto = com.codigoProducto;
    comandas[cantComandas].comanda.cantidad = com.cantidad;
    comandas[cantComandas].comanda.comision = com.comision;
    cantComandas++;

    // --- DESCONTAR STOCK DEL INVENTARIO ---
    fseek(fInventario, 0, SEEK_SET);
    Producto p;
    while (fread(&p, sizeof(Producto), 1, fInventario) == 1) {
        if (p.codigo == com.codigoProducto) {
            p.stockActual -= com.cantidad;
            fseek(fInventario, -(long)sizeof(Producto), SEEK_CUR);
            fwrite(&p, sizeof(Producto), 1, fInventario);
            fflush(fInventario);
            break;
        }
    }
}
FILE* fMozos = fopen("mozos.dat", "wb");
if (fMozos == NULL) {
    cout << "Error al crear mozos.dat\n";
    fclose(fHistoricas);
    fclose(fInventario);
    return 1;
}

for (int i = 0; i < cantMozos; i++) {
    fwrite(&mozos[i], sizeof(Mozo), 1, fMozos);
}

fclose(fMozos);
cout << "Se creó el archivo con todos los mozos perfectamente!!"<< endl;
fclose(fHistoricas);
fclose(fInventario);
};