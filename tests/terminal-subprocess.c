/**
 * AI generated code (experiment). It would be nice to be able to
 * launch a (presumably) TUI based editor as part of the roci
 * debugger, etc.
 */

// gcc tests/terminal-subproces.c -o build-dir/bin/terminal-subprocess

#define _GNU_SOURCE

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

/* Enter Raw Mode & Alternate Screen */
void tui_enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;

    /* Input: disable break, CR to NL, parity check, strip 8th bit, flow control */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* Output: disable post-processing */
    raw.c_oflag &= ~(OPOST);
    /* Control: 8-bit chars */
    raw.c_cflag |= (CS8);
    /* Local: disable echo, canonical mode, extended input, signals (SIGINT/SIGTSTP) */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* Minimum bytes and timeout */
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    /* Switch to alternate screen, hide cursor */
    write(STDOUT_FILENO, "\x1b[?1049h\x1b[?25l", 14);
}

/* Restore Cooked Mode & Normal Screen */
void tui_disable_raw_mode(void) {
    /* Switch back to normal screen, restore cursor */
    write(STDOUT_FILENO, "\x1b[?1049l\x1b[?25h", 14);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/* Redraw parent UI */
void tui_redraw(void) {
    write(STDOUT_FILENO, "\x1b[2J\x1b[H", 7); /* Clear screen, cursor home */
    const char msg[] =
      "--- Parent TUI Active ---\r\n\r\n"
      "Press [e] to launch $EDITOR/nano\r\n"
      "Press [y] to launch yazi\r\n"
      "Press [q] to exit\r\n"
      ;
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

#define MAX_ARGS 64

/**
 * Splits a command string into an argv array.
 * Modifies a copy of the input string in-place to avoid per-token mallocs.
 * Returns dynamically allocated argv array terminated by NULL, or NULL on error.
 * Free the returned pointer with free_argv().
 */
char **tokenize_command(const char *cmd_str) {
    if (!cmd_str || !*cmd_str) return NULL;

    char **argv = malloc(sizeof(char *) * MAX_ARGS);
    char *buf = strdup(cmd_str);
    if (!argv || !buf) {
        free(argv);
        free(buf);
        return NULL;
    }

    int argc = 0;
    char *p = buf;
    char *token_start = NULL;
    char *dst = buf;
    char in_quote = 0; // 0, '\'', or '\"'

    while (*p && argc < MAX_ARGS - 1) {
        // Skip leading whitespace between tokens
        while (*p && isspace((unsigned char)*p) && !in_quote) {
            p++;
        }
        if (!*p) break;

        token_start = dst;

        while (*p) {
            if (*p == '\\' && *(p + 1) && in_quote != '\'') {
                // Handle escaped characters (except inside single quotes)
                p++;
                *dst++ = *p++;
            } else if ((*p == '\'' || *p == '"') && (!in_quote || in_quote == *p)) {
                // Toggle quote state
                in_quote = in_quote ? 0 : *p;
                p++;
            } else if (isspace((unsigned char)*p) && !in_quote) {
                // Token delimiter
                p++;
                break;
            } else {
                *dst++ = *p++;
            }
        }

        *dst++ = '\0';
        argv[argc++] = token_start;
    }

    argv[argc] = NULL;
    return argv;
}

void free_argv(char **argv) {
    if (!argv) return;
    // argv[0] holds the start of the contiguous strdup buffer
    if (argv[0]) {
        free(argv[0]);
    }
    free(argv);
}

/* Launch a subprocess giving it full terminal control */
int tui_spawn_subprocess(char *const argv[]) {
    // 1. Teardown TUI state & return terminal to cooked mode
    tui_disable_raw_mode();

    // 2. Ignore job control signals in parent so terminal handoff doesn't stop us
    struct sigaction sa_ign, sa_orig_int, sa_orig_quit, sa_orig_tstp, sa_orig_ttou, sa_orig_ttin;
    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_flags = 0;

    sigaction(SIGINT,  &sa_ign, &sa_orig_int);
    sigaction(SIGQUIT, &sa_ign, &sa_orig_quit);
    sigaction(SIGTSTP, &sa_ign, &sa_orig_tstp);
    sigaction(SIGTTOU, &sa_ign, &sa_orig_ttou);
    sigaction(SIGTTIN, &sa_ign, &sa_orig_ttin);

    pid_t parent_pgid = getpgrp();
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        // Re-enable TUI before returning
        tui_enable_raw_mode();
        tui_redraw();
        return -1;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        // Restore default signal dispositions for the child
        struct sigaction sa_dfl;
        sa_dfl.sa_handler = SIG_DFL;
        sigemptyset(&sa_dfl.sa_mask);
        sa_dfl.sa_flags = 0;

        sigaction(SIGINT,  &sa_dfl, NULL);
        sigaction(SIGQUIT, &sa_dfl, NULL);
        sigaction(SIGTSTP, &sa_dfl, NULL);
        sigaction(SIGTTOU, &sa_dfl, NULL);
        sigaction(SIGTTIN, &sa_dfl, NULL);

        // Put child in its own process group
        pid_t child_pid = getpid();
        setpgid(child_pid, child_pid);

        // Hand foreground terminal ownership to child
        tcsetpgrp(STDIN_FILENO, child_pid);

        // Execute target binary
        execvp(argv[0], argv);

        // If exec fails:
        perror("execvp");
        _exit(127);
    }

    // --- PARENT PROCESS ---
    // Ensure child is in its own group even if race condition occurs before child exec
    setpgid(pid, pid);
    tcsetpgrp(STDIN_FILENO, pid);

    // Wait for the child to exit
    int status;
    while (waitpid(pid, &status, WUNTRACED) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            break;
        }
        // Handle stopped child (e.g. suspended via SIGTSTP) if needed
        if (WIFSTOPPED(status)) {
            break;
        }
    }

    // Reclaim terminal control for the parent process group
    tcsetpgrp(STDIN_FILENO, parent_pgid);

    // Restore original parent signal handlers
    sigaction(SIGINT,  &sa_orig_int, NULL);
    sigaction(SIGQUIT, &sa_orig_quit, NULL);
    sigaction(SIGTSTP, &sa_orig_tstp, NULL);
    sigaction(SIGTTOU, &sa_orig_ttou, NULL);
    sigaction(SIGTTIN, &sa_orig_ttin, NULL);

    // Re-enter raw mode and redraw
    tui_enable_raw_mode();
    tui_redraw();

    return status;
}

int main(void) {
    tui_enable_raw_mode();
    tui_redraw();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 'q') {
            break;
        } else if (c == 'e') {
            char *editor = getenv("EDITOR");
	    if (!editor || !*editor) {
	      editor = "nano";
	    }
	    char **editor_argv = tokenize_command(editor);
	    if (editor_argv) {
	      tui_spawn_subprocess(editor_argv);
	      free_argv(editor_argv);
	    }
        } else if (c == 'y') {
	  // TODO(jawilson): yazi --chooser-file.
	  // TODO(jawilson): fzf --output // --multi
	  // TODO(jawilson): lf -selection-path=<path>
	  // TODO(jawilson): ranger --choosefile=<path> // --choosefiles
	  char *args[] = { "yazi", NULL };  // or "--no-window-system"
	  tui_spawn_subprocess(args);
	}
    }

    tui_disable_raw_mode();
    return 0;
}
