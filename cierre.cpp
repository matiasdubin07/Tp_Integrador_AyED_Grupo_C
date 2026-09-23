#include <cstudio>

struct Comanda{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision
}

const int Max_Archivos=7;
FILE* archivos[Max_Archivos];
Comanda r[Max_Archivos];
int leido[Max_Archivos]; // 1 si hay registro cargado, 0 si no existe ningun registro cargado
int cantidad_archivos=0;

for(int i=0; i<cantidad_archivos; i++){
    leido[i]=fread(&r[i], sizeof(Comanda), 1, archivos[i]);

}