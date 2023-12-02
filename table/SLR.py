import csv
from typing import Literal, Tuple

NON_TERMINAL = {"S", "F", "A", "T", "C", "E", "I", "B", "R"}

LEFT_CURLY = "{"
RIGHT_CURLY = "}"


class Builder:
    def __init__(self):
        self.table = []
        self.row = []

    def push_cell(
        self,
        op: Literal["SLR_SHIFT", "SLR_REDUCE", "SLR_GOTO", "SLR_EMPTY"],
        value: int,
    ):
        self.row.append(f"{LEFT_CURLY}{op}, {value}{RIGHT_CURLY}")

    def new_line(self):
        joined_row = "{}{}{}".format(LEFT_CURLY, ",".join(self.row), RIGHT_CURLY)
        self.table.append(joined_row)
        self.row = []

    def build(self) -> str:
        return "{}{}{}".format(LEFT_CURLY, ",".join(self.table), RIGHT_CURLY)


def parse_ops(
    raw: str
) -> Tuple[Literal["SLR_SHIFT", "SLR_REDUCE", "SLR_GOTO", "SLR_EMPTY"], int]:
    if len(raw) == 0:
        return ("SLR_EMPTY", 0)

    if raw[0].isdigit():
        return ("SLR_GOTO", int(raw))

    if raw[0] == "s":
        return ("SLR_SHIFT", int(raw[1:]))

    if raw[0] == "r":
        return ("SLR_REDUCE", int(raw[1:]))

    return ("SLR_EMPTY", 0)


with open("SLR.csv", newline="") as csvfile:
    reader = csv.DictReader(csvfile)
    shift_reduce = Builder()
    goto = Builder()
    for row in reader:
        for sym, op in row.items():
            op, val = parse_ops(op)

            if sym == "state" or sym == "S'":
                continue

            if sym in NON_TERMINAL:
                goto.push_cell(op, val)
            else:
                shift_reduce.push_cell(op, val)

        goto.new_line()
        shift_reduce.new_line()

    print(shift_reduce.build())
    print(goto.build())
