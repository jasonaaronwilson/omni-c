#define fn_t(return_type, ...) decltype((return_type(*)(__VA_ARGS__))0)
