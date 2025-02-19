import datetime
import os
import re
import sqlite3
import sys

DB_FILE = os.path.expanduser("~/.quickcd.sqlite3")


def main(file):

    data = open(file, "r").readlines()
    pattern = re.compile(
        r"^(?P<year>\d\d\d\d)-(?P<month>\d\d)-(?P<day>\d\d) (?P<hour>\d\d):(?P<min>\d\d):(?P<sec>\d\d) CET \w+ "
        r"(?P<path>[a-zA-Z0-9/\-\.]+) (?P<elapsed>\d+)s (?P<ret>\d+) → (?P<cmd>.*)"
        + "\n$"
    )
    rows = []
    for r in data:
        m = pattern.match(r)
        assert m

        dt = datetime.datetime(
            int(m.group("year")),
            int(m.group("month")),
            int(m.group("day")),
            int(m.group("hour")),
            int(m.group("min")),
            int(m.group("sec")),
        )
        rows.append(
            (
                dt.strftime("%Y-%m-%d %H:%M:%S"),
                m.group("cmd"),
                m.group("path"),
                int(m.group("ret")),
                float(m.group("elapsed")),
            )
        )

    with sqlite3.Connection(DB_FILE) as conn:
        with conn:
            cur = conn.cursor()
            cur.executemany(
                "INSERT INTO commands (timestamp, command, directory, return_value, time) "
                "VALUES (?, ?, ?, ?, ?)",
                rows,
            )

    print("Done")


main(sys.argv[1])
