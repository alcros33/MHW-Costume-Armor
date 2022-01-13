import csv, sys
from pathlib import Path
import requests as req

LANGS = [
    ('https://mhw.poedb.tw/cht/transmog', "TW 繁體中文"),
    ('https://mhw.poedb.tw/jpn/transmog', "JA 日本語"),
    ('https://mhw.poedb.tw/chs/transmog', "CN 簡体中文"),
    ('https://mhw.poedb.tw/kor/transmog', "KR 한국어"),
    ('https://mhw.poedb.tw/rus/transmog', "RU Русский"),
    ('https://mhw.poedb.tw/ger/transmog', "DE Deutsch"),
    ('https://mhw.poedb.tw/fre/transmog', "FR Français"),
    ('https://mhw.poedb.tw/ita/transmog', "IT Italiano"),
    ('https://mhw.poedb.tw/pol/transmog', "PL Polski"),
    ('https://mhw.poedb.tw/ptb/transmog', "PO Português do Brasil"),
    ('https://mhw.poedb.tw/ara/transmog', "AR ﺎﻠﻋﺮﺒﻳﺓ"),
]


def main():
    doc = req.get("https://mhw.poedb.tw/eng/transmog").content
    tpos, tend = doc.find(b'<tbody>'), doc.find(b'</tbody>')
    doc = doc[tpos + 11:tend].decode('utf-8')
    doc = doc.replace("<span class='badge float-right'>", "").replace('</span>','')
    data = []
    for row in doc.split('<tr>'):
        ent = row[4:].split('<td>')
        data.append([
            int(ent[0]), *map(lambda e: "Ok" if e else "", ent[1:]),
            max(ent[1:])
        ])

    for url, lang in LANGS:
        doc = req.get(url).content
        tpos, tend = doc.find(b'<tbody>'), doc.find(b'</tbody>')
        doc = doc[tpos + 11:tend].decode('utf-8')
        doc = (doc.replace('<ICON ALPHA>', 'α').replace('<ICON BETA>', 'β')
                .replace('<ICON GAMMA>', 'γ')
                .replace("<span class='badge float-right'>", "").replace('</span>','')
        )
        for it, row in enumerate(doc.split('<tr>')):
            ent = row[4:].split('<td>')
            data[it].append(max(ent[1:]))

    HEADER = ['ID', 'Head', 'Chest', 'Arm', 'Waist', 'Leg', "US English",
            ] + [t[1] for t in LANGS]

    if len(sys.argv) == 1:
        OUTFILE = Path(__file__).resolve().parent / "ArmorData.csv"
    else:
        OUTFILE = Path(sys.argv[1]).resolve()

    with OUTFILE.open('w', newline='', encoding="utf-8") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(HEADER)
        writer.writerows(data)

if __name__ == "__main__":
    main()