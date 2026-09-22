#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "estructuras.h"

const char ARCHIVO_HISTORICAS[] = "comandas_historicas.dat";
const char ARCHIVO_INVENTARIO[] = "inventario.dat";
const char ARCHIVO_MOZOS[] = "mozos.dat";
const int LARGO_FECHA = 11;
const int LARGO_NOMBRE_ARCHIVO = 40;

struct FechaProcesada {
    char valor[LARGO_FECHA];
};

void cifrarClave(const char* clave, char* claveCifrada) {
    int i = 0;
    while (clave[i] != '\0' && i < 19) {
        claveCifrada[i] = static_cast<char>(clave[i] + DESPLAZAMIENTO_CLAVE);
        i++;
    }
    claveCifrada[i] = '\0';
}

int buscarMozo(const Mozo* mozos, int cantidadMozos, const char* nombre) {
    for (int i = 0; i < cantidadMozos; i++) {
        if (strcmp(mozos[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

int buscarFecha(const FechaProcesada* fechas, int cantidadFechas, const char* fecha) {
    for (int i = 0; i < cantidadFechas; i++) {
        if (strcmp(fechas[i].valor, fecha) == 0) {
            return i;
        }
    }
    return -1;
}

void nombreArchivoDiario(const char* fecha, char* nombreArchivo) {
    snprintf(nombreArchivo, LARGO_NOMBRE_ARCHIVO, "comandas_%s.dat", fecha);
}

int compararComandasPorMozo(const void* primero, const void* segundo) {
    const Comanda* a = static_cast<const Comanda*>(primero);
    const Comanda* b = static_cast<const Comanda*>(segundo);

    if (a->idMozo < b->idMozo) return -1;
    if (a->idMozo > b->idMozo) return 1;
    return 0;
}

int ordenarArchivoDiario(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "rb");
    if (archivo == NULL) {
        printf("No se pudo abrir %s para ordenar.\n", nombreArchivo);
        return 0;
    }

    fseek(archivo, 0, SEEK_END);
    long bytes = ftell(archivo);
    rewind(archivo);

    if (bytes < 0 || bytes % sizeof(Comanda) != 0) {
        printf("El archivo %s tiene un formato invalido.\n", nombreArchivo);
        fclose(archivo);
        return 0;
    }

    int cantidad = static_cast<int>(bytes / sizeof(Comanda));
    if (cantidad == 0) {
        fclose(archivo);
        return 1;
    }

    Comanda* comandas = static_cast<Comanda*>(malloc(cantidad * sizeof(Comanda)));
    if (comandas == NULL) {
        printf("No hay memoria para ordenar %s.\n", nombreArchivo);
        fclose(archivo);
        return 0;
    }

    int leidos = static_cast<int>(fread(comandas, sizeof(Comanda), cantidad, archivo));
    fclose(archivo);
    if (leidos != cantidad) {
        printf("No se pudo leer completo %s.\n", nombreArchivo);
        free(comandas);
        return 0;
    }

    qsort(comandas, cantidad, sizeof(Comanda), compararComandasPorMozo);
    archivo = fopen(nombreArchivo, "wb");
    if (archivo == NULL) {
        printf("No se pudo reescribir %s.\n", nombreArchivo);
        free(comandas);
        return 0;
    }

    int escritos = static_cast<int>(fwrite(comandas, sizeof(Comanda), cantidad, archivo));
    fclose(archivo);
    free(comandas);

    if (escritos != cantidad) {
        printf("No se pudo escribir completo %s.\n", nombreArchivo);
        return 0;
    }
    return 1;
}

int descontarStock(FILE* inventario, int codigoProducto, int cantidad) {
    Producto producto;
    rewind(inventario);

    while (fread(&producto, sizeof(Producto), 1, inventario) == 1) {
        if (producto.codigo == codigoProducto) {
            producto.stockActual -= cantidad;
            fseek(inventario, -static_cast<long>(sizeof(Producto)), SEEK_CUR);
            if (fwrite(&producto, sizeof(Producto), 1, inventario) != 1) {
                return 0;
            }
            fflush(inventario);
            return 1;
        }
    }
    return 0;
}

int main() {
    if (sizeof(ComandaHistorica) != 76 || sizeof(Producto) != 64) {
        printf("Error de estructuras: ComandaHistorica=%d, Producto=%d.\n",
               static_cast<int>(sizeof(ComandaHistorica)),
               static_cast<int>(sizeof(Producto)));
        return 1;
    }

    FILE* historicas = fopen(ARCHIVO_HISTORICAS, "rb");
    FILE* inventario = fopen(ARCHIVO_INVENTARIO, "r+b");
    if (historicas == NULL || inventario == NULL) {
        printf("Se necesitan %s e %s en la carpeta de ejecucion.\n",
               ARCHIVO_HISTORICAS, ARCHIVO_INVENTARIO);
        if (historicas != NULL) fclose(historicas);
        if (inventario != NULL) fclose(inventario);
        return 1;
    }

    fseek(historicas, 0, SEEK_END);
    long bytesHistoricas = ftell(historicas);
    rewind(historicas);
    if (bytesHistoricas < 0 || bytesHistoricas % sizeof(ComandaHistorica) != 0) {
        printf("%s tiene un formato invalido.\n", ARCHIVO_HISTORICAS);
        fclose(historicas);
        fclose(inventario);
        return 1;
    }

    int capacidad = static_cast<int>(bytesHistoricas / sizeof(ComandaHistorica));
    Mozo* mozos = static_cast<Mozo*>(calloc(capacidad, sizeof(Mozo)));
    FechaProcesada* fechas = static_cast<FechaProcesada*>(calloc(capacidad, sizeof(FechaProcesada)));
    if ((capacidad > 0) && (mozos == NULL || fechas == NULL)) {
        printf("No hay memoria suficiente para procesar las comandas.\n");
        fclose(historicas);
        fclose(inventario);
        free(mozos);
        free(fechas);
        return 1;
    }

    int cantidadMozos = 0;
    int cantidadFechas = 0;
    int cantidadComandas = 0;
    int erroresStock = 0;
    ComandaHistorica historica;

    while (fread(&historica, sizeof(ComandaHistorica), 1, historicas) == 1) {
        int posicionMozo = buscarMozo(mozos, cantidadMozos, historica.nombreMozo);
        if (posicionMozo == -1) {
            posicionMozo = cantidadMozos;
            mozos[posicionMozo].idMozo = cantidadMozos + 1;
            strncpy(mozos[posicionMozo].nombre, historica.nombreMozo,
                    sizeof(mozos[posicionMozo].nombre) - 1);

            char claveInicial[20];
            snprintf(claveInicial, sizeof(claveInicial), "%d", mozos[posicionMozo].idMozo);
            cifrarClave(claveInicial, mozos[posicionMozo].password);
            mozos[posicionMozo].totalComision = 0.0f;
            cantidadMozos++;
        }
        mozos[posicionMozo].totalComision += historica.comision;

        int posicionFecha = buscarFecha(fechas, cantidadFechas, historica.fecha);
        char archivoDiario[LARGO_NOMBRE_ARCHIVO];
        nombreArchivoDiario(historica.fecha, archivoDiario);
        FILE* diario = NULL;
        if (posicionFecha == -1) {
            strncpy(fechas[cantidadFechas].valor, historica.fecha,
                    sizeof(fechas[cantidadFechas].valor) - 1);
            cantidadFechas++;
            diario = fopen(archivoDiario, "wb");
        } else {
            diario = fopen(archivoDiario, "ab");
        }

        if (diario == NULL) {
            printf("No se pudo crear o abrir %s.\n", archivoDiario);
            fclose(historicas);
            fclose(inventario);
            free(mozos);
            free(fechas);
            return 1;
        }

        Comanda comanda;
        comanda.idMozo = mozos[posicionMozo].idMozo;
        comanda.codigoProducto = historica.codigoProducto;
        comanda.cantidad = historica.cantidad;
        comanda.comision = historica.comision;
        int escribioComanda = fwrite(&comanda, sizeof(Comanda), 1, diario) == 1;
        fclose(diario);
        if (!escribioComanda) {
            printf("No se pudo escribir una comanda en %s.\n", archivoDiario);
            fclose(historicas);
            fclose(inventario);
            free(mozos);
            free(fechas);
            return 1;
        }

        if (!descontarStock(inventario, historica.codigoProducto, historica.cantidad)) {
            printf("Advertencia: no se pudo actualizar el producto %d.\n", historica.codigoProducto);
            erroresStock++;
        }
        cantidadComandas++;
    }
    fclose(historicas);
    fclose(inventario);

    FILE* archivoMozos = fopen(ARCHIVO_MOZOS, "wb");
    if (archivoMozos == NULL ||
        fwrite(mozos, sizeof(Mozo), cantidadMozos, archivoMozos) != static_cast<size_t>(cantidadMozos)) {
        printf("No se pudo generar %s.\n", ARCHIVO_MOZOS);
        if (archivoMozos != NULL) fclose(archivoMozos);
        free(mozos);
        free(fechas);
        return 1;
    }
    fclose(archivoMozos);

    for (int i = 0; i < cantidadFechas; i++) {
        char archivoDiario[LARGO_NOMBRE_ARCHIVO];
        nombreArchivoDiario(fechas[i].valor, archivoDiario);
        if (!ordenarArchivoDiario(archivoDiario)) {
            free(mozos);
            free(fechas);
            return 1;
        }
    }

    printf("Normalizacion terminada: %d comandas, %d mozos y %d dias procesados.\n",
           cantidadComandas, cantidadMozos, cantidadFechas);
    if (erroresStock > 0) {
        printf("Advertencias de inventario: %d.\n", erroresStock);
    }

    free(mozos);
    free(fechas);
    return 0;
}
