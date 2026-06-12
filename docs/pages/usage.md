# Usage {#usage}

This page covers day-to-day operation of `openpresso-ctl`.

---

## Command format

`openpresso-ctl` requires exactly one subcommand. The general invocation form is:

```text
openpresso-ctl [-s SOCKET] COMMAND [COMMAND-OPTIONS]
```

The global endpoint option is:

```text
-s, --socket   unix://path or host:port
```

If no endpoint is given, the client uses `unix://@OPENPRESSOD_SOCKET_PATH@`. The same value may be provided through the `OPENPRESSO_SOCK` environment variable.

---

## Help output

Show top-level help:

```sh
openpresso-ctl --help
```

Show expanded help for all subcommands:

```sh
openpresso-ctl --help-all
```

---

## Common control tasks

**Reset the scales:**

```sh
openpresso-ctl reset-scales
```

**Turn the machine on:**

```sh
openpresso-ctl power-on
```

**Turn the machine off:**

```sh
openpresso-ctl power-off
```

**Check current power state:**

```sh
openpresso-ctl get-power
```

**Start a brew:**

```sh
openpresso-ctl start-brew
```

**Abort a brew:**

```sh
openpresso-ctl stop-brew
```

**Check whether a brew is running:**

```sh
openpresso-ctl get-brew-state
```

**Enable steam mode:**

```sh
openpresso-ctl steam-on
```

**Disable steam mode:**

```sh
openpresso-ctl steam-off
```

**Check current steam mode state:**

```sh
openpresso-ctl get-steam
```

---

## Profiles and user settings

**Print the active brew profile:**

```sh
openpresso-ctl get-profile
```

**Upload a brew profile from a file:**

```sh
openpresso-ctl set-profile my-profile.json
```

**Upload a brew profile from standard input:**

```sh
cat my-profile.json | openpresso-ctl set-profile
```

**Print current user settings:**

```sh
openpresso-ctl get-user-settings
```

**Update steam temperature:**

```sh
echo "{ \"steamTemperature\": 155000 }" | openpresso-ctl set-user-settings
```

---

## Live monitoring

**Stream state-change events:**

```sh
openpresso-ctl events
```

**Stream live metrics once per second:**

```sh
openpresso-ctl metrics
```

**Stream live metrics every 0.5 seconds with PID data:**

```sh
openpresso-ctl metrics --rate 0.5 --pid-source brew
```

---

## Endpoint selection

Connect to the default Unix domain socket explicitly:

```sh
openpresso-ctl --socket unix://@OPENPRESSOD_SOCKET_PATH@ get-power
```

Connect to a TCP endpoint:

```sh
openpresso-ctl --socket 192.168.1.10:5001 get-power
```

Set the endpoint through the environment:

```sh
export OPENPRESSO_SOCK=unix://@OPENPRESSOD_SOCKET_PATH@
openpresso-ctl get-profile
```

---

## Security considerations

By default the safest deployment model is to use the daemon's Unix domain socket at `@OPENPRESSOD_SOCKET_PATH@`. This relies on local file-system permissions and matches the default `openpressod` setup.

If TCP is enabled in `openpressod` config, `openpresso-ctl` can connect to it directly, but the daemon provides no built-in authentication, authorization, or transport encryption. Any TCP endpoint should therefore be exposed only within a trusted network boundary.
