// -------------------- FUNCTION TYPES --------------------

typedef void (*init_func)(void);
typedef void (*update_func)(void);
// -------------------- FUNCTION LIST --------------------

#define FUNCTION_LIST \
    X(init)           \
    X(update)
