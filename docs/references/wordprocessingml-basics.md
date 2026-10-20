# WordprocessingML Basics (`word/document.xml`)

Source: **ECMA-376 Part 1**, WordprocessingML section.

## Document element tree

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
  <w:body>
    <w:p>                                <!-- paragraph -->
      <w:pPr>…</w:pPr>                   <!-- optional, MUST be first child -->
      <w:r>                              <!-- run -->
        <w:rPr>…</w:rPr>                 <!-- optional, MUST be first child -->
        <w:t xml:space="preserve">Text</w:t>
      </w:r>
    </w:p>
    <w:tbl>…</w:tbl>                     <!-- table -->
    <w:sectPr>…</w:sectPr>               <!-- final section properties (last child of body) -->
  </w:body>
</w:document>
```

Body content model (`w:body`): a sequence of block-level elements
(`w:p`, `w:tbl`, `w:sdt`, `w:altChunk`, proofing markers) followed by an
optional final `w:sectPr`. Section properties for every section *except the
last* live inside the last paragraph's `w:pPr/w:sectPr`.

## Paragraph content

`w:p` may contain:

- `w:r` — plain runs
- `w:hyperlink` — contains runs; target via `r:id`
- `w:bookmarkStart` / `w:bookmarkEnd` — zero-width markers
- `w:proofErr`, `w:permStart/End`, `w:ins`, `w:del` — proofing/permission/
  revision markers (`w:ins`/`w:del` wrap runs for track changes)
- `w:sdt` — structured document tag (content control) wrapper
- `w:fldSimple`, `w:instrText` — fields (TOC, page numbers, …)
- `w:commentRangeStart/End`, `m:oMath` — comments, math

Everything that renders text ultimately bottoms out in **runs**.

## Run structure

```xml
<w:r>
  <w:rPr>                                <!-- run properties, first -->
    <w:b/><w:i/><w:sz w:val="24"/>
    <w:rFonts w:ascii="Calibri" w:eastAsia="等线"/>
    <w:color w:val="2F5496"/>
  </w:rPr>
  <w:t>Text content</w:t>                <!-- text -->
</w:r>
```

A run may carry other payloads instead of / besides `w:t`:

| Element | Meaning |
|---------|---------|
| `w:t` | text |
| `w:br` | line break (typed Enter+Shift) |
| `w:cr` | carriage break (same rendering, different semantics) |
| `w:tab` | tab character |
| `w:softHyphen`, `w:noBreakHyphen` | hyphenation controls |
| `w:sym` | symbol from a specific font codepoint |
| `w:drawing`, `w:pict`, `w:object` | image/shape/OLE payload |
| `w:footnoteReference`, `w:endnoteReference`, `w:commentReference` | anchors |
| `w:instrText`, `w:delText`, `w:delInstrText` | field codes / deleted text |

### Text whitespace

XML parsers collapse whitespace by default. Leading/trailing spaces inside
`w:t` are only preserved when the producer writes
`xml:space="preserve"`:

```xml
<w:t xml:space="preserve">  padded  </w:t>
```

Word **always** writes it when needed; some producers always write it. The
library should honor `xml:space="preserve"` and, defensively, keep interior
whitespace of `w:t` regardless (collapsing only at parse boundaries you
control). Conversely, whitespace *between block elements* in the XML source is
irrelevant and must be discarded.

Adjacent `w:r` elements with identical properties are common (one per
keystroke pattern); joining them is an optional optimization, but
**splitting** runs on write is required when applying mixed formatting.

## Minimal and realistic examples

Paragraph with style + direct formatting:

```xml
<w:p>
  <w:pPr>
    <w:pStyle w:val="Heading1"/>
    <w:jc w:val="center"/>
  </w:pPr>
  <w:r><w:rPr><w:b/></w:rPr><w:t>Title</w:t></w:r>
</w:p>
```

Hyperlink:

```xml
<w:hyperlink r:id="rId5" w:history="1">
  <w:r><w:rPr><w:u w:val="single"/><w:color w:val="0563C1"/></w:rPr>
    <w:t>Example</w:t></w:r>
</w:hyperlink>
```

Break:

```xml
<w:r><w:br/></w:r>
```

## Section properties (overview)

```xml
<w:sectPr>
  <w:pgSz w:w="11906" w:h="16838"/>                 <!-- A4 in twips -->
  <w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440" …/>
  <w:cols w:space="708"/>
  <w:headerReference r:id="rId3" w:type="default"/>
  <w:footerReference r:id="rId4" w:type="default"/>
</w:sectPr>
```

Page geometry is in twips; multiple sections allow mixed orientation/margins.

## Mapping to the docx-lib object model

Current model in `include/docx/model/`:

| OOXML | docx-lib | Notes |
|-------|----------|-------|
| `w:body` children | `ContainerNode` children of `Document` | ordered block list |
| `w:p` | `Paragraph : ContainerNode` | fluent setters (`set_alignment`, `set_bold`, …) |
| `w:r` | `Run` (planned) | belongs to `run_context`/formatting pipeline |
| `w:t` | `Text` (planned) | must record `xml:space` |
| `w:tbl` | `Table` (planned) | contains `Cell`s |
| `w:pPr` / `w:rPr` | `ParagraphProperties` / `RunProperties` | see [run-and-paragraph-properties.md](run-and-paragraph-properties.md) |
| visiting | `Node::accept(NodeVisitor&)` | use the visitor for serialization/queries |

Design implications:

1. Preserve **document order** strictly — block order and run order are the
   rendering.
2. Keep a per-node link to its source (part name + XPath-ish path or raw XML
   node) so unknown constructs can be round-tripped.
3. `w:p` may be completely empty (`<w:p/>` = blank line) — that's legal and
   meaningful (spacing, numbering).
4. Styles (`w:pStyle`/`w:rStyle`) are *references*; resolved later per
   [styles.md](styles.md).
