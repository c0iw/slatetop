# Usage

## Running

    slatetop

Opens a full-screen launcher. Press `Escape` to close, `Enter` to launch
the selected application.

## Keyboard shortcuts

- `Escape` - close the launcher
- `Enter` - launch the selected application
- Arrow keys - navigate the grid

## Search

Start typing to filter applications by name. The first match is selected
automatically, so you can press `Enter` right away.

## Configuration file

Edit `~/.config/slatetop/slatetop.conf`:

    [Window]
    fullscreen = true
    width = 800
    height = 600

    [Appearance]
    font = Open Sans
    font_size = 14
    label_size = 14
    icon_size = 64
    background = #2e2e2e
    foreground = #ffffff
    selection = #b4b4b4
    opacity = 0.85

    [Grid]
    columns = 4

Changes take effect on the next launch. No rebuild required.

## Cache management

The application cache is stored at `~/.cache/slatetop/apps.cache`. It is
rebuilt automatically when the modification time of any application
directory changes.

To force a rebuild (for example, after manually editing a `.desktop` file):

    slatetop --reload-apps

## Transparency

Semi-transparent background requires a running compositor. On Openbox,
use `picom`

Set `opacity` in the config to control the transparency level
(0.0 = fully transparent, 1.0 = fully opaque).
