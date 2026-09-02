# 06 — Montaje sobre cartulina y PVC de colores

La obra está construida con **cartulina y PVC transparente de colores**, no con vidrio
emplomado. Eso cambia tres cosas respecto a una vidriera real: cómo se disipa el calor,
dónde puede estar la fuente y —lo más importante para el resultado— **qué color acaba
saliendo**.

---

## 1. La fuente va fuera de la obra

**50 W de fuente conmutada y 230 V no entran en una estructura de cartulina.** La fuente,
el fusible y todo el cableado de red van en una caja aparte, ventilada y no combustible,
fuera de la pieza.

A la obra entra únicamente el cable de **5 V**, más el cable del conmutador. Los 5 V son
inofensivos al tacto y no pueden provocar un incendio por sí solos; el riesgo está
entero en el lado de red, así que se queda entero fuera.

Si la obra se mueve o se manipula (una exposición, un traslado), esto además convierte el
transporte en algo trivial: se desconecta un cable de baja tensión y ya está.

---

## 2. Disipación: aluminio entre la tira y la cartulina

La cartulina es un aislante térmico: no saca el calor, lo guarda. Y el **PVC flexible
empieza a reblandecerse en torno a los 60-70 °C**, mucho antes de lo que la gente supone.

La solución es sencilla y barata: **pega la tira sobre una pletina o un perfil de
aluminio, y el aluminio sobre la cartulina.** El aluminio hace dos cosas a la vez:

- Saca el calor del punto de contacto y lo reparte en toda su superficie, de modo que
  ningún punto de la cartulina o del PVC ve una temperatura alta.
- Baja la temperatura de unión de los LED, que es lo que determina su vida útil
  ([05-vida-util-de-los-leds.md](05-vida-util-de-los-leds.md)).

Un perfil de aluminio para tira LED con difusor opal resuelve disipación y difusión de
una sola vez, y es lo más limpio. Una pletina lisa de 20-25 mm también sirve.

**No enrolles la tira sobrante ni la dejes en su bobina**: concentra el calor en unos
pocos centímetros y es justo el escenario que hay que evitar con cartulina alrededor.

---

## 3. Difusión: los puntos se ven

A través de PVC transparente, cien LED se ven como **cien puntos**, no como luz. Es el
error más común en este tipo de piezas y se arregla con dos medidas combinadas:

- **Distancia:** entre **4 y 8 cm** entre la tira y el panel de PVC. Más cerca se ven los
  puntos; más lejos se pierde intensidad y definición.
- **Una hoja difusora** entre la tira y el PVC de color: papel vegetal, PVC opal o
  acetato satinado. Es lo que convierte los puntos en superficie.

Si el fondo interior de la caja es cartulina blanca, mejor: rebota la luz y ayuda a
homogeneizar. Una cartulina negra por dentro se come la luz.

---

## 4. El PVC de color resta, no suma

Esta es la parte que más va a cambiar el aspecto de la obra, y conviene entenderla antes
de montar nada.

**Un filtro de color no añade color: quita todo lo demás.** Un panel de PVC rojo solo
deja pasar longitudes de onda largas. Cuando el arcoíris le manda verde, ese panel no se
pone verde: **se apaga casi por completo**.

Con luz muy saturada detrás de filtros de colores, el resultado es que los paneles se van
encendiendo y apagando por turnos según por dónde pase el arcoíris. La obra **parpadea**
en vez de fundirse, que es justo lo contrario de lo que buscan los fundidos suaves.

### La solución está en un parámetro

```c
#define SATURACION  190   // en config.h
```

Una luz **menos saturada** lleva algo de todas las longitudes de onda. Así cada panel
puede transmitir la suya, ninguno se queda negro, y el arcoíris pasa a **modular el
conjunto** —desplazando el peso del color de unos paneles a otros— en vez de encender y
apagar trozos.

| `SATURACION` | Resultado con PVC de colores |
|---|---|
| 255 | Los paneles se encienden y apagan por turnos. Dramático, pero parpadea |
| 190 *(por defecto)* | Todos los paneles vivos, con el color rotando por encima |
| 150 | Brillo muy parejo, casi blanco cálido con un tinte que viaja |

No hay un valor correcto: depende de lo saturado que sea tu PVC y del efecto que
busques. **Pruébalo con la obra montada**, no antes: el mismo valor da resultados muy
distintos según los colores que hayas elegido.

### Y también se come la luz

Un filtro de color saturado puede dejar pasar **solo un 20-30 %** de la luz que le llega.
La obra se verá bastante más apagada que la tira desnuda, así que es probable que
necesites subir `BRILLO_MAXIMO` por encima de 150.

Eso va en contra de la vida útil de los LED. Si acabas subiéndolo, **compénsalo activando
`APAGADO_AUTOMATICO_H`**: recortar horas encendido es una palanca bastante más eficaz que
recortar brillo, y no cuesta nada visualmente.

---

## 5. Ventilación

Una caja de cartulina cerrada acumula calor y no lo suelta. Deja **aberturas arriba y
abajo**, aunque queden ocultas por la parte trasera: el aire frío entra por abajo y el
caliente sale por arriba sin necesidad de ventilador.

---

## Lista de comprobación del montaje

- [ ] Fuente, fusible y cableado de 230 V **fuera** de la obra, en caja ventilada.
- [ ] A la pieza solo entran 5 V y el cable del conmutador.
- [ ] Tira sobre aluminio; aluminio sobre la cartulina. Nada de tira pegada al cartón.
- [ ] Tira sobrante cortada por las marcas, no enrollada.
- [ ] 4-8 cm entre la tira y el PVC, con hoja difusora en medio.
- [ ] Aberturas de ventilación arriba y abajo.
- [ ] Tras 2 h encendida: aluminio, PVC y placa **tibios**, nunca calientes.
- [ ] `SATURACION` ajustado con la obra montada, no antes.
