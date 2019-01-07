import os, json, sys, argparse
import openpyxl #Excel Reaer thing


parser = argparse.ArgumentParser(description='Generates .json configuration file using a copy of the MHW Layered Armor spreadsheet, downloaded as an .xlsx file')
parser.add_argument('--source', type=str, default="scripts/MHW Layered Armor.xlsx",help='The .xlsx file')
parser.add_argument('--dest', type=str, default="resources/ArmorData.json",help='The where to write the json file')

ArmorNames = ["Head","Body","Arms","Waist","Legs"] #Names of the armor pieces
Chars = "BCDEF" # The spreadsheet columns


# Checks if a cell has a bg color other than none
def CellDanger(Sheet, i, c):
    return (Sheet[f'{c}{i}'].fill.bgColor.value != "00000000")

# Checks if the entire row is dangerous
def RowDanger(Sheet,i):
    if CellDanger(Sheet,i,'G'):
        return True
    for c in Chars:
        if not CellDanger(Sheet,i,c):
            return False
    return True

# Returns the name of the Row
def RowName(Sheet,i):
    for c in Chars:
        if Sheet[f'{c}{i}'].value != "?" :
            return Sheet[f'{c}{i}'].value.replace("Armor", "")
    return "?"


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
        Name = RowName(Sheet,i)

        if Name == "?":
            continue

        Dic[Name] = { "ID": int(Sheet[f'A{i}'].value) , "Danger" : RowDanger(Sheet,i) }
        for c in range(5):
            if CellDanger(Sheet, i, Chars[c]) and not Dic[Name]["Danger"]:
                Dic[Name][ArmorNames[c]] = False
            else :
                Dic[Name][ArmorNames[c]] = True

    Dic = dict(sorted(Dic.items()))

    with open(JSON_OUT, 'w') as f:
        print(f"Writting to {JSON_OUT}")
        json.dump(Dic,f)
            
