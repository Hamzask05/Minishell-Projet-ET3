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

**✅ Implémenté :**
Nous avons développé la boucle principale avec :
- `fgets()` pour lire les commandes utilisateur
- `printf()` + `fflush()` pour l'affichage du prompt `minishell> `
- `strcspn()` pour supprimer le `\n` final
- Gestion des builtins `exit` et `echo` via `strcmp()`

**Tests :**
![Test Partie 1](https://github.com/user-attachments/assets/e59324f2-9d69-4593-98e8-20ddc60a03ec)

---

## Partie 2 — Exécution de commandes

**✅ Implémenté :**
- Exécution des commandes externes avec `fork()`, `execvp()` et `waitpid()`
- Builtin `cd` sans fork via `chdir()` (modifie le répertoire du shell parent)

**Tests :**
**2.a - Commandes externes :**
![Test Partie 2.a](https://github.com/user-attachments/assets/f8a62e96-2eed-4bed-b281-e1af51c4fcfd)

**2.b - Builtin `cd` :**
![Test Partie 2.b](https://github.com/user-attachments/assets/f0e2d1d1-3b10-463c-9955-9f9ce4c3625f)

---

## Partie 3 — Redirections & Pipes

**✅ Implémenté :**
- Détection des tokens `>`, `>>`, `<` dans `parse_line()`
- Application des redirections avec `open()` + `dup2()` dans le processus fils
- Pipes via `execute_pipe()` avec `pipe()`, `fork()` ×2 et `dup2()`

**Tests :**

**3.a - Redirections :**
![Test redirection >](https://github.com/user-attachments/assets/c32b8e9a-4727-48d9-ad7f-4e74f85bea28)
![Test redirection <](https://github.com/user-attachments/assets/1fe2dd39-d288-43de-853f-c4155ff27b6e)

**3.b - Pipes :**
![Test pipe](https://github.com/user-attachments/assets/21f01f1f-5c06-47df-a5c1-86e5befd38e6)

---

## Partie 4 — Signaux & Background

**✅ Implémenté :**
- `init_shell()` : `signal(SIG_IGN)` pour ignorer Ctrl-C/Z, `setpgid()` + `tcsetpgrp()` pour isoler le shell
- Détection de `&` → `launch_job(foreground=0)`
- Gestion des groupes de processus

**Tests :**

**4.b - Background :**
![Test background](https://github.com/user-attachments/assets/b60cf966-8a20-4a2d-ad0b-5b69a542673e)

**4.c - Signaux Ctrl-C/Z :**
![Test Ctrl-C](https://github.com/user-attachments/assets/8694f389-f91c-4a4c-8a8e-df75fe0292cd)
![Test Ctrl-Z](https://github.com/user-attachments/assets/e39b82a6-6e2d-4de6-9b65-2fd9ebc73d65)

---

## Partie 5 — Job Control

**✅ Implémenté :**
- Liste chaînée de jobs (`job_t`)
- `builtin_jobs()`, `builtin_fg()`, `builtin_bg()` avec `kill(SIGCONT)`
- Nettoyage des zombies via `waitpid(WNOHANG)` dans `update_job_statuses()`

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
