# Reference Documentation

Curated reference material for implementing a full `.docx` (Office Open XML /
OOXML) parsing and writing library. These notes are summaries written for the
docx-lib implementers — they are **not** a substitute for the specifications,
which are the canonical source (see below).

## Reading order (mapped to library components)

| # | Document | Covers | Relevant library code |
|---|----------|--------|-----------------------|
| 1 | [units-and-namespaces.md](units-and-namespaces.md) | Namespaces, measurement units, Strict vs. Transitional | `include/docx/common/` |
| 2 | [opc-package.md](opc-package.md) | The `.docx` ZIP container, part anatomy, package layout | `include/docx/opc/` |
| 3 | [content-types.md](content-types.md) | `[Content_Types].xml` | `include/docx/opc/package.hpp` |
| 4 | [relationships.md](relationships.md) | `.rels` relationship parts, `r:id` references | `include/docx/opc/` |
| 5 | [wordprocessingml-basics.md](wordprocessingml-basics.md) | `document.xml`, the element tree, paragraphs/runs/text | `include/docx/model/` |
| 6 | [run-and-paragraph-properties.md](run-and-paragraph-properties.md) | `w:pPr` / `w:rPr` and their mapping to docx-lib enums | `include/docx/style/` |
| 7 | [styles.md](styles.md) | `styles.xml`, style inheritance and resolution | `include/docx/style/` |
| 8 | [tables.md](tables.md) | `w:tbl` / `w:tr` / `w:tc` | `include/docx/model/table.hpp`, `cell.hpp` |
| 9 | [numbering.md](numbering.md) | `numbering.xml`, lists and counters | (planned) |

## Canonical specifications

- **ECMA-376** (free PDFs, current = 5th edition, 2021):
  <https://ecma-international.org/publications-and-standards/standards/ecma-376/>
  - Part 1 — *Fundamentals and Markup Language Reference* (WordprocessingML,
    DrawingML, SpreadsheetML element reference)
  - Part 2 — *Open Packaging Conventions* (ZIP/OPC package model)
  - Part 3 — *Marking Compatibility and Extensions* (`mc:` markup compatibility)
  - Unofficial convenience mirror of the free PDFs:
    <https://github.com/QtExcel/ecma-376-5th>
- **ISO/IEC 29500** (the ISO version of OOXML; Transitional + Strict variants;
  paid): <https://www.iso.org/standard/71691.html> (Part 1)
- **[MS-OI29500]** — Microsoft's free *Office Implementation Notes* for
  ISO 29500, including the Word 2010+ `w14`/`w15` extension schemas:
  <https://learn.microsoft.com/en-us/openspecs/office_standards/ms-oi29500/>
- **[MS-OFFSTANDLP]** — Microsoft Office standards support list (which
  specification versions/errata Word implements):
  <https://learn.microsoft.com/en-us/openspecs/office_standards/ms-offstandlp/>
- **ooxml.dev Spec Explorer** — browsable, searchable rendering of the ECMA-376
  schemas (extremely useful for looking up an element/attribute quickly):
  <https://ooxml.dev/spec/>

## Implementation references (prior art)

Studying these mature implementations is often faster than reading the spec
end-to-end:

- [dotnet/Open-XML-SDK](https://github.com/dotnet/Open-XML-SDK) — Microsoft's
  official SDK; generated strongly-typed DOM for every part.
- [apache/poi](https://poi.apache.org/) — Java, `XWPF` package for `.docx`.
- [python-docx](https://github.com/python-openxml/python-docx) — small, very
  readable Python implementation; great for understanding the object model.
- [docx4j](https://www.docx4j.org/) — Java, JAXB-based OXML model.
- [LibreOffice](https://dev.libreoffice.org/) — `oox` import/export filter
  (handles messy real-world files).

## Ground rules for this implementation

1. **Spec first**: when behavior is ambiguous, ECMA-376 Part 1/2 wins; then
   [MS-OI29500] for how Word actually behaves.
2. **Be liberal in what you accept**: real-world files violate the schemas
   routinely (files produced by third-party tools, hand-edited XML). Unknown
   elements/attributes must be ignored or preserved, never fatal.
3. **Round-trip fidelity matters**: a "fully functional" parser should keep
   parts/properties it does not understand instead of dropping them.
4. Respect `mc:Ignorable` — listed extension namespaces may appear anywhere and
   must not break parsing (see [units-and-namespaces.md](units-and-namespaces.md)).

> **Licensing note**: the specifications are copyrighted by ECMA/ISO/Microsoft.
> These notes only summarize and link to them; do not copy spec text or schemas
> into this repository.
