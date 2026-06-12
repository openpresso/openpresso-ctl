# openpresso-ctl {#mainpage}

> @doxyconfig PROJECT_BRIEF

## Overview

`openpresso-ctl` is the command-line client for [openpressod](https://github.com/openpresso/openpressod). It is intended for direct terminal access to the daemon's gRPC interface, allowing users to query machine state, manage brew profiles, update user settings, control power and steam mode, start or stop brews, reset the scales, and stream live events or metrics.

The tool is primarily aimed at advanced users, integrators, and developers who want a lightweight interface to `openpressod` without using a dedicated web frontend. It connects to the daemon over a Unix domain socket by default, or to a TCP endpoint when TCP has been enabled in `openpressod` config.

## Getting started

To start using `openpresso-clt` follow steps from the [installation guide](installation.md) and then see [usage page](usage.md).