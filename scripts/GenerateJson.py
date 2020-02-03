import os, json, sys, argparse
import openpyxl #Excel Reaer thing

parser = argparse.ArgumentParser(description='Generates .json configuration file using a copy of the MHW Layered Armor spreadsheet, downloaded as an .xlsx file')
parser.add_argument('--source', type=str, default="scripts/MHW_Armor_Data.xlsx",help='The .xlsx file')
parser.add_argument('--dest', type=str, default="resources/ArmorData.json",help='The where to write the .json file')

ArmorNames = ["Head", "Body","Arms", "Waist", "Legs"] #Names of the armor pieces

# Checks if a cell has a bg color other than none
def cellDanger(Sheet, i, c):
    return (Sheet[i][c+1].fill.bgColor.value != "00000000")

# Checks if the entire row is dangerous
def rowDanger(Sheet,i):
    for c in range(5):
        if cellDanger(Sheet,i,c):
            return True
    return False

def assertNamesNotNull(Entry, Langs, ID):
    for i in Langs:
        if not Entry[i] or Entry[i] == "":
            print(f"Null value in spreadsheet ID={ID}")
            sys.exit(-1)


if __name__ == "__main__":
    args = parser.parse_args()
    EXCEL_IN = args.source
    JSON_OUT = args.dest
    print(f"Opening {EXCEL_IN}...")
    try:
        Sheet = openpyxl.load_workbook(EXCEL_IN).worksheets[0]
    except:
        print(f"ERROR While opening {EXCEL_IN}")
        sys.exit(-1)

    Langs = []
    it  = 6
    while True:
        if not Sheet[1][it].value:
            break
        Langs.append(Sheet[1][it].value)
        it += 1

    Dic = {}
    DicNames = {}
    for i in range(2, 512):
        if rowDanger(Sheet,i):
            continue 
        Name = Sheet[i][6].value
        ID = Sheet[i][0].value

        if Name == "?":
            continue

        if Name in DicNames:
            print(f"Duplicates {i}")
        DicNames[Name] = 0

        Dic[ID] = {Lng: LName.value for Lng, LName in zip(Langs, Sheet[i][6:])}
        assertNamesNotNull(Dic[ID], Langs, ID)
        Dic[ID]["Mode"] = "".join(str(int(Sheet[i][j+1].value != "?")) for j in range(5))

    Dic = dict(sorted(Dic.items(), key=lambda x:x[1]["English"]))

    with open(JSON_OUT, 'w') as f:
        print(f"Writting to {JSON_OUT}")
        f.write(json.dumps(Dic).replace("},", "},\n"))
