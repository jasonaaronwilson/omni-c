///
/// The error system is simple enough to be used in plain C for
/// non-local error handling. Low-level library routines just throw
/// errors. Program don't strictly need to set up a default handler
/// but doing so might allow them to cleanup, etc. If a resource like
/// a file is opened and must be closed, intermediate handlers can be
/// established that simply rethrows the error after some cleanup.
///

typedef error_t = struct {
    char *message;
    void *extra;
    char* filename;
    error_code_t error_code;
    int line;
};

// volatile?
typedef error_frame_t = struct {
    jmp_buf env;
    error_t *error;
    error_frame_t *prev;
};

/* _Thread_local */ error_frame_t* active_error_frame = nullptr;

///
/// error_t* error = try_call(foo(10, blah, blah));
/// if (error != nullptr) {
///    ...;
/// }
///
#define try_call(expr) ({                                               \
    volatile error_frame_t _frame = {0};		                \
    volatile error_t *_caught = NULL;                                   \
    _frame.error = NULL;                                                \
    _frame.prev = cast(error_frame_t*, active_error_frame);		\
    active_error_frame = cast(error_frame_t *, &_frame);		\
    if (setjmp(*(cast(jmp_buf*, &_frame.env))) == 0) {			\
        expr;                                                           \
    } else {                                                            \
        _caught = _frame.error;                                         \
    }                                                                   \
    active_error_frame = _frame.prev;                                   \
    cast(error_t *, _caught);						\
})

///
/// error_t* error = make_error(ERROR_ILLEGAL_STATE);
/// error->message = "Not strictly necessary...";
/// throw(error);
///

_Noreturn void throw_error(error_t *error) {
  if (!active_error_frame) {
    fprintf(stderr, "Unhandled throw code %d, %s:%d", error->error_code, 
	    error->filename, error->line);
    fprintf(stderr, "Exiting...");
    exit(1);
  }
  active_error_frame->error = error;
  longjmp(*(cast(jmp_buf*, &active_error_frame->env)), 1);
}

error_t* make_error_impl(error_code_t error_code, char* filename, int line) {
  error_t* result = malloc_struct(error_t);
  result->error_code = error_code;
  result->filename = filename;
  result->line = line;
  return result;
}

#define make_error(error_code) make_error_impl(error_code, __FILE__, __LINE__)
