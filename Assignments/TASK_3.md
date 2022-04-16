# Assertions et exceptions

## Objectif 1 - Crash des avions

Actuellement, quand un avion s'écrase, une exception de type `AircraftCrash` (qui est un alias de `std::runtime_error`
déclaré dans `config.hpp`) est lancée.

1. Faites en sorte que le programme puisse continuer de s'exécuter après le crash d'un avion. Pour cela, remontez
   l'erreur jusqu'à un endroit approprié pour procéder à la suppression de cet avion (assurez-vous bien que plus
   personne ne référence l'avion une fois l'exception traitée). Vous afficherez également le message d'erreur de l'
   exception dans `cerr`.

- Pour faire en sorte que le programme puisse continuer de s'exécuter après le crash d'un avion,
  dans `AircraftManager::move()`, dans la lambda de notre `aircrafts.erase()`, on entoure
  notre `return !aircraft->move();` d'un `try catch` qui catch les exceptions de type `std::runtime_error`. Dans
  notre `catch`, on affiche l'erreur et on `return true` afin que `erase` supprime l'aircraft qui a throw l'exception.\
  De cette façon, lorsqu'un avion `throw AircraftCrash` à cause d'un crash, on remonte l'erreur jusqu'à
  notre `aircrafts.erase()` qui supprime l'aircraft.\
  Pour tester que notre fonctionnalité marche bien, on enlève dans `Aircraft::operate_landing_gear()` la
  ligne `landing_gear_deployed = true;` : ainsi, tous les avions qui vont atteindre la piste d'atterrissage crash et un
  message d'erreur s'affiche dans `cerr` et le programme continue bien de s'exécuter.

2. Introduisez un compteur qui est incrémenté chaque fois qu'un avion s'écrase. Choisissez une touche du clavier qui n'a
   pas encore été utilisée (`m` par exemple ?) et affichez ce nombre dans la console lorsque l'utilisateur appuie
   dessus.

- On rajoute un attribut `unsigned int nb_crash` dans notre `AircraftManager` qui va s'incrémenter à chaque fois qu'on
  va rentrer dans notre catch et on ajoute

3. Si vous avez fini d'implémenter la gestion du kérosène (Task_2 - Objectif 2 - A), lancez une exception de
   type `AircraftCrash` lorsqu'un avion tombe à court essence. Normalement, cette exception devrait être traitée de la
   même manière que lorsqu'un avion s'écrase parce qu'il a atterrit trop vite.

- Lorsqu'on teste dans `Aircraft::move()` si le fuel a atteint 0, on ne va désormais plus `return false` mais throw
  l'exception `throw AircraftCrash { flight_number + " is out of fuel" };`.

4. **BONUS** Rédéfinissez `AircraftCrash` en tant que classe héritant de `std::runtime_error`, plutôt qu'en tant qu'
   alias. Arrangez-vous pour que son constructeur accepte le numéro de vol, la position, la vitesse de l'avion au moment
   du crash, et la raison du crash (`"out of fuel"` / `"bad landing"`). Vous utiliserez toutes ces informations pour
   générer le joli message de l'exception.

- On crée une classe `AircraftCrash` qui hérite de `std::runtime_error` qui attend dans son constructeur le numéro de
  vol, la position, la vitesse de l'avion au moment du crash et une string expliquant raison du crash. Puis on
  implémente la fonction `std::runtime_error` avec notre fonction `AircraftCrash::build_error_msg(...)`. Enfin,
  dans `Aircraft::move()`, on fournit les différents paramètres nécessaires à la création de notre exception.

## Objectif 2 - Détecter les erreurs de programmation

Pour sécuriser votre code, repassez sur les différentes fonctions de votre programme et ajoutez des assertions
permettant de vérifier qu'elles sont correctement utilisées. Voici quelques idées :

- fonctions d'initialisation appelées une seule fois
    - Ajout d'assert dans `TowerSimulation::init_airport()` et `TowerSimulation::init_aircraftManager()`
      pour s'assurer que les fonctions ne sont appelés qu'une seule fois. A priori, pas besoin de le faire
      pour `TowerSimulation::launch()` puisque dans nos fonctions d'initialisations on vérifie déjà indirectement qu'on
      lance la fonction qu'une seule fois (si on la lance 2 fois, on lance également 2 fois nos fonctions
      d'initialisations).


- état attendu d'un objet lorsqu'une fonction est appelée dessus
    - Aircraft :
        - `Aircraft::turn_to_waypoint()` : on veut que nos waypoints ne soient jamais vide lorsqu'on appelle la
          fonction. On enlève donc la condition avec `if` qu'on remplace par un assert.
        - `Aircraft::arrive_at_terminal()` : si `is_at_terminal` est déjà à `true`, on appelle la fonction sur un
          aircraft qui est déjà arrivé à un terminal et qui a donc déjà commencé son service : si on rappelle la
          fonction, on recommencera le service à zéro
        - `Aircraft::refill(unsigned int& fuel_stock)` : on vérifie que le niveau de `fuel` d'un aircraft n'est pas
          supérieure au niveau max de fuel `MAX_FUEL`.

    - Airport :
        - `Airport::reserve_terminal(Aircraft& aircraft)` : on vérifie que `terminals` n'est pas vide quand on utilise
          la fonction, auquel cas on ne pourra jamais réserver de terminaux (même si on va également vérifier ce test
          dans notre constructeur).

    - Terminal :
        - `Terminal::finish_service()` : on vérifie que `current_aircraft != nullptr` puisqu'on ne peut pas finir le
          service d'un aircraft si le terminal n'en a pas.

    - Tower :
        - `Tower::arrived_at_terminal(const Aircraft& aircraft)` : on vérifie que l'aircraft est bien sur le sol, auquel
          cas on ne peut pas le faire arriver dans un terminal et commencer son service.

- vérification de certains paramètres de fonctions
    - Aircraft :
        - Constructeur : on vérifie que `flight_number` n'est pas une chaîne vide.

    - AircraftFactory :
        - `AircraftFactory::print_nb_airline(size_t n)` : on vérifie que `n` soit bien inférieur à `NB_AIRLINES`.

    - AircraftType :
        - Constructeur : on vérifie que `max_ground_speed`, `max_air_speed` et `max_accel` ne sont pas négatifs.

    - AircraftManager : vu qu'on utilise des `std::unique_ptr<Aircraft>` dans `AircraftManager`, on doit tout le temps
      vérifier que nos pointeurs ne sont pas `nullptr`
        - `AircraftManager::move()` : on check si les éléments qu'on compare dans notre `sort()` ne sont pas `nullptr`
          et également quand dans notre `std::remove_if`, avant de faire `move()` sur les aircrafts, on vérifie qu'ils
          ne sont pas `nullptr`.
        - `AircraftManager::emplace_aircraft(std::unique_ptr<Aircraft> aircraft)` : on check que l'`aircraft` qu'on
          rajoute n'est pas `nullptr`.
        - `AircraftManager::get_required_fuel()` : on check encore une fois que les aircrafts sur lesquels on récupère
          le niveau de fuel ne sont pas `nullptr`.

    - Airport :
        - Constructeur : on vérifie que `terminals` n'est pas vide puisqu'on a aucune fonction pour rajouter de
          terminaux dans notre `Airport`.
        - `Airport::reserve_terminal(Aircraft& aircraft)` : on vérifie qu'`airport` n'a pas déjà de terminal, sinon on
          pourrait avoir un aircraft avec plusieurs terminaux.
        - `Airport::start_path(const size_t terminal_number)` : on vérifie que `terminal_number` n'est pas plus grand
          que le nombre de terminaux de `AirportType type` (on rajoute au passage une
          fonction `AirportType::get_nb_terminals()` pour récupérer le nombre de terminaux d'un `AirportType`).
        - `Airport::get_terminal(const size_t terminal_num)`: on vérifie que `terminal_num` n'est pas plus grand que la
          taille de `terminals`.

    - Terminal :
        - Constructeur : on vérifie que les vecteurs `terminal_pos` et `runways` ne sont pas vides.
        - `Terminal::air_to_terminal(...)` & `Terminal::terminal_to_air(...)` : de la même façon que
          pour `Airport::get_terminal(const size_t terminal_num)`, on vérifie que `terminal_num` est inférieur à la
          taille du vecteur `terminal_pos` et on fait la même chose pour `runway_num` et le vecteur `runways`.