#!/usr/bin/env python3
"""Package the author-provided score as deterministic MXL test fixtures."""

from pathlib import Path
from zipfile import ZIP_DEFLATED, ZIP_STORED, ZipFile, ZipInfo


def main():
    assets = Path(__file__).resolve().parent.parent / "tests" / "mixer" / "assets"
    score = (assets / "c64-composition-named.xml").read_bytes()
    container = (
        '<?xml version="1.0" encoding="UTF-8"?>'
        '<container><rootfiles><rootfile full-path="scores/demo.musicxml" '
        'media-type="application/vnd.recordare.musicxml+xml"/></rootfiles></container>'
    ).encode()
    for suffix, method in (("", ZIP_DEFLATED), ("-stored", ZIP_STORED)):
        with ZipFile(assets / f"c64-composition{suffix}.mxl", "w") as archive:
            for name, data in (("META-INF/container.xml", container), ("scores/demo.musicxml", score)):
                info = ZipInfo(name, (2026, 9, 11, 0, 0, 0))
                info.compress_type = method
                info.external_attr = 0o100644 << 16
                archive.writestr(info, data)


if __name__ == "__main__":
    main()
