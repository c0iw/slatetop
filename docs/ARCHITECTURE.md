# Architecture

Slatetop is organized into small, single-purpose modules. Each module has
a clear responsibility and a minimal interface.

## Module overview

    src/
    ├── main.c        - entry point, argument parsing
    ├── slatetop.c    - GtkApplication class, UI construction, event handling
    ├── desktop.c     - .desktop file parser
    ├── cache.c       - application list cache
    └── config.c      - configuration file reader

## Data flow

1. `main()` parses arguments. If `--reload-apps` is passed, it rebuilds the
   cache and exits.
2. Otherwise, `main()` creates a `SlatetopApplication` and runs it.
3. On `activate`, the application:
   - Loads the config via `slatetop_config_load()`.
   - Loads the application list. If the cache is valid, it uses
     `slatetop_cache_load()`. Otherwise, it calls `slatetop_app_list_load()`
     and saves the result with `slatetop_cache_save()`.
   - Builds the UI: window, search entry, flowbox with icons.
   - Connects signals: search, activation, key press, background draw.

## Cache invalidation

The cache is considered valid if the modification time of all scanned
directories has not changed since the cache was saved. The directories are:

- `/usr/share/applications`
- `/usr/local/share/applications`
- `~/.local/share/applications`

If any directory's mtime differs from the stored value, the cache is
rebuilt.

## Configuration

The config is stored at `~/.config/slatetop/slatetop.conf` in INI format
(parsed with `GKeyFile` from GLib). Missing values fall back to defaults
defined in `slatetop_config_set_defaults()`.

## UI theming

The window background is drawn manually via Cairo in the `draw` signal,
using the background color and opacity from the config. Widgets (entry,
flowbox) are styled via an embedded CSS provider with priority
`GTK_STYLE_PROVIDER_PRIORITY_USER + 1`, which overrides system themes.

## Dependencies

- **GTK3** - window, widgets, CSS, icons.
- **GLib** - GList, GKeyFile, GDir, GString, memory management.

No other libraries are used.
