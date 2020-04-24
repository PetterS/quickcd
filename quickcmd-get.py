#!/usr/bin/env python3
import sqlite3
import sys

from prompt_toolkit import prompt, PromptSession, HTML
from prompt_toolkit.completion import FuzzyCompleter, Completer, Completion
from prompt_toolkit.filters import has_completions, completion_is_selected
from prompt_toolkit.key_binding import KeyBindings


class DirsCompleter:
    def __init__(self, filename):
        self.conn = sqlite3.connect(f"file:{filename}?mode=ro", uri=True)

    def get_completions(self, document, complete_event):
        c = self.conn.cursor()
        yield from (
            Completion(row[0])
            for row in c.execute("SELECT command from commands ORDER BY count DESC;")
        )


def main(args):
    if len(args) != 2:
        sys.stderr.write(f"Usage: {args[0]} <sqlite3 db>\n")
        sys.exit(1)

    key_bindings = KeyBindings()

    @key_bindings.add("enter", filter=has_completions & ~completion_is_selected)
    def _(event):
        event.current_buffer.go_to_completion(0)
        event.current_buffer.validate_and_handle()

    base_completer = DirsCompleter(args[1])
    completer = FuzzyCompleter(base_completer)
    session = PromptSession(
        HTML("<yellow>→ </yellow>"), completer=completer, complete_while_typing=True
    )
    try:
        answer = session.prompt(
            pre_run=session.default_buffer.start_completion, key_bindings=key_bindings
        )
    except KeyboardInterrupt:
        sys.exit(2)
    sys.stdout.write(answer)


if __name__ == "__main__":
    main(sys.argv)
