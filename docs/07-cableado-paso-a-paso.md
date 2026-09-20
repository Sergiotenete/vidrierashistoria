# 07 — Cableado paso a paso

El esquema del doc 01 dice **qué está conectado con qué**. Esto dice **qué hacer con las
manos, y en qué orden**.

---

## La idea que lo desbloquea todo

Las dos líneas horizontales del esquema no son una abstracción: son **dos puntos de
reparto de verdad**. Uno lleva el +5 V; el otro, la masa.

En este proyecto la fuente es un **GeeekPi de 5 V / 5 A USB-C** con enchufe y cable ya
montados, así que esos dos puntos se materializan con un **latiguillo hembra USB-C** de
dos hilos (rojo y negro) y **dos conectores rápidos de palanca** tipo WAGO: uno para todos
los rojos y otro para todos los negros.

> **Lo primero de todo:** enchufa la fuente, conecta solo el latiguillo y mide entre el
> hilo rojo y el negro con la punta roja en el rojo. **~5,1 V** confirma de una vez las dos
> cosas que hacen falta: que el latiguillo lleva las resistencias `CC` que despiertan a la
> fuente, y que los colores no están invertidos. **0 V** significa que le faltan esas
> resistencias y hay que cambiar de latiguillo. Ver doc 01 §1 bis.

**Todo lo demás cuelga de esas dos regletas**: el ESP32, el adaptador de nivel, los
condensadores, la tira. En cuanto las montas, el resto del cableado deja de tener
misterio — cada cosa va de un punto a una de las dos regletas, y nada se encadena con
nada.

---

## Qué hay físicamente dentro de la caja

Cuatro cosas, y ninguna toca a las demás salvo por las regletas:

1. **La fuente**, con los bornes de red a un lado y los de 5 V al otro.
2. **Los dos conectores rápidos** (uno para los rojos, otro para los negros), alimentados
   directamente por los dos hilos del latiguillo USB-C. Sin fusible: el latiguillo ya
   limita a 3 A y la fuente lleva su propia protección.
3. **Una placa auxiliar** de perfboard con **U1**, **R1** y los condensadores **C2**,
   **C3** y **C4**. Una plaquita de 5 × 5 cm sobra.
4. **El ESP32**, sobre zócalos de tira para poder sacarlo sin desoldar nada. Se alimenta
   por su **puerto USB-C**, con un latiguillo macho que sale de los conectores rápidos;
   de sus pines solo salen dos cables de señal, `GPIO 13` y `GPIO 27`.

**C1** (220-470 µF) es la excepción: va en el otro extremo, junto a la regleta de salida
**J1**, lo más cerca posible de donde arranca la tira.

---

## El adaptador de nivel, patilla por patilla

Es donde más gente se atasca. El 74AHCT125 lleva **cuatro búferes** y solo usas uno; los
otros tres hay que dejarlos deshabilitados y con la entrada atada, no al aire.

```
                    ╭───╮
        GND ──── 1 ─┤1OE     VCC├─ 14 ──── +5 V
    GPIO 13 ──── 2 ─┤1A      4OE├─ 13 ──── +5 V
   R1 → DIN ──── 3 ─┤1Y       4A├─ 12 ──── GND
       +5 V ──── 4 ─┤2OE      4Y├─ 11 ──── sin conectar
        GND ──── 5 ─┤2A      3OE├─ 10 ──── +5 V
sin conectar ──  6 ─┤2Y       3A├─  9 ──── GND
        GND ──── 7 ─┤GND      3Y├─  8 ──── sin conectar
                    └───────────┘
                   U1 · 74AHCT125
```

- **Solo se usa el búfer 1**: entra por la patilla 2, sale por la 3.
- La **patilla 1 va a GND** porque el permiso de salida es **activo a nivel bajo**. Si
  compras un **74AHCT126** por error, es idéntico salvo que el permiso es activo a nivel
  alto y esa patilla iría a +5 V.
- Los tres búferes sobrantes se **deshabilitan** (4, 10 y 13 a +5 V) y sus entradas se
  **atan a masa** (5, 9 y 12). Una entrada CMOS al aire capta ruido y consume de más.
- **C4** (100 nF) va entre las patillas **14 y 7**, pegado al integrado.

### Por qué U1 se alimenta a 5 V y no a 3,3 V

La salida de un búfer sube hasta *su propia* tensión de alimentación. Aliméntalo a 5 V y
convierte los 3,3 V que le entran del ESP32 en 5 V limpios a la salida, que es
exactamente lo que la tira necesita ver. Alimentado a 3,3 V no hace nada útil.

---

## Lista de cables

Cada fila es un cable físico.

| # | Desde | Hasta | Cable |
|---|---|---|---|
| 1-3 | — | — | *No hay cableado de red: la fuente trae enchufe y cable de fábrica* |
| 4 | Latiguillo, hilo **rojo** | Conector rápido **rojo** | ya viene en el latiguillo |
| 5 | Latiguillo, hilo **negro** | Conector rápido **negro** | ya viene en el latiguillo |
| 6 | Conector rojo | Latiguillo USB-C **macho**, hilo rojo | ya viene en el latiguillo |
| 7 | Conector negro | El mismo latiguillo, hilo negro | ya viene en el latiguillo |
| 7 bis | Ese latiguillo | Puerto **USB-C del ESP32** | enchufado, no soldado |
| 8 | Conector rojo | Placa aux. — U1 pat. 14, C2, C3 | 0,5 mm² rojo |
| 9 | Conector negro | Placa aux. — U1 pat. 7, C2, C3 | 0,5 mm² negro |
| 10 | ESP32 `GPIO 13` | U1 patilla 2 | 0,25 mm², lo más corto posible |
| 11 | U1 patilla 3 | R1, y de R1 a `J1 · DIN` | 0,25 mm² apantallado; pantalla a GND **solo** en este extremo |
| 12 | ESP32 `GPIO 27` | S1, un polo | Par trenzado 0,25 mm² |
| 13 | S1, otro polo | Conector negro | el otro hilo del mismo par |
| 14 | Conector rojo | `J1 · +5 V`, y C1 a J1 | 1,5 mm² rojo |
| 15 | Conector negro | `J1 · GND` | 1,5 mm² negro |
| 16 | J1 (3 vías) | Tira, extremo **DIN** | +5 V, GND y datos |
| 17 | Conector rojo | Tira, **extremo final** +5 V | 1,0 mm² rojo — inyección |
| 18 | Conector negro | Tira, **extremo final** GND | 1,0 mm² negro — inyección |

Fíjate en que **los cables 14 a 18 salen todos de los mismos dos conectores rápidos**, no
encadenados unos de otros. Eso es lo que significa «alimentar en estrella»: si encadenas
la inyección del extremo final desde el principio de la tira, no estás inyectando nada,
solo has puesto el mismo cable dos veces.

---

## Orden de montaje

1. Monta la **placa auxiliar** en el banco, fuera de la caja: U1 en su zócalo, R1, C2,
   C3, C4 y las ataduras de las patillas sobrantes. Repásala con el polímetro antes de
   instalarla.
2. Enchufa la fuente con **solo el latiguillo** conectado y mide entre rojo y negro:
   **~5,1 V** o no sigas.
3. **Desenchufa.** Monta los dos conectores rápidos (cables 4–5). Comprueba que entre uno
   y otro **no hay continuidad**.
4. Cuelga la **placa auxiliar** de los conectores (8–9) y monta el latiguillo macho
   (6–7). Enchufa ese latiguillo al **puerto USB-C del ESP32**: la placa debe arrancar.
   Mide **5 V entre las patillas 14 y 7 de U1**.
5. Desenchufa. Cablea las **señales** (10–13) y el conmutador. Con el firmware grabado, la
   consola serie debe reaccionar al accionar S1 aunque no haya tira conectada.
6. Desenchufa. Monta **C1**, la regleta **J1** y la **tira** (14–16), respetando el
   extremo DIN.
7. Desenchufa. Añade las dos **inyecciones** del extremo final (17–18). Como la tira es
   perimetral, ese extremo te queda al lado del principio. Ya puedes pasar a la puesta en
   marcha del doc 01 §8.

---

## Los cuatro errores que más se repiten

| Error | Qué pasa |
|---|---|
| **Masa suelta** | Alimentar el ESP32 por su USB desde un portátil y la tira desde la fuente, sin unir las masas. La tira hace cosas raras y nadie entiende por qué. Si programas con el USB conectado, une igualmente el GND del ESP32 a la regleta |
| **Tira del revés** | Entrar por `DO` en vez de por `DIN`. La tira lleva flechas serigrafiadas: los datos van en el sentido de la flecha. Al revés no enciende ni un LED y parece una avería grave |
| **Inyección falsa** | Llevar la inyección del extremo final desde el principio de la tira en vez de desde la regleta. No corrige nada, porque la caída ya se ha producido en ese mismo cable |
| **Pantalla doble** | Conectar la malla del cable de datos por los dos extremos. Crea un bucle de masa que mete ruido justo en la señal que querías proteger. Solo en el lado del ESP32 |
