# 07 — Cableado paso a paso

El esquema del doc 01 dice **qué está conectado con qué**. Esto dice **qué hacer con las
manos, y en qué orden**.

---

## La idea que lo desbloquea todo

Las dos líneas horizontales del esquema no son una abstracción: son **dos puntos de
reparto de verdad**. Uno lleva el +5 V; el otro, la masa.

En este proyecto la fuente es un **bloque sellado con conector de barril**, así que esos
dos puntos se materializan con un **adaptador de barril a tornillos** (5,5 × 2,1 mm) y
**dos conectores rápidos de palanca** tipo WAGO: uno para todos los rojos y otro para
todos los negros. El fusible de 5 A va a la salida del conector rojo.

> **Lo primero de todo:** mide con el polímetro cuál de los dos tornillos del adaptador
> es el positivo, con la fuente enchufada y nada más conectado. Confundirlos destruye el
> ESP32 y la tira en el acto. Márcalo con rotulador.

**Todo lo demás cuelga de esas dos regletas**: el ESP32, el adaptador de nivel, los
condensadores, la tira. En cuanto las montas, el resto del cableado deja de tener
misterio — cada cosa va de un punto a una de las dos regletas, y nada se encadena con
nada.

---

## Qué hay físicamente dentro de la caja

Cuatro cosas, y ninguna toca a las demás salvo por las regletas:

1. **La fuente**, con los bornes de red a un lado y los de 5 V al otro.
2. **Las dos regletas** (+5 V y GND), con el portafusibles **F1** intercalado entre el
   borne `+V` de la fuente y la regleta de +5 V.
3. **Una placa auxiliar** de perfboard con **U1**, **R1** y los condensadores **C2**,
   **C3** y **C4**. Una plaquita de 5 × 5 cm sobra.
4. **El ESP32**, sobre zócalos de tira para poder sacarlo sin desoldar nada.

**C1** (1000 µF) es la excepción: va en el otro extremo, junto a la regleta de salida
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
| 1 | Red — fase | Fuente, borne `L` | Manguera 3 × 1,0 mm² con tierra |
| 2 | Red — neutro | Fuente, borne `N` | de la misma manguera |
| 3 | Red — tierra | Fuente, borne `⏚` | de la misma manguera. **No es opcional** |
| 4 | Fuente `+V` | Entrada de **F1** | 1,5 mm² rojo |
| 5 | Salida de **F1** | Regleta **+5 V** | 1,5 mm² rojo |
| 6 | Fuente `−V` | Regleta **GND** | 1,5 mm² negro |
| 7 | Regleta +5 V | ESP32 pin `5V` (o `VIN`) | 0,5 mm² rojo |
| 8 | Regleta GND | ESP32 pin `GND` | 0,5 mm² negro |
| 9 | Regleta +5 V | Placa aux. — U1 pat. 14, C2, C3 | 0,5 mm² rojo |
| 10 | Regleta GND | Placa aux. — U1 pat. 7, C2, C3 | 0,5 mm² negro |
| 11 | ESP32 `GPIO 13` | U1 patilla 2 | 0,25 mm², lo más corto posible |
| 12 | U1 patilla 3 | R1, y de R1 a `J1 · DIN` | 0,25 mm² apantallado; pantalla a GND **solo** en este extremo |
| 13 | ESP32 `GPIO 27` | S1, un polo | Par trenzado 0,25 mm² |
| 14 | S1, otro polo | Regleta GND | el otro hilo del mismo par |
| 15 | Regleta +5 V | `J1 · +5 V`, y C1 a J1 | 1,5 mm² rojo |
| 16 | Regleta GND | `J1 · GND` | 1,5 mm² negro |
| 17 | J1 (3 vías) | Tira, extremo **DIN** | +5 V, GND y datos |
| 18 | Regleta +5 V | Tira, **extremo final** +5 V | 1,0 mm² rojo — inyección |
| 19 | Regleta GND | Tira, **extremo final** GND | 1,0 mm² negro — inyección |

Fíjate en que **los cables 15 a 19 salen todos de las mismas dos regletas**, no
encadenados unos de otros. Eso es lo que significa «alimentar en estrella»: si encadenas
la inyección del extremo final desde el principio de la tira, no estás inyectando nada,
solo has puesto el mismo cable dos veces.

---

## Orden de montaje

1. Monta la **placa auxiliar** en el banco, fuera de la caja: U1 en su zócalo, R1, C2,
   C3, C4 y las ataduras de las patillas sobrantes. Repásala con el polímetro antes de
   instalarla.
2. Cablea **la red a la fuente** (cables 1–3) y cierra esa parte. Da tensión y ajusta la
   fuente a **5,00 V** en sus propios bornes.
3. **Desconecta la red.** Monta F1 y las dos regletas (4–6). Comprueba que entre las dos
   regletas **no hay continuidad**.
4. Cuelga el **ESP32** y la **placa auxiliar** de las regletas (7–10). Da tensión: el
   ESP32 debe arrancar. Mide **5 V entre las patillas 14 y 7 de U1**.
5. Desconecta. Cablea las **señales** (11–14) y el conmutador. Con el firmware grabado, la
   consola serie debe reaccionar al accionar S1 aunque no haya tira conectada.
6. Desconecta. Monta **C1**, la regleta **J1** y la **tira** (15–17), respetando el
   extremo DIN.
7. Desconecta. Añade las dos **inyecciones** del extremo final (18–19). Ya puedes pasar a
   la puesta en marcha del doc 01 §8.

---

## Los cuatro errores que más se repiten

| Error | Qué pasa |
|---|---|
| **Masa suelta** | Alimentar el ESP32 por USB desde un portátil y la tira desde la fuente, sin unir las masas. La tira hace cosas raras y nadie entiende por qué. Si programas con el USB conectado, une igualmente el GND del ESP32 a la regleta |
| **Tira del revés** | Entrar por `DO` en vez de por `DIN`. La tira lleva flechas serigrafiadas: los datos van en el sentido de la flecha. Al revés no enciende ni un LED y parece una avería grave |
| **Inyección falsa** | Llevar la inyección del extremo final desde el principio de la tira en vez de desde la regleta. No corrige nada, porque la caída ya se ha producido en ese mismo cable |
| **Pantalla doble** | Conectar la malla del cable de datos por los dos extremos. Crea un bucle de masa que mete ruido justo en la señal que querías proteger. Solo en el lado del ESP32 |
