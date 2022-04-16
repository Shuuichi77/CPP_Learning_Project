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
- état attendu d'un objet lorsqu'une fonction est appelée dessus
- vérification de certains paramètres de fonctions
- ...
