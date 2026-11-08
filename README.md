# Hack Assembler

Proyecto 7 de **Arquitectura y Organización de Computadoras** (Ingeniería de Software, UADY), basado en el curso Nand2Tetris.

**Equipo:** Misa y Leo
**Lenguaje:** C++17

## ¿Qué es?

Un ensamblador que traduce programas escritos en el lenguaje ensamblador **Hack** (`.asm`) a código binario **Hack** de 16 bits (`.hack`).

Dado un archivo `Prog.asm`, el programa genera `Prog.hack` en la misma carpeta (si ya existe, lo sobrescribe). La salida debe ser **idéntica** a la del ensamblador oficial de Nand2Tetris.

Se asume que el código fuente siempre es válido, así que no se implementa manejo de errores.

## Compilar y usar

```bash
g++ -std=c++17 -o assembler assembler.cpp
./assembler Add.asm        # genera Add.hack
```

En Windows: `assembler.exe Add.asm`

## Cómo funciona

1. **Limpieza:** se quitan comentarios (`//...`), espacios, tabs y líneas vacías.
2. **Primera pasada:** se recorren las líneas y cada etiqueta `(XXX)` se guarda en la tabla de símbolos con la dirección ROM de la instrucción que le sigue.
3. **Segunda pasada:** se traduce cada instrucción:
   - **Instrucción A** (`@valor`): `0` + dirección en 15 bits. El valor puede ser un número, un símbolo predefinido o una etiqueta. Si es una variable nueva, se le asigna una dirección de RAM a partir de la 16.
   - **Instrucción C** (`dest=comp;jump`): `111` + `comp` (7 bits) + `dest` (3 bits) + `jump` (3 bits).

### Símbolos predefinidos

| Símbolo | Dirección |
|---|---|
| `R0` – `R15` | 0 – 15 |
| `SP`, `LCL`, `ARG`, `THIS`, `THAT` | 0, 1, 2, 3, 4 |
| `SCREEN` | 16384 |
| `KBD` | 24576 |

## Estructura del código (`assembler.cpp`)

| Parte | Descripción | Responsable |
|---|---|---|
| Tablas `COMP`, `DEST`, `JUMP` | Códigos binarios de cada campo | Listo |
| `crearTablaSimbolos()` | Símbolos predefinidos | Listo |
| `limpiar()` | Quita comentarios y espacios | Listo |
| `main()` | Lectura del `.asm` y escritura del `.hack` | Listo |
| `traducirC()` | Separar y traducir `dest=comp;jump` | Misa |
| `primeraPasada()` | Registrar etiquetas | Leo |
| `traducirA()` | Traducir `@valor` (números, símbolos y variables) | Leo |

## Pruebas

El desarrollo se hace en dos etapas:

| Etapa | Qué incluye | Programas de prueba |
|---|---|---|
| 1. Básica | Sin símbolos | `Add.asm`, `MaxL.asm`, `RectL.asm` |
| 2. Completa | Etiquetas y variables | `Add.asm`, `Max.asm`, `Rect.asm` |

Qué hace cada programa:
- **Add:** suma 2 + 3 y guarda el resultado en `R0` (esperado: `R0 = 5`).
- **Max:** calcula el máximo entre `R0` y `R1` y lo guarda en `R2`.
- **Rect:** dibuja un rectángulo de 16 píxeles de ancho y `R0` píxeles de alto en la esquina superior izquierda de la pantalla.

Para verificar cada programa:
1. **Comparación:** en el Assembler del IDE online de Nand2Tetris, traducir el `.asm`, pegar nuestro `.hack` en el panel *Compare code* y presionar *Compare*. Debe aparecer *Comparison successful*.
2. **Ejecución:** cargar nuestro `.hack` en el CPU Emulator, poner los valores de prueba en RAM (`R0`, `R1` según el programa), ejecutarlo y revisar el resultado.

## Entregable

El código **no se entrega**. Se entrega un **único PDF** con:

1. **Portada:** título del proyecto, nombres, materia y fecha de entrega.
2. **Resultados de pruebas** para Add, Max y Rect, cada uno con:
   - captura de la comparación exitosa contra el ensamblador oficial;
   - captura de la ejecución correcta en el CPU Emulator.
3. **Ejemplo de traducción:** explicación de cómo se traducen al menos 3 instrucciones de uno de los programas a binario de 16 bits, incluyendo al menos una con un símbolo (etiqueta o variable).

Las capturas deben ser legibles e indicar claramente a qué programa corresponden.
