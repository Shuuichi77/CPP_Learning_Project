# Algorithmes

## Objectif 1 - Refactorisation de l'existant

### A - Structured Bindings

`TowerSimulation::display_help()` est chargé de l'affichage des touches disponibles. Dans sa boucle,
remplacez `const auto& ks_pair` par un structured binding adapté.

Si vous ne savez plus ce qu'est un structured binding,
le [chapitre 6](https://laefy.github.io/CPP_Learning/chapter6/1-searches/) est votre ami (Google aussi d'ailleurs).

- On remplace simplement le `const auto& ks_pair` par `[key, _]` puisque seul la clé nous intéresse

### B - Algorithmes divers

1. `AircraftManager::move()` supprime les avions de la `move_queue` dès qu'ils sont "hors jeux". En pratique, il y a des
   opportunités pour des pièges ici. Pour les éviter, `<algorithm>` met à disposition la fonction `std::remove_if`.
   Remplacez votre boucle avec un appel à `std::remove_if`.

**Attention**: pour cela, il est nécessaire que `AircraftManager` stocke les avions dans un `std::vector` ou `std::list`
.

- On remplace notre boucle `for` par :

```cpp
aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(), 
                               [](const std::unique_ptr<Aircraft>& aircraft) { return !aircraft->move(); }), 
                aircrafts.end());
```

2. Pour des raisons de statistiques, on aimerait bien être capable de compter tous les avions de chaque airline. A cette
   fin, rajoutez des callbacks sur les touches `0`..`7` de manière que le nombre d'avions appartenant à `airlines[x]`
   soit affiché en appuyant sur `x`. Rendez-vous compte de quelle classe peut acquérir cette information. Utilisez la
   bonne fonction de `<algorithm>` pour obtenir le résultat.

- C'est la classe `AircraftFactory` qui a accès à l'array `_airlines`, c'est donc à cette classe qu'on va donner ce
  rôle.\
  On va donc rajouter la fonction `AircraftFactory::print_nb_airline(int n) const` qui va afficher le nombre d'avions de
  l'airline `n` à l'aide d'un `std::count_if` sur son `_aircrafts_flight_numbers`.\
  Puis dans `TowerSimulation::create_keystrokes()`, on rajoute les keystrokes nécessaires pour afficher le nombre
  d'avions appartenant à l'airline `n` de 0 à 7.

### C - Relooking de Point3D

La classe `Point3D` présente beaucoup d'opportunités d'appliquer des algorithmes. Particulièrement, des formulations de
type `x() = ...; y() = ...; z() = ...;` se remplacent par un seul appel à la bonne fonction de la librairie standard.
Remplacez le tableau `Point3D::values` par un `std::array` et puis, remplacez le code des fonctions suivantes en
utilisant des fonctions de `<algorithm>` / `<numeric>`:

1. `Point3D::operator*=(const float scalar)`

```c++
std::transform(values.begin(), values.end(), values.begin(), 
               [scalar](float value) -> float { return value * scalar; });
```

2. `Point3D::operator+=(const Point3D& other)` et `Point3D::operator-=(const Point3D& other)`

```c++
// Pour +=
std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<float> {});

// Pour -= 
std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<float> {});
```

3. `Point3D::length() const`

```c++
std::sqrt(std::accumulate(values.begin(), values.end(), 0.0f, 
                          [](float current, float next) { return current + (next * next); }));
```

---

## Objectif 2 - Rupture de kérosène

Vous allez introduire la gestion de l'essence dans votre simulation.\
Comme le but de ce TP est de vous apprendre à manipuler les algorithmes de la STL, avant d'écrire une boucle,
demandez-vous s'il n'existe pas une fonction d'`<algorithm>` ou de `<numeric>` qui permet de faire la même chose.

La notation tiendra compte de votre utilisation judicieuse de la librairie standard.

### A - Consommation d'essence

Ajoutez un attribut `fuel` à `Aircraft`, et initialisez-le à la création de chaque avion avec une valeur aléatoire
comprise entre `150` et `3'000`.\

- A l'initialisation de `fuel`, on rajoute un `unsigned int fuel = MIN_FUEL + (rand() % (MAX_FUEL - MIN_FUEL)) + 1`
  où `MIN_FUEL = 150` et `
  MAX_FUEL = 3000`.

Décrémentez cette valeur dans `Aircraft::move` si l'avion est en vol.\

- Quand on teste si l'aircraft is_on_ground(), si on va dans le `else`, on décrémente la valeur du fuel puisque c'est à
  ce moment-là qu'on teste si l'aircraft est en vol ou non. On ne le met pas juste avant le `return true` à la fin
  puisque si l'avion est dans un terminal (donc on n'est pas rentré dans le `if (!is_at_terminal)`), alors l'aircraft
  est forcément sur terre (dans un terminal).

Lorsque cette valeur atteint 0, affichez un message dans la console pour indiquer le crash, et faites en sorte que
l'avion soit supprimé du manager.

- Lorsqu'on décrémente la valeur de `fuel` dans `Aircraft::move()`, on regarde si la valeur vaut 0 auquel cas
  on `return false` afin que la fonction `AircraftManager::move()` supprime l'aircraft.

N'hésitez pas à adapter la borne `150` - `3'000`, de manière à ce que des avions se crashent de temps en temps.

- En mettant la valeur du fuel à `unsigned int fuel = 150`, on remarque que tous les avions se crashent au bout d'un
  certain temps : à priori, on a l'effet escompté.

### B - Un terminal s'il vous plaît

Afin de minimiser les crashs, il va falloir changer la stratégie d'assignation des terminaux aux avions. Actuellement,
chaque avion interroge la tour de contrôle pour réserver un terminal dès qu'il atteint son dernier `Waypoint`. Si un
terminal est libre, la tour lui donne le chemin pour l'atteindre, sinon, elle lui demande de tourner autour de
l'aéroport. Pour pouvoir prioriser les avions avec moins d'essence, il faudrait déjà que les avions tentent de réserver
un terminal tant qu'ils n'en n'ont pas (au lieu de ne demander que lorsqu'ils ont terminé leur petit tour).

1. Introduisez une fonction `bool Aircraft::has_terminal() const` qui indique si un terminal a déjà été réservé pour
   l'avion (vous pouvez vous servir du type de `waypoints.back()`).

- L'aircraft qui utilise la fonction `has_terminal()` regarde s'il existe dans la map `reserved_terminals` de `Tower`
  avec un `std::find` (on crée au passage un getter pour la map). Si c'est le cas, un terminal a déjà été réservé pour
  l'aircraft.\
  **NB :** on n'utilise pas `waypoints.back().is_at_terminal()` comme conseillé car cela peut poser problème quand un
  aircraft attend de finir son service/entretien dans un terminal : dans ce cas précis, l'aircraft a son `waypoints`
  vide, donc `has_terminal()` renvoie `false` alors que l'aircraft est dans un terminal. On pourrait aussi introduire un
  bool mais j'en parle dans le NB2 de la question suivante.

2. Ajoutez une fonction `bool Aircraft::is_circling() const` qui indique si l'avion attend qu'on lui assigne un terminal
   pour pouvoir atterrir.

- Dans un premier temps, on sait que si un aircraft n'a pas de terminal (`!has_terminal()`), alors il attend qu'on lui
  assigne un terminal.\
  Cependant, il existe un cas où l'aircraft n'a pas de terminal et n'attend pas qu'on lui en assigne un : lorsqu'il est
  en train de sortir d'un terminal puisqu'à priori on ne veut pas qu'un aircraft retourne dans un terminal.\
  C'est pourquoi j'ai introduit un nouvel attribut à la classe `Aircraft` nommé `bool is_leaving_terminal` qui est
  initialisé à `false` et qui sera changé à `true` uniquement dans la
  fonction `Tower::get_instructions(Aircraft& aircraft)` (`Tower` qui a accès aux champs privés de `Aircraft` vu qu'il
  est friend class) lorsqu'on check si l'aircraft a fini son service/entretien dans un terminal. Si c'est le cas, alors
  on peut mettre l'attribut à `true`.\
  **NB :** A noter que je n'ai pas trouvé d'autres moyens pour déterminer qu'un aircraft est sur son chemin "terminal to
  air" à part introduire un attribut bool à `Aircraft`. En effet, quand il emprunte ce chemin, son dernier waypoint sera
  de type `wp_air` (cf. `AirportType::terminal_to_air(...)`) donc on ne peut pas se servir du type de `waypoints.back()`
  .
  **NB2:** Il est vrai qu'en introduisant un nouvel attribut, on pourrait faire en sorte que celui-ci check si un
  aircraft a déjà eu un terminal (`bool had_terminal` qu'on met à `true` dès lors qu'un aircraft réserve un terminal)
  ce qui nous permettrait d'utiliser cet attribut dans `bool Aircraft::has_terminal() const` et donc de se passer
  du `std::find`. Mais le principe de l'exercice étant d'utiliser les fonctions d'`<algorithm>` ou de `<numeric>`, je
  n'ai pas utilisé cette idée.

3. Introduisez une fonction `WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)` qui essaye de réserver
   un `Terminal`. Si c'est possible, alors elle retourne un chemin vers ce `Terminal`, et un chemin vide autrement (vous
   pouvez vous inspirer / réutiliser le code de `Tower::get_instructions`).

- On écrit plus ou moins la même chose que dans `Tower::get_instructions` à l'exception près qu'on fait toujours
  le `return vp.first` puisque celui-ci retournera de lui-même un chemin vide si on ne peut pas réserver de terminal.

4. Modifiez la fonction `move()` de `Aircraft` afin qu'elle appelle `Tower::reserve_terminal` si l'avion est en attente.
   Si vous ne voyez pas comment faire, vous pouvez essayer d'implémenter ces instructions :\
   \- si l'avion a terminé son service et sa course, alors on le supprime de l'aéroport (comme avant),\
   \- si l'avion attend qu'on lui assigne un terminal, on appelle `Tower::reserve_terminal` et on modifie
   ses `waypoints` si le terminal a effectivement pu être réservé,\
   \- si l'avion a terminé sa course actuelle, on appelle `Tower::get_instructions` (comme avant).

- Après avoir testé si le niveau du fuel est à zéro, on check si l'aircraft attend qu'on lui assigne un
  terminal (`is_circling()`) auquel cas on essaye de lui réserver un terminal avec `Tower::reserve_terminal` et si la
  fonction ne retourne pas de chemin vide, alors on assigne ce nouveau chemin à l'aircraft afin qu'il se rende au
  terminal qu'il a réservé.

### C - Minimiser les crashs

Grâce au changement précédent, dès lors qu'un terminal est libéré, il sera réservé lors du premier appel
à `Aircraft::move` d'un avion recherchant un terminal. Pour vous assurer que les terminaux seront réservés par les
avions avec le moins d'essence, vous allez donc réordonner la liste des `aircrafts` avant de les mettre à jour.

Vous devrez placer au début de la liste les avions qui ont déjà réservé un terminal.\
Ainsi, ils pourront libérer leurs terminaux avant que vous mettiez à jour les avions qui essayeront de les réserver.

La suite de la liste sera ordonnée selon le niveau d'essence respectif de chaque avion.

Par exemple :

```b
A - Reserved / Fuel: 100
B - NotReserved / Fuel: 50
C - NotReserved / Fuel: 300
D - NotReserved / Fuel: 150
E - Reserved / Fuel: 2500
```

pourra être réordonné en

```b
A - Reserved / Fuel: 100
E - Reserved / Fuel: 2500
B - NotReserved / Fuel: 50
D - NotReserved / Fuel: 150
C - NotReserved / Fuel: 300
```

Assurez-vous déjà que le conteneur `AircraftManager::aircrafts` soit ordonnable (`vector`, `list`, etc).\
Au début de la fonction `AircraftManager::move`, ajoutez les instructions permettant de réordonner les `aircrafts` dans
l'ordre défini ci-dessus.

- On ajoute un simple `std::sort` au début de la fonction `AircraftManager::move()` et on trie les aircraft en fonction
  de leur niveau de fuel (on rajoute un getter sur `fuel` au passage).\
  Cependant, avant de comparer le niveau de fuel, on n'oublie pas de regarder si un des deux aircrafts a un terminal et
  l'autre non : dans ce cas-là, c'est celui qui a un terminal qui a la priorité sur l'autre.

### D - Réapprovisionnement

Afin de pouvoir repartir en toute sécurité, les avions avec moins de `200` unités d'essence doivent être
réapprovisionnés par l'aéroport pendant qu'ils sont au terminal.

1. Ajoutez une fonction `bool Aircraft::is_low_on_fuel() const`, qui renvoie `true` si l'avion dispose de moins de `200`
   unités d'essence.\
   Modifiez le code de `Terminal` afin que les avions qui n'ont pas suffisamment d'essence restent bloqués.\
   Testez votre programme pour vérifier que certains avions attendent bien indéfiniment au terminal. Si ce n'est pas le
   cas, essayez de faire varier la constante `200`.

    - Dans `Terminal::is_servicing()`, on fait un `||` afin de regarder si `current_aircraft->is_low_on_fuel()`. De
      cette façon, la fonction renverra `true` si le `current_aircraft` n'a pas assez d'essence.

2. Dans `AircraftManager`, implémentez une fonction `get_required_fuel`, qui renvoie la somme de l'essence manquante (le
   plein, soit `3'000`, moins la quantité courante d'essence) pour les avions vérifiant les conditions suivantes :\
   \- l'avion est bientôt à court d'essence\
   \- l'avion n'est pas déjà reparti de l'aéroport.

- On va créer un `std::accumulate` dans `AircraftManager::get_required_fuel()` qui va additionner le niveau de fuel de
  chaque avion du `std::vector` de `AircraftManager` si l'avion respecte les conditions énoncées. De plus, pour savoir
  si un avion n'est pas déjà reparti de l'aéroport, on passe la fonction `Aircraft::is_on_ground()` en public.

3. Ajoutez deux attributs `fuel_stock` et `ordered_fuel` dans la classe `Airport`, que vous initialiserez à 0.\
   Ajoutez également un attribut `next_refill_time`, aussi initialisé à 0.\
   Enfin, faites en sorte que la classe `Airport` ait accès à votre `AircraftManager` de manière à pouvoir l'interroger.

- Pour que la classe `Airport` ait accès à notre `AircraftManager` de `TowerSimulation`, on ajoute un
  champ `const AircraftManager& aircraft_manager` dans `Airport`. Puis désormais lorsqu'on initialise notre `Airport`
  dans `TowerSimulation::init_airport()`, on lui fournit l'attribut `aircraft_manager`.

4. Ajoutez une fonction `refill` à la classe `Aircraft`, prenant un paramètre `fuel_stock` par référence non-constante.
   Cette fonction rempliera le réservoir de l'avion en soustrayant ce dont il a besoin de `fuel_stock`. Bien
   entendu, `fuel_stock` ne peut pas devenir négatif.\
   Indiquez dans la console quel avion a été réapprovisionné ainsi que la quantité d'essence utilisée.

- Avant de refill le `fuel` de l'aircraft, on regarde si la quantité nécessaire pour refill l'aircraft est inférieure à
  `fuel_stock`, auquel cas on peut remplir totalement le fuel de l'aircraft et soustraire cette valeur à `fuel_stock`
  .\
  Si on n'a pas assez de `fuel_stock`, on remplit le `fuel` de l'aircraft avec le montant actuel de `fuel_stock` et on
  met la valeur de celui-ci à zéro puisqu'on utilise tout le stock pour le mettre dans l'aircraft.

5. Définissez maintenant une fonction `refill_aircraft_if_needed` dans la classe `Terminal`, prenant un
   paramètre `fuel_stock` par référence non-constante. Elle devra appeler la fonction `refill` sur l'avion actuellement
   au terminal, si celui-ci a vraiment besoin d'essence.

- Dans la fonction `Terminal::refill_aircraft_if_needed(int& fuel_stock)`, on va utiliser la fonction qu'on vient de
  créer `Aircraft::refill(int& fuel_stock)`. On remarque que cette fonction n'est pas une fonction `const` puisqu'on
  modifie la valeur de `fuel`. Or, l'attribut `current_aircraft` de `Terminal` est déclaré `const` : on doit donc
  enlever le mot clé `const` pour implémenter notre fonction `refill_aircraft_if_needed`.

6. Modifiez la fonction `Airport::move`, afin de mettre-en-oeuvre les étapes suivantes.\
   \- Si `next_refill_time` vaut 0 :\
   \* `fuel_stock` est incrémenté de la valeur de `ordered_fuel`.\
   \* `ordered_fuel` est recalculé en utilisant le minimum entre `AircraftManager::get_required_fuel()` et `5'000` (il
   s'agit du volume du camion citerne qui livre le kérosène).\
   \* `next_refill_time` est réinitialisé à `100`.\
   \* La quantité d'essence reçue, la quantité d'essence en stock et la nouvelle quantité d'essence commandée sont
   affichées dans la console.\
   \- Sinon `next_refill_time` est décrémenté.\
   \- Chaque terminal réapprovisionne son avion s'il doit l'être.

- Afin que chaque terminal réapprovisionne son avion, après le `t.move();` dans `Airport::move`, on
  rajoute `t.refill_aircraft_if_needed(fuel_stock);`.\
  Pour le reste, cf. `Airport::move`.

### E - Déréservation

Si vous avez suffisamment testé votre programme, vous avez dû vous apercevoir que parfois, certains terminaux arrêtaient
d'être réservés et utilisés.\
En effet, lorsque les avions se crashent alors qu'ils avaient un terminal de réservé, rien n'a été fait pour s'assurer
que le terminal allait de nouveau être libre.

Pour garantir cela, vous allez modifier le destructeur de `Aircraft`. Si l'avion a réservé un terminal, assurez-vous que
celui-ci est correctement libéré. Pour cela, vous aurez besoin de rajouter une fonction dans la classe `Tower`.
Choisissez-lui un nom qui décrit correctement ce qu'elle fait.

### F - Paramétrage (optionnel)

Pour le moment, tous les avions ont la même consommation d'essence (1 unité / trame) et la même taille de
réservoir (`3'000`).

1. Arrangez-vous pour que ces deux valeurs soient maintenant déterminées par le type de chaque avion (`AircraftType`).

2. Pondérez la consommation réelle de l'avion par sa vitesse courante. La consommation définie dans `AircraftType` ne
   s'appliquera que lorsque l'avion est à sa vitesse maximale.

3. Un avion indique qu'il a besoin d'essence lorsqu'il a moins de `200` unités. Remplacez cette valeur pour qu'elle
   corresponde à la quantité consommée en 10s à vitesse maximale.\
   Si vous n'avez pas fait la question bonus de TASK_0, notez bien que la fonction `move` de chaque avion devrait être
   appelée `DEFAULT_TICKS_PER_SEC` fois par seconde. 
