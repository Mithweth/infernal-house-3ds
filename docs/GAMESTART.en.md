# Game start configuration file

This document describes the `autorun.inf` file, which defines how a new
game starts: the first room, the background music and the items already
in the inventory.

## 1. Location and loading

The file is written in:

```text
resources/game/autorun.inf
```

The build copies every file of `resources/game/` to RomFS, and the game
reads it from:

```text
romfs:/game/autorun.inf
```

The file is read **once**, when the game starts up, before the inventory
and the HUD are initialized. If the file is missing or invalid, the game
does not start.

Its contents are then applied **every time a new game starts** from the
title screen:

1. the inventory, the game states and the HUD are reset;
2. the music is started, if `music` is set;
3. the items listed in `items` are added to the inventory;
4. the player enters the room named by `open`.

## 2. General syntax

The file follows a simplified INI syntax:

```ini
[AutoRun]
open=hall
music=romfs:/audio/background.ogg
items=
```

- Each setting is a `key=value` line.
- Whitespace around the key, around the `=` and at the end of the value
  is ignored.
- Keys are case-sensitive: `open` is valid, `Open` is not.
- Empty lines are ignored.
- A line whose first non-blank character is `#` or `;` is a comment.
  Comments must be on their own line: end-of-line comments are not
  supported and become part of the value.
- A line starting with `[` is a section header. Section headers are
  ignored: `[AutoRun]` is only there for readability, and keys are read
  wherever they are in the file.
- Lines without `=` and unknown keys are silently ignored.
- A line must not exceed 511 characters.

## 3. Keys

| Key     | Required | Description                                         |
|---------|----------|-----------------------------------------------------|
| `open`  | yes      | Room in which a new game starts.                    |
| `music` | no       | Background music played during the game.            |
| `items` | no       | Items already in the inventory when the game starts.|

Each key may appear only once. A duplicate key makes loading fail.

### 3.1. open

```ini
open=hall
```

Name of the starting room, that is the name of its directory in
`resources/rooms/` (loaded from `romfs:/rooms/<name>`).

This key is required: without it, loading fails with
`Missing open in game configuration`.

The room is only loaded when a game starts, not when the file is read. A
misspelled room name is therefore only reported at that moment, with
`Cannot enter room: <name>`.

### 3.2. music

```ini
music=romfs:/audio/background.ogg
```

Full RomFS path of an Ogg Vorbis file (mono or stereo). The music is
played when a game starts, and played again when the player leaves a
mini-game.

Without this key, the game is silent: no music is played, neither at the
start nor after a mini-game.

### 3.3. items

```ini
items=FLASHLIGHT, SCREWDRIVER
```

Comma-separated list of item identifiers, as declared by the `ITEM`
directives of `resources/inventory/inventory`. They are added to the
inventory in the order of the list.

- Spaces around each identifier are ignored, and empty entries are
  skipped.
- Only the first 16 items are kept; the following ones are ignored.
- An unknown identifier does not prevent the game from starting: the
  game prints `Unknown inventory item: <id>` and skips it.
- An item listed twice is only added once.
- An empty value (`items=`) or a missing key means the inventory starts
  empty.

## 4. Complete example

```ini
# Configuration of a new game
[AutoRun]

# First room
open=hall

# Background music
music=romfs:/audio/background.ogg

# Starting inventory
items=FLASHLIGHT, MAGNIFYING_GLASS
```

## 5. Common mistakes

### Adding a comment at the end of a line

```ini
open=hall # starting room
```

The room name becomes `hall # starting room`. Put the comment on its own
line.

### Giving a relative path for the music

```ini
music=audio/background.ogg
```

The path is used as is: always give the full `romfs:/...` path.

### Using the item name instead of its identifier

`items` expects the identifier declared after `ITEM` (e.g. `FLASHLIGHT`),
not the translation key nor the image name.

### Splitting items over several lines

```ini
items=FLASHLIGHT
items=SCREWDRIVER
```

The second line is a duplicate key and makes loading fail. List all the
items on a single line.
