#!/usr/bin/env python3
import sqlite3
import sys

from prompt_toolkit import prompt, PromptSession, HTML
from prompt_toolkit.completion import FuzzyCompleter, Completer, Completion


class DirsCompleter:
    def __init__(self, filename):
        self.conn = sqlite3.connect(f"file:{filename}?mode=ro", uri=True)

    def get_completions(self, document, complete_event):
        c = self.conn.cursor()
        yield from (
            Completion(row[0])
            for row in c.execute("SELECT name from dirs ORDER BY count DESC;")
        )


def main(args):
    if len(args) != 2:
        sys.stderr.write(f"Usage: {args[0]} <sqlite3 db>\n")
        sys.exit(1)

    base_completer = DirsCompleter(args[1])
    completer = FuzzyCompleter(base_completer)
    session = PromptSession(HTML("<yellow>cd </yellow>"), completer=completer, complete_while_typing=True)
    try:
        answer = session.prompt(pre_run=session.default_buffer.start_completion)
    except KeyboardInterrupt:
        sys.exit(2)
    sys.stdout.write(answer)


if __name__ == "__main__":
    main(sys.argv)
