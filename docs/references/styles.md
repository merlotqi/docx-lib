# Styles (`word/styles.xml`)

Source: **ECMA-376 Part 1**, "Styles".

## Structure

```xml
<w:styles xmlns:w="…wordprocessingml/2006/main">
  <w:docDefaults>                       <!-- defaults for ALL text -->
    <w:rPrDefault>
      <w:rPr><w:rFonts w:ascii="Calibri" w:eastAsia="等线" w:hAnsi="Calibri"/>
             <w:sz w:val="22"/><w:lang w:val="en-US"/></w:rPr>
    </w:rPrDefault>
    <w:pPrDefault>
      <w:pPr><w:spacing w:after="160" w:line="259" w:lineRule="auto"/></w:pPr>
    </w:pPrDefault>
  </w:docDefaults>

  <w:style w:type="paragraph" w:default="1" w:styleId="Normal">
    <w:name w:val="Normal"/>
    <w:uiPriority w:val="0"/>
    <w:qFormat/>                        <!-- shown in Quick Style gallery -->
  </w:style>

  <w:style w:type="paragraph" w:styleId="Heading1">
    <w:name w:val="heading 1"/>
    <w:basedOn w:val="Normal"/>         <!-- inheritance parent -->
    <w:next w:val="Normal"/>            <!-- style of the NEXT paragraph -->
    <w:link w:val="Heading1Char"/>      <!-- matching character style -->
    <w:uiPriority w:val="9"/>
    <w:qFormat/>
    <w:pPr><w:keepNext/><w:outlineLvl w:val="0"/><w:spacing …/></w:pPr>
    <w:rPr><w:b/><w:sz w:val="32"/><w:color w:val="2F5496"/></w:rPr>
  </w:style>

  <w:style w:type="character" w:styleId="Strong">
    <w:name w:val="Strong"/><w:basedOn w:val="DefaultParagraphFont"/>
    <w:rPr><w:b/></w:rPr>
  </w:style>

  <w:style w:type="numbering" w:styleId="…">…</w:style>
  <w:style w:type="table" w:styleId="TableGrid">…</w:style>

  <w:latentStyles w:defLockedState="0" w:defUIPriority="99"
                  w:defSemiHidden="1" w:defUnhideWhenUsed="1" w:defQFormat="0">
    <w:lsdException w:name="heading 2" w:uiPriority="9" w:qFormat="1"/>
    …
  </w:latentStyles>
</w:styles>
```

## Key concepts

- **`w:styleId`** — the identifier referenced by `w:pStyle/@w:val`,
  `w:rStyle/@w:val`, `w:tblStyle/@w:val`. Not necessarily unique across
  types in broken files; Word treats type+ID as the key.
- **`w:type`** — `paragraph | character | numbering | table`.
  Paragraph and character styles can be paired (`w:link`) so applying one
  implies the other.
- **`w:name`** — the human-visible name (localized); UI uses this, XML
  references use `styleId`. They often differ (`Heading1` vs. `heading 1`).
- **`w:basedOn`** — inheritance chain; cycles are invalid but occur in the
  wild (guard against infinite loops!). Chain ends at implicit
  `Normal`/`DefaultParagraphFont` (not always declared explicitly).
- **`w:next`** — style auto-applied to the following paragraph after typing in
  this style (editor concern; a reader may ignore).
- **`w:default="1"`** — the default style of its type (usually `Normal`).
- **`w:qFormat`**, **`w:uiPriority`**, **`w:semiHidden`**,
  **`w:unhideWhenUsed`**, **`w:locked`** — UI presentation metadata.
- **`w:latentStyles`** — metadata about styles *not* explicitly present but
  creatable on demand (large list of `lsdException`s). Purely informational
  for a parser — safe to ignore.

Word's shipped defaults: `Normal`, `Heading1`…`Heading9`, `Title`,
`Subtitle`, `Quote`, `IntenseQuote`, `ListParagraph`, `TableGrid`, plus
character styles `DefaultParagraphFont`, `Strong`, `Emphasis`, `Hyperlink`.
Don't assume they exist — always fall back through `basedOn` → docDefaults.

## Style application in document XML

```xml
<w:p><w:pPr><w:pStyle w:val="Heading1"/></w:pPr>…</w:p>
<w:r><w:rPr><w:rStyle w:val="Strong"/></w:rPr>…</w:r>
<w:tbl><w:tblPr><w:tblStyle w:val="TableGrid"/></w:tblPr>…</w:tbl>
```

## Effective-value algorithm

```
resolve(element, property P):
    if element.rPr/pPr directly specifies P (present, incl. <X/> = on)
        → use it   (false via w:val="0" is ALSO a direct spec)
    for style in [styleId chain via basedOn, … , default style of type]:
        if style defines P → use it
    if docDefaults defines P → use it
    → application default (schema defaults / Word defaults)
```

Per-property only; a style that omits P passes through to the next layer.

## Implementation notes for docx-lib

- `include/docx/style/` holds property structs; add a `StyleRegistry` that:
  1. parses `styles.xml` into a map `styleId → {type, basedOn, pPr, rPr}`;
  2. pre-flattens each style's inherited properties (with cycle guard);
  3. exposes `effective_paragraph_properties(pStyle, direct_pPr)`.
- Character styles apply to runs, paragraph styles to paragraphs, table styles
  to table/cell — but **direct formatting always wins**, including "explicit
  off" (`<w:b w:val="0"/>` overrides a style's `<w:b/>`).
- Unknown `w:styleId` references (missing style) must degrade gracefully to
  the next layer — happens frequently in truncated files.
- Missing `styles.xml` entirely is legal: fall back to docDefaults (which also
  live in the missing part → application defaults).
