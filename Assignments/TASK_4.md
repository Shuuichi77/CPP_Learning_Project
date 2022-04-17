## Templates

### Objectif 1 - Devant ou derrière ?

La fonction `Aircraft::add_waypoint` permet de rajouter une étape au début ou à la fin du parcours de l'avion. Pour
distinguer ces deux cas, elle prend un argument booléen `front` (on parle alors de "flag") qui est évalué à l'exécution.
Votre objectif consistera à modifier cette fonction afin d'économiser cette évaluation.

1. Aujourd'hui, cette fonction n'est pas utilisée dans le code (oups). Du coup, pour être sûr que vos futurs changements
   fonctionneront correctement, commencez par modifier le code de la classe `Aircraft` de manière à remplacer :

```cpp
waypoints = control.get_instructions(*this);
```

par

```cpp
auto front = false;
for (const auto& wp: control.get_instructions(*this))
{
   add_waypoint(wp, front);
}
```

- cf. `aircraft.cpp`

2. Modifiez `Aircraft::add_waypoint` afin que l'évaluation du flag ait lieu à la compilation et non à l'exécution. Que
   devez-vous changer dans l'appel de la fonction pour que le programme compile ?

- On enlève le paramètre `const bool front` pour le passer en template  `template<const bool Front>`.\
  Puis dans le `if (front)`, on rajoute `constexpr` après le `if` pour que notre compilateur ne conserve que la branche
  qui sera évalué à `true`.

3. **BONUS** En utilisant [GodBolt](https://godbolt.org/), comparez le code-assembleur généré par les fonctions
   suivantes:

<table border="0">
 <tr>
    <td><pre lang="c++">
    int minmax(const int x, const int y, const bool min) {
        return x &lt; y ? (min ? x : y) : (min ? y : x);
    }
    </pre></td>
    <td><pre lang="c++">
    template&lt;bool min&gt;
    int minmax(const int x, const int y){
        return x &lt; y ? (min ? x : y) : (min ? y : x);
    }
    </pre></td>
 </tr>
</table>

### Objectif 2 - Points génériques

1. Reprenez les classes dans `geometry.hpp` et inspirez-vous de `Point2D` et `Point3D` pour définir une unique
   classe-template `Point` paramétrée par la dimension (nombre de coordonnées) et leur type (entier/float/double). Pour
   ce qui est des constructeurs, vous n'ajouterez pour le moment que le constructeur par défaut.

- On veut que notre classe-template soit paramétrée par la dimension et son type : on écrit
  donc `template<typename ElementType, const size_t Size>`.\
  On déclare également l'attribut `std::array<ElementType, Size> values;`\
  Puis on déclare les différentes fonctions en s'inspirant surtout de `Point3D`:
    - `Point& operator+=(const Point& other)`
    - `Point& operator-=(const Point& other)`
    - `Point& operator*=(const Point& other)`
    - `Point& operator*=(const ElementType scalar)`
    - `Point operator+(const Point& other) const`
    - `Point operator-(const Point& other) const`
    - `Point operator*(const Point& other) const`
    - `Point operator*(const ElementType scalar) const`
    - `Point operator-() const` : c'est la seule fonction qui changera un peu par rapport aux implémentations
      de `Point2D` et `Point3D` : en effet, on va appliquer `std::transform` sur notre `values` qui va remplir sur un
      nouvel objet `new_point` tel que la lambda est la suivante `[](ElementType e) { return -e; }`. Puis on retourne
      notre `new_point`.
    - `ElementType length()`
    - `ElementType distance_to(const Point& other)`
    - `Point& normalize(const ElementType target_len = (ElementType) 1)`
    - `Point& cap_length(const ElementType max_len)`

2. Ajoutez une fonction libre `test_generic_points` à votre programme, que vous appelerez depuis le `main`. Placez le
   code suivant dans cette fonction et modifiez-le plusieurs fois, pour vérifier que le compilateur est capable de
   générer des classes à partir de votre template sans problème :

```cpp
Point<...> p1;
Point<...> p2;
auto p3 = p1 + p2;
p1 += p2;
p1 *= 3; // ou 3.f, ou 3.0 en fonction du type de Point
```

- Dans notre fonction, on va tester toutes les opérations suivantes :
    - +=
    - -=
    - *=
    - p1 + p2
    - p1 - p2
    - p1 * p2
    - -p1

3. Ajoutez le constructeur à 2 paramètres de `Point2D` et le constructeur à 3 paramètres de `Point3D` dans votre
   classe-template. Modifiez `Point2D` et `Point3D` afin d'en faire des alias sur des classes générées à partir du
   template `Point` (respectivement, 2 floats et 3 floats). Vérifiez que votre programme compile et fonctionne comme
   avant.

- Pour faire de `Point2D` et `Point3D` des alias, on écrit `using Point2D = Point<float, 2>`
  et `using Point3D = Point<float, 3>` et on ajoute leur constructeur `Point(ElementType x, ElementType y)`
  et `Point(ElementType x, ElementType y, ElementType z)`.x\
  On remarque à la compilation que dans `texture.hpp`, dans `GL::Texture2D`, on a besoin de `values` qui étaient
  anciennement `float values[2] {}`. Or, on a désormais un `std::array` : on va donc dans un premier temps créer un
  getter pour `values` et dans `glVertex2fv(vertex.values)`, on va plutôt
  écrire `glVertex2fv(vertex.get_values().data())`.\
  De cette façon, notre code compile bien.

4. Dans la fonction `test_generic_points`, essayez d'instancier un `Point2D` avec 3 arguments. Que se passe-t-il ?
   Comment pourriez-vous expliquer que cette erreur ne se produise que maintenant ?

- Si on essaye d'initialiser un `Point2D`, qui a donc un `std::array values` de taille 2, avec 3 arguments, alors on va
  utiliser le constructeur à 3 arguments. Cependant, on a trop d'éléments dans notre array : on ne peut donc pas
  compiler.

5. Que se passe-t-il maintenant si vous essayez d'instancier un `Point3D` avec 2 arguments ? Utilisez un `static_assert`
   afin de vous assurer que personne ne puisse initialiser un `Point3D` avec seulement deux éléments. Faites en de même
   dans les fonctions `y()` et `z()`, pour vérifier que l'on ne puisse pas les appeler sur des `Point` qui n'ont pas la
   dimension minimale requise.

- Si cette fois-ci on instancie un `Point3D` avec 2 arguments, alors on va utiliser le constructeur à 2 arguments et
  remplir les 2 premières cases de notre array avec ces 2 arguments : la 3ᵉ sera quant à lui initialiser automatiquement
  à 0 : pas de problème de taille d'array dans ce cas-ci.\
  On va mettre des `static_assert` dans nos constructeurs pour s'assurer que `Size` est bien de la bonne valeur (2 ou 3
  en fonction du nombre d'arguments de notre constructeur), puis on fait la même chose pour les fonctions `y()` et `z()`
  .

6. Plutôt qu'avoir un constructeur pour chaque cas possible (d'ailleurs, vous n'avez pas traité tous les cas possibles,
   juste 2D et 3D), vous allez utiliser un variadic-template et du perfect-forwarding pour transférer n'importe quel
   nombre d'arguments de n'importe quel type directement au constructeur de `values`.  
   Vous conserverez bien entendu le `static_assert` pour vérifier que le nombre d'arguments passés correspond bien à la
   dimension du `Point`.\
   En faisant ça, vous aurez peut-être désormais des problèmes avec la copie des `Point`. Que pouvez-vous faire pour
   supprimer l'ambiguité ?

- TODO

7. **BONUS** En utilisant SFINAE, faites en sorte que le template `Point` ne puisse être instancié qu'avec des
   types [arithmétiques](https://en.cppreference.com/w/cpp/types/is_arithmetic).
