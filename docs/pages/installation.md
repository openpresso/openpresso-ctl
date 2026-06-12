# Installation {#installation}

This page covers how to install `openpresso-ctl` with a package manager on a Debian-based system.

---

## Requirements

- A Debian-based Linux distribution (Debian, Ubuntu, Raspberry Pi OS, or a compatible derivative).
- `apt-get` available for package installation.
- `curl` available for repository setup.

---

## Adding the package repository

The Openpresso packages are hosted on Cloudsmith. Run the following command to add the repository and its signing key:

```sh
curl -1sLf \
  'https://dl.cloudsmith.io/public/openpresso/@OPENPRESSO_CTL_REPO_CHANNEL@/setup.deb.sh' \
  | sudo -E bash
```

This script registers the repository in `/etc/apt/sources.list.d/` and imports the GPG signing key required to verify packages.

---

## Installing the package

After adding the repository, install `openpresso-ctl` with:

```sh
sudo apt-get update
sudo apt-get install openpresso-ctl
```

The package installs the `openpresso-ctl` binary into the normal system command path so it can be run directly from the shell.

---

## Installing from a local `.deb`

A package downloaded from [GitHub Releases](https://github.com/openpresso/openpresso-ctl/releases) can be installed directly with `apt-get`:

```sh
sudo apt-get install ./openpresso-ctl_<version>_<arch>.deb
```

Using `apt-get` rather than `dpkg -i` is preferred because `apt-get` resolves package dependencies in the same step when possible.

---

## Verifying the installation

Confirm that the binary is available in `PATH`:

```sh
command -v openpresso-ctl
```

Print version:

```sh
openpresso-ctl --version
```

Show top-level help:

```sh
openpresso-ctl --help
```

Verify that the client can reach the daemon and request if the machine is turned on:

```sh
openpresso-ctl get-power
```

---

## Removing the package

To remove the package with `apt-get`:

```sh
sudo apt-get remove openpresso-ctl
```

