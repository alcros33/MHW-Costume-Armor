import os, json, sys, argparse
import openpyxl #Excel Reaer thing

parser = argparse.ArgumentParser(description='Generates .json configuration file using a copy of the MHW Layered Armor spreadsheet, downloaded as an .xlsx file')
parser.add_argument('--source', type=str, default="scripts/MHW_Armor_Data.xlsx",help='The .xlsx file')
parser.add_argument('--dest', type=str, default="resources/ArmorData.json",help='The where to write the .json file')

ArmorNames = ["Head","Body","Arms","Waist","Legs"] #Names of the armor pieces

# Checks if a cell has a bg color other than none
def CellDanger(Sheet, i, c):
    return (Sheet[i][c+1].fill.bgColor.value != "00000000")

# Checks if the entire row is dangerous
def RowDanger(Sheet,i):
    for c in range(5):
        if CellDanger(Sheet,i,c):
            return True
    return False

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

    Dic = {}
    for i in range(2,255):    
        Name = Sheet[i][6].value

        if Name == "?":
            continue

        if Name in Dic:
            print(f"Oh shit {i}")

        Dic[Name] = { "ID": int(Sheet[f'A{i}'].value) , "Danger" : RowDanger(Sheet,i) }
        for j in range(5):
            Dic[Name][ArmorNames[j]] = (Sheet[i][j+1].value != "?")

    Dic = dict(sorted(Dic.items()))

    with open(JSON_OUT, 'w') as f:
        print(f"Writting to {JSON_OUT}")
        f.write(json.dumps(Dic, sort_keys=True).replace("},", "},\n"))
