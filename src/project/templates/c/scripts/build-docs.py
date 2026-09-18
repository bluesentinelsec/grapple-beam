"""Build Markdown with Pandoc and check local links in the resulting site."""
from pathlib import Path
from html.parser import HTMLParser
from urllib.parse import urlsplit, unquote
import shutil
import subprocess

root = Path(__file__).resolve().parents[1]
out = root / "build/site"
if out.is_symlink():
    raise SystemExit("build/site must not be a symlink")
shutil.rmtree(out, ignore_errors=True)
out.mkdir(parents=True)
files = list((root / "docs").rglob("*.md")) + [root / "README.md"]
for source in files:
    relative = source.relative_to(root).with_suffix(".html")
    destination = out / relative
    destination.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["pandoc", str(source), "--standalone", "--metadata", "title=@GAME_ID@",
                    "--lua-filter", str(root / "scripts/markdown-links.lua"), "-o", str(destination)],
                   check=True, timeout=60)
(out / "index.html").write_text('<!doctype html><meta http-equiv="refresh" content="0;url=docs/index.html"><a href="docs/index.html">Game documentation</a>', encoding="utf-8")
class Links(HTMLParser):
    def __init__(self):
        super().__init__(); self.links = []; self.ids = set()
    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if "id" in attrs: self.ids.add(attrs["id"])
        if tag == "a" and "href" in attrs: self.links.append(attrs["href"])
parsed = {}
for page in out.rglob("*.html"):
    parser = Links(); parser.feed(page.read_text(encoding="utf-8")); parsed[page.resolve()] = parser
for page, parser in parsed.items():
    for link in parser.links:
        url = urlsplit(link)
        if url.scheme or url.netloc: continue
        target = (page.parent / unquote(url.path)).resolve() if url.path else page
        if not target.is_relative_to(out.resolve()) or not target.is_file():
            raise SystemExit(f"Broken local link: {page}: {link}")
        if url.fragment and target in parsed and unquote(url.fragment) not in parsed[target].ids:
            raise SystemExit(f"Missing anchor: {page}: {link}")
print(f"Built and checked {len(parsed)} pages in {out}")
