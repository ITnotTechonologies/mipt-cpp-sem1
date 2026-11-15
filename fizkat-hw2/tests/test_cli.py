import os
from pathlib import Path
import subprocess
import unittest

PROGRAM = Path(os.environ.get("FIZKAT", "./fizkat")).resolve()


class FleetTests(unittest.TestCase):
    def check_session(self, commands, expected):
        result = subprocess.run(
            [PROGRAM], input=commands, text=True, capture_output=True, timeout=5
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, "")
        self.assertEqual(result.stdout.splitlines(), expected)

    def test_statement(self):
        self.check_session(
            """add scooter S-1 ModelX 2.5 8
add bike B-1 City 4
add scooter S-2 brokenX 1.0 9
add helmet H-1 Pro
add dock D-1 Hub 10.0
list
novice
energy
quit
""",
            ["error: inspection rejected", "B-1 City", "D-1 Hub", "H-1 Pro",
             "S-1 ModelX", "S-1 ModelX", "12.5"],
        )

    def test_mixed_fleet(self):
        self.check_session(
            """add bike Z Road 5
add ebike E Swift 0.3 6
add dock D brokenDock 0.2
add lock A Steel
add scooter S Zip 2 10
list
novice
count_novice
report
remove E
report
count_novice
""",
            ["A Steel", "D brokenDock", "E Swift", "S Zip", "Z Road",
             "E Swift", "S Zip", "2", "5 2.5", "4 2.2", "1"],
        )

    def test_focus_observes_one_instance(self):
        self.check_session(
            """show_focus
add bike A City 1
add bike B City 1
focus A
focus missing
remove B
show_focus
remove A
add bike A City 6
show_focus
focus A
show_focus
""",
            ["expired", "error: unknown id", "A City", "expired", "A City"],
        )

    def test_invalid_commands_are_atomic(self):
        invalid = [
            ("add bike X Joined 11", "invalid score"),
            ("add scooter X Empty -1 6", "invalid energy"),
            ("add ebike X sneakyBROkenx 1 6", "inspection rejected"),
            ("add bike X City 0", "invalid score"),
            ("add dock X Hub 1x", "invalid number"),
            ("add bike X City 5.5", "invalid number"),
            ("add dock X Hub 1e999", "invalid number"),
            ("add helmet", "missing argument"),
            ("add skate X Fast", "unknown item type"),
            ("add helmet A Other", "duplicate id"),
            ("add lock X Guard extra", "extra argument"),
            ("remove A extra", "extra argument"),
            ("remove missing", "unknown id"),
            ("focus missing", "unknown id"),
            ("quit extra", "extra argument"),
            ("unknown", "unknown command"),
        ]
        for command, error in invalid:
            with self.subTest(command=command):
                self.check_session(
                    f"add dock A Hub 1.5\nfocus A\n{command}\n"
                    "report\nlist\nshow_focus\n",
                    [f"error: {error}", "1 1.5", "A Hub", "A Hub"],
                )

    def test_empty_and_quit(self):
        self.check_session(
            "\nlist\nnovice\nenergy\ncount_novice\nreport\nquit\nunknown\n",
            ["0.0", "0", "0 0.0"],
        )


if __name__ == "__main__":
    unittest.main()
