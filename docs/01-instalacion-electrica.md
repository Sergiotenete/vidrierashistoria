# 01 — Instalación eléctrica de la obra

Instalación de **una sola obra** iluminada con **100 LED WS2812B**, gobernada por un
**conmutador on/off**, alimentada con una **fuente de 5 V / 10 A**.

---

## 1. Presupuesto eléctrico

| Concepto | Cálculo | Resultado |
|---|---|---|
| Consumo teórico máximo (100 LED en blanco pleno) | 100 × 60 mA | **6,0 A** |
| Consumo real con arcoíris a brillo 180/255 | ~100 × 22 mA | **~2,2 A** |
| ESP32 (con radio apagada) | — | **~0,08 A** |
| Techo impuesto por el firmware (`MAX_MILIAMPERIOS`) | — | **6,0 A** |
| Fuente | 5 V × 10 A | **50 W** |

La fuente queda trabajando en el peor caso al **60 %** de su capacidad, y en régimen
normal por debajo del **25 %**. Eso es exactamente lo que se busca en una instalación
que va a estar encendida a diario: una fuente al 50-60 % funciona fría, y la vida de
sus condensadores electrolíticos se duplica aproximadamente por cada 10 °C menos de
temperatura.

> No subas `MAX_MILIAMPERIOS` por encima de 8000. Trabajar una fuente al 100 % de
> forma continua es la causa número uno de averías en este tipo de montajes.

---

## 2. Lista de materiales

| Cant. | Componente | Notas |
|---|---|---|
| 1 | ESP32 DevKit v1 (30 o 38 pines) | Cualquier placa con ESP32-WROOM sirve |
| 1 | Tira WS2812B, 100 LED | 60 LED/m → 1,67 m; 30 LED/m → 3,33 m |
| 1 | Fuente conmutada 5 V / 10 A | Con toma de tierra y bornes de tornillo |
| 1 | Conmutador basculante o de palanca, 1 circuito (SPST) | **De señal, no de red** — ver §5 |
| 1 | Resistencia 330 Ω, 1/4 W | En serie con la línea de datos |
| 1 | Condensador electrolítico 1000 µF / 16 V | En la entrada de la tira |
| 1 | Condensador cerámico 100 nF | Junto a la alimentación del ESP32 |
| 1 | Adaptador de nivel 74AHCT125 (o SN74HCT245) | Recomendado — ver §4 |
| 1 | Portafusibles + fusible 7,5 A | En el +5 V, antes de la tira |
| — | Cable 1,5 mm² (16 AWG) rojo/negro | Alimentación e inyección |
| — | Cable apantallado o par trenzado 0,25 mm² | Datos y conmutador |
| 1 | Perfil de aluminio con difusor opal | Disipa y homogeneiza la luz |
| 1 | Caja de conexiones ventilada | Aloja fuente, ESP32 y fusible |

---

## 3. Esquema de conexión

```
    RED 230 V                 FUENTE 5 V / 10 A
   L ───────────────────────► L
   N ───────────────────────► N
   PE ──────────────────────► ⏚  (tierra a la carcasa: obligatorio)

                        ┌── +5V ──┬──────────────┬───────────────┐
                        │         │              │               │
                        │      [FUSIBLE       [ESP32]         (inyección
                        │       7,5 A]         pin 5V/VIN       al final
                        │         │                             de la tira)
                        │         │                                │
                        │    ┌────┴───────────────┐                │
                        │    │  + 1000 µF/16 V    │                │
                        │    │  ──┬──             │                │
                        │        GND              │                │
                        │         │               │                │
                        │    ╔════╧═══════════════╧════════════════╧═══╗
                        │    ║        TIRA WS2812B — 100 LED           ║
                        │    ║  +5V   GND   DIN                        ║
                        │    ╚═══════════════╤═════════════════════════╝
                        │                    │
                        │            [74AHCT125]  3,3 V → 5 V
                        │                    │
                        │                 [330 Ω]
                        │                    │
                        └── GND ─────┬───────┴──── GPIO 13 (DATOS) ── ESP32
                                     │
                                     ├──────────── GND ───────────── ESP32
                                     │
                                     └──[ CONMUTADOR ]── GPIO 27 ─── ESP32
```

Reglas que no se negocian:

1. **GND común.** El negativo de la fuente, el GND del ESP32 y el GND de la tira
   deben estar unidos. Sin masa común la línea de datos no tiene referencia y la
   tira parpadea en colores aleatorios.
2. **Resistencia de 330 Ω en serie con DIN**, montada lo más cerca posible del
   ESP32 (o del adaptador de nivel). Amortigua reflexiones y protege el primer LED.
3. **Condensador de 1000 µF entre +5 V y GND en la entrada de la tira**, respetando
   la polaridad. Absorbe el pico de corriente del arranque.
4. **Fusible de 7,5 A** en el +5 V que va a la tira. La fuente da 10 A: sin fusible,
   un cortocircuito en la tira dispone de 10 A para hacer daño.
5. **Nunca alimentes la tira desde el pin 5V del ESP32.** Esa pista no aguanta ni 1 A.

---

## 4. Adaptador de nivel 3,3 V → 5 V

El WS2812B pide un "1" lógico de al menos 0,7 × VDD = **3,5 V**, y el ESP32 entrega
**3,3 V**. Suele funcionar, pero es un margen negativo: en cuanto el cable de datos
es largo o la temperatura sube, aparecen parpadeos o el primer LED se queda en blanco.

Para una instalación permanente, usa un **74AHCT125** (o SN74HCT245) alimentado a 5 V:
convierte los 3,3 V del ESP32 en 5 V limpios. Cuesta menos de un euro y elimina el
fallo intermitente más habitual de estos montajes.

Alternativas si no tienes el integrado a mano:
- Sacrificar el primer LED (se conecta y se ignora en el conteo), que actúa de repetidor.
- Bajar la alimentación **del primer LED** a ~4,4 V con un diodo 1N4007 en serie,
  reduciendo así su umbral. Solución de emergencia, no definitiva.

---

## 5. El conmutador: cómo debe ir cableado

**El conmutador NO corta la corriente. Es una señal que lee el ESP32.**

Esto es lo que hace posible el fundido de salida: si el conmutador cortase la red o el
+5 V, la placa moriría en el mismo instante en que la apagas y no habría fundido, solo
un corte seco. Con el conmutador de señal, al accionarlo el ESP32 sigue vivo, ejecuta
los 4 segundos de *fade out* y deja la tira en negro.

Cableado:

```
GPIO 27 ──────────[ CONMUTADOR ]────────── GND
```

- El GPIO usa la resistencia de *pull-up* interna del ESP32: **contacto cerrado = ON**.
- Si la tirada de cable hasta el conmutador supera 1,5-2 m, añade:
  - **par trenzado o cable apantallado** (la pantalla a GND solo en el lado ESP32),
  - una **resistencia de 10 kΩ de GPIO 27 a 3,3 V** (pull-up externo, más firme que el interno),
  - un **condensador de 100 nF** entre GPIO 27 y GND, junto a la placa.
- Si al montarlo la lógica queda al revés, pon `CONMUTADOR_INVERTIDO 1` en `config.h`.
- Si prefieres un **pulsador** en vez de un interruptor enclavado, pon
  `MODO_CONMUTADOR 1`: cada pulsación alterna encendido y apagado.

### Las tres topologías posibles

| | Topología | Fade in | Fade out | Consumo en reposo | Veredicto |
|---|---|---|---|---|---|
| **A** | Conmutador de señal a GPIO (**recomendada**) | Sí | **Sí** | ESP32 ~0,4 W + tira ~0,5 W | La del proyecto |
| **B** | Conmutador cortando la red o el +5 V general | Sí (al arrancar) | **No, imposible** | 0 W | Solo si te obligan a un corte total |
| **C** | A + relé/MOSFET de lado alto en el +5 V de la tira | Sí | **Sí** | ESP32 ~0,4 W | La más limpia si te molesta el reposo |

La topología **C** se activa con `USAR_CORTE_ALIMENTACION 1` en `config.h`: el firmware
espera a que termine el fundido de salida y solo entonces corta la tira.

> **Importante en la topología C:** el corte debe ser de **lado alto**, sobre el +5 V.
> Los módulos MOSFET baratos suelen ser de lado bajo (cortan el GND), y eso rompe la
> masa común: el pin de datos quedaría inyectando corriente a la tira sin alimentar.
> Usa un módulo de relé de 5 V o un módulo MOSFET de canal P de lado alto.

---

## 6. Inyección de corriente y secciones de cable

Con 100 LED, la caída de tensión a lo largo de la pista de cobre de la tira hace que el
final se vea más apagado y virado a rojo. Solución: **alimentar la tira por los dos extremos**.

- **Inyección obligatoria:** principio y final de la tira.
- **Inyección adicional:** si la tira es de 30 LED/m (3,3 m), añade un punto intermedio.
- Los cables de inyección salen **directamente de los bornes de la fuente**, no en
  cadena desde el punto anterior.
- Los datos entran **solo por un extremo** (DIN del primer LED). No se inyectan datos.

Secciones para tiradas de hasta 3 m:

| Tramo | Corriente | Sección mínima |
|---|---|---|
| Fuente → tira (principal) | 6 A | 1,5 mm² (16 AWG) |
| Cada inyección | 3 A | 1,0 mm² (18 AWG) |
| Datos y conmutador | mA | 0,25 mm² apantallado |

---

## 7. Montaje sobre la vidriera

1. **Perfil de aluminio con difusor opal** por detrás de la vidriera. Cumple dos
   funciones: disipa el calor de la tira (los WS2812B duran mucho más templados) y
   difumina los puntos de luz, que si no se transparentan a través del vidrio.
2. **Distancia al vidrio:** entre 3 y 6 cm. Más cerca se ven los puntos; más lejos se
   pierde saturación de color.
3. **No enrolles la tira sobrante** ni la dejes en su bobina: concentra el calor.
   Corta por las marcas de corte y sobra lo que no uses.
4. **Fija con el perfil, no solo con el adhesivo de la tira.** El adhesivo 3M cede con
   el calor y el paso de los meses, sobre todo en vertical.
5. **Humedad.** Si la obra está en un templo, un sótano o cerca de un muro exterior,
   usa tira **IP65** y monta la caja de conexiones fuera de la zona de condensación.
6. **Caja de conexiones ventilada.** Nunca hermética: la fuente necesita disipar sus
   50 W nominales. Rejillas arriba y abajo, y accesible para mantenimiento.

---

## 8. Puesta en marcha (lista de comprobación)

Antes de dar tensión, con la fuente desconectada de la red:

- [ ] Continuidad entre el GND de la fuente, el GND del ESP32 y el GND de la tira.
- [ ] Sin continuidad entre +5 V y GND (comprobar cortocircuitos).
- [ ] Polaridad del condensador de 1000 µF correcta (la banda es el negativo).
- [ ] Fusible de 7,5 A montado en el +5 V.
- [ ] Toma de tierra conectada a la carcasa de la fuente.
- [ ] La tira entra por su extremo **DIN**, no por DO (mira las flechas impresas).

Primer arranque:

1. Ajusta la fuente a **5,0-5,1 V** con el potenciómetro y un multímetro, sin carga.
2. Da tensión con el conmutador en **OFF**. La tira debe quedarse apagada.
3. Pasa el conmutador a **ON**: 3 s de fundido de entrada y arcoíris en movimiento.
4. Con la obra encendida, mide la tensión **al final de la tira**: si baja de 4,6 V,
   te falta inyección de corriente.
5. Vuelve a **OFF**: 4 s de fundido de salida hasta negro.
6. Déjala 2 horas encendida y toca la fuente, el perfil de aluminio y el ESP32.
   Deben estar tibios, nunca calientes al punto de no poder mantener el dedo.
