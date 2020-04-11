# QuickCD

Collects statistics of which directories you use and introduces a command (e.g. `qcd`) that fuzzy-matches in the history for quick change of directories.

# Demo
![Screenshot](quickcd-demo.gif)

# Installation
1. `make`
1. `sudo -H python3 -m pip install prompt_toolkit`
1. Run the collection script before every prompt; in bash:
   ```
   quickcdStore() {
       /path/to/quickcd-store ~/.quickcd.sqlite3 $PWD
   }
   export PROMPT_COMMAND=quickcdStore
   ```
1. Create an alias for switching directories; in bash:
   ```
   alias qcd="cd \$(python3 /path/to/quickcd-get.py ~/.quickcd.sqlite3)"
   ```
