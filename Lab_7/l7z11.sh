#!/usr/bin/env bash
# Wrapper to run l7z11.bt tracing a program by name.
# Usage: ./run_l7z11.sh /path/to/program [args...]
# Requires l7z11.bt in the same directory and sudo privileges.

set -e

SCRIPT="l7z11.bt"
if [ ! -f "$SCRIPT" ]; then
  echo "Error: $SCRIPT not found in $(pwd)" >&2
  exit 2
fi

if [ $# -lt 1 ]; then
  echo "Usage: $0 /path/to/program [args...]" >&2
  exit 2
fi

PROG="$1"; shift
COMM=$(basename "$PROG")

# create temporary script with target_comm substituted
TMP=$(mktemp /tmp/l7z11-XXXX.bt)
sed "s/target_comm = \"l7z11\";/target_comm = \"$COMM\";/" "$SCRIPT" > "$TMP"

# run under bpftrace (-c runs the program and traces it)
echo "Running under bpftrace, filtering by comm = $COMM"
sudo bpftrace "$TMP" -c "$PROG $*"

rm -f "$TMP"