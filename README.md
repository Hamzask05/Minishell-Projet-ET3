# Mini-Shell — SERRAJ Mehdi & SKALI Hamza — ET3

![Mini-Shell - Projet Polytech ET3](https://github.com/user-attachments/assets/882bcc0b-7509-448f-9f72-9fb3e27bcad9)

## Introduction

Dans le cadre du cours de **Système d'exploitations**, nous avons implémenté un **mini-shell** en C, c'est-à-dire un interpréteur de commandes Unix simplifié qui reproduit les principaux comportements d'un terminal classique : affichage d'un prompt, lecture des commandes utilisateur, lancement des processus, et gestion de leur exécution.

Le projet est structuré en **5 parties progressives**, allant de la boucle de lecture des commandes (`REPL`) jusqu'au contrôle complet des jobs en arrière-plan.

Nous avons également ajouté des **fonctionnalités bonus** pour explorer davantage les mécanismes du shell et créer des raccourcis pratiques.

**Compilation et exécution :**
```bash
gcc -Wall -o minishell v2.c
./minishell
```

---

## Partie 1 — Boucle REPL

Nous avons développé la boucle principale de notre shell en utilisant la fonction `fgets()` pour lire les commandes utilisateur, `printf()` combiné à `fflush()` pour afficher proprement le prompt `minishell>`, et `strcspn()` pour supprimer le caractère de fin de ligne `\n`. Les commandes internes `exit` et `echo` sont traitées directement dans la boucle principale grâce à `strcmp()`.


**Tests :**
![Test Partie 1](https://github.com/user-attachments/assets/e59324f2-9d69-4593-98e8-20ddc60a03ec)

---

## Partie 2 — Exécution de commandes

Nous avons mis en place l'exécution des commandes externes en utilisant le schéma classique `fork()`, `execvp()` et `waitpid()`. La commande interne `cd` est traitée de manière spéciale sans création de processus fils, directement via `chdir()`, car elle doit modifier le répertoire de travail du shell parent lui-même.


**Tests :**
**2.a - Commandes externes :**
![Test Partie 2.a](https://github.com/user-attachments/assets/f8a62e96-2eed-4bed-b281-e1af51c4fcfd)

**2.b - Builtin `cd` :**
![Test Partie 2.b](https://github.com/user-attachments/assets/f0e2d1d1-3b10-463c-9955-9f9ce4c3625f)

---

## Partie 3 — Redirections & Pipes

Dans cette partie, nous avons étendu notre parseur pour détecter les tokens de redirection `>`, `>>` et `<` dans la fonction `parse_line()`. Ces redirections sont ensuite appliquées dans le processus fils grâce à `open()` et `dup2()`. Pour les pipes, nous avons créé la fonction `execute_pipe()` qui utilise `pipe()`, deux `fork()` et `dup2()` pour connecter la sortie du premier processus à l'entrée du second.

**Tests :**

**3.a - Redirections :**
![Test redirection >](https://github.com/user-attachments/assets/c32b8e9a-4727-48d9-ad7f-4e74f85bea28)
![Test redirection <](https://github.com/user-attachments/assets/1fe2dd39-d288-43de-853f-c4155ff27b6e)

**3.b - Pipes :**
![Test pipe](https://github.com/user-attachments/assets/21f01f1f-5c06-47df-a5c1-86e5befd38e6)

---

## Partie 4 — Signaux & Background

Nous avons initialisé notre shell dans `init_shell()` en ignorant les signaux Ctrl-C et Ctrl-Z via `signal(SIG_IGN)`, et en l'isolant dans son propre groupe de processus avec `setpgid()` et `tcsetpgrp()`. L'exécution en arrière-plan est détectée par la présence du token `&` en fin de ligne, ce qui déclenche `launch_job()` avec le paramètre `foreground=0`.

**Tests :**

**4.b - Background :**
![Test background](https://github.com/user-attachments/assets/b60cf966-8a20-4a2d-ad0b-5b69a542673e)

**4.c - Signaux Ctrl-C/Z :**
![Test Ctrl-C](https://github.com/user-attachments/assets/8694f389-f91c-4a4c-8a8e-df75fe0292cd)
![Test Ctrl-Z](https://github.com/user-attachments/assets/e39b82a6-6e2d-4de6-9b65-2fd9ebc73d65)

---

## Partie 5 — Job Control

Nous avons créé une structure de données `job_t` pour maintenir une liste chaînée des jobs actifs. Les commandes `jobs`, `fg` et `bg` sont implémentées respectivement dans `builtin_jobs()`, `builtin_fg()` et `builtin_bg()` en utilisant `kill(SIGCONT)` pour relancer les jobs suspendus et `tcsetpgrp()` pour leur donner le contrôle du terminal. Le nettoyage automatique des processus zombies est assuré par `update_job_statuses()` qui appelle `waitpid(WNOHANG)` en début de chaque itération de la boucle principale.

**Tests :**

![Test jobs complet](https://github.com/user-attachments/assets/691a859b-1383-4f9a-9afa-388c46864801)
![Test jobs liste](https://github.com/user-attachments/assets/f8de3cb6-e007-4e5e-95dd-b2e95a151642)
![Test fg/bg](https://github.com/user-attachments/assets/c0e8a287-953f-4c03-b72c-f5b1fc5f5b91)
![Test Ctrl-Z + jobs](https://github.com/user-attachments/assets/5cf5eb9f-25b9-4a42-af42-47921dfb0a19)

---

## 🎁 Bonus

Nous avons implémenté plusieurs fonctionnalités bonus pour enrichir l'expérience :

- **Splash screen** animé avec codes ANSI + `usleep()`
- **Animation Matrix** avec caractères aléatoires et couleurs
- **Builtin `oasisPops`** : ouvre le portail Polytech via `system("open ...")`

> 📸 **Splash screen**  
> ![Bonus Splash](screenshots/bonus_splash.png)

> 📸 **Matrix**  
> ![Bonus Matrix](screenshots/bonus_matrix.png)

---


**SERRAJ Mehdi & SKALI Hamza — ET3 Système**
