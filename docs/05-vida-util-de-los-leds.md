# 05 — Que los LED no sufran con el tiempo

Este documento va de longevidad, no de consumo. Ordena lo que de verdad envejece a un
WS2812B y qué hacer con cada cosa.

---

## Primero, una corrección: el reposo del ESP32 no es la palanca que buscas

Si el objetivo es que los LED duren, dormir el ESP32 **no aporta prácticamente nada**, y
conviene entender por qué antes de montar nada.

Un LED envejece por **depreciación lumínica**: el fósforo y el encapsulante de silicona
se degradan con la temperatura, y el die pierde eficiencia. Ese reloj **solo corre
mientras el LED está encendido y caliente**. Un WS2812B alimentado pero en negro no
emite luz: su controlador interno consume ~1 mA y disipa unos 5 mW. Los 100 juntos son
0,5 W repartidos en casi dos metros de tira, es decir, uno o dos grados sobre la
temperatura ambiente. **Eso no envejece nada.**

Y en cuanto a la placa, el deep sleep va incluso ligeramente en contra: cada despertar
es un **arranque en frío**, con su pico de corriente y su ciclo térmico. Una placa
encendida de forma continua y templada sufre menos que una que arranca en frío varias
veces al día.

Hay **un solo argumento real** a favor de cortar el raíl de la tira por salud de los
LED, y es de segundo orden: el marco conductor de los WS2812B está plateado, y en
ambiente húmedo la plata bajo tensión sufre **migración electroquímica** además de la
corrosión normal. Una tira sin tensión no tiene ese mecanismo activo. Si la obra está en
un templo, un sótano o un muro exterior, es una razón legítima para activar
`USAR_CORTE_ALIMENTACION`. Pero el sellado y el ambiente pesan mucho más que eso.

---

## Lo que envejece de verdad a un WS2812B, por orden

### 1. Temperatura de unión — con diferencia, lo primero

La depreciación lumínica sigue una ley de Arrhenius: como regla práctica, **cada ~10 °C
menos aproximadamente duplica la vida útil**. Todo lo demás de esta lista es secundario.

Lo que la determina:

- **El brillo.** Es la variable de la que dispones. La potencia disipada crece de forma
  casi lineal con el ajuste de brillo, pero el ojo responde a la raíz: es la asimetría
  que hay que explotar.
- **La disipación.** Una tira pegada a un perfil de aluminio puede trabajar 20-30 °C
  más fría que la misma tira pegada a madera, cartón-pluma o directamente al vidrio.
  Es la mejora más grande que puedes hacer con un componente pasivo.
- **La ventilación.** Una caja cerrada anula el aluminio.

**He bajado `BRILLO_MAXIMO` de 180 a 150.** La cuenta:

| | 180 | 150 |
|---|---|---|
| Potencia disipada | 100 % | **83 %** |
| Brillo percibido por el ojo | 100 % | **89 %** |

Un 11 % más tenue a cambio de un 17 % menos de calor. En una vidriera retroiluminada, la
diferencia visual es difícil de apreciar sin comparar las dos lado a lado; la diferencia
en temperatura de unión sí se nota en años de servicio. Si el espacio está muy iluminado
y necesitas más, sube a 180 sabiendo lo que cuesta.

### 2. Horas encendido acumuladas — la segunda palanca, y es enorme

El desgaste de un LED se cuenta en horas encendido. Una obra encendida 24 h al día
envejece **tres veces más rápido** que una encendida 8 h. Ningún ajuste de brillo
compensa eso.

El riesgo real de un conmutador manual no es el uso normal: es **el olvido**. Un
conmutador que se queda en ON un fin de semana largo son 65 horas de desgaste que nadie
quería gastar.

**He añadido `APAGADO_AUTOMATICO_H`**, desactivado por defecto:

```c
#define APAGADO_AUTOMATICO_H  8   // horas de encendido continuo; 0 = desactivado
```

Pasado ese tiempo la obra se apaga sola **con su fundido de salida normal**, y queda
enclavada: aunque el conmutador siga en ON, no vuelve a encenderse. Para encender de
nuevo hay que pasar el conmutador por OFF y volver a ON. Así un conmutador olvidado no
puede reencender la obra por su cuenta.

Lo he dejado en `0` porque cambia el contrato del conmutador que pediste y no quiero que
una obra se apague sola en mitad de una visita sin que lo hayas decidido tú. Dicho eso:
**si me preguntas qué activaría yo para que los LED duren, es esto, con 6 u 8 horas.**
Es la medida más eficaz de todo el documento.

### 3. Tensión de alimentación

El WS2812B tiene su máximo absoluto en torno a **6 V** y su rango recomendado llega a
unos 5,3 V. Hay una mala costumbre muy extendida: **subir la fuente a 5,3 o 5,5 V para
compensar la caída de tensión de la tira**. Eso somete a los 100 LED a sobretensión
permanente, y castiga sobre todo a los primeros, que son los que ven la tensión más alta.

**Ajusta la fuente a 5,00-5,05 V y corrige la caída con cobre, no con voltios**: inyección
por los dos extremos y sección suficiente (doc 01 §6). Compruébalo **con la obra
encendida**, no en vacío: muchas fuentes conmutadas dan de más sin carga.

### 4. Corrosión del marco plateado — el factor ambiental

El encapsulado de un WS2812B es plástico con el marco conductor **plateado**, y la plata
se ennegrece con compuestos de azufre y humedad. Cuando el reflector se oscurece, la
tira pierde luz y acaba fallando, sin que la culpa sea del die.

En el emplazamiento típico de una vidriera esto es más relevante de lo que parece:

- **Velas, incienso y humo** liberan compuestos de azufre. Un templo es un ambiente
  agresivo para la plata.
- **La silicona acética** (la barata, la que huele a vinagre) libera ácido acético al
  curar y corroe los contactos. Usa **silicona neutra** (oxímica o alcoxi), o mejor,
  ninguna en contacto con la tira.
- **Espumas, adhesivos y masillas baratas** dentro de una caja cerrada liberan gases que
  se quedan ahí. Ventila o no los uses.
- **Condensación** en muros exteriores y sótanos.

Medidas: tira **IP65** con funda de silicona si el ambiente es dudoso, nada de sellantes
acéticos, y caja ventilada en vez de hermética. Una caja hermética no protege de la
humedad: la encierra.

### 5. Ciclos térmicos

Encender 100 LED de golpe es un salto térmico brusco que, repetido miles de veces,
fatiga las soldaduras y el flexible de la tira. **Los fundidos de 3 y 4 segundos ya
resuelven esto**: la carga térmica entra y sale en rampa. Es una ventaja que ya tienes
por cómo está hecha la instalación.

### 6. Transitorios y la línea de datos

El primer LED de la tira es el que más muere, y casi siempre por lo mismo: recibir señal
de datos cuando la tira no está alimentada, o antes que la alimentación. La corriente
entra por los diodos de protección de la entrada.

Ya está cubierto: **330 Ω en serie** con la línea de datos, **1000 µF** en la entrada de
la tira, espera de estabilización al arrancar, y si activas el corte del raíl, el
firmware **deja el pin de datos en alta impedancia** mientras la tira está sin tensión.

---

## Lo que ya juega a favor en esta instalación

Merece la pena señalarlo, porque no es casual:

- **El arcoíris reparte el desgaste.** Cada LED pasa por todo el círculo cromático, así
  que los tres dies (rojo, verde y azul) acumulan horas de forma pareja. Una luz estática
  —un blanco cálido, un azul fijo— castiga siempre a los mismos dies y produce **deriva
  de color** con los años, porque el rojo AlInGaP y el azul InGaN no envejecen al mismo
  ritmo. Con un arcoíris en movimiento, ese problema prácticamente desaparece.
- **Nunca se muestra blanco pleno.** Un color saturado enciende uno o dos dies, no los
  tres. El consumo real es la mitad que en blanco, y el calor también.
- **El techo de potencia de FastLED** impide que ningún cambio futuro de efecto lleve la
  tira a plena carga por descuido.

---

## Resumen: qué hacer, por orden de impacto

| | Medida | Impacto en la vida de los LED |
|---|---|---|
| 1 | Perfil de aluminio + caja ventilada | **Muy alto** — decide la temperatura de unión |
| 2 | `APAGADO_AUTOMATICO_H 8` | **Muy alto** — recorta horas acumuladas |
| 3 | `BRILLO_MAXIMO 150` (hecho) | **Alto** — 17 % menos de calor |
| 4 | Fuente a 5,00 V exactos, medidos con carga | **Alto** — evita sobretensión permanente |
| 5 | Ambiente: IP65, silicona neutra, sin azufre | **Alto** en templos y sitios húmedos |
| 6 | Inyección por los dos extremos | Medio — tensión uniforme, sin extremos forzados |
| 7 | Fundidos de entrada y salida (hecho) | Medio — sin choques térmicos |
| 8 | `USAR_CORTE_ALIMENTACION 1` | **Bajo** — solo por migración electroquímica en ambiente húmedo |
| 9 | `MODO_REPOSO` (dormir el ESP32) | **Ninguno** para los LED. Para la placa, ligeramente negativo |

---

## Sobre la salud de la placa

Para el ESP32 el orden es distinto, y también es corto:

| Medida | Por qué |
|---|---|
| Alimentar por el pin `5V`, nunca por el micro-USB permanente | El USB es un conector de datos: se afloja y da microcortes |
| Desacoplo (100 nF + 220-470 µF) junto a la placa | Evita que los picos de la tira disparen el *brownout* |
| WiFi y Bluetooth apagados (hecho) | Menos consumo y varios grados menos |
| Caja ventilada | La fuente es lo que calienta, y calienta a todo lo que tiene al lado |
| Fuente al 50-60 % de carga (hecho) | Sus condensadores electrolíticos son lo primero que se seca |
| Watchdog (hecho) | No alarga la vida, pero evita desplazamientos por un cuelgue |
| **No** dormir ni reiniciar «por si acaso» | Cada arranque en frío es un ciclo térmico y un pico de corriente |

El chip en sí está especificado para funcionamiento continuo. Lo que se muere antes en
estas instalaciones es siempre lo mismo: los condensadores de la fuente, por calor.
