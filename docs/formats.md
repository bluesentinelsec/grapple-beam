---
title: Formats
description: "JSON, TOML, and YAML parsing — the three data formats games actually need, statically built with their native APIs unchanged."
---

# Formats — `Grapple::Formats`

Static-only builds of the three data-format parsers games actually use —
level files, configs, and everything ini-shaped. All plain C, MIT, zero
external dependencies (not even SDL). The native upstream APIs are
exposed unchanged.

| Format | Library | Version | Header |
|--------|---------|---------|--------|
| JSON | cJSON | v1.7.19 | `<cJSON.h>` |
| TOML | tomlc99 | pinned commit | `<toml.h>` |
| YAML | libyaml | 0.2.5 | `<yaml.h>` |

For engine/player preferences use the [typed settings API](cli-implementation.md),
which validates canonical keys and values. These parsers are for your own data
formats; they do not automatically apply settings to an engine.

```c
#include <cJSON.h>

cJSON *level = cJSON_Parse(text);
if (level) {
    const cJSON *hp_value = cJSON_GetObjectItemCaseSensitive(level, "hp");
    if (cJSON_IsNumber(hp_value)) {
        double hp = hp_value->valuedouble;
        /* Validate your game's range before using hp. */
        (void)hp;
    }
    char *out = cJSON_Print(level);
    if (out) { /* Write or consume out here. */ cJSON_free(out); }
    cJSON_Delete(level);
}
```

A failed parse returns NULL. cJSON item pointers borrow the document lifetime.
With tomlc99, check `toml_parse` and each datum's `ok` field, free returned strings
with `free`, and release tables with `toml_free`. With libyaml, check parser/event
results and pair each successful initialization with its matching delete call.

The [Tiled](tiled.md) module uses cJSON internally to validate map
files before parsing; C++ gets a RAII `JsonDocument` owner; Lua and Ruby
get the `JSON.*` module — see [C++](cpp.md) and
[Scripting](scripting.md).

Provenance for all three:
[`deps/formats.md`](https://github.com/bluesentinelsec/grapple-beam/blob/main/deps/formats.md).
