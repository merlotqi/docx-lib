# Units, Namespaces, and Strict vs. Transitional

Two things bite every OOXML newcomer immediately: **every quantity has its own
unit**, and there are **two generations of namespaces**.

## Measurement units

There is no single unit system; each attribute family defines its own. Getting
these wrong by 20× or 2× is the classic OOXML bug.

| Unit | Definition | Used by | Example |
|------|------------|---------|---------|
| **twip** | 1/20 of a point = 1/1440 inch | indents (`w:ind`), spacing before/after, table/cell widths (`w:w`), page size/margins | `w:left="720"` = 0.5 in |
| **half-point** | 1/2 pt | font size (`w:sz`, `w:szCs`) | `w:sz w:val="24"` = 12 pt |
| **240ths of a line** | line spacing when `w:lineRule="auto"` | `w:spacing w:line` | `w:line="480"` = double spacing |
| **EMU** (English Metric Unit) | 914 400 EMU = 1 inch; 12 700 EMU = 1 pt | DrawingML inline/anchor shapes, images (`wp:extent`, `a:off`, `a:ext`) | `cx="914400"` = 1 in |
| **percent** | hundredths of a percent in some attrs | e.g. `w:tabs` positions (`w:val` percent), `wp:wrap*` | `val="5000"` = 50 % |
| **relative units** | 1/100 of a character/percent | `w:firstLineChars`, `w:hangingChars` (preferred by Word UI) | — |

Important line-spacing rule (`w:spacing`):

- `w:lineRule="auto"` → `w:line` is in **240ths of a line** (240 = single,
  360 = 1.5, 480 = double).
- `w:lineRule="exact"` or `"atLeast"` → `w:line` is in **twips**.

This matches `docx::common::LineSpacingRule { Auto, Exact, AtLeast }`.

Colors are 6-digit uppercase hex strings without `#` (`w:val="FF0000"`), or the
special value `auto`. Some attributes also allow `"none"`.
`docx::common::Rgb::from_hex()` expects exactly the 6-digit form — the parser
must filter out `auto`/`none` before calling it.

**Rule of thumb for conversion helpers the library will need:**
twips ↔ points (`/20`), twips ↔ inches (`/1440`), half-points ↔ points (`/2`),
EMU ↔ inches (`/914400`).

## Namespaces

### Transitional (the common case)

| Prefix | Namespace URI | What it defines |
|--------|---------------|-----------------|
| `w` | `http://schemas.openxmlformats.org/wordprocessingml/2006/main` | Core word processing elements |
| `r` | `http://schemas.openxmlformats.org/officeDocument/2006/relationships` | `r:id`, `r:embed`, `r:link` attributes |
| `wp` | `http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing` | Drawing anchors/inline |
| `a` | `http://schemas.openxmlformats.org/drawingml/2006/main` | DrawingML (shapes, fills) |
| `pic` | `http://schemas.openxmlformats.org/drawingml/2006/picture` | Picture elements |
| `w14` | `http://schemas.microsoft.com/office/word/2010/wordml` | Word 2010 extensions (`w14:checkbox`, colors, etc.) |
| `w15` | `http://schemas.microsoft.com/office/word/2012/wordml` | Word 2012+ extensions (e.g. `w15:color`) |
| `mc` | `http://schemas.openxmlformats.org/markup-compatibility/2006` | Markup compatibility (`mc:Ignorable`, `mc:AlternateContent`) |
| `cp` | `http://schemas.openxmlformats.org/package/2006/metadata/core-properties` | `docProps/core.xml` |
| `ct` | `http://schemas.openxmlformats.org/package/2006/content-types` | `[Content_Types].xml` |
| `pr` | `http://schemas.openxmlformats.org/package/2006/relationships` | `*.rels` parts |

XML parsers often see *unprefixed* default namespaces instead of `w:` prefixes;
match on **namespace URI + local name**, never on the prefixed string.

### Strict

Strict documents use the `purl.oclc.org/ooxml` family instead, e.g.:

- `http://purl.oclc.org/ooxml/wordprocessingml/main` (instead of `.../wordprocessingml/2006/main`)
- `http://purl.oclc.org/ooxml/officeDocument/2006/relationships`
- `http://purl.oclc.org/ooxml/package/2006/content-types`

Structurally Strict and Transitional are near-identical (element/attribute
names differ only by namespace). In practice ~99.9% of files are Transitional;
a robust library should at least *detect* Strict and map its namespaces to the
same internal model rather than reject the file.

## `mc:Ignorable` and forward compatibility

The root element of a part typically looks like:

```xml
<w:document
    xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
    xmlns:w14="http://schemas.microsoft.com/office/word/2010/wordml"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    mc:Ignorable="w14">
```

`mc:Ignorable="w14"` means: consumers that don't understand the `w14` namespace
may silently ignore any element/attribute in it — everything else must be
preserved. `mc:AlternateContent` provides explicit `<mc:Fallback>` branches for
mutually exclusive variants (common around drawings).

**Parser implications:**

1. Never fail on unknown elements/attributes — check `mc:Ignorable` first, but
   even without it, ignore-and-continue (or preserve verbatim) is the right
   default for round-trip fidelity.
2. Word writes new features constantly (`w15:`, `w16:`, …); a fixed whitelist
   of namespaces ages badly.

## XML conventions in OOXML parts

- Declaration is typically `<?xml version="1.0" encoding="UTF-8" standalone="yes"?>`.
- Encoding is UTF-8 in practice (UTF-16 also legal — let the XML parser decide).
- Whitespace between elements is insignificant; `w:t` text content is not
  (needs `xml:space="preserve"` to keep leading/trailing spaces — see
  [wordprocessingml-basics.md](wordprocessingml-basics.md)).
- Elements are sequence-constrained: schema-defined sibling order (e.g.
  `w:pPr` must be the **first** child of `w:p`) is enforced by Word but not by
  all producers — parsers should not require it.
