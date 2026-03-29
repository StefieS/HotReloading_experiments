# HotReloading Experiments

A minimal proof-of-concept for **hot reloading** in C using shared libraries (`dlopen`/`dlclose`). The main program runs in a loop, watches a compiled plugin (`.so`) for changes, and automatically reloads it — without restarting the process.

---

## How It Works

The project is split into two parts:

**`main.c` — the host process**
Runs an infinite loop that polls `build/plugin.so` for file modification time changes. When a change is detected, it:
1. Closes the old shared library with `dlclose`
2. Copies `plugin.so` to `plugin_temp.so` (to avoid file-locking issues)
3. Opens the copy with `dlopen`
4. Re-resolves all plugin function pointers via `dlsym`
5. Calls `init()` on the freshly loaded plugin, then resumes calling `update()` every second

**`plugin.c` — the hot-reloadable module**
Implements two functions: `init()` and `update()`. You can edit, recompile, and replace the `.so` while the host is still running — the host will pick up the new version automatically.

**`functions.h` — the contract**
Defines function pointer types (`init_func`, `update_func`) and a `FUNCTION_LIST` macro used with X-macro pattern to declare, load, and validate all plugin functions in one place. Adding a new plugin function only requires updating this list.

---

## Project Structure

```
.
├── main.c          # Host process — watches and reloads the plugin
├── plugin.c        # Hot-reloadable plugin (init + update)
├── functions.h     # Function type definitions and X-macro list
└── Makefile        # Build system
```

**Build output** goes into `build/`:
```
build/
├── plugin.so       # Compiled plugin shared library
└── plugin_temp.so  # Temporary copy used by the running process
```

---

## Getting Started

### Prerequisites

- GCC
- Linux (uses `dlfcn.h`, `sys/stat.h`, and POSIX file APIs)

### Build & Run

```bash
# Build everything and run
make run

# Or build first, then run manually
make
./main
```

### Hot Reload in Action

While `./main` is running, open a second terminal and rebuild only the plugin:

```bash
make plugin
```

The host process will detect the updated `.so`, reload it, and call `init()` on the new version — all without restarting.

---

## Makefile Targets

| Target   | Description                              |
|----------|------------------------------------------|
| `all`    | Build both `main` and `plugin.so`        |
| `run`    | Build everything and execute `./main`    |
| `plugin` | Rebuild only the plugin shared library   |
| `clean`  | Remove `build/` directory and `main`     |

---

## Extending the Plugin

To add a new function to the plugin:

1. Add its typedef and name to `functions.h`:
   ```c
   typedef void (*render_func)(void);

   #define FUNCTION_LIST \
     X(init)             \
     X(update)           \
     X(render)
   ```

2. Implement it in `plugin.c`:
   ```c
   void render(void) {
       printf("Rendering frame\n");
   }
   ```

3. Call it from the `main.c` loop where appropriate.

The X-macro in `load_functions()` will automatically handle declaration, symbol resolution, and error reporting for the new function.

---

## Notes

- The temp-copy trick (`plugin.so` → `plugin_temp.so`) prevents issues with overwriting a file that is currently mapped into memory.
- This is an **experiments** repo — the focus is on exploring the mechanism, not production hardening.
