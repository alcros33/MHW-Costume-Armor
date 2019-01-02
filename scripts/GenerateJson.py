import os, json, sys, argparse
import openpyxl #Excel Reaer thing


parser = argparse.ArgumentParser(description='Generates .json configuration file using a copy of the MHW Layered Armor spreadsheet, downloaded as an .xlsx file')
parser.add_argument('--source', type=str, default="scripts/MHW Layered Armor.xlsx",help='The .xlsx file')
parser.add_argument('--dest', type=str, default="resources/ArmorData.json",help='The where to write the json file')

ArmorNames = ["Head","Body","Arms","Waist","Legs"] #Names of the armor pieces
Chars = "BCDEF" # The spreadsheet columns

def RowEmpty(i):
    for c in Chars:
        if Sheet[f'{c}{i}'].value != "?" :
            return False
    return True

def RowDanger(i):
    for c in "BCDEFG":
        bgValue = Sheet[f'{c}{i}'].fill.bgColor.value
        if bgValue != "00000000":
            return True
    return False

def GetName(D):
    for name in ArmorNames:
        if D[name] != "?":
            return D[name]

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

    Dic = []
    for i in range(2,255):    
        if RowEmpty(i):
            continue
        Dic.append({ "ID": int(Sheet[f'A{i}'].value) , "Danger" : RowDanger(i) })
        for c in range(5):
            Dic[-1][ArmorNames[c]] = Sheet[f'{Chars[c]}{i}'].value.replace("Armor","")

    Dic = sorted(Dic,key = lambda x : GetName(x) )

    with open(JSON_OUT, 'w') as f:
        print(f"Writting to {JSON_OUT}")
        json.dump(Dic,f)
            
