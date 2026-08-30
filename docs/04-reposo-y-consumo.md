# 04 — Poner el ESP32 en reposo hasta recibir señal

Sí, se puede: el ESP32 puede quedarse dormido con la obra apagada y despertar cuando
el conmutador cierra el contacto. Está implementado en `config.h`:

```c
#define MODO_REPOSO   0   // 0 = sin reposo · 1 = light sleep · 2 = deep sleep
```

Antes de activarlo conviene entender qué se gana de verdad, porque el resultado
depende mucho más de la placa que del modo de reposo que elijas.

---

## Los tres modos

### `MODO_REPOSO 0` — sin reposo (por defecto)

El ESP32 sigue ejecutando el bucle con la obra apagada, simplemente sin refrescar la
tira. Respuesta instantánea, consola serie siempre disponible, cero complicaciones.

### `MODO_REPOSO 1` — light sleep

Al terminar el fundido de salida el chip se duerme y **reanuda la ejecución en la
línea siguiente** cuando el conmutador cambia de nivel. No hay rearranque: la máquina
de estados, la RAM y el reloj de `millis()` sobreviven intactos. El despertar es
inmediato, por debajo del milisegundo.

> **Verifícalo en tu placa antes de darlo por bueno.** El light sleep del ESP32 puede
> dejar en estado indefinido la configuración del periférico **RMT**, que es el que
> FastLED usa para generar la señal de los WS2812B, y FastLED no expone una forma
> limpia de reinicializarlo. Si tras el primer ciclo de reposo la tira no responde o
> muestra colores basura, pasa a `MODO_REPOSO 2`. No he podido comprobarlo aquí: es
> una prueba de dos minutos con la placa delante.

### `MODO_REPOSO 2` — deep sleep (la opción fiable)

El chip se apaga casi por completo y despierta por **EXT0** en el pin del conmutador.
Al despertar **la placa rearranca desde `setup()`**, lo cual es precisamente lo que la
hace robusta: todos los periféricos, RMT incluido, quedan reinicializados desde cero.

Coste: **0,3-0,5 s** entre accionar el conmutador y el comienzo del fundido de entrada.
Con `MODO_REPOSO 2` puedes bajar `ESPERA_ARRANQUE_MS` a ~150, porque al despertar la
obra arranca siempre desde negro y no hay pico de corriente que amortiguar.

Requisito: `PIN_CONMUTADOR` debe ser un GPIO del dominio RTC (0, 2, 4, 12-15, 25-27,
32-39). El **27** por defecto lo es. Si pones otro que no lo sea, el firmware **no
compila** y te avisa con un `#error`, en vez de fallar silenciosamente en obra.

El fundido de salida **no se pierde en ningún modo**: el reposo se activa cuando el
fundido ya ha terminado y la tira está en negro, más un margen de `RETARDO_REPOSO_MS`
(2 s por defecto).

---

## Cuánto se ahorra realmente

Aquí está el matiz importante. Órdenes de magnitud típicos, referidos al raíl de 5 V:

| Elemento | Activo | Light sleep | Deep sleep |
|---|---|---|---|
| Chip ESP32 solo (radio apagada) | ~40 mA | ~1 mA | ~0,01 mA |
| **Placa DevKit completa** | ~60-80 mA | **~15-25 mA** | **~8-25 mA** |
| 100 WS2812B en reposo (apagados pero alimentados) | ~100 mA | ~100 mA | ~100 mA |

**En un DevKit, el deep sleep NO te lleva a microamperios.** El suelo de consumo lo
ponen tres cosas que siguen alimentadas aunque el ESP32 duerma:

1. El **chip USB-serie** (CP2102 o CH340), que consume aunque no haya nada conectado.
2. La **corriente de reposo del regulador AMS1117** de la placa.
3. El **LED rojo de alimentación**, soldado con su resistencia, siempre encendido.

Para llegar de verdad a los ~10 µA hay que usar un **módulo ESP32-WROOM-32 desnudo**
con tu propio regulador de bajo reposo, o desoldar el LED de la placa. Con la
instalación colgada de la red, eso no compensa el trabajo.

### Qué medida ahorra más

Ordenado por lo que realmente aporta, con la obra apagada:

| Medida | Ahorro |
|---|---|
| **Cortar el raíl de la tira** (`USAR_CORTE_ALIMENTACION 1`, doc 02 opción C) | **−100 mA** |
| Dormir el ESP32, en placa DevKit | −40 a −65 mA |
| Dormir el ESP32, en módulo WROOM desnudo | −80 mA |

Lo que más pesa en el reposo **no es el ESP32: son los 100 WS2812B**. Cada LED tiene
dentro un controlador que sigue trabajando aunque el LED se vea negro, y 100 × 1 mA son
100 mA. Si te importa el consumo en reposo, lo primero es el corte del raíl, no el sleep.

### La cuenta completa

| Configuración | Reposo |
|---|---|
| Actual (sin reposo, sin corte) | ~180 mA · **0,9 W** |
| Deep sleep, DevKit, sin corte de raíl | ~120 mA · 0,6 W |
| Deep sleep + corte de raíl, DevKit | ~15-25 mA · **0,1 W** |
| Deep sleep + corte de raíl, módulo desnudo | ~1 mA · 0,005 W |

Con la obra encendida 8 h al día, las 16 h restantes en reposo suponen unos
**5 kWh al año** en la configuración actual, frente a **0,6 kWh** con deep sleep y corte
de raíl. Menos de un euro de diferencia al año.

**Conclusión honesta: si la instalación va enchufada a la red, el reposo no se
justifica por la factura.** Se justifica por estas otras razones:

- **Alimentación por batería o panel solar.** Ahí es decisivo, no opcional.
- **Menos calor** en la caja de conexiones, si está poco ventilada o empotrada.
- **Los WS2812B no envejecen** mientras la obra está apagada (esto lo da el corte del
  raíl, no el sleep).
- Porque prefieres que la instalación esté genuinamente dormida y no simplemente a oscuras.

---

## Configuración recomendada si decides activarlo

```c
#define MODO_REPOSO             2      // deep sleep: rearranque limpio
#define USAR_CORTE_ALIMENTACION 1      // relé/MOSFET de lado alto en el +5 V
#define ESPERA_ARRANQUE_MS      150    // recorta el retardo al despertar
#define RETARDO_REPOSO_MS       2000
```

Con el corte de raíl activo, el firmware **retiene el nivel del pin del relé durante el
deep sleep** (`gpio_hold_en` + `gpio_deep_sleep_hold_en`) y lo libera al rearrancar. Sin
esa retención el pin quedaría flotante mientras el chip duerme y el relé haría lo que
quisiera. Aun así, **pon una resistencia de 10 kΩ entre la entrada del relé y GND** como
seguro: es un céntimo y cubre el arranque en frío, antes de que `setup()` llegue a
configurar el pin.

Si activas deep sleep con `USAR_CORTE_ALIMENTACION`, `PIN_ALIM_LEDS` también debe ser un
GPIO RTC de salida (0, 2, 4, 12-15, 25-27, 32-33). El **26** por defecto lo es, y de
nuevo el firmware no compila si eliges otro.

---

## Lo que hay que tener en cuenta

| Punto | Detalle |
|---|---|
| **Retardo al despertar** | Deep sleep: 0,3-0,5 s antes de que empiece el fundido. Light sleep: imperceptible |
| **Consola serie** | Dormido no hay diagnóstico. Para depurar, `MODO_REPOSO 0` temporalmente |
| **Grabar firmware** | El autorreset por DTR/RTS del conversor USB sigue funcionando aunque la placa duerma. Si algún intento falla, mantén pulsado BOOT al iniciar la subida. Los 2 s de `RETARDO_REPOSO_MS` también dan margen |
| **Despertares espurios** | Un cable largo hasta el conmutador puede captar ruido y despertar la placa. No enciende la obra: al releer el conmutador con antirrebote no hay orden de encendido y vuelve sola al reposo tras `RETARDO_REPOSO_MS`. Aun así, monta el pull-up externo de 10 kΩ y el condensador de 100 nF del doc 01 §5 |
| **Pulsador momentáneo** | `MODO_CONMUTADOR 1` funciona con deep sleep: como solo se duerme estando apagada, cualquier pulsación significa "enciende". El firmware no se duerme si el pulsador sigue apretado, para no despertar en el acto |
| **Ampliaciones futuras** | Si algún día añades horario por RTC, sensores o red, el deep sleep complica las cosas: el chip no está ejecutando nada. Con `MODO_REPOSO 0` esas ampliaciones son triviales |
| **Ciclos de arranque** | Cada despertar de deep sleep es un arranque en frío. Con unas pocas maniobras al día es irrelevante; no lo es si la obra se enciende y apaga decenas de veces |

---

## Si lo que buscas es consumo cero de verdad

Ningún modo de reposo llega a 0 W: el ESP32 tiene que seguir alimentado para poder
despertar. Solo hay dos caminos a 0 W reales:

- **Conmutador cortando la red** (topología B del doc 01 §5). Consumo nulo, pero
  **pierdes el fundido de salida**: la placa muere en el instante en que la apagas.
- **Autoapagado con relé biestable o enclavamiento por MOSFET**: el conmutador da
  tensión, el ESP32 se auto-mantiene alimentado mediante un enclavamiento, y al terminar
  el fundido de salida **se corta a sí mismo la alimentación**. Es la única forma de
  tener 0 W *y* fundido de salida.

Lo segundo funciona bien, pero exige circuitería adicional (un enclavamiento con MOSFET
de canal P o un relé biestable con dos bobinas) y hay que diseñarlo con cuidado para que
el arranque en frío sea fiable. No está implementado en este proyecto. Si la instalación
va a red, no merece la pena; si va a batería, es la opción correcta y merece la pena
plantearla desde el principio.
