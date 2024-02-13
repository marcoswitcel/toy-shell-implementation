#!/usr/bin/env bash

# https://unix.stackexchange.com/questions/292137/tmux-script-to-launch-several-commands
# https://man.openbsd.org/OpenBSD-current/man1/tmux.1#split-window

tmux new-session  'make run' \; split-window -h 'tail -f ./bin/debug_file.log' \; select-pane -L
