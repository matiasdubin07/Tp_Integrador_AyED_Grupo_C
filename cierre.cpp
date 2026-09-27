#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Comanda{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const int Max_Archivos=7;

int PedirCantidadDias(){
    int cantidad_archivos;
    cout<<"Cuantos dias trabajo esta semana";
    cin>>cantidad_archivos;
    return cantidad_archivos;
}


void CargarArchivosDelDia(FILE* archivos[], Comanda r[], int leido[], int cantidad_archivos){
    for(int i=0; i<cantidad_archivos; i++){
        archivos[i]=PedirYAbrirArchivoDelDia();
         if(archivos[i]==NULL){
            cout<<"No se pudo abrir el archivo del dia "<<i+1<<endl;
            leido[i]=0;
        }
    }
    for(int i=0; i<cantidad_archivos; i++){
        if(archivos[i]!=NULL){
            leido[i]=fread(&r[i], sizeof(Comanda), 1, archivos[i]);
        }
    }
}

FILE* PedirYAbrirArchivoDelDia(){
    char fecha[11];
    cout<<"Ingrese la fecha del dia (DD-MM-AAAA)";
    cin>>fecha;
    char nombreArchivo[30];
    strcpy(nombreArchivo, "comandas_");
    strcat(nombreArchivo, fecha);
    strcat(nombreArchivo, ".dat");
    FILE* archivo=fopen(nombreArchivo, "rb");
    if(archivo==NULL){
        cout<<"No se encontro el archivo del dia "<<fecha<<endl;
    }
    return archivo;
}



int BuscarMenor( Comanda r[], int leido[], int cantidad){
    int pos=-1;
    for(int i=0; i<cantidad; i++){
        if(leido[i]==1 && (pos==-1 || r[i].idMozo < r[pos].idMozo)){
            pos=i;
        }
    }
    return pos;
}

FILE* PedirYAbrirArchivoDeSalida(){
    char nombreSalida[30];
    int semana;
    int mes;
    cout<<"Ingrese el numero de semana: ";
    cin>>semana;
    cout<<"Ingrese el numero de mes: ";
    cin>>mes;
    sprintf(nombreSalida, "comandas_semana_s%d-%02d.dat", semana, mes);
    return fopen(nombreSalida, "wb");
}


int main(){
    FILE* archivos[Max_Archivos];
    Comanda r[Max_Archivos];
    int leido[Max_Archivos]; // 1 si hay registro cargado, 0 si no existe ningun registro cargado
    int cantidad_archivos= PedirCantidadDias();
    CargarArchivosDelDia(archivos, r, leido, cantidad_archivos);
     
    FILE* salida=PedirYAbrirArchivoDeSalida();
}