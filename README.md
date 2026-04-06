# Mini-Shell — SERRAJ Mehdi-SKALI Hamza — ET3
<img width="688" height="299" alt="Mini-Shell - Projet Polytech ET3" src="https://github.com/user-attachments/assets/882bcc0b-7509-448f-9f72-9fb3e27bcad9" />


## Introduction

Dans le cadre du cours de Système, nous avons implémenté un interpréteur de commandes Unix simplifié en C, c’est-à-dire un programme capable d’interpréter et d’exécuter des commandes Unix de manière interactive, à la manière d’un terminal simplifié. L’idée était de reproduire les principaux comportements d’un shell classique : affichage d’un prompt, lecture des commandes saisies par l’utilisateur, lancement des processus, et gestion de leur exécution.

Le projet est divisé en 5 parties progressives, allant de la boucle de lecture de commandes jusqu'au contrôle complet des jobs en arrière-plan. 

Nous avons par ailleurs implémenté quelques fonctionnalités bonus afin de mieux comprendre les mécanismes du shell, mais aussi pour créer d'éventuels raccourcis qui pourraient être intéressants dans un contexte d'utilisation quotidienne.

**Compilation et exécution :**
```bash
gcc -Wall -o minishell v2.c
./minishell
```

---

## Partie 1 — Boucle REPL

Nous avons implémenté la boucle principale à l'aide de `fgets` pour lire les commandes, `printf` et `fflush` pour afficher le prompt, et `strcspn` pour supprimer le `\n`. Les builtins `exit` et `echo` sont gérés directement dans la boucle avec `strcmp`.

**Tests :**


<img width="1105" height="147" alt="Pasted Graphic 3" src="https://github.com/user-attachments/assets/e59324f2-9d69-4593-98e8-20ddc60a03ec" />

---

## Partie 2 — Exécution de commandes

Nous avons implémenté l'exécution des commandes externes à l'aide de `fork`, `execvp` et `waitpid`. Le builtin `cd` est géré sans fork à l'aide de `chdir` car il doit modifier le répertoire du shell lui-même.

**Tests :**


Partie 2.a
<img width="1329" height="425" alt="Pasted Graphic 4" src="https://github.com/user-attachments/assets/f8a62e96-2eed-4bed-b281-e1af51c4fcfd" />
Partie 2.b
<img width="674" height="277" alt="minishell pwd" src="https://github.com/user-attachments/assets/f0e2d1d1-3b10-463c-9955-9f9ce4c3625f" />


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

Partie 3.a
<img width="813" height="300" alt="total 360" src="https://github.com/user-attachments/assets/c32b8e9a-4727-48d9-ad7f-4e74f85bea28" />
Hostname n’existe pas sur Mac, on essaie alors avec hosts:
<img width="819" height="252" alt="# Host Database" src="https://github.com/user-attachments/assets/1fe2dd39-d288-43de-853f-c4155ff27b6e" />

Partie 3.b
<img width="489" height="181" alt="minishell c" src="https://github.com/user-attachments/assets/21f01f1f-5c06-47df-a5c1-86e5befd38e6" />

---

## Partie 4 — Signaux & Background

Nous avons implémenté la gestion des signaux dans `init_shell` à l'aide de `signal(SIG_IGN)` pour que le shell ignore Ctrl-C et Ctrl-Z, et `setpgid` et `tcsetpgrp` pour isoler le shell dans son propre groupe. Le background est géré en détectant `&` en dernier token et en appelant `launch_job` avec `foreground=0`.

**Tests :**
```
minishell> sleep 10      → Ctrl-C : sleep tué, shell continue
minishell> sleep 10      → Ctrl-Z : sleep suspendu, shell reprend
minishell> sleep 5 &     → prompt immédiat sans attendre
```
Partie 4.b
<img width="204" height="62" alt="minishell" src="https://github.com/user-attachments/assets/b60cf966-8a20-4a2d-ad0b-5b69a542673e" />

Partie 4.c
<img width="203" height="42" alt="minishell sleep 10" src="https://github.com/user-attachments/assets/8694f389-f91c-4a4c-8a8e-df75fe0292cd" />
<img width="229" height="43" alt="minishell sleep 10" src="https://github.com/user-attachments/assets/e39b82a6-6e2d-4de6-9b65-2fd9ebc73d65" />


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

<img width="630" height="210" alt="image" src="https://github.com/user-attachments/assets/691a859b-1383-4f9a-9afa-388c46864801" />

<img width="630" height="88" alt="image" src="https://github.com/user-attachments/assets/f8de3cb6-e007-4e5e-95dd-b2e95a151642" />
<img width="630" height="292" alt="image" src="https://github.com/user-attachments/assets/c0e8a287-953f-4c03-b72c-f5b1fc5f5b91" />
<img width="630" height="448" alt="image" src="https://github.com/user-attachments/assets/5cf5eb9f-25b9-4a42-af42-47921dfb0a19" />




---

## Bonus

Nous avons implémenté un splash screen à l'aide de codes ANSI et `usleep` pour l'animation, une animation `matrix` à l'aide de caractères aléatoires et de codes de couleur ANSI, et un builtin `oasisPops` à l'aide de `system("open ...")` pour ouvrir le portail Polytech.

> 📸 ![Splash screen](screenshots/bonus_splash.png)

> 📸 ![Matrix](screenshots/bonus_matrix.png)

---

## Bugs connus

- Les pipes multiples ne sont pas supportés (`cmd1 | cmd2 | cmd3`)
- Les redirections dans les pipes ne sont pas supportées (`ls | grep .c > out.txt`)
