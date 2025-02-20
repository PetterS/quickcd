
function timer_start {
  timer=${timer:-$SECONDS}
}
trap 'timer_start' DEBUG

function timer_stop {
  timer_result=$(($SECONDS - $timer))
  unset timer
}

# Enable logging of all commands.
# Right before prompting for the next command, save the previous
# command in a file.
promptFunc() {
  ret_val=$?
  timer_stop
  last_command_id=$(history 1 | sed -r "s/^ *([0-9]+).*/\1/")
  if [ "$last_command_id" != "$last_written_command_id" ]
  then
    last_command=$(history 1 | sed -r 's/^ *[0-9]+ +//')
    # Save Bash history immediately and not upon exit.
    history -a
    last_written_command_id=$last_command_id

	~/bin/quickcd-store ~/.quickcd.sqlite3 "$PWD" "$last_command" "$ret_val" "$timer_result"
  fi
}
PROMPT_COMMAND=promptFunc
