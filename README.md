# TP Integrador
**Algoritmos y Estructuras de Datos — UTN FRBA**  
**Grupo C**

---

## 👥 Integrantes

| Nombre y Apellido | Usuario de GitHub | Programa / Rol Principal |
|-------------------|-------------------|--------------------------|
| Ian Jolodovsky      | @IanJolodovsky    | normalizacion.cpp        |
| [Ariela Libezuk] | @[arielalibezuk]    | ventas.cpp               |
| [Matias Corizzo] | @[MatiasCorizzo14]    | cierre.cpp               |
| [Nicolas Kaplun] | @[nkaplun07]    | resumen.cpp              |
| [Matias Dubin] | @[matiasdubin07]    | normalizacion.cpp / Testing |

---

## 📂 Estructura del Proyecto

El sistema está dividido en cuatro programas independientes en C++, cada uno con su propio `main()`:

- **`normalizacion.cpp`**: Procesa las comandas históricas sin formato del sistema anterior, genera el archivo maestro de mozos (`mozos.dat`), separa las ventas por día (`comandas_DD-MM-AAAA.dat` ordenadas por mozo) y actualiza el stock inicial en `inventario.dat`.
- **`ventas.cpp`**: Permite la carga interactiva de ventas diarias con login por mozo, validación y descuento de stock en tiempo real vía búsqueda binaria, cálculo automático de comisiones y actualización de `mozos.dat`. Al finalizar la jornada, deja la planilla del día ordenada por mozo.
- **`cierre.cpp`**: Realiza el apareo (merge) de hasta 7 planillas diarias para consolidar una única planilla semanal (`comandas_semana_sX-mm.dat`) ordenada por `idMozo`.
- **`resumen.cpp`**: Procesa la planilla semanal mediante corte de control por mozo, emitiendo un reporte por pantalla con las cantidades vendidas, comisiones liquidadas y el total global del buffet.

---

## ⚙️ Compilación y Ejecución

Se recomienda compilar con `g++` (estándar C++11 o superior):

### 1. Compilación
```bash
g++ -o normalizacion normalizacion.cpp
g++ -o ventas ventas.cpp
g++ -o cierre cierre.cpp
g++ -o resumen resumen.cpp
```

### 2. Orden de Ejecución

Los programas deben ejecutarse en el siguiente orden secuencial:

1. **`./normalizacion`**  
   - *Requiere:* `comandas_historicas.dat`, `inventario.dat` (en el directorio de ejecución).  
   - *Genera:* `mozos.dat`, `comandas_DD-MM-AAAA.dat` (por cada fecha histórica) y actualiza `inventario.dat`.

2. **`./ventas`**  
   - *Requiere:* `mozos.dat`, `inventario.dat`.  
   - *Solicita:* Fecha de la jornada (`DD-MM-AAAA`), número de mozo, clave, producto y cantidad.  
   - *Genera / Modifica:* `comandas_DD-MM-AAAA.dat`, actualiza `inventario.dat` y `mozos.dat`.

3. **`./cierre`**  
   - *Requiere:* Las planillas diarias correspondientes a la semana a cerrar (`comandas_DD-MM-AAAA.dat`).  
   - *Solicita:* Cantidad de días (1 a 7), fecha de cada día, número de semana y mes.  
   - *Genera:* `comandas_semana_sX-mm.dat`.

4. **`./resumen`**  
   - *Requiere:* `comandas_semana_sX-mm.dat`.  
   - *Solicita:* Nombre del archivo semanal a procesar.  
   - *Emite:* Reporte por pantalla con el totalizado por mozo y el total general.

---

## 🧠 Resoluciones Técnicas y Algoritmos Clave

### 1. Normalización de Datos Históricos (`normalizacion.cpp`)
- **Unicidad de Mozos:** Se identifican los nombres únicos en `comandas_historicas.dat` asignando IDs numéricos correlativos (`idMozo = 1, 2, ...`).
- **Claves Iniciales Seguras:** Se genera para cada mozo una contraseña inicial (su número de mozo) encriptada con corrimiento César ($K = 5$), cumpliendo con la directiva del enunciado.
- **Actualización Directa de Inventario:** Mediante `fseek` relativo sobre `inventario.dat`, se descuentan las unidades vendidas sin recrear el archivo en disco.
- **Partición y Ordenamiento Diario:** Se ordenan las ventas históricas en memoria por fecha e ID de mozo (Bubble Sort) y se escriben secuencialmente en sus respectivos archivos diarios.

### 2. Carga Diaria y Búsqueda Binaria (`ventas.cpp`)
- **Búsqueda Binaria en Disco:** Como `inventario.dat` se encuentra ordenado por `codigo` de producto, se calcula la cantidad de registros (`ftell / sizeof(Producto)`) y se aplica búsqueda binaria realizando accesos directos con `fseek(f, medio * sizeof(Producto), SEEK_SET)` para una búsqueda en tiempo $O(\log N)$.
- **Cifrado Reversible de Claves:** Se implementa un cifrado por corrimiento César ($K = 5$). Al ingresar la clave, se aplica la misma transformación para compararla contra la almacenada en `mozos.dat`. En caso de ser el primer ingreso, se le solicita definir su clave y se persiste ya encriptada.
- **Actualización de Comisiones:** Tras cada venta confirmada (10% de tasa), se actualiza el acumulado `totalComision` del mozo en `mozos.dat`.
- **Ordenamiento Físico Directo:** Al finalizar el día, se aplica un ordenamiento por intercambio directamente sobre el archivo binario del día, garantizando que quede ordenado por `idMozo` para los pasos posteriores.

### 3. Consolidación Semanal por Apareo (`cierre.cpp`)
- **Algoritmo de Apareo (Merge):** Al contar con hasta 7 archivos diarios ya ordenados por `idMozo`, se realiza un apareo simultáneo utilizando una función `BuscarMenor()`. Se leen registros de cabecera (`leido[i]`) y se escribe al archivo destino `comandas_semana_sX-mm.dat` siempre el registro con menor `idMozo`, avanzando únicamente el puntero de dicho archivo.
- **Validación de Límites:** Se controla estrictamente que la cantidad de días ingresada pertenezca al rango $[1, 7]$, evitando desbordes de memoria.

### 4. Corte de Control (`resumen.cpp`)
- **Corte de Control Uninivel:** Dado que `comandas_semana_sX-mm.dat` está ordenado por `idMozo`, se implementa la técnica clásica de corte de control con lectura adelantada (`fread`).
- **Agrupamiento y Totales:** En el bucle anidado se acumulan las unidades vendidas y comisiones del mozo actual. Al cambiar el `idMozo`, se imprime el subtotal del mozo y se suma al gran acumulador del buffet (`totalBuffet`), resolviendo limpiamente el caso del último registro sin lecturas redundantes.
