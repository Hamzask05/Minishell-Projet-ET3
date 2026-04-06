# Mini-shell

## Présentation
Ce projet consiste à implémenter un mini-shell en C capable d’exécuter des commandes Unix simples, de gérer certaines commandes internes, les redirections, les pipes et le contrôle des jobs.

## Compilation
```bash
gcc -Wall -o minishell v2.c
```

## Fonctionnalités implémentées

### 1. Boucle principale REPL
- Affichage du prompt.
- Lecture des commandes utilisateur.
- Gestion de `exit`.
- Gestion de `echo`.

### 2. Exécution des commandes
- Exécution des commandes externes avec `fork` et `execvp`.
- Attente de la fin du processus fils avec `waitpid`.
- Gestion des erreurs pour les commandes inexistantes.

### 3. Commandes internes
- `cd`
- `jobs`
- `fg`
- `bg`

### 4. Redirections
- Redirection de l’entrée standard.
- Redirection de la sortie standard.
- Gestion de l’append.

### 5. Pipes
- Exécution d’une commande avec un pipe entre deux processus.

### 6. Gestion des jobs
- Création d’une liste de jobs.
- Gestion des états `RUNNING`, `STOPPED` et `DONE`.
- Support du foreground et du background.
- Gestion des signaux clavier.

## Tests réalisés

### Test 1 : Commande simple
Commande :
```bash
echo hello
```

Résultat attendu :
- Affichage de `hello`.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 1]

---

### Test 2 : Changement de répertoire
Commande :
```bash
cd /tmp
pwd
```

Résultat attendu :
- Le répertoire courant devient `/tmp`.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 2]

---

### Test 3 : Redirection
Commande :
```bash
ls > out.txt
cat out.txt
```

Résultat attendu :
- Le contenu de `ls` est écrit dans `out.txt`.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 3]

---

### Test 4 : Pipe
Commande :
```bash
ls | grep .c
```

Résultat attendu :
- Affichage uniquement des fichiers `.c`.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 4]

---

### Test 5 : Exécution en background
Commande :
```bash
sleep 5 &
```

Résultat attendu :
- Le prompt revient immédiatement.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 5]

---

### Test 6 : Gestion des jobs
Commande :
```bash
jobs
```

Résultat attendu :
- Affichage des jobs actifs ou suspendus.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 6]

---

### Test 7 : Foreground / background
Commande :
```bash
sleep 10
```

Puis :
- `Ctrl-C`
- `Ctrl-Z`
- `bg`
- `fg`

Résultat attendu :
- Le shell reste actif.
- Le job est stoppé puis relancé correctement.

Résultat obtenu :
- OK

[Ajouter ici une capture d’écran du test 7]

## Bonus éventuels
- Splash screen au démarrage.
- Commande personnalisée supplémentaire.
- Toute autre fonctionnalité non demandée mais ajoutée au projet.

## Bugs connus / limites
- [À compléter si besoin]
- [À compléter si besoin]

## Auteurs
- Nom 1
- Nom 2
