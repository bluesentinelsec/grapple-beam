#!/usr/bin/env python3
"""Prepare the authorized Guitar Pro example's standard staves for MuseScore import.

The paired TAB notes are removed before the cross-application round trip because
MuseScore 4.1.1 changes their sounding octaves when importing this Guitar Pro file.
This preparation is specific to this fixture, not an engine import heuristic.
"""
from pathlib import Path
import argparse
import xml.etree.ElementTree as ET


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    root = ET.parse(Path(__file__).resolve().parents[1] /
                    "tests/mixer/assets/c64-composition-named.xml").getroot()
    for part in root.findall("part"):
        for measure in part.findall("measure"):
            for child in list(measure):
                if child.tag == "backup" or (child.tag == "note" and child.findtext("staff") == "2"):
                    measure.remove(child)
                elif child.tag == "note":
                    for staff in child.findall("staff"):
                        child.remove(staff)
                elif child.tag == "attributes":
                    for attribute in list(child):
                        if attribute.get("number") == "2":
                            child.remove(attribute)
                        elif attribute.tag == "staves":
                            attribute.text = "1"
                        elif attribute.get("number") == "1":
                            attribute.attrib.pop("number")
    ET.indent(root)
    ET.ElementTree(root).write(args.output, encoding="utf-8", xml_declaration=True)


if __name__ == "__main__":
    main()
