# 01 — Instalación eléctrica de la obra

Instalación de **una sola obra** iluminada con **100 LED WS2812B**, gobernada por un
**conmutador on/off**, alimentada con una **fuente de 5 V / 10 A**.

---

## 1. Presupuesto eléctrico

| Concepto | Cálculo | Resultado |
|---|---|---|
| Consumo teórico máximo (100 LED en blanco pleno) | 100 × 60 mA | **6,0 A** |
| Consumo real con arcoíris a brillo 128/255 | ~100 × 16 mA | **~1,7 A** |
| ESP32 (con radio apagada) | — | **~0,08 A** |
| Techo impuesto por el firmware (`MAX_MILIAMPERIOS`) | — | **2,2 A** |
| Carga sobre un conector USB-C de 3 A | 1,7 A / 3 A | **57 %** |
| Fuente | 5 V × 10 A | **50 W** |

La fuente queda trabajando en el peor caso al **60 %** de su capacidad, y en régimen
normal por debajo del **25 %**. Eso es exactamente lo que se busca en una instalación
que va a estar encendida a diario: una fuente al 50-60 % funciona fría, y la vida de
sus condensadores electrolíticos se duplica aproximadamente por cada 10 °C menos de
temperatura.

> No subas `MAX_MILIAMPERIOS` por encima de 8000. Trabajar una fuente al 100 % de
> forma continua es la causa número uno de averías en este tipo de montajes.

---

## 1 bis. La alimentación elegida

Tras descartar varias opciones, la instalación se alimenta así:

```
  Fuente GeeekPi 5 V / 5 A USB-C  (tipo Raspberry Pi 5, 25 W, cable fijo)
            │
            ▼
  Latiguillo hembra USB-C de 2 hilos   ← máximo 3 A: este es el techo real
            │  rojo (+5 V) · negro (GND)
            ▼
  Dos conectores rápidos de palanca (WAGO), uno para rojos y otro para negros
            │
            ├──► ESP32           (pin 5V / GND)
            ├──► tira, principio (+5 V / GND)
            ├──► tira, final     (inyección)
            └──► conmutador      (retorno a negro)
```

**El techo no lo pone la fuente, lo pone el latiguillo: 3 A.** Los 5 A del enchufe no
llegan a verse, porque un latiguillo pasivo negocia como mucho la corriente por defecto
de USB-C, y en cualquier caso el máximo estándar a 5 V son 3 A. Da igual: el consumo real
son ~1,7 A, un 57 % de carga.

Estas fuentes de Raspberry Pi entregan **5,1 V** en lugar de 5,0 para compensar la caída
del cable. Está dentro del rango que aguanta el WS2812B y juega a favor.

### Antes de montar nada: comprobar que el latiguillo despierta a la fuente

Un cargador USB-C **no saca tensión a ciegas**. Solo enciende la salida cuando detecta dos
resistencias de **5,1 kΩ** en los pines `CC` del aparato conectado. Muchos latiguillos las
llevan y muchos no, y la descripción del producto casi nunca lo aclara.

Enchufa la fuente, conecta solo el latiguillo y mide entre el hilo rojo y el negro:

| Medida | Qué significa |
|---|---|
| ~5,1 V | Lleva las resistencias. Adelante |
| 0 V | No las lleva. Al ser un cable moldeado no se pueden añadir: usa una plaquita de circuito impreso, o un cargador **USB-A** con cable A→C, que no negocia nada |

## 2. Lista de materiales

| Cant. | Componente | Notas |
|---|---|---|
| 1 | ESP32 DevKit v1 (30 o 38 pines) | Cualquier placa con ESP32-WROOM sirve |
| 1 | Tira WS2812B, 100 LED | 60 LED/m → 1,67 m; 30 LED/m → 3,33 m |
| 1 | Fuente GeeekPi 5 V / 5 A USB-C | Enchufe y cable ya montados. Entrega 5,1 V |
| 1 | Latiguillo hembra USB-C, 2 hilos | Máx. 3 A. **Comprobar que lleva las resistencias CC** |
| 2 | Conector rápido de palanca (WAGO 221) | 3 o 5 huecos. Uno para los rojos, otro para los negros |
| 1 | Conmutador basculante o de palanca, 1 circuito (SPST) | **De señal, no de red** — ver §5 |
| 1 | Resistencia 330 Ω, 1/4 W | En serie con la línea de datos |
| 1 | Condensador electrolítico 1000 µF / 16 V | En la entrada de la tira |
| 1 | Condensador cerámico 100 nF | Junto a la alimentación del ESP32 |
| 1 | Adaptador de nivel 74AHCT125 (o SN74HCT245) | Recomendado — ver §4. Alimentado a 5 V |
| 1 | Condensador cerámico 100 nF (C4) | Entre las patillas 14 y 7 de U1 |
| — | Cable 0,75-1,0 mm² rojo/negro | Alimentación e inyección (basta: son 1,7 A) |
| — | Cable apantallado o par trenzado 0,25 mm² | Datos y conmutador |
| 1 | Perfil o pletina de aluminio | Disipa; la tira va perimetral sobre él |
| 1 | Caja de conexiones ventilada | Aloja el ESP32 y los conectores rápidos |

---

> Para el cableado físico —lista de cables uno a uno, patillaje del 74AHCT125 y orden de
> montaje— ver [07-cableado-paso-a-paso.md](07-cableado-paso-a-paso.md).

## 3. Esquema de conexión

```
  FUENTE GeeekPi 5 V / 5 A USB-C
    │    enchufe y cable de fábrica: no hay nada de 230 V que cablear
    ▼
  LATIGUILLO HEMBRA USB-C                    máximo 3 A  ←  el techo real
    │
    ├── hilo rojo  ──►  CONECTOR RÁPIDO ROJO
    │                     ├──►  ESP32, pin 5V
    │                     ├──►  tira, principio   (+5 V)
    │                     └──►  tira, final       (+5 V, inyección)
    │
    └── hilo negro ──►  CONECTOR RÁPIDO NEGRO
                          ├──►  ESP32, pin GND
                          ├──►  tira, principio   (GND)
                          ├──►  tira, final       (GND, inyección)
                          └──►  conmutador, un polo


  SEÑALES

    ESP32  GPIO 13  ──►  [74AHCT125]  ──►  [330 Ω]  ──►  tira, DIN
    ESP32  GPIO 27  ──►  [CONMUTADOR]  ──►  conector rápido negro


  Y en la entrada de la tira, entre +5 V y GND:  condensador 1000 µF
  (la banda impresa es el negativo)
```

La tira es **perimetral**, así que su extremo final queda cerca del principio y el cable
de inyección sale corto.

Reglas que no se negocian:

1. **GND común.** El negativo de la fuente, el GND del ESP32 y el GND de la tira
   deben estar unidos. Sin masa común la línea de datos no tiene referencia y la
   tira parpadea en colores aleatorios.
2. **Resistencia de 330 Ω en serie con DIN**, montada lo más cerca posible del
   ESP32 (o del adaptador de nivel). Amortigua reflexiones y protege el primer LED.
3. **Condensador de 1000 µF entre +5 V y GND en la entrada de la tira**, respetando
   la polaridad. Absorbe el pico de corriente del arranque.
4. **Todo sale de los dos conectores rápidos**, nunca encadenado de un punto al
   siguiente. Si la inyección del final la llevas desde el principio de la tira, no
   estás inyectando nada.
5. **Nunca alimentes la tira desde el pin 5V del ESP32.** Esa pista no aguanta ni 1 A.
6. **Sin fusible.** Con la fuente de 10 A habría sido obligatorio; el latiguillo USB-C ya
   limita a 3 A y la fuente lleva protección propia.

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

## 7. Montaje sobre la obra

La obra está construida con **cartulina y PVC transparente de colores**, no con vidrio
emplomado. El montaje mecánico y térmico está en
[06-montaje-cartulina-pvc.md](06-montaje-cartulina-pvc.md); lo que no se negocia:

1. **La fuente va fuera de la obra.** 50 W de fuente conmutada y 230 V no entran en una
   estructura de cartulina. Caja aparte, ventilada y no combustible. A la pieza solo
   llega el cable de 5 V.
2. **La tira va sobre pletina o perfil de aluminio**, y el aluminio sobre la cartulina.
   La cartulina no disipa y el PVC flexible se reblandece a partir de unos 60-70 °C.
3. **Difusión obligatoria.** A través de PVC transparente los LED se ven como puntos.

## 8. Puesta en marcha (lista de comprobación)

Antes de dar tensión, con la fuente desconectada de la red:

- [ ] Continuidad entre el GND de la fuente, el GND del ESP32 y el GND de la tira.
- [ ] Sin continuidad entre +5 V y GND (comprobar cortocircuitos).
- [ ] Polaridad del condensador de 1000 µF correcta (la banda es el negativo).
- [ ] La tira entra por su extremo **DIN**, no por DO (mira las flechas impresas).

Primer arranque:

1. **Mide el latiguillo** con la fuente enchufada y nada más conectado, punta roja en el
   hilo rojo: **~5,1 V** confirma a la vez que lleva las resistencias `CC` y que los
   colores no están invertidos. Ver §1 bis. La fuente no lleva ajuste de tensión.
2. Da tensión con el conmutador en **OFF**. La tira debe quedarse apagada.
3. Pasa el conmutador a **ON**: 3 s de fundido de entrada y arcoíris en movimiento.
4. Con la obra encendida, mide la tensión **al final de la tira**: si baja de 4,6 V,
   te falta inyección de corriente.
5. Vuelve a **OFF**: 4 s de fundido de salida hasta negro.
6. Déjala 2 horas encendida y toca la fuente, el perfil de aluminio y el ESP32.
   Deben estar tibios, nunca calientes al punto de no poder mantener el dedo.
