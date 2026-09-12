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

## 3. Difusión: ya la tienes resuelta por construcción

La tira va **perimetral** y los LED **no se ven en ningún momento** de forma directa.
Eso, que parece un detalle de montaje, resuelve de un plumazo el error más común de estas
piezas: **no hay puntos que difuminar**, porque nadie ve los LED. La luz sale, rebota en
la cartulina blanca y llega al filtro ya repartida. La cartulina es el difusor.

No hace falta hoja difusora. Lo que sí conviene cuidar:

- **Todo el interior en blanco**: fondo y también los laterales. Cada superficie blanca
  es un rebote más, y cada rebote reparte mejor la luz. Una cara negra por dentro es luz
  tirada a la basura.
- **Profundidad de la caja.** Con la tira en el borde, la luz tiene que cruzar el hueco
  hasta el centro. Si la caja es muy plana, se nota el borde más brillante que el centro.
  Como regla práctica, deja una **profundidad de al menos 1/6 del lado corto** del panel.
- **Paneles no demasiado grandes.** Un panel muy ancho iluminado solo por su perímetro
  se apaga por el centro. Si te pasa, es cuestión de profundidad o de subdividir.
- **Apunta la tira hacia dentro o hacia el fondo**, nunca hacia el espectador aunque esté
  oculta: cuanto más rebote antes de salir, más pareja sale la luz.

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

Y con la tira **perimetral**, esa mezcla es muy fuerte: cada punto del panel recibe luz
de tramos de tira lejanos entre sí, con tonos muy distintos. Prácticamente tienes una
**caja mezcladora**.

Eso es bueno —ningún panel se va a quedar negro— pero tiene su propio riesgo, que es el
contrario: **que se mezcle tanto que salga casi blanca** y el color del arcoíris apenas se
aprecie.

### Entonces, ¿qué valor pongo?

Son **dos mandos sobre el mismo compromiso**, y en tu caso los dos apuntan en la misma
dirección:

| Ajuste | Qué controla |
|---|---|
| `SATURACION` | Cuánto blanco lleva de origen cada LED |
| `EXTENSION_ARCOIRIS` | Cuántos tonos distintos hay a la vez en la tira, que es cuánto se mezclan al rebotar |

Con tira perimetral la caja ya mezcla mucho por su cuenta, así que **la luz tiene que
salir saturada para sobrevivir al rebote**. Por eso `SATURACION` está en **220**, no en un
valor intermedio.

Enciéndela y mira:

| Lo que ves | Qué hacer |
|---|---|
| Se ve lavado, casi blanco, el color no se aprecia | **Sube** `SATURACION` hacia 240 |
| Sigue lavado con la saturación a tope | **Baja** `EXTENSION_ARCOIRIS` a 1, o a 0 |
| Los paneles se encienden y se apagan por turnos | **Baja** `SATURACION` hacia 180 |
| Todos los paneles vivos, con el color viajando por encima | Déjalo |

Con `EXTENSION_ARCOIRIS` a **0** toda la obra está del mismo color a la vez y va virando
despacio por el círculo cromático. Con una caja mezcladora es probablemente el ajuste que
mejor se vea: el color llega limpio al filtro en lugar de promediarse a blanco.

Es un cambio de una línea en `config.h` y treinta segundos de regrabar. No le des más
vueltas antes de verlo.

### Cuánta luz te come

Un **filtro transparente** —el tuyo— es mucho más generoso que un plástico opaco de
color: deja pasar aproximadamente entre un **30 y un 60 %** según el color, y los tonos
claros bastante más que los azules y rojos profundos. Contando además el rebote en la
cartulina, digamos que la obra se verá **en torno a la mitad de brillante** que la tira
desnuda.

Con la tira perimetral pierdes algo más, porque la luz hace uno o dos rebotes antes de
salir y la cartulina devuelve el 80 % en cada uno. Aun así, con `BRILLO_MAXIMO` en 128
debería bastar de sobra para una pieza vista de cerca en interior. Si se te queda corta,
antes de subir el brillo prueba a **blanquear más el interior** o a **reducir la
profundidad muerta**: es luz gratis y no cuesta vida útil.

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
- [ ] Interior todo blanco, incluidos los laterales.
- [ ] Profundidad suficiente para que el centro del panel no se vea más apagado que el borde.
- [ ] Aberturas de ventilación arriba y abajo.
- [ ] Tras 2 h encendida: aluminio, PVC y placa **tibios**, nunca calientes.
- [ ] `SATURACION` ajustado con la obra montada, no antes.
- [ ] Fondo interior de cartulina **blanca**, no negra: rebota y mezcla la luz.
