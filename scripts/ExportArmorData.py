import os, json, sys, argparse, csv
from pathlib import Path

from requests.api import head

SCRIPTS_DIR = Path(__file__).resolve().parent
BASE_DIR = SCRIPTS_DIR.parent
parser = argparse.ArgumentParser(description='Transforms a .csv file containing armor data into a .cpp file containing amor data')
parser.add_argument('--src', type=Path, default=SCRIPTS_DIR/"ArmorData.csv")
parser.add_argument('--dst', type=Path, default=BASE_DIR/"include"/"ui"/"ArmorData.hpp")

def main():
    header = '#include "AmorData.hpp\n'

    args = parser.parse_args()
    CSV_IN: Path = args.src
    HPP_OUT: Path = args.dst
    with CSV_IN.open(newline='', encoding="utf-8") as csvfile:
        reader = csv.reader(csvfile, delimiter=',')
        rows = list(reader)

    Langs = rows[0][6:]
    count = int(rows[-1][0]) + 1
    print(f"ArmorCount: {count}")
    modes = [0 for _ in range(count)]
    names = {l:["" for _ in range(count)] for l in Langs}
    for row in rows[1:]:
        ID = int(row[0])
        for it, p in enumerate(row[1:6]):
            modes[ID] |= (bool(p)<<it)
        for lan, name in zip(Langs, row[6:]):
            names[lan][ID] = name.replace('"', '\\"')

    header = header.format(count)
    fmtedModes = ','.join(str(m) for m in modes)
    qsl = lambda m: f'{{_QSL("{m}")}}'

    with HPP_OUT.open('w', encoding="utf-8") as f:
        f.write(header)
        f.write("const ArmorModeT armorMode{")
        f.write(fmtedModes)
        f.write("};\n")
        f.write("const ArmorTranslationsT armorData{\n")
        for l in Langs:
            f.write(f'    {{_QSL("{l}"), {{\n        ')
            for m in names[l]:
                f.write(f'_QSL("{m}"),')
            f.write("\n        }\n    },")
        f.write("};\n")

if __name__ == "__main__":
    main()