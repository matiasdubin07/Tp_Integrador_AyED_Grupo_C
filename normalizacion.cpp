#include <cstdio>
#include <cstdlib>
#include <cstring>

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

void cifrar(char origen[], char destino[]) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i] + K;
        i++;
    }
    destino[i] = '\0';
}

int buscarMozo(Mozo mozos[], int cantidadMozos, char nombre[]) {
    for (int i = 0; i < cantidadMozos; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) return i;
    }
    return -1;
}

int buscarFecha(char fechas[][11], int cantidadFechas, char fecha[]) {
    for (int i = 0; i < cantidadFechas; i++) {
        if (strcmp(fechas[i], fecha) == 0) return i;
    }
    return -1;
}

void actualizarStock(FILE* inventario, int codigo, int cantidad) {
    Producto producto;
    rewind(inventario);

    while (fread(&producto, sizeof(Producto), 1, inventario) == 1) {
        if (producto.codigo == codigo) {
            producto.stockActual -= cantidad;
            fseek(inventario, -sizeof(Producto), SEEK_CUR);
            fwrite(&producto, sizeof(Producto), 1, inventario);
            return;
        }
    }
}

void ordenarDia(char nombreArchivo[]) {
    FILE* archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL) return;

    fseek(archivo, 0, SEEK_END);
    int cantidad = ftell(archivo) / sizeof(Comanda);
    rewind(archivo);

    Comanda* comandas = (Comanda*)malloc(cantidad * sizeof(Comanda));
    if (comandas == NULL) {
        fclose(archivo);
        return;
    }

    fread(comandas, sizeof(Comanda), cantidad, archivo);
    fclose(archivo);

    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - i - 1; j++) {
            if (comandas[j].idMozo > comandas[j + 1].idMozo) {
                Comanda auxiliar = comandas[j];
                comandas[j] = comandas[j + 1];
                comandas[j + 1] = auxiliar;
            }
        }
    }

    archivo = fopen(nombreArchivo, "wb");
    if (archivo != NULL) {
        fwrite(comandas, sizeof(Comanda), cantidad, archivo);
        fclose(archivo);
    }
    free(comandas);
}

int main() {
    if (sizeof(ComandaHistorica) != 76 || sizeof(Producto) != 64) {
        printf("Error en los tamanos de las estructuras.\n");
        return 1;
    }

    FILE* historicas = fopen("comandas_historicas.dat", "rb");
    FILE* inventario = fopen("inventario.dat", "r+b");
    if (historicas == NULL || inventario == NULL) {
        printf("Faltan comandas_historicas.dat o inventario.dat.\n");
        return 1;
    }

    fseek(historicas, 0, SEEK_END);
    int cantidadHistoricas = ftell(historicas) / sizeof(ComandaHistorica);
    rewind(historicas);

    ComandaHistorica* historicasLeidas = (ComandaHistorica*)malloc(cantidadHistoricas * sizeof(ComandaHistorica));
    Mozo* mozos = (Mozo*)calloc(cantidadHistoricas, sizeof(Mozo));
    char (*fechas)[11] = (char (*)[11])calloc(cantidadHistoricas, sizeof(*fechas));
    if (historicasLeidas == NULL || mozos == NULL || fechas == NULL) {
        printf("No hay memoria suficiente.\n");
        fclose(historicas);
        fclose(inventario);
        return 1;
    }

    fread(historicasLeidas, sizeof(ComandaHistorica), cantidadHistoricas, historicas);
    fclose(historicas);

    int cantidadMozos = 0;
    int cantidadFechas = 0;

    for (int i = 0; i < cantidadHistoricas; i++) {
        int posicionMozo = buscarMozo(mozos, cantidadMozos, historicasLeidas[i].nombreMozo);
        if (posicionMozo == -1) {
            posicionMozo = cantidadMozos;
            mozos[posicionMozo].idMozo = cantidadMozos + 1;
            strcpy(mozos[posicionMozo].nombre, historicasLeidas[i].nombreMozo);

            char clave[20];
            sprintf(clave, "%d", mozos[posicionMozo].idMozo);
            cifrar(clave, mozos[posicionMozo].password);
            cantidadMozos++;
        }
        mozos[posicionMozo].totalComision += historicasLeidas[i].comision;

        char archivoDiario[40];
        sprintf(archivoDiario, "comandas_%s.dat", historicasLeidas[i].fecha);
        int fechaNueva = buscarFecha(fechas, cantidadFechas, historicasLeidas[i].fecha) == -1;
        FILE* diario = fopen(archivoDiario, fechaNueva ? "wb" : "ab");
        if (diario != NULL) {
            Comanda comanda;
            comanda.idMozo = mozos[posicionMozo].idMozo;
            comanda.codigoProducto = historicasLeidas[i].codigoProducto;
            comanda.cantidad = historicasLeidas[i].cantidad;
            comanda.comision = historicasLeidas[i].comision;
            fwrite(&comanda, sizeof(Comanda), 1, diario);
            fclose(diario);
        }

        if (fechaNueva) {
            strcpy(fechas[cantidadFechas], historicasLeidas[i].fecha);
            cantidadFechas++;
        }
        actualizarStock(inventario, historicasLeidas[i].codigoProducto, historicasLeidas[i].cantidad);
    }
    fclose(inventario);

    FILE* archivoMozos = fopen("mozos.dat", "wb");
    if (archivoMozos != NULL) {
        fwrite(mozos, sizeof(Mozo), cantidadMozos, archivoMozos);
        fclose(archivoMozos);
    }

    for (int i = 0; i < cantidadFechas; i++) {
        char archivoDiario[40];
        sprintf(archivoDiario, "comandas_%s.dat", fechas[i]);
        ordenarDia(archivoDiario);
    }

    printf("Listo: %d mozos y %d dias procesados.\n", cantidadMozos, cantidadFechas);
    free(historicasLeidas);
    free(mozos);
    free(fechas);
    return 0;
}
