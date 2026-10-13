#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

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
        cout << "No se pudo abrir mozos.dat. Corrieron normalizacion antes?" << endl;
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
        cout << "Error: el mozo " << idMozo << " no existe." << endl;
        fclose(f);
        return false;
    }

    if (m.password[0] == '\0')
    {
        char nueva[20];

        cout << "Es tu primera vez. Elegi tu clave: ";
        cin >> nueva;

        encriptar(nueva);
        strcpy(m.password, nueva);

        fseek(f, -(long)sizeof(Mozo), SEEK_CUR);
        fwrite(&m, sizeof(Mozo), 1, f);
        fclose(f);

        cout << "Clave guardada. Bienvenido, " << m.nombre << "." << endl;
        return true;
    }

    fclose(f);

    char clave[20];

    cout << "Clave: ";
    cin >> clave;

    char claveGuardada[20];
    strcpy(claveGuardada, m.password);
    desencriptar(claveGuardada);

    if (strcmp(clave, claveGuardada) != 0)
    {
        cout << "Error: clave incorrecta." << endl;
        return false;
    }

    cout << "Bienvenido, " << m.nombre << "." << endl;
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

    cout << "Codigo de producto: ";
    cin >> codigo;

    cout << "Cantidad: ";
    cin >> cantidad;

    if (cantidad <= 0)
    {
        cout << "Error: la cantidad tiene que ser mayor a 0." << endl;
        return;
    }

    FILE* fInv = fopen("inventario.dat", "rb+");

    if (fInv == NULL)
    {
        cout << "No se pudo abrir inventario.dat" << endl;
        return;
    }

    Producto p;
    long pos;

    if (!buscarProducto(fInv, codigo, p, pos))
    {
        cout << "Error: el producto " << codigo << " no existe." << endl;
        fclose(fInv);
        return;
    }

    if (p.stockActual < cantidad)
    {
        cout << "Error: stock insuficiente de '" << p.descripcion << "' (quedan " << p.stockActual << ")." << endl;
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
        cout << "No se pudo abrir la planilla del dia." << endl;
        return;
    }

    fwrite(&c, sizeof(Comanda), 1, fDia);
    fclose(fDia);

    FILE* fMozos = fopen("mozos.dat", "rb+");
    if (fMozos != NULL)
    {
        Mozo m;
        while (fread(&m, sizeof(Mozo), 1, fMozos) == 1)
        {
            if (m.idMozo == idMozo)
            {
                m.totalComision += c.comision;
                fseek(fMozos, -(long)sizeof(Mozo), SEEK_CUR);
                fwrite(&m, sizeof(Mozo), 1, fMozos);
                break;
            }
        }
        fclose(fMozos);
    }

    int enteros = (int)c.comision;
    int centavos = (int)((c.comision - enteros) * 100 + 0.5);

    cout << "Venta cargada: " << cantidad << " x " << p.descripcion << " | comision: " << enteros << ".";
    if (centavos < 10)
    {
        cout << "0";
    }
    cout << centavos << endl;
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

    cout << "Fecha del dia (DD-MM-AAAA): ";
    cin >> fecha;

    while (!fechaValida(fecha))
    {
        cout << "Formato invalido. Ejemplo: 03-06-2025" << endl;
        cout << "Fecha del dia (DD-MM-AAAA): ";
        cin >> fecha;
    }

    armarNombreArchivo(fecha, nombreArchivoDia);

    FILE* fDia = fopen(nombreArchivoDia, "ab");

    if (fDia == NULL)
    {
        cout << "No se pudo abrir ni crear " << nombreArchivoDia << endl;
        return 1;
    }

    fclose(fDia);

    int opcion = -1;

    while (opcion != 0)
    {
        cout << "\n===== BUFFET DE ALBERTO - Ventas del " << fecha << " =====" << endl;
        cout << "1. Cargar una venta" << endl;
        cout << "0. Terminar" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1)
        {
            int idMozo;

            cout << "Numero de mozo: ";
            cin >> idMozo;

            if (loginMozo(idMozo))
            {
                cargarVenta(idMozo, nombreArchivoDia);
            }
        }
        else if (opcion != 0)
        {
            cout << "Opcion invalida." << endl;
        }
    }

    ordenarPorMozo(nombreArchivoDia);
    cout << "Planilla " << nombreArchivoDia << " ordenada por mozo. Hasta luego!" << endl;

    return 0;
}
