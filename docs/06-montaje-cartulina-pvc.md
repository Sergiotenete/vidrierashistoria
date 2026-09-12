# 06 — Montaje sobre cartulina y PVC de colores

La obra está construida con **cartulina blanca y PVC transparente de colores**, no con
vidrio emplomado: el PVC no es opaco, es un **filtro de color**, y la cartulina blanca
hace de fondo. Eso cambia tres cosas respecto a una vidriera real: cómo se disipa el
calor, dónde puede estar la fuente y —lo más importante para el resultado— **qué color
acaba saliendo**.

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

A través de un filtro transparente, cien LED se ven como **cien puntos**, no como luz —
el filtro tiñe el color pero no difumina nada. Con un plástico opaco te lo perdonaría;
con el tuyo, no. Se arregla con dos medidas combinadas:

- **Distancia:** entre **4 y 8 cm** entre la tira y el panel de PVC. Más cerca se ven los
  puntos; más lejos se pierde intensidad y definición.
- **Una hoja difusora** entre la tira y el PVC de color: papel vegetal, PVC opal o
  acetato satinado. Es lo que convierte los puntos en superficie.

**Tu cartulina blanca ya trabaja a favor**: rebota la luz y ayuda a homogeneizar, además
de mezclar los colores (§4). Cuanto más la aproveches —fondo blanco, laterales blancos—,
menos se notarán los puntos y menos brillo necesitarás. Una cartulina negra por dentro se
comería la luz.

---

## 4. El color: un filtro resta, pero la cartulina blanca mezcla

Aquí hay dos efectos que tiran en sentidos contrarios, y el resultado depende de cómo
esté montada la obra.

### El efecto que quita: el filtro resta

**Un filtro de color no añade color: quita todo lo demás.** Un panel de PVC rojo solo
deja pasar longitudes de onda largas. Cuando el arcoíris le manda verde, ese panel no se
pone verde: **se apaga**.

Llevado al extremo, con luz muy saturada y cada panel iluminado por sus propios LED, los
paneles se encienden y se apagan por turnos según por dónde pase el arcoíris. La obra
**parpadea** en vez de fundirse, que es lo contrario de lo que buscan los fundidos.

### El efecto que salva: la cartulina blanca mezcla

Aquí juega a tu favor cómo está construida la pieza. La **cartulina blanca es un
reflector difuso excelente** —devuelve en torno al 80 % de la luz que recibe, repartida
en todas direcciones—, así que la luz de LED vecinos con tonos distintos rebota y **se
mezcla antes de llegar al filtro**.

Cuanta más mezcla haya, más se parece la luz que llega a cada panel a una luz blanca, y
más saturación aguanta la obra sin que ningún panel se quede negro.

### Entonces, ¿qué valor pongo?

**No se decide sobre el papel: se decide con la obra montada**, porque depende de la
geometría. Si cada panel tiene sus LED justo detrás y separados por tabiques, hay poca
mezcla. Si todos comparten una caja blanca, hay mucha.

`SATURACION` está en **190**, un punto intermedio. Enciéndela y mira:

| Lo que ves | Qué hacer |
|---|---|
| Los paneles se encienden y se apagan por turnos | **Baja** hacia 160 |
| Se ve lavado, casi blanco, el color no se aprecia | **Sube** hacia 230 |
| Todos los paneles vivos, con el color viajando por encima | Déjalo |

Es un cambio de una línea en `config.h` y treinta segundos de regrabar. No le des más
vueltas antes de verlo.

### Cuánta luz te come

Un **filtro transparente** —el tuyo— es mucho más generoso que un plástico opaco de
color: deja pasar aproximadamente entre un **30 y un 60 %** según el color, y los tonos
claros bastante más que los azules y rojos profundos. Contando además el rebote en la
cartulina, digamos que la obra se verá **en torno a la mitad de brillante** que la tira
desnuda.

Con `BRILLO_MAXIMO` en 128 eso debería bastar de sobra para una pieza vista de cerca en
interior. Si aun así se te queda corta, súbelo — pero antes prueba a **acercar la
cartulina blanca** o a mejorar la difusión, que es luz gratis y no cuesta vida útil.

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
- [ ] Fondo interior de cartulina **blanca**, no negra: rebota y mezcla la luz.
