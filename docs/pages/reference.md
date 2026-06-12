# Reference {#reference}

This page documents the command-line interface exposed by `openpresso-ctl`.

---

## Global options

| Option | Argument | Default | Description |
|--------|----------|---------|-------------|
| `-s`, `--socket` | Endpoint | `unix://@OPENPRESSOD_SOCKET_PATH@` | Daemon endpoint in `unix://path` or `host:port` form. Can also be set through `OPENPRESSO_SOCK`. |
| `--version` | — | — | Print version and exit. |
| `--help` | — | — | Show top-level help and exit. |
| `--help-all` | — | — | Expand help for all subcommands and exit. |

---

## Scale commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `reset-scales` | — | Zero the weight scales. |

---

## Brew profile commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `get-profile` | — | Print the active brew profile as JSON. |
| `set-profile` | `[file]` | Upload a brew profile from proto3 JSON. If `file` is omitted, input is read from standard input. |

`set-profile` accepts field names in either camelCase or snake_case. Duration values must use the protobuf JSON duration form such as `"30s"` or `"4.5s"`.

---

## User settings commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `get-user-settings` | — | Print the current user settings as JSON. |
| `set-user-settings` | `[file]` | Upload user settings from proto3 JSON. If `file` is omitted, input is read from standard input. |

---

## Power commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `power-on` | — | Turn the machine on. |
| `power-off` | — | Turn the machine off. |
| `get-power` | — | Query the current power state. |

---

## Brew commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `start-brew` | — | Start the brew sequence. |
| `stop-brew` | — | Abort the brew sequence. |
| `get-brew-state` | — | Query whether a brew is currently running. |

---

## Steam commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `steam-on` | — | Enable steam mode. |
| `steam-off` | — | Disable steam mode. |
| `get-steam` | — | Query the current steam mode state. |

---

## Streaming commands

| Command | Arguments | Description |
|---------|-----------|-------------|
| `events` | — | Stream state-change events until interrupted. |
| `metrics` | `[-r RATE] [--pid-source SOURCE]` | Stream live sensor metrics until interrupted. |

`metrics` supports the following options:

| Option | Argument | Default | Description |
|--------|----------|---------|-------------|
| `-r`, `--rate` | Seconds | `1.0` | Update interval in seconds. Must be a positive number. |
| `--pid-source` | Source | `none` | Include PID metrics for a selected controller. Matching is case-insensitive. |

Valid values for `--pid-source` are:

| Value | Meaning |
|------|---------|
| `none` | No PID metrics. |
| `brew` | Brew temperature controller. |
| `steam-preheat` | Steam preheat temperature controller. |
| `steam-active` | Steam active temperature controller. |

---

## Environment

| Variable | Description |
|----------|-------------|
| `OPENPRESSO_SOCK` | Default daemon endpoint used when `--socket` is not specified. |

---

## Exit behavior

A successful command returns exit status `0`. Invalid arguments, connection failures, or RPC-level errors return a non-zero status.
