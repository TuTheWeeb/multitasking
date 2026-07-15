
// The Evaluation Engine (Forces the preprocessor to resolve deferred macros)
#define EVAL(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) __VA_ARGS__

// The Deferment Trick
#define DEFER(id) id EMPTY()
#define EMPTY()

// GENERIC MAP Blueprint
// Takes an 'op' (operation macro), a 'sep' (separator), and your variables
#define MAP(op, sep, ...) __VA_OPT__(EVAL(MAP_HELPER(op, sep, __VA_ARGS__)))

#define MAP_HELPER(op, sep, x, ...)                                            \
  op(x) __VA_OPT__(sep DEFER(MAP_INDIRECT)()(op, sep, __VA_ARGS__))

#define MAP_INDIRECT() MAP_HELPER

#define UNWRAP(...) __VA_ARGS__
