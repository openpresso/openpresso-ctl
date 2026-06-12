% OPENPRESSO-CTL(1)
% Openpresso
% @OPENPRESSO_CTL_VERSION@

# NAME

**openpresso-ctl** — CLI client for the openpressod espresso machine daemon

# SYNOPSIS

```
openpresso-ctl [-s SOCKET] COMMAND [COMMAND-OPTIONS]
openpresso-ctl --help
openpresso-ctl --help-all
```

# DESCRIPTION

**openpresso-ctl** is the command-line interface for **openpressod(8)**, the Openpresso hardware control daemon. It communicates with the daemon over a gRPC connection on a Unix domain socket or TCP endpoint, allowing users to query and control the espresso machine from the terminal.

All functionality is exposed as subcommands grouped by concern: machine power, brew control, steam mode, brew profiles, user settings, scales, and live sensor streaming.

# OPTIONS

**-s** *SOCKET*, **--socket** *SOCKET*
:   The daemon endpoint to connect to. Accepts a Unix domain socket path in the form `unix://path` or a TCP address in the form `host:port`. Defaults to `unix://@OPENPRESSOD_SOCKET_PATH@`. Can also be set via the **OPENPRESSO_SOCK** environment variable.

**--help**
:   Show top-level help and exit.

**--help-all**
:   Expand and show help for all subcommands and exit.

# COMMANDS

Exactly one subcommand must be provided.

## Scale

**reset-scales**
:   Zero the weight scales (tare).

## Brew Profile

**get-profile**
:   Print the active brew profile to stdout as proto3 JSON.

**set-profile** [*file*]
:   Upload a brew profile to the daemon. The profile is read from *file* if provided, or from standard input if omitted. The input must be valid proto3 JSON. Field names follow the proto3 JSON mapping and may be camelCase or snake_case. Duration values must use the `"Xs"` string form (e.g. `"30s"`, `"4.5s"`).

Example JSON:

```json
{
    "name": "Espresso",
    "temperature": 93000,
    "totalWeight": 36000,
    "steps": [
        { "pressure": 2000, "totalWeight": 3000 },
        { "flowRate": 2500, "stepTime": "5s" },
        { "pressure": 9000, "totalWeight": 36000 }
    ]
}
```

A brew profile contains a **name**, a target **temperature** (milli °C), an optional stop condition (**totalTime** or **totalWeight** in milligrams), and one or more **steps**. Each step specifies a target (**pressure** in mbar or **flowRate** in milligrams per second) and an advance condition (**stepTime**, **totalTime**, or **totalWeight**, may be ommited for the last step).

## User Settings

**get-user-settings**
:   Print the current user settings to stdout as JSON.

**set-user-settings** [*file*]
:   Update user settings. The settings is read from *file* if provided, or from standard input if omitted. The input must be valid proto3 JSON. Field names follow the proto3 JSON mapping and may be camelCase or snake_case.

Example JSON:

```json
{
    "steamTemperature": 155000
}
```

Settings currently contain only  **steamTemperature** (milli °C).

## Power

**power-on**
:   Turn the machine on.

**power-off**
:   Turn the machine off.

**get-power**
:   Query and print the current power state.

## Brew

**start-brew**
:   Start the brew sequence using the active profile.

**stop-brew**
:   Abort a running brew sequence.

**get-brew-state**
:   Query whether a brew is currently running.

## Steam

**steam-on**
:   Enable steam mode.

**steam-off**
:   Disable steam mode.

**get-steam**
:   Query the current steam mode state.

## Streaming

**events**
:   Stream state-change events from the daemon continuously. Each event reports a changed state: power, brew progress, steam mode, user settings, or brew profile. Press **Ctrl+C** to quit.

**metrics** [*OPTIONS*]
:   Stream live sensor data from the daemon continuously. Press **Ctrl+C** to quit. Each sample includes temperature, pressure, weight, and flow rate. Optionally includes PID controller internals.

    **-r** *RATE*, **--rate** *RATE*
    :   Update interval in seconds. Must be a positive number (e.g. `0.5`, `1`, `2`). Defaults to `1.0`.

    **--pid-source** *SOURCE*
    :   Include PID metrics for the specified temperature controller. Valid values:

        - `none` — no PID metrics (default)
        - `brew` — brew temperature controller
        - `steam-preheat` — steam preheat temperature controller
        - `steam-active` — steam active temperature controller

        Matching is case-insensitive.

# ENVIRONMENT

**OPENPRESSO_SOCK**
:   Daemon endpoint. Overridden by **--socket**. Accepts the same `unix://path` or `host:port` syntax.

# EXIT STATUS

**0**
:   Success.

**Non-zero**
:   An error occurred (connection failure, gRPC error, invalid arguments, etc.).

# EXAMPLES

Zero the scales before pulling a shot:

```sh
openpresso-ctl reset-scales
```

Turn the machine on:

```sh
openpresso-ctl power-on
```

Inspect the active brew profile:

```sh
openpresso-ctl get-profile
```

Upload a brew profile from a file:

```sh
openpresso-ctl set-profile my-profile.json
```

Upload a brew profile from stdin:

```sh
cat my-profile.json | openpresso-ctl set-profile
```

Set steam temperature to 155 °C:

```sh
echo "{ \"steamTemperature\": 155000 }" | openpresso-ctl set-user-settings
```

Start a brew:

```sh
openpresso-ctl start-brew
```

Stream live metrics at 0.5 s intervals with brew PID data:

```sh
openpresso-ctl metrics --rate 0.5 --pid-source brew
```

Watch state-change events:

```sh
openpresso-ctl events
```

Connect to a daemon over TCP instead of the default socket:

```sh
openpresso-ctl --socket 192.168.1.10:5001 get-power
```

Use a custom socket path via the environment variable:

```sh
export OPENPRESSO_SOCK=unix://@OPENPRESSOD_SOCKET_PATH@
openpresso-ctl get-profile
```

# PROTOCOL

**openpresso-ctl** communicates with **openpressod** over **gRPC** using the Protocol Buffers definitions from the openpresso-proto repository:

- https://github.com/openpresso/openpresso-proto

The service definition lives in `openpresso.proto`. The connection uses no transport security (`InsecureChannelCredentials`); it is assumed that the Unix domain socket or any local TCP endpoint is trusted.

# SECURITY

The gRPC interface exposed by **openpressod** provides no built-in authentication or transport encryption. When connecting over TCP (e.g. `host:port`), ensure the network path is trusted. Unix domain socket access is controlled by file system permissions on the socket itself (default: accessible to the `openpressod` group).

See **openpressod(8)** for a full discussion of the daemon's security model.

# SEE ALSO

**openpressod(8)**, **openpressod.conf(5)**
