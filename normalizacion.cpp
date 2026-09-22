#include <cstdio>
#include <cstring>

const int MAX_COMANDAS = 1000;
const int MAX_MOZOS = 100;
const int MAX_DIAS = 100;
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
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    FILE* archivoHistoricas = fopen("comandas_historicas.dat", "rb");
    FILE* archivoInventario = fopen("inventario.dat", "r+b");

    if (archivoHistoricas == NULL || archivoInventario == NULL) {
        printf("Faltan los archivos de datos.\n");
        return 1;
    }

    ComandaHistorica historicas[MAX_COMANDAS];
    Mozo mozos[MAX_MOZOS] = {};
    char fechas[MAX_DIAS][11] = {};

    int cantidadHistoricas = 0;
    while (fread(&historicas[cantidadHistoricas], sizeof(ComandaHistorica), 1, archivoHistoricas) == 1) {
        cantidadHistoricas++;
        if (cantidadHistoricas == MAX_COMANDAS) break;
    }
    fclose(archivoHistoricas);

    int cantidadMozos = 0;
    int cantidadFechas = 0;

    for (int i = 0; i < cantidadHistoricas; i++) {
        int posicionMozo = -1;

        for (int j = 0; j < cantidadMozos; j++) {
            if (strcmp(mozos[j].nombre, historicas[i].nombreMozo) == 0) {
                posicionMozo = j;
            }
        }

        if (posicionMozo == -1) {
            posicionMozo = cantidadMozos;
            mozos[posicionMozo].idMozo = cantidadMozos + 1;
            strcpy(mozos[posicionMozo].nombre, historicas[i].nombreMozo);

            char clave[20];
            sprintf(clave, "%d", mozos[posicionMozo].idMozo);
            for (int j = 0; clave[j] != '\0'; j++) {
                mozos[posicionMozo].password[j] = clave[j] + K;
            }
            cantidadMozos++;
        }

        mozos[posicionMozo].totalComision += historicas[i].comision;

        int fechaNueva = 1;
        for (int j = 0; j < cantidadFechas; j++) {
            if (strcmp(fechas[j], historicas[i].fecha) == 0) {
                fechaNueva = 0;
            }
        }

        char nombreArchivo[40];
        sprintf(nombreArchivo, "comandas_%s.dat", historicas[i].fecha);
        FILE* archivoDia;

        if (fechaNueva == 1) {
            strcpy(fechas[cantidadFechas], historicas[i].fecha);
            cantidadFechas++;
            archivoDia = fopen(nombreArchivo, "wb");
        } else {
            archivoDia = fopen(nombreArchivo, "ab");
        }

        Comanda comanda;
        comanda.idMozo = mozos[posicionMozo].idMozo;
        comanda.codigoProducto = historicas[i].codigoProducto;
        comanda.cantidad = historicas[i].cantidad;
        comanda.comision = historicas[i].comision;
        fwrite(&comanda, sizeof(Comanda), 1, archivoDia);
        fclose(archivoDia);

        Producto producto;
        rewind(archivoInventario);
        while (fread(&producto, sizeof(Producto), 1, archivoInventario) == 1) {
            if (producto.codigo == historicas[i].codigoProducto) {
                producto.stockActual -= historicas[i].cantidad;
                fseek(archivoInventario, -sizeof(Producto), SEEK_CUR);
                fwrite(&producto, sizeof(Producto), 1, archivoInventario);
                break;
            }
        }
    }
    fclose(archivoInventario);

    FILE* archivoMozos = fopen("mozos.dat", "wb");
    fwrite(mozos, sizeof(Mozo), cantidadMozos, archivoMozos);
    fclose(archivoMozos);

    for (int i = 0; i < cantidadFechas; i++) {
        char nombreArchivo[40];
        sprintf(nombreArchivo, "comandas_%s.dat", fechas[i]);

        FILE* archivoDia = fopen(nombreArchivo, "rb");
        Comanda comandasDia[MAX_COMANDAS];
        int cantidadDia = 0;

        while (fread(&comandasDia[cantidadDia], sizeof(Comanda), 1, archivoDia) == 1) {
            cantidadDia++;
        }
        fclose(archivoDia);

        for (int j = 0; j < cantidadDia - 1; j++) {
            for (int k = 0; k < cantidadDia - j - 1; k++) {
                if (comandasDia[k].idMozo > comandasDia[k + 1].idMozo) {
                    Comanda auxiliar = comandasDia[k];
                    comandasDia[k] = comandasDia[k + 1];
                    comandasDia[k + 1] = auxiliar;
                }
            }
        }

        archivoDia = fopen(nombreArchivo, "wb");
        fwrite(comandasDia, sizeof(Comanda), cantidadDia, archivoDia);
        fclose(archivoDia);
    }

    printf("Normalizacion terminada.\n");
    printf("Mozos: %d\n", cantidadMozos);
    printf("Dias: %d\n", cantidadFechas);
    return 0;
}
