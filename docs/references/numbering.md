# Numbering (lists) (`word/numbering.xml`)

Source: **ECMA-376 Part 1**, "Numbering".

## Two-level indirection

```
paragraph (w:pPr/w:numPr/w:numId)
    └─► w:num[@w:numId]  ──►  w:abstractNum[@w:abstractNumId]   (the template)
                                    └─► w:lvl[@w:ilvl = 0..8]   (per-level config)
```

- **`abstractNum`** defines *how* a list looks (format, text pattern, indent)
  and can be shared by many documents-level `num` instances.
- **`num`** is the concrete instance bound to a `numId`; it may *override* an
  abstract level via `w:lvlOverride` (e.g. restart numbering at 1).

## Example

```xml
<w:numbering xmlns:w="…wordprocessingml/2006/main">
  <w:abstractNum w:abstractNumId="0">
    <w:nsid w:val="1A2B3C4D"/>
    <w:multiLevelType w:val="hybridMultilevel"/>
    <w:tmpl w:val="…"/>
    <w:lvl w:ilvl="0">
      <w:start w:val="1"/>                    <!-- restart value -->
      <w:numFmt w:val="decimal"/>             <!-- 1, 2, 3 … -->
      <w:lvlText w:val="%1."                  <!-- pattern: %1=ilvl0 counter -->
                 />
      <w:lvlJc w:val="left"/>                 <!-- justification -->
      <w:pPr><w:ind w:left="720" w:hanging="360"/></w:pPr>   <!-- twips -->
      <w:rPr>…</w:rPr>                        <!-- run formatting of the marker -->
    </w:lvl>
    <w:lvl w:ilvl="1">
      <w:start w:val="1"/>
      <w:numFmt w:val="lowerLetter"/>
      <w:lvlText w:val="%2."                  <!-- %2 = level-1 counter -->
                 />
      <w:lvlRestart w:val="1"/>               <!-- restart when level 0 changes -->
      <w:pPr><w:ind w:left="1440" w:hanging="360"/></w:pPr>
    </w:lvl>
    <!-- … ilvl 0..8 … -->
  </w:abstractNum>

  <w:num w:numId="1">
    <w:abstractNumId w:val="0"/>
  </w:num>

  <!-- second list sharing the same template but independent counters -->
  <w:num w:numId="2">
    <w:abstractNumId w:val="0"/>
    <w:lvlOverride w:ilvl="0">
      <w:startOverride w:val="1"/>            <!-- restart numbering -->
    </w:lvlOverride>
  </w:num>
</w:numbering>
```

Paragraph using it:

```xml
<w:p>
  <w:pPr>
    <w:numPr>
      <w:ilvl w:val="0"/>
      <w:numId w:val="1"/>
    </w:numPr>
  </w:pPr>
  <w:r><w:t>First item</w:t></w:r>
</w:p>
```

## Key attributes

| Element/attr | Meaning | Notes |
|--------------|---------|-------|
| `w:numFmt/@w:val` | counter style | `decimal`, `lowerLetter`, `upperLetter`, `lowerRoman`, `upperRoman`, `bullet`, `none`, `decimalZero`, `chicago`, `ideographDigital`, … |
| `w:lvlText/@w:val` | rendered text pattern | `%1`=level 0, `%2`=level 1, …; literal text around placeholders (`"a)"`, `"Chapter %1"`) |
| `w:start/@w:val` | initial counter | |
| `w:lvlRestart/@w:val` | restart this level when level *n* changes | 1-based: `1` = restart when ilvl 0 changes |
| `w:lvlJc` | marker justification | `left \| center \| right` |
| `w:suff` | separator after marker | `tab \| space \| nothing` |
| `w:isLgl` | render lower levels as decimal | legal-style numbering |
| `w:pStyle` | (on `w:lvl`) binds a level to a paragraph style | makes `ListParagraph`-style-driven lists |
| bullet levels | `numFmt="bullet"` + `w:lvlText w:val="•"` + `w:rFonts` | marker is a glyph from a font |
| `w:lvlPicBulletId` | picture bullets | needs `w:lvlPicBullet` definition |
| `w:tentative` | on `w:lvlOverride` … | rare |

## Reader algorithm

1. Index `abstractNum` by `abstractNumId`, `num` by `numId`.
2. For each paragraph with `w:numPr`: get `numId → abstractNumId`, pick
   `w:lvl[@w:ilvl]`, apply `lvlOverride/startOverride` if present.
3. **Counting state is document-order dependent**: maintain per-`numId`
   counters keyed by level; increment on each paragraph using that level;
   reset deeper levels when an upper level increments (per `lvlRestart`).
   Paragraphs *without* `numPr` still continue the list if they follow a
   numbered paragraph and inherit `numId` through a style (`w:pStyle` →
   `w:lvl/w:pStyle` binding) — Word does this for `ListParagraph`.
4. Unknown `numId` → treat as unnumbered (don't fail).

## Writer algorithm

1. Allocate `abstractNumId`/`numId` not used in the package.
2. Emit `w:abstractNum` with one `w:lvl` per used level.
3. Emit `w:num` (+ `startOverride` when a list must restart at 1).
4. Add `w:numPr` to each paragraph (`ilvl` + `numId`).
5. Remember: `word/_rels/document.xml.rels` must contain the `numbering`
   relationship and `[Content_Types].xml` the override — see
   [relationships.md](relationships.md).

## docx-lib status

Not yet started (no `include/docx/` counterpart). When implemented, place it
next to styles as a document-level resource (`Numbering` registry analogous to
`StyleRegistry`), since it also needs document-order counting state that does
not fit into the static tree.
