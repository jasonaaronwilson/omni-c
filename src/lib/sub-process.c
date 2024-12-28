#line 2 "sub-process.c"
#ifndef _SUB_PROCESS_H_
#define _SUB_PROCESS_H_

/**
 * @file sub-process.c
 *
 * Utility to launch a sub-processes and interfact with them.
 *
 * (This version will probably only work on Unix like operating
 * systems but we should be able to make an MS-Windows version at some
 * point.)
 */

/**
 * @enum sub_process_exit_status_t
 */
typedef enum {
  EXIT_STATUS_UNKNOWN,
  EXIT_STATUS_NORMAL_EXIT,
  EXIT_STATUS_SIGNAL,
  EXIT_STATUS_ABNORMAL
} sub_process_exit_status_t;

typedef struct {
  // launch parameters
  value_array_t* argv; // argv[0] is the program executable path

  // child process information
  pid_t pid;
  int stdin;
  int stdout;
  int stderr;

  // exit info
  sub_process_exit_status_t exit_status;
  int exit_code;
  int exit_signal;
} sub_process_t;

sub_process_t* make_sub_process(value_array_t* argv);

boolean_t sub_process_launch(sub_process_t* sub_process);

uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data,
                           uint64_t start_position);

void sub_process_close_stdin(sub_process_t* sub_process);

void sub_process_read(sub_process_t* sub_process, buffer_t* stdout,
                      buffer_t* stderr);

void sub_process_wait(sub_process_t* sub_process);

void sub_process_launch_and_wait(sub_process_t* sub_process,
                                 buffer_t* child_stdin, buffer_t* child_stdout,
                                 buffer_t* child_stderr);

#endif /* _SUB_PROCESS_H_ */

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @function make_sub_process
 *
 * Make and return an *unlaunched* sub-process.
 */
sub_process_t* make_sub_process(value_array_t* argv) {
  sub_process_t* result = malloc_struct(sub_process_t);
  result->argv = argv;
  result->exit_code = -1;
  return result;
}

/**
 * @function sub_process_launch
 *
 * Do the actual sub process launch using the "arguments" in
 * sub_process_t.
 */
boolean_t sub_process_launch(sub_process_t* sub_process) {
  uint64_t length = sub_process->argv->length;
  if (length < 1) {
    log_fatal("Expected at least the program path in argv");
    fatal_error(ERROR_ILLEGAL_STATE);
  }

  // 1. Convert value_array_t to char** for execvp
  char** argv = cast(typeof(char**),
                     malloc_bytes((length + 1) * sizeof(typeof(char*))));
  for (int i = 0; i < length; i++) {
    argv[i] = value_array_get_ptr(sub_process->argv, i, typeof(char*));
  }
  // This shouldn't be necessary because we used malloc_bytes which
  // zeros allocations but it might look like an error.
  argv[length] = NULL;
  // char** envp = NULL;

  // 2. Create pipes for stdin of the sub process as well as to
  // capture stdout and stderr.
  int stdin_pipe[2] = {0};
  int stdout_pipe[2] = {0};
  int stderr_pipe[2] = {0};
  if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1
      || pipe(stderr_pipe) == -1) {
    log_fatal("Failed to create pipe for stdin, stdout or stderr");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }

  // 3. Fork the process
  pid_t pid = fork();
  if (pid == -1) {
    log_fatal("fork() failed.");
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  }

  if (pid == 0) {
    /* ====================================================================== */
    /* Child Process */
    /* ====================================================================== */

    // 4. Redirect stdin, stdout and stderr to pipes
    dup2(stdin_pipe[0], STDIN_FILENO);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdin_pipe[0]);
    close(stdin_pipe[1]);
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[0]);
    close(stderr_pipe[1]);

    // 5. Final "exec" to the command
    int exec_exit_status = execvp(argv[0], argv);
    // execvp should not return!
    log_fatal("execvp returned non zero exit status %d", exec_exit_status);
    fatal_error(ERROR_ILLEGAL_STATE);
    return false;
  } else {
    /* ====================================================================== */
    /* Parent Process */
    /* ====================================================================== */

    // 6. Close write ends of the pipes in the parent since we will
    // only be reading from the pipe.
    close(stdin_pipe[0]);  // Close read end of stdin pipe
    close(stdout_pipe[1]); // Close write end of stdout pipe
    close(stderr_pipe[1]); // Close write end of stderr pipe

    // 7. Record the pid, stdout, and stderr.
    sub_process->pid = pid;
    sub_process->stdin = stdin_pipe[1];
    sub_process->stdout = stdout_pipe[0];
    sub_process->stderr = stderr_pipe[0];

    free_bytes(argv);

    return true;
  }
}

uint64_t sub_process_write(sub_process_t* sub_process, buffer_t* data,
                           uint64_t start_position) {
  int stdin_fd = sub_process->stdin;

  // Set the file descriptor to non-blocking mode
  int flags = fcntl(stdin_fd, F_GETFL, 0);
  fcntl(stdin_fd, F_SETFL, flags | O_NONBLOCK);

  ssize_t bytes_written = write(stdin_fd, &data->elements[start_position],
                                (data->length - start_position));

  if (bytes_written == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    } else {
      // An actual error occurred
      log_fatal("Error writing to subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
  }

  return bytes_written;
}

void sub_process_close_stdin(sub_process_t* sub_process) {
  if (sub_process->stdin != -1) { // Check if stdin is still open
    if (close(sub_process->stdin) == -1) {
      log_fatal("Error closing subprocess stdin: %s", strerror(errno));
      fatal_error(ERROR_ILLEGAL_STATE);
    }
    sub_process->stdin = -1; // Mark stdin as closed
  }
}

void sub_process_read(sub_process_t* sub_process, buffer_t* stdout,
                      buffer_t* stderr) {
  if (stdout != NULL) {
    buffer_read_ready_bytes_file_number(stdout, sub_process->stdout,
                                        0xffffffff);
  }
  if (stderr != NULL) {
    buffer_read_ready_bytes_file_number(stderr, sub_process->stderr,
                                        0xffffffff);
  }
}

// (Private) common tail to is_sub_process_running and
// sub_process_wait. This should only be called once per sub-process.
void sub_process_record_exit_status(sub_process_t* sub_process, pid_t pid,
                                    int status) {
  if (pid == -1) {
    sub_process->exit_status = EXIT_STATUS_ABNORMAL;
    return;
  }

  // Check the exit status and return the exit code
  if (WIFEXITED(status)) {
    sub_process->exit_status = EXIT_STATUS_NORMAL_EXIT;
    sub_process->exit_code = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    sub_process->exit_status = EXIT_STATUS_SIGNAL;
    sub_process->exit_signal = WTERMSIG(status);
  } else {
    sub_process->exit_status = EXIT_STATUS_ABNORMAL;
  }
}

/**
 * @function is_sub_process_running
 *
 * Return true if we recorded that the process already exited or else
 * do a non-blocking call to waitpid to see if it has already
 * exited. This function is idempotent once we reach the exit state.
 */
boolean_t is_sub_process_running(sub_process_t* sub_process) {
  if (sub_process->exit_status != EXIT_STATUS_UNKNOWN) {
    return false;
  }

  int status = 0;
  pid_t result = waitpid(sub_process->pid, &status, WNOHANG);
  if (result == 0) {
    return true;
  }
  sub_process_record_exit_status(sub_process, result, status);
  return false;
}

/**
 * @function sub_process_wait
 *
 * Do a blocking call to waitpid unless we've already recorded that
 * this sub-process has exited in which case it is a NOP. This
 * function is idempotent once we reach the exit state.
 */
void sub_process_wait(sub_process_t* sub_process) {
  if (sub_process->exit_status != EXIT_STATUS_UNKNOWN) {
    int status = 0;
    pid_t result = waitpid(sub_process->pid, &status, 0);
    sub_process_record_exit_status(sub_process, result, status);
  }
}

/**
 * @function sub_process_launch_and_wait
 *
 * Make it much more convenient to use sub-processes in the common
 * case where all you want to do is (possibly) send the contents of a
 * buffer to the sub-processes stdin, collect the sub-processes stdout
 * and stderr until all is collected and the sub-processes has exited.
 *
 * Note that buffers may be NULL in which case that data is ignored.
 */
void sub_process_launch_and_wait(sub_process_t* sub_process,
                                 buffer_t* child_stdin, buffer_t* child_stdout,
                                 buffer_t* child_stderr) {
  sub_process_launch(sub_process);
  uint64_t written = 0;
  do {
    if (child_stdin != NULL && written < child_stdin->length) {
      written += sub_process_write(sub_process, child_stdin, written);
      if (written >= child_stdin->length) {
        sub_process_close_stdin(sub_process);
      }
    }
    sub_process_read(sub_process, child_stdout, child_stderr);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, child_stdout, child_stderr);
  sub_process_wait(sub_process);
}
