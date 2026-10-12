#include <cstdio>
#include <cstring>

struct Mozo
{
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Producto
{
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Comanda
{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const float TASA_COMISION = 0.10f;
const int K = 5;

void encriptar(char clave[])
{
    int i = 0;

    while (clave[i] != '\0')
    {
        clave[i] = clave[i] + K;
        i++;
    }
}

void desencriptar(char clave[])
{
    int i = 0;

    while (clave[i] != '\0')
    {
        clave[i] = clave[i] - K;
        i++;
    }
}

void armarNombreArchivo(const char fecha[], char nombre[])
{
    strcpy(nombre, "comandas_");
    strcat(nombre, fecha);
    strcat(nombre, ".dat");
}

bool fechaValida(const char fecha[])
{
    if (strlen(fecha) != 10)
    {
        return false;
    }

    if (fecha[2] != '-' || fecha[5] != '-')
    {
        return false;
    }

    for (int i = 0; i < 10; i++)
    {
        if (i != 2 && i != 5)
        {
            if (fecha[i] < '0' || fecha[i] > '9')
            {
                return false;
            }
        }
    }

    return true;
}

bool loginMozo(int idMozo)
{
    FILE* f = fopen("mozos.dat", "rb+");

    if (f == NULL)
    {
        printf("No se pudo abrir mozos.dat. Corrieron normalizacion antes?\n");
        return false;
    }

    Mozo m;
    bool existe = false;

    while (fread(&m, sizeof(Mozo), 1, f) == 1)
    {
        if (m.idMozo == idMozo)
        {
            existe = true;
            break;
        }
    }

    if (!existe)
    {
        printf("Error: el mozo %d no existe.\n", idMozo);
        fclose(f);
        return false;
    }

    if (m.password[0] == '\0')
    {
        char nueva[20];

        printf("Es tu primera vez. Elegi tu clave: ");
        scanf("%19s", nueva);

        encriptar(nueva);
        strcpy(m.password, nueva);

        fseek(f, -(long)sizeof(Mozo), SEEK_CUR);
        fwrite(&m, sizeof(Mozo), 1, f);
        fclose(f);

        printf("Clave guardada. Bienvenido, %s.\n", m.nombre);
        return true;
    }

    fclose(f);

    char clave[20];

    printf("Clave: ");
    scanf("%19s", clave);

    char claveGuardada[20];
    strcpy(claveGuardada, m.password);
    desencriptar(claveGuardada);

    if (strcmp(clave, claveGuardada) != 0)
    {
        printf("Error: clave incorrecta.\n");
        return false;
    }

    printf("Bienvenido, %s.\n", m.nombre);
    return true;
}

bool buscarProducto(FILE* f, int codigo, Producto& p, long& pos)
{
    fseek(f, 0, SEEK_END);
    long cantidad = ftell(f) / sizeof(Producto);

    long inicio = 0;
    long fin = cantidad - 1;

    while (inicio <= fin)
    {
        long medio = (inicio + fin) / 2;

        fseek(f, medio * sizeof(Producto), SEEK_SET);
        fread(&p, sizeof(Producto), 1, f);

        if (p.codigo == codigo)
        {
            pos = medio;
            return true;
        }
        else if (p.codigo < codigo)
        {
            inicio = medio + 1;
        }
        else
        {
            fin = medio - 1;
        }
    }

    return false;
}

void cargarVenta(int idMozo, const char nombreArchivoDia[])
{
    int codigo;
    int cantidad;

    printf("Codigo de producto: ");
    scanf("%d", &codigo);

    printf("Cantidad: ");
    scanf("%d", &cantidad);

    if (cantidad <= 0)
    {
        printf("Error: la cantidad tiene que ser mayor a 0.\n");
        return;
    }

    FILE* fInv = fopen("inventario.dat", "rb+");

    if (fInv == NULL)
    {
        printf("No se pudo abrir inventario.dat\n");
        return;
    }

    Producto p;
    long pos;

    if (!buscarProducto(fInv, codigo, p, pos))
    {
        printf("Error: el producto %d no existe.\n", codigo);
        fclose(fInv);
        return;
    }

    if (p.stockActual < cantidad)
    {
        printf("Error: stock insuficiente de '%s' (quedan %d).\n", p.descripcion, p.stockActual);
        fclose(fInv);
        return;
    }

    p.stockActual = p.stockActual - cantidad;
    fseek(fInv, pos * sizeof(Producto), SEEK_SET);
    fwrite(&p, sizeof(Producto), 1, fInv);
    fclose(fInv);

    Comanda c;
    c.idMozo = idMozo;
    c.codigoProducto = codigo;
    c.cantidad = cantidad;
    c.comision = p.precio * cantidad * TASA_COMISION;

    FILE* fDia = fopen(nombreArchivoDia, "ab");

    if (fDia == NULL)
    {
        printf("No se pudo abrir la planilla del dia.\n");
        return;
    }

    fwrite(&c, sizeof(Comanda), 1, fDia);
    fclose(fDia);

    printf("Venta cargada: %d x %s | comision: %.2f\n", cantidad, p.descripcion, c.comision);
}

void ordenarPorMozo(const char nombreArchivo[])
{
    FILE* f = fopen(nombreArchivo, "rb+");

    if (f == NULL)
    {
        return;
    }

    fseek(f, 0, SEEK_END);
    long cantidad = ftell(f) / sizeof(Comanda);

    Comanda a;
    Comanda b;

    for (long i = 0; i < cantidad - 1; i++)
    {
        for (long j = 0; j < cantidad - 1 - i; j++)
        {
            fseek(f, j * sizeof(Comanda), SEEK_SET);
            fread(&a, sizeof(Comanda), 1, f);
            fread(&b, sizeof(Comanda), 1, f);

            if (a.idMozo > b.idMozo)
            {
                fseek(f, j * sizeof(Comanda), SEEK_SET);
                fwrite(&b, sizeof(Comanda), 1, f);
                fwrite(&a, sizeof(Comanda), 1, f);
            }
        }
    }

    fclose(f);
}

int main()
{
    char fecha[11];
    char nombreArchivoDia[40];

    printf("Fecha del dia (DD-MM-AAAA): ");
    scanf("%10s", fecha);

    while (!fechaValida(fecha))
    {
        printf("Formato invalido. Ejemplo: 03-06-2025\n");
        printf("Fecha del dia (DD-MM-AAAA): ");
        scanf("%10s", fecha);
    }

    armarNombreArchivo(fecha, nombreArchivoDia);

    FILE* fDia = fopen(nombreArchivoDia, "ab");

    if (fDia == NULL)
    {
        printf("No se pudo abrir ni crear %s\n", nombreArchivoDia);
        return 1;
    }

    fclose(fDia);

    int opcion = -1;

    while (opcion != 0)
    {
        printf("\n===== BUFFET DE ALBERTO - Ventas del %s =====\n", fecha);
        printf("1. Cargar una venta\n");
        printf("0. Terminar\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        if (opcion == 1)
        {
            int idMozo;

            printf("Numero de mozo: ");
            scanf("%d", &idMozo);

            if (loginMozo(idMozo))
            {
                cargarVenta(idMozo, nombreArchivoDia);
            }
        }
        else if (opcion != 0)
        {
            printf("Opcion invalida.\n");
        }
    }

    ordenarPorMozo(nombreArchivoDia);
    printf("Planilla %s ordenada por mozo. Hasta luego!\n", nombreArchivoDia);

    return 0;
}
