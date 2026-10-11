# Manual Técnico Extendido - CinemaUSAC (Fase 2)

***

## 1. Introducción y Arquitectura General del Sistema

**CinemaUSAC (Fase 2)** constituye la refactorización arquitectónica mayor del cine departamental universitario. El propósito fundamental de esta fase es la erradicación del uso de memoria genérica abstracta basada en la Standard Template Library (STL), obligando al núcleo computacional a emplear exclusivamente **Tipos de Datos Abstractos (TDA) nativos y de construcción manual**. Las únicas excepciones permitidas en este estricto ecosistema son la recolección de punteros temporales en vectores de `std::vector` (exclusivamente para los procesos de retorno y visualización en Qt) y manipulaciones nativas de la clase `std::string`.

La arquitectura elegida se ampara en el concepto de **Monolito con Diseño Capas (N-Tier Architecture)** apoyado sobre el patrón Modelo-Vista-Controlador embebido dentro de la lógica del MOC de Qt.

### 1.1 Topología del Proyecto y Lógica de Construcción
La distribución granular atiende al paradigma de separación estricta:

* **`/include/models/`**: (Objetos Entidad "POD"). Definen las estructuras base de la información. Cuentan con sobrecarga de operadores de comparación y asignación para facilitar la limpieza semántica de las validaciones en niveles superiores.
* **`/include/structures/`**: (Capa Estructural de Negocios). Contiene los encabezados `.hpp` que albergan la lógica de manipulación de punteros `T*` y `T**`. Cada clase tiene control absoluto sobre el ciclo de vida de los Nodos alojados, utilizando la destructiva función recursiva para salvaguardar la memoria (`delete` en `postorden`).
* **`/include/services/`**: (Servicios Side-Effect). Engloba lecturas externas, mutaciones del I/O (lectura desde y escritura hacia Disco Duro). Comprende el parseo del formato `.json` a través de la librería `nlohmann`.
* **`/include/ui/`**: Archivos manejados por el compilador de Meta-objetos de Qt (MOC) para establecer la renderización de la ventana (Win32 / X11). Posee el control lógico de Interrupciones de Sistema (Señales y Slots).

***

## 2. Análisis Asintótico y Lógica de TDAs (Árboles y Grafos)

Esta sección explora detalladamente la complejidad del algoritmo y las decisiones de diseño del *Memory Management* tras bastidores.

### 2.1. Árbol Binario de Búsqueda de Películas (BST)
El **BST** (Binary Search Tree) funge como eje cardinal del almacén. Todos los elementos gráficos cuelgan de las llaves indexadas en él.

* **Key (Clave Primaria):** Código Alfanumérico.
* **Construcción y Comparación:** A nivel máquina las películas utilizan `std::string::compare` o los sobrecargados `<`, `>`. Todo nodo con valor menor a la raíz desciende al puntero izquierdo (`izquierdo`), y todo valor mayor asume el puntero derecho (`derecho`).
* **Complejidad y Desempeño Asintótico:**
  * **Time Complexity (Avg):** Búsqueda en $O(\log n)$, Inserción y Eliminación en $O(\log n)$.
  * **Time Complexity (Worst-Case):**  Debido a que este no es un árbol balanceable forzadamente (diferente al AVL), su peor caso es la degeneración linear convirtiéndolo en $O(n)$, algo improbable pero estadísticamente posible a raíz de cargas masivas JSON previamente ordenadas numéricamente.
  * **Space Complexity:** $O(n)$
* **Retorno de Memoria (Callbacks):** Se construyeron algoritmos de recorrido recursivos `PreOrden`, `InOrden` y `PostOrden`. Estas funciones transitan el puntero `raiz` pasando como argumento referencias explícitas a vectores `std::vector<NodoBST*>& resultado`. Se evita duplicar un copiado de objetos pesados garantizando solo la transferencia de los punteros hacia la tabla gráfica `QTableWidget`.

### 2.2. Árbol Adelson-Velsky y Landis de Matrices (AVL)
Dado el constante escrutinio y carga masiva al que son sometidas las *Funciones Cinematográficas* (Proyecciones), se exige la capacidad de auto-balance.

* **Ecuación Vectorial de Balanceo (Factor de Equilibrio, FE):**
  La salud del árbol tras cualquier inserción de la carga JSON depende del recalculo absoluto de las alturas. 
  $$ FE(N) = h(N.derecho) - h(N.izquierdo) $$
  Un Nodo balanceado arrojará $FE = \{-1,\ 0,\ 1\}$. Si la cota superara estos límites empíricos, el motor procede a pivotar el árbol para estabilizar la raíz más cercana al colapso.

* **Mecánica Matemática de Rotación (El Motor C++)**
  ```cpp
  NodoAVL* ArbolAVL::rotacionIzquierda(NodoAVL* x) {
      NodoAVL* y = x->derecho;
      NodoAVL* T2 = y->izquierdo;
      y->izquierdo = x;        // X desciende.
      x->derecho = T2;         // El huérfano adopta nueva rama.
      actualizarAltura(x);     // Compensación ascendente H+1.
      actualizarAltura(y);
      return y;                // Nueva raíz entregada.
  }
  ```
  De esta única primitiva geométrica el sistema desarrolla LL, RR, LR (Rotación Doble) y RL. La complejidad temporal (Time Complexity) final está sellada mecánicamente en $\Theta(\log n)$ logrando resistencia suprema a listas ordenadas, destruyendo la posibilidad de un árbol ramificado al 100%.

### 2.3. Árbol B (Orden 4) Indexador del Registro de Clientes
Diseño inspirado explícitamente en el funcionamiento estructural de Base de Datos Paginadas (RDBMS y SQLite). Su objetivo es minimizar lecturas aprovechando arquitecturas CPU Multi-Caché L1/L2.

* **Reglas del Orden $M=4$ (Cuatro Ramificaciones y Tres Celdas):**
  Un nodo (una "Página") consta de `claves[MAX_CLAVES_B]` ($MAX=3$) y de `hijos[MAX_HIJOS_B]` ($MAX=4$). Al ser el espacio corto, si una celda alcanza $3$ objetos y solicita una cuarta adición ocurre **Split (División Celular):**
  - La clave mediana es desprendida e impulsada recursivamente al NodoPadre. Las claves a su izquierda caen a un nuevo hijo izquierdo y las de su derecha conforman el hijo derecho. Esta fragmentación asincrónica consolida el árbol elevando las bases sin arrastrar las hojas hacia la superficie.
* **Separación de Interface y Almacenamiento (Lista Doble Sombra):**
  A pesar del increíble control iterativo, si los nodos sufrieran intercambios posicionales los objetos se arrastrarían; lo cual es destructivo.
  **La Innovación:** `EstructuraClientes` encapsula una lista doble normal. Cada que se inserta, el objeto material reside en este `NodoCliente` y a su vez lanza hacia la red indexadora (El Árbol B) únicamente **un puntero crudo (void pointer type abstraction)** para ser atrapado y referenciado como llave. Esto nos concede un motor rápido transcurriendo un Iterador Bidireccional en las UI Tablas.

### 2.4. Tabla Hash Uniforme de Reservaciones
Debido a que el control de ticket transcurre frenéticamente durante compras cruzadas en la taquilla, una búsqueda $O(\log n)$ es ineficiente. La solución en esta arquitectura es una Tabla Hashing Polinomial acotada por división.
* **Fórmula de Dispersión (Hashing String-to-Int):**
  Se emplea una variante del algoritmo Bernstein (djb2). Multiplicamos los arreglos de caracteres ASCII de cada ticket de reserva (Ej."R1_P3") y se desplazan sumando el siguiente Char y multiplicando por un Primo estipulado como el $31$. Finalmente dividimos a módulo del tamaño matricial de buckets estáticos.
* **Gestor de Colisiones (Closed Mapping/Separate Chaining):** 
  Al insertar un `COD` cuya matemática retorne el index 7 y éste se encontrara pre-ocupado, la celda instanciará su conector y arrastrará el objeto como un **Siguiente Nodo** (Cola LIFO/FIFO Simple `ListaReservas`), manteniendo íntegras las dos reservaciones y evitando rebalses por "Open Addressing" y sus bucles probados catastróficos.

### 2.5. Ciclo de Vida y Persistencia de la Matriz Dispersa Constante
A pesar de su naturaleza compleja entrelazando nodos en una cruz cuadriculada (Up, Down, Left, Right), el éxito de esta estructura reside su **Reciclaje Dinámico Constante.**
La estructura maneja Listas enlazadas para `CabecerasFilas` y `CabecerasColumnas`.
- **Problema de Memoria (Leaks):** Cargar mapas de 5x5 por cientos de funciones acarrearía un consumo RAM absurdo.
- **Solución Asíncrona:** Cuando el usuario escoge la película "Joker" de un combobox, el componente despacha un Trigger (Señal `onCurrentIndexChanged`). El sistema invoca al destructor de la Matriz `vaciar()` aniquilando implosionadamente del Heap de memoria toda columna, fila y celda. Subsiguiente se invoca desde *Disco* los bytes pertenecientes exclusivamente al archivo `F0XX.json`, reconstruyendo una matriz que abarca solo el espacio transaccional deseado y sin acumulación de memoria.

***

## 3. Inyección y Persistencia (Nlohmann's JSON Wrapper)
La capa mediadora I/O manipula la biblioteca `nlohmann`.
* **Deserialización y SegFault Safety:** Durante la macro carga el compilador de Qt invoca los Repaints o actualizaciones de los ComboBox de manera espontánea durante el proceso de adición a los Árboles. Esto crea colisiones del modelo hilo-seguro debido a un apuntador al infinito.
La lógica para controlarlo intercede:
  ```cpp
  comboBoxPeliculas->blockSignals(true);
  JSONReader::cargarPeliculasDesdeJSON(...); // Inserciones a BST y AVL.
  comboBoxPeliculas->blockSignals(false); 
  ```
  Esto bloquea la cascada recursiva previniendo Segmentation Faults C++ (0xC000005). Y garantiza una red estática durante los primeros 5 milisegundos de lecturas a disco masivas.

***

## 4. Ingeniería Gráfica en Reportes (Graphviz DOT Streams)
CinemaUSAC no almacena las imágenes; las recrea. Utiliza renderización al vuelo.
1. Se abre el flujo `ofstream dotFile` en la carpeta `bin/`.
2. Para arreglos complejos (TablaHash & Nodo B-tree) las primitivas poligonales no surten efecto, se despliega lenguaje de marca compatible integrado al render de Graphviz:
   ```dot
   node4 [shape=plaintext, label=<
      <TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0">
        <TR><TD> ID: 123 </TD></TR>
      </TABLE>>];
   ```
3. Se invoca una llamada de System Process subproceso hacia `dot -Tpng`. Las imágenes saltan a compresión Vectorizada e integradas tras el llamado del archivo directo al sistema visor QPixmap.

---
