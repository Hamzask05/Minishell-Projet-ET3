# Mini-Shell — Skali-Serraj — ET3

## Compilation
```bash
gcc -Wall -o minishell v2.c
./minishell
```

---

## Partie 1 — Boucle REPL
Implémenté : prompt, lecture avec `fgets`, gestion EOF (Ctrl-D), suppression du `\n`, builtins `exit` et `echo`.

Test :
```
minishell> echo bonjour le monde
minishell> exit
```

---

## Partie 2 — Exécution de commandes
Implémenté : `fork/execvp/waitpid` pour les commandes externes, builtin `cd` avec `chdir()` (sans fork car doit modifier l'état du shell).

Test :
```
minishell> ls -l
minishell> cd /tmp
minishell> pwd
```

---

## Partie 3 — Redirections & Pipes
Implémenté : détection de `>`, `>>`, `<` dans `parse_line`, application avec `dup2` dans le fils. Pipe simple `|` avec `pipe()` + deux fils.

Test :
```
minishell> ls -l > /tmp/out.txt
minishell> cat < /tmp/out.txt
minishell> ls | grep .c
```

---

## Partie 4 — Signaux & Background
Implémenté : `init_shell` ignore les signaux avec `SIG_IGN`, crée son groupe avec `setpgid`, prend le terminal avec `tcsetpgrp`. Détection de `&` en dernier token pour lancer en background.

Test :
```
minishell> sleep 10      → Ctrl-C : sleep tué, shell continue
minishell> sleep 10      → Ctrl-Z : sleep suspendu, shell reprend
minishell> sleep 5 &     → prompt immédiat
```

---

## Partie 5 — Job Control
Implémenté : liste chaînée `job_list`, builtins `jobs`, `fg %n`, `bg %n`. Nettoyage des zombies via `waitpid(WNOHANG)` en début de boucle.

Test :
```
minishell> sleep 30 &
minishell> jobs
[1] Running  sleep 30
minishell> fg %1
```

---

## Bonus
- **Splash screen** : animation ASCII art + barre de chargement au démarrage
- **matrix** : animation pluie de caractères verts (`matrix`)
- **oasisPops** : ouvre le portail Polytech dans le navigateur

## Bugs connus
- Pipes multiples non supportés (`cmd1 | cmd2 | cmd3`)
- Redirections dans les pipes non supportées
