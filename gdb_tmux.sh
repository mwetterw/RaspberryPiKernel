#/bin/bash
SESSION=raspigdb

MAIN=1
MEMORY=2
STACK=3
DASHBOARD=4
REGISTERS=5

function get_tty() {
    tmux list-panes -t "$SESSION":1 -F '#{pane_tty}' | awk -v "l=$1" 'NR==l{print $1}'
}

tmux new -d -s "$SESSION"
tmux rename-window -t "$SESSION" "Main"

tmux split-window -h
tmux split-window -h
tmux select-pane -t "$SESSION:.$MAIN"
tmux split-window -vf
tmux split-window -h
tmux select-pane -t "$SESSION:.$MAIN"

for i in $(seq $MAIN $REGISTERS); do
    tmux respawn-pane -k -t "$SESSION:.$i" "/bin/sh"
done

tmux resize-pane -L -t "$SESSION:.$MAIN" 25
tmux resize-pane -R -t "$SESSION:.$REGISTERS" 30
tmux resize-pane -D -t "$SESSION:.$MEMORY" 18

tmux send-keys -t "$SESSION:.$MAIN" "make run" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard history" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard expressions" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard threads" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard registers -output $(get_tty $REGISTERS)" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard memory -output $(get_tty $MEMORY)" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard stack -output $(get_tty $STACK)" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard -output $(get_tty $DASHBOARD)" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard assembly -style context 14" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard source -style context 12" C-m
tmux send-keys -t "$SESSION:.$MAIN" "dashboard" C-m C-l

tmux at -t "$SESSION"
