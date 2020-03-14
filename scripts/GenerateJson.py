import os, json, sys, argparse, csv

parser = argparse.ArgumentParser(description='Generates .json configuration file using a copy of the MHW Layered Armor spreadsheet, downloaded as an .csv file')
parser.add_argument('--source', type=str, default="scripts/MHW_Armor_Data - MHW Layered Armor.csv",help='The .csv file')
parser.add_argument('--dest', type=str, default="resources/ArmorData.json",help='The where to write the .json file')

ArmorNames = ["Head", "Body","Arms", "Waist", "Legs"] #Names of the armor pieces

if __name__ == "__main__":
    args = parser.parse_args()
    CSV_IN = args.source
    JSON_OUT = args.dest
    print(f"Opening {CSV_IN}...")
    with open(CSV_IN, newline='', encoding="utf-8") as csvfile:
        reader = csv.reader(csvfile, delimiter=',', quotechar='|')
        rows = list(reader)
    
    Langs = rows[0][6:]

    Dic = {}
    names = set()
    for row in rows[1:]:
        name = row[6]
        ID = row[0]

        if name == "?":
            continue

        if name in names:
            print(f"Duplicates {ID}")
        names.add(name)

        Dic[ID] = {Lng: LName for Lng, LName in zip(Langs, row[6:])}
        Dic[ID]["Mode"] = "".join('1' if row[j] != "?" else '0' for j in range(1, 6))

    Dic = dict(sorted(Dic.items(), key=lambda x:x[1]["English"]))

    with open(JSON_OUT, 'w') as f:
        print(f"Writting to {JSON_OUT}")
        f.write(json.dumps(Dic).replace("},", "},\n"))
