# Run & Paragraph Properties (`w:rPr`, `w:pPr`)

This is the mapping table between the spec and docx-lib's `docx::style` types.

## `w:rPr` — run properties

Child order in the schema: `w:rStyle`, `w:rFonts`, `w:b`, `w:bCs`, `w:i`,
`w:iCs`, `w:caps`, `w:smallCaps`, `w:strike`, `w:dstrike`, `w:outline`,
`w:shadow`, `w:emboss`, `w:imprint`, `w:noProof`, `w:snapToGrid`,
`w:vanish`, `w:webHidden`, `w:color`, `w:spacing`, `w:w`, `w:kern`,
`w:position`, `w:sz`, `w:szCs`, `w:highlight`, `w:u`, `w:effect`,
`w:bdr`, `w:shd`, `w:fitText`, `w:vertAlign`, `w:rtl`, `w:cs`, `w:em`,
`w:lang`, `w:eastAsianLayout`, `w:specVanish`, `w:oMath`.
(Space-restricted in headers/footers etc.)

| Spec element | Attribute | Values / unit | docx-lib API |
|--------------|-----------|---------------|--------------|
| `w:rFonts` | `w:ascii`, `w:hAnsi`, `w:eastAsia`, `w:cs` | font names | `RunProperties::set_ascii_font()`, `set_east_asia_font()` |
| `w:sz`, `w:szCs` | `w:val` | **half-points** (`24` = 12 pt) | `set_size(size_pt)` — API takes pt, XML stores pt×2 |
| `w:b`, `w:bCs` | `w:val` (on/off) | `1/0/true/false/on/off` | `set_bold(bool)` |
| `w:i`, `w:iCs` | `w:val` | on/off | `set_italic(bool)` |
| `w:strike` | `w:val` | on/off (`w:dstrike` = double) | `set_strike(bool)` |
| `w:u` | `w:val`, `w:color` | see below | `set_underline(type)` |
| `w:color` | `w:val` | `RRGGBB` or `auto` | via `common::Rgb` (filter `auto` first!) |
| `w:highlight` | `w:val` | ST_Highlight: `none, black, yellow, green, cyan, blue, magenta, red, darkBlue, darkCyan, darkGreen, darkMagenta, darkRed, darkYellow, darkGray, lightGray, white` | `style::HighlightColor` (1:1 match) |
| `w:vertAlign` | `w:val` | `baseline \| superscript \| subscript` | `style::RunVerticalAlign` |
| `w:smallCaps`, `w:caps` | `w:val` | on/off | (planned) |
| `w:spacing` | `w:val` | twips (character spacing) | (planned) |
| `w:w` | `w:val` | 1/2 pt (scale width) | (planned) |
| `w:shd` | fill/pattern | shading | (planned) |
| `w:bdr` | — | text border | via `style::Border` (planned) |
| `w:lang` | `w:val`, `w:eastAsia`, `w:bidi` | BCP-47 tags | (planned) |

### `w:u` values ↔ `UnderlineType`

ST_Underline common values: `none, single, double, thick, dotted, dashed,
dotDash, dotDotDash, wave` plus "heavy" variants (`dottedHeavy`,
`dashedHeavy`, …) — matching the existing `style::UnderlineType` naming
(`DottedHeavy`, `DashedHeavy`, `Wave`). Absent element = inherited from style;
`<w:u w:val="none"/>` = explicitly off (must be distinguishable from absent!).

### On/off semantics

Any `onOff`-typed attribute accepts `1 | true | on` and `0 | false | off`.
**Element presence matters**: `<w:b/>` (no `w:val`) means *enabled*; an
absent element means *inherit from style*. Model this as
`std::optional<bool>` in property structs — `false` and "not specified" are
different (classic bug source).

## `w:pPr` — paragraph properties

Schema order: `w:pStyle`, `w:keepNext`, `w:keepLines`, `w:pageBreakBefore`,
`w:framePr`, `w:widowControl`, `w:numPr`, `w:suppressLineNumbers`, `w:pBdr`,
`w:shd`, `w:tabs`, `w:suppressAutoHyphens`, `w:kinsoku`, `w:wordWrap`,
`w:overflowPunct`, `w:topLinePunct`, `w:autoSpaceDE`, `w:autoSpaceDN`,
`w:bidi`, `w:adjustRightInd`, `w:snapToGrid`, `w:spacing`, `w:ind`,
`w:contextualSpacing`, `w:mirrorIndents`, `w:suppressOverlap`, `w:jc`,
`w:textDirection`, `w:textAlignment`, `w:textboxTightWrap`, `w:outlineLvl`,
`w:divId`, `w:cnfStyle`, `w:rPr`, `w:sectPr`, `w:pPrChange`.

| Spec element | Attributes / unit | docx-lib |
|--------------|-------------------|----------|
| `w:pStyle` | `w:val` = styleId | resolves via [styles.md](styles.md) |
| `w:jc` | `w:val` = `left \| center \| right \| both \| distribute \| start \| end \| thaiDistribute` | `common::TextAlignment` |
| `w:spacing` | `w:before`/`w:after` (twips), `w:line` + `w:lineRule` (`auto` → 240ths of line, `exact`/`atLeast` → twips) | `common::LineSpacingRule` |
| `w:ind` | `w:left`/`w:right`/`w:firstLine`/`w:hanging` (twips); `w:*Chars` variants (1/100 char) | (planned `ParagraphProperties`) |
| `w:numPr` | `w:ilvl`, `w:numId` | list membership — see [numbering.md](numbering.md) |
| `w:pBdr` | `w:top/w:left/w:bottom/w:right/w:between/w:bar` each a border | `style::Border` |
| `w:shd` | fill color, pattern | (planned) |
| `w:keepNext`, `w:keepLines`, `w:pageBreakBefore`, `w:widowControl` | on/off | (planned) |
| `w:tabs` | tab stops (`w:val`, `w:pos` twips) | (planned) |
| `w:rPr` | run properties **default for the paragraph's runs** | `RunProperties` |
| `w:sectPr` | section break carried by this paragraph | (planned) |

### `w:jc` ↔ `common::TextAlignment`

| Spec `w:val` | Meaning | Library enumerator |
|--------------|---------|--------------------|
| `left` | left-aligned | `Left` |
| `center` | centered | `Center` |
| `right` | right-aligned | `Right` |
| `both` | **justified** (inter-word spacing stretched) | `Both` |
| `distribute` | distributed (stretches every line incl. last) | `Distribute` |
| `thaiDistribute` | Thai-script distributed variant | — (map to `Distribute`) |
| `start`/`end` | RTL-aware left/right | — (map by paragraph direction) |

⚠️ Design note: there is **no** `justify` literal in ST_Jc — "justified" is the
value `both`. The current `TextAlignment::Justify` enumerator has no distinct
spec counterpart; either alias it to `Both` or drop it before the API freezes,
otherwise writers will have to invent a value Word doesn't understand.

## Resolution order (critical for correct rendering)

For any property, effective value =

```
direct formatting (w:pPr/w:rPr on the element)
  > paragraph/run style chain (w:pStyle → basedOn → … → Normal)
  > w:docDefaults
  > application defaults
```

Empty element (`<w:b/>`) = force-on at that layer; `w:val="0"` = force-off;
absent = pass-through to lower layers. Implement resolution once, centrally
(see [styles.md](styles.md)) — every reader/renderer bug will trace back here
otherwise.
