/* ============================================================
 * minishell_skeleton.c  –  Squelette TD Mini-Shell  (ET3)
 * Compilez avec : gcc -Wall -o minishell minishell_final.c
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

/* ─── Constantes ────────────────────────────────────────────── */
#define MAX_LINE  1024
#define MAX_ARGS  64

/* ─── Variables globales shell ──────────────────────────────── */
int   shell_terminal;
pid_t shell_pgid;
struct termios shell_tmodes;

/* ─── Structure Job ─────────────────────────────────────────── */
typedef enum { RUNNING, STOPPED, DONE } job_status_t;

typedef struct job {
    int           job_id;
    pid_t         pgid;
    char          command[256];
    job_status_t  status;
    struct job   *next;
} job_t;

job_t *job_list = NULL;
int    next_job_id = 1;

/* ─── Prototypes ────────────────────────────────────────────── */
void   init_shell(void);
int    parse_line(char *line, char **argv, int max_args);
void   execute_command(int argc, char **argv);
void   launch_job(int argc, char **argv, int foreground);
void   wait_for_job(pid_t pgid);
job_t *add_job(pid_t pgid, const char *cmd);
void   remove_job(pid_t pgid);
void   update_job_statuses(void);
void   builtin_jobs(void);
void   builtin_fg(char *arg);
void   builtin_bg(char *arg);

typedef struct {
    char *input_file;
    char *output_file;
    int   append;
} redirection_t;

redirection_t redir;

/* ================================================================
 * init_shell : prendre le contrôle du terminal
 * ================================================================ */
void init_shell(void) {
    shell_terminal = STDIN_FILENO;

    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
        kill(-shell_pgid, SIGTTIN);

    signal(SIGINT,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    shell_pgid = getpid();
    setpgid(shell_pgid, shell_pgid);
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
}

/* ================================================================
 * parse_line : découpe `line` en tokens dans argv[]
 * ================================================================ */
int parse_line(char *line, char **argv, int max_args) {
    int argc = 0;
    char *token = strtok(line, " \t");

    redir.input_file  = NULL;
    redir.output_file = NULL;
    redir.append      = 0;

    while (token && argc < max_args - 1) {
        if (strcmp(token, ">") == 0) {
            redir.output_file = strtok(NULL, " \t");
            redir.append = 0;
        }
        else if (strcmp(token, ">>") == 0) {
            redir.output_file = strtok(NULL, " \t");
            redir.append = 1;
        }
        else if (strcmp(token, "<") == 0) {
            redir.input_file = strtok(NULL, " \t");
        }
        else {
            argv[argc++] = token;
        }
        token = strtok(NULL, " \t");
    }
    argv[argc] = NULL;
    return argc;
}

/* ================================================================
 * execute_pipe
 * ================================================================ */
void execute_pipe(char **argv_left, char **argv_right) {
    int pipefd[2];
    if (pipe(pipefd) < 0) { perror("pipe"); return; }

    pid_t pid_left = fork();
    if (pid_left == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(argv_left[0], argv_left);
        perror(argv_left[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid_right = fork();
    if (pid_right == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(argv_right[0], argv_right);
        perror(argv_right[0]);
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid_left,  NULL, 0);
    waitpid(pid_right, NULL, 0);
}

/* ================================================================
 * execute_command : point d'entrée principal
 * ================================================================ */
void execute_command(int argc, char **argv) {

    /* Détecter un pipe | */
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "|") == 0) {
            argv[i] = NULL;
            execute_pipe(argv, argv + i + 1);
            return;
        }
    }

    /* Builtin cd */
    if (strcmp(argv[0], "cd") == 0) {
        char *dir = argv[1];
        if (dir == NULL)
            dir = getenv("HOME");
        if (chdir(dir) < 0)
            perror("cd");
        return;
    }

    /* TODO 5a : builtin jobs */
    if (strcmp(argv[0], "jobs") == 0) {
        builtin_jobs();
        return;
    }

    /* TODO 5b : builtins fg et bg */
    if (strcmp(argv[0], "fg") == 0) {
        builtin_fg(argv[1]);
        return;
    }
    if (strcmp(argv[0], "bg") == 0) {
        builtin_bg(argv[1]);
        return;
    }

    /* Détecter & en dernier token */
    int foreground = 1;
    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
        foreground = 0;
        argv[--argc] = NULL;
    }

    launch_job(argc, argv, foreground);
}

/* ================================================================
 * launch_job : fork + exec avec gestion des groupes de processus
 * ================================================================ */
void launch_job(int argc, char **argv, int foreground) {
    (void)argc;

    /* Construire la commande en string pour l'affichage dans jobs */
    char cmd[256] = "";
    for (int i = 0; argv[i] != NULL; i++) {
        if (i > 0) strncat(cmd, " ", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, argv[i], sizeof(cmd) - strlen(cmd) - 1);
    }

    pid_t pid = fork();
    if (pid == 0) {
        /* 4e : mettre le fils dans son propre groupe */
        setpgid(0, 0);

        /* 4f : si foreground, donner le terminal au fils */
        if (foreground)
            tcsetpgrp(shell_terminal, getpgrp());

        /* 4g : rétablir les signaux par défaut */
        signal(SIGINT,  SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        /* Appliquer les redirections */
        if (redir.input_file) {
            int fd = open(redir.input_file, O_RDONLY);
            if (fd < 0) { perror(redir.input_file); exit(EXIT_FAILURE); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (redir.output_file) {
            int flags = O_WRONLY | O_CREAT | (redir.append ? O_APPEND : O_TRUNC);
            int fd = open(redir.output_file, flags, 0644);
            if (fd < 0) { perror(redir.output_file); exit(EXIT_FAILURE); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(argv[0], argv);
        perror(argv[0]);
        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        /* 4h : setpgid depuis le père (évite la race condition) */
        setpgid(pid, pid);

        /* TODO 5c : ajouter le job à job_list */
        add_job(pid, cmd);

        if (foreground) {
            /* 4i : donner le terminal au fils depuis le père */
            tcsetpgrp(shell_terminal, pid);
            wait_for_job(pid);
            /* 4j : reprendre le terminal */
            tcsetpgrp(shell_terminal, shell_pgid);
        } else {
            /* Trouver le job qu'on vient d'ajouter pour afficher son id */
            job_t *j = job_list;
            while (j && j->pgid != pid) j = j->next;
            printf("[%d] %d\n", j ? j->job_id : next_job_id - 1, pid);
        }
    } else {
        perror("fork");
    }
}

/* ================================================================
 * wait_for_job : attendre qu'un job passe en STOPPED ou DONE
 * ================================================================ */
void wait_for_job(pid_t pgid) {
    int status;
    pid_t p;

    do {
        p = waitpid(-pgid, &status, WUNTRACED);
    } while (p > 0 && !WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));

    /* Mettre à jour le statut dans job_list */
    job_t *j = job_list;
    while (j && j->pgid != pgid) j = j->next;

    if (j) {
        if (WIFSTOPPED(status)) {
            j->status = STOPPED;
            printf("\n[%d] Stopped  %s\n", j->job_id, j->command);
        } else {
            j->status = DONE;
        }
    }
}

/* ================================================================
 * Gestion de la liste de jobs
 * ================================================================ */

/* TODO 5d : add_job — initialiser les champs, insérer en tête */
job_t *add_job(pid_t pgid, const char *cmd) {
    job_t *j =(job_t *) malloc(sizeof(job_t));// on met le cas sinon ça ne fonctionne pas
    if (!j) { perror("malloc"); return NULL; }

    j->job_id = next_job_id++;
    j->pgid   = pgid;
    j->status = RUNNING;
    strncpy(j->command, cmd, sizeof(j->command) - 1);
    j->command[sizeof(j->command) - 1] = '\0';

    /* Insertion en tête de liste */
    j->next   = job_list;
    job_list  = j;

    return j;
}

/* TODO 5e : remove_job — retirer et libérer */
void remove_job(pid_t pgid) {
    job_t **p = &job_list;
    while (*p) {
        if ((*p)->pgid == pgid) {
            job_t *tmp = *p;
            *p = tmp->next;
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

/* TODO 5f : update_job_statuses — collecter les zombies sans bloquer */
void update_job_statuses(void) {
    int status;
    pid_t p;

    while ((p = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        /* Trouver le job correspondant */
        job_t *j = job_list;
        while (j && j->pgid != p) j = j->next;
        if (!j) continue;

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            printf("[%d] Done     %s\n", j->job_id, j->command);
            remove_job(j->pgid);
        } else if (WIFSTOPPED(status)) {
            j->status = STOPPED;
        }
    }
}

/* TODO 5g : builtin_jobs — afficher la liste */
void builtin_jobs(void) {
    update_job_statuses();  /* rafraîchir avant d'afficher */

    job_t *j = job_list;
    while (j) {
        const char *status_str;
        switch (j->status) {
            case RUNNING: status_str = "Running "; break;
            case STOPPED: status_str = "Stopped "; break;
            case DONE:    status_str = "Done    "; break;
            default:      status_str = "Unknown "; break;
        }
        printf("[%d] %s %s\n", j->job_id, status_str, j->command);
        j = j->next;
    }
}

/* TODO 5h : builtin_fg — envoyer SIGCONT, rendre le terminal, attendre */
void builtin_fg(char *arg) {
    job_t *j = NULL;

    if (arg == NULL || strcmp(arg, "") == 0) {
        /* Sans argument : prendre le premier job de la liste */
        j = job_list;
    } else if (arg[0] == '%') {
        /* Syntaxe fg %1 */
        int id = atoi(arg + 1);
        job_t *cur = job_list;
        while (cur) {
            if (cur->job_id == id) { j = cur; break; }
            cur = cur->next;
        }
    }

    if (!j) {
        fprintf(stderr, "fg: no such job\n");
        return;
    }

    j->status = RUNNING;

    /* Rendre le terminal au job */
    tcsetpgrp(shell_terminal, j->pgid);

    /* Envoyer SIGCONT au groupe */
    kill(-j->pgid, SIGCONT);

    /* Attendre qu'il se termine ou soit stoppé */
    wait_for_job(j->pgid);

    /* Reprendre le terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Nettoyer si terminé */
    if (j->status == DONE)
        remove_job(j->pgid);
}

/* TODO 5i : builtin_bg — envoyer SIGCONT sans rendre le terminal */
void builtin_bg(char *arg) {
    job_t *j = NULL;

    if (arg == NULL || strcmp(arg, "") == 0) {
        j = job_list;
    } else if (arg[0] == '%') {
        int id = atoi(arg + 1);
        job_t *cur = job_list;
        while (cur) {
            if (cur->job_id == id) { j = cur; break; }
            cur = cur->next;
        }
    }

    if (!j) {
        fprintf(stderr, "bg: no such job\n");
        return;
    }

    j->status = RUNNING;
    printf("[%d] %s &\n", j->job_id, j->command);

    /* Envoyer SIGCONT au groupe sans rendre le terminal */
    kill(-j->pgid, SIGCONT);
}
/* ================================================================
 * splash_screen : animation de démarrage du shell
 * ================================================================ */
void splash_screen(void) {
    printf("\033[32m");  
    printf("\n");
    printf("  ____  _  _    _    _      ___      ____  ____  ____   ____      _      ____  \n");
    printf(" / ___|| |/ /  / \\  | |    |_ _|    / ___|| ___||  _ \\ |  _ \\    / \\    |_  _| \n");
    printf(" \\___ \\| ' /  / _ \\ | |__   | |     \\___ \\| _|  | |_) )| |_) )  / _ \\    | |  \n");
    printf("  ___) || . \\ / ___ \\| |___  | |      ___) || |___|  _ < |  _ <  / ___ \\   | |  \n");
    printf(" |____/ |_|\\_\\/_/   \\_\\____||___|    |____/ |_____||_| \\_\\|_| \\_\\/_/   \\_\\ |_|  \n");
    printf("\n");
    printf("                                S H E L L\n");
    printf("\n");
    printf("                      Mini-Shell — Projet Polytech ET3\n");
    printf("\033[0m");  
    printf("\n");

    /* barre de chargement */
    printf(" Chargement  [");
    fflush(stdout);

    for (int i = 0; i < 20; i++) {
        usleep(80000);   /* pause 80ms par bloc */
        printf("\033[32m█\033[0m");   /* bloc vert */
        fflush(stdout);
    }

    printf("] 100%%\n\n");
    fflush(stdout);
}
/* ================================================================
 * main
 * ================================================================ */
int main(void)
{
    char  line[MAX_LINE];
    char *argv[MAX_ARGS];

    splash_screen();// la fct d'affichage// Bonus

    init_shell();

    while (1)
    {
        /* TODO 5j : update_job_statuses() en début de boucle */
        update_job_statuses();

        /* 1a — Afficher le prompt */
        printf("minishell> ");
        fflush(stdout);

        /* 1b — Lire une ligne */
        if (fgets(line, MAX_LINE, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        /* 1d — Supprimer le '\n' final */
        line[strcspn(line, "\n")] = '\0';

        /* Parser */
        int argc = parse_line(line, argv, MAX_ARGS);
        if (argc == 0)
            continue;

        /* Builtin exit */
        if (strcmp(argv[0], "exit") == 0)
            break;

        /* Builtin echo */
        if (strcmp(argv[0], "echo") == 0) {
            for (int i = 1; i < argc; i++) {
                printf("%s", argv[i]);
                if (i < argc - 1)
                    printf(" ");
            }
            printf("\n");
            continue;
        }

        if (strcmp(argv[0], "oasisPops") == 0) {
            system("open https://polytech-saclay.oasis.aouka.org");
            continue;
        }

        execute_command(argc, argv);
    }

    return 0;
}