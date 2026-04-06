# Mini-Shell — Skali-Serraj — ET3

## Introduction

Dans le cadre du cours de Système, nous avons implémenté un interpréteur de commandes Unix simplifié en C. Le projet est divisé en 5 parties progressives, allant de la boucle de lecture de commandes jusqu'au contrôle complet des jobs en arrière-plan. Nous avons également ajouté plusieurs fonctionnalités bonus.

**Compilation et exécution :**
```bash
gcc -Wall -o minishell v2.c
./minishell
```

---

## Partie 1 — Boucle REPL

Nous avons implémenté la boucle principale à l'aide de `fgets` pour lire les commandes, `printf` et `fflush` pour afficher le prompt, et `strcspn` pour supprimer le `\n`. Les builtins `exit` et `echo` sont gérés directement dans la boucle avec `strcmp`.

**Tests :**
```
minishell> echo bonjour le monde
bonjour le monde
minishell> exit
```

> 📸 ![Partie 1](screenshots/partie1.png)

---

## Partie 2 — Exécution de commandes

Nous avons implémenté l'exécution des commandes externes à l'aide de `fork`, `execvp` et `waitpid`. Le builtin `cd` est géré sans fork à l'aide de `chdir` car il doit modifier le répertoire du shell lui-même.

**Tests :**
```
minishell> ls -l
minishell> cd /tmp
minishell> pwd
/tmp
```

> 📸 ![Partie 2](screenshots/partie2.png)

---

## Partie 3 — Redirections & Pipes

Nous avons implémenté les redirections en détectant les tokens `>`, `>>` et `<` dans `parse_line` et en les appliquant dans le fils à l'aide de `open` et `dup2`. Les pipes sont gérés dans `execute_pipe` à l'aide de `pipe`, `fork` et `dup2` pour connecter la sortie du premier fils à l'entrée du second.

**Tests :**
```
minishell> ls -l > /tmp/out.txt
minishell> cat < /tmp/out.txt
minishell> echo hello >> /tmp/out.txt
minishell> ls | grep .c
minishell> cat /etc/hosts | wc -l
```

> 📸 ![Redirections](screenshots/partie3_redirections.png)

> 📸 ![Pipes](screenshots/partie3_pipes.png)

---

## Partie 4 — Signaux & Background

Nous avons implémenté la gestion des signaux dans `init_shell` à l'aide de `signal(SIG_IGN)` pour que le shell ignore Ctrl-C et Ctrl-Z, et `setpgid` et `tcsetpgrp` pour isoler le shell dans son propre groupe. Le background est géré en détectant `&` en dernier token et en appelant `launch_job` avec `foreground=0`.

**Tests :**
```
minishell> sleep 10      → Ctrl-C : sleep tué, shell continue
minishell> sleep 10      → Ctrl-Z : sleep suspendu, shell reprend
minishell> sleep 5 &     → prompt immédiat sans attendre
```

> 📸 ![Ctrl-C](screenshots/partie4_ctrlc.png)

> 📸 ![Ctrl-Z](screenshots/partie4_ctrlz.png)

> 📸 ![Background](screenshots/partie4_background.png)

---

## Partie 5 — Job Control

Nous avons implémenté une liste chaînée de jobs à l'aide de la struct `job_t`. Les builtins `jobs`, `fg` et `bg` sont gérés respectivement dans `builtin_jobs`, `builtin_fg` et `builtin_bg` à l'aide de `kill(SIGCONT)` et `tcsetpgrp`. Le nettoyage des zombies est effectué à l'aide de `waitpid(WNOHANG)` dans `update_job_statuses` appelée en début de chaque itération.

**Tests :**
```
minishell> sleep 30 &
[1] 1234
minishell> sleep 20 &
[2] 1235
minishell> jobs
[1] Running  sleep 30
[2] Running  sleep 20
minishell> fg %1
minishell> sleep 10
^Z
[1] Stopped  sleep 10
minishell> bg %1
[1] sleep 10 &
```

> 📸 ![Jobs](screenshots/partie5_jobs.png)

> 📸 ![fg/bg](screenshots/partie5_fgbg.png)

---

## Bonus

Nous avons implémenté un splash screen à l'aide de codes ANSI et `usleep` pour l'animation, une animation `matrix` à l'aide de caractères aléatoires et de codes de couleur ANSI, et un builtin `oasisPops` à l'aide de `system("open ...")` pour ouvrir le portail Polytech.

> 📸 ![Splash screen](screenshots/bonus_splash.png)

> 📸 ![Matrix](screenshots/bonus_matrix.png)

---

## Bugs connus

- Les pipes multiples ne sont pas supportés (`cmd1 | cmd2 | cmd3`)
- Les redirections dans les pipes ne sont pas supportées (`ls | grep .c > out.txt`)
