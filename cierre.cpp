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

int PedirCantidadDias();
void CargarArchivosDelDia(FILE* archivos[], Comanda r[], int leido[], int cantidad_archivos);
FILE* PedirYAbrirArchivoDelDia();
FILE* PedirYAbrirArchivoDeSalida();
int Apareo(FILE* archivos[], Comanda r[], int leido[], int cantidad_archivos, FILE* salida);
int BuscarMenor(Comanda r[], int leido[], int cantidad);
void CerrarArchivos(FILE* archivos[], int cantidad_archivos);

int main(){
    FILE* archivos[Max_Archivos];
    Comanda r[Max_Archivos];
    int leido[Max_Archivos]; // 1 si hay registro cargado, 0 si no existe ningun registro cargado
    int cantidad_archivos= PedirCantidadDias();
    CargarArchivosDelDia(archivos, r, leido, cantidad_archivos);
     
    FILE* salida=PedirYAbrirArchivoDeSalida();
    if(salida==NULL){
        cout<<"No se pudo crear el archivo de salida"<<endl;
        CerrarArchivos(archivos, cantidad_archivos);
        return 1;
    }
    int grabados=Apareo(archivos, r, leido, cantidad_archivos, salida);
    fclose(salida);
    CerrarArchivos(archivos, cantidad_archivos);
    return 0;
}

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

int Apareo(FILE* archivos[], Comanda r[], int leido[], int cantidad_archivos, FILE* salida){
    int grabados=0;
    int pos=BuscarMenor(r, leido, cantidad_archivos);
    while(pos!=-1){
        fwrite(&r[pos], sizeof(Comanda), 1, salida);
        grabados++;
        leido[pos]=fread(&r[pos], sizeof(Comanda), 1, archivos[pos]);
        pos=BuscarMenor(r, leido, cantidad_archivos);
    }
    return grabados;
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

void CerrarArchivos(FILE* archivos[], int cantidad_archivos){
    for(int i=0; i<cantidad_archivos; i++){
        if(archivos[i]!=NULL){
            fclose(archivos[i]);
        }
    }
}
