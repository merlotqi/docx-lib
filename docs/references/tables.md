# Tables (`w:tbl`)

Source: **ECMA-376 Part 1**, "Tables".

## Structure

```xml
<w:tbl>
  <w:tblPr>                                   <!-- table properties (first) -->
    <w:tblStyle w:val="TableGrid"/>           <!-- table style reference -->
    <w:tblW w:w="0" w:type="auto"/>           <!-- overall width -->
    <w:jc w:val="center"/>
    <w:tblBorders>
      <w:top w:val="single" w:sz="4" w:space="0" w:color="000000"/>
      <w:left w:val="single" w:sz="4" w:space="0" w:color="000000"/>
      <w:bottom …/><w:right …/>
      <w:insideH …/>                          <!-- horizontal inner lines -->
      <w:insideV …/>                          <!-- vertical inner lines -->
    </w:tblBorders>
    <w:tblCellMar>                            <!-- default cell margins -->
      <w:top w:w="0" w:type="dxa"/><w:left w:w="108" w:type="dxa"/>…
    </w:tblCellMar>
    <w:tblLayout w:type="fixed"/>             <!-- fixed | autofit -->
    <w:tblLook w:val="04A0" w:firstRow="1" w:lastRow="0"
               w:firstColumn="1" w:lastColumn="0" w:noHBand="0" w:noVBand="1"/>
  </w:tblPr>

  <w:tblGrid>                                 <!-- REQUIRED column grid -->
    <w:gridCol w:w="4680"/>                   <!-- twips -->
    <w:gridCol w:w="4680"/>
    <w:gridCol w:w="4680"/>
  </w:tblGrid>

  <w:tr>                                      <!-- row -->
    <w:trPr>…</w:trPr>                        <!-- optional, first -->
    <w:tc>                                    <!-- cell -->
      <w:tcPr>                                <!-- optional, first -->
        <w:tcW w:w="4680" w:type="dxa"/>      <!-- dxa = twips | pct | auto -->
        <w:gridSpan w:val="2"/>               <!-- horizontal merge: spans 2 cols -->
        <w:vMerge w:val="restart"/>           <!-- vertical merge: start -->
        <w:shd w:val="clear" w:color="auto" w:fill="D9E2F3"/>
        <w:vAlign w:val="center"/>
        <w:tcBorders>…</w:tcBorders>
      </w:tcPr>
      <w:p><w:r><w:t>Cell</w:t></w:r></w:p>
      <!-- each cell holds block content: w:p and nested w:tbl allowed -->
    </w:tc>
    …
  </w:tr>
  …
</w:tbl>
```

## Invariants (and how real files bend them)

| Rule | Reality |
|------|---------|
| `w:tblGrid` required, one `w:gridCol` per grid column | some producers omit/desync it — rebuild from `tcW`+`gridSpan` if missing |
| Every `w:tc` covers `gridSpan` columns starting at its position | cells may *under*-fill a row (row ends early) — handle gracefully |
| Every `w:tc` must contain **at least one `w:p`** (even when empty) | nested tables must be followed by an empty `<w:p/>` after the `w:tbl` inside the cell — Word inserts it; keep it |
| `w:tr` children order: `trPr`, then cells | tolerate otherwise |
| Row heights: `w:trHeight w:val` (twips), `w:hRule = atLeast \| exact \| auto` | — |

## Merges

- **Horizontal**: `w:tcPr/w:gridSpan w:val="N"` — cell occupies N grid
  columns; no continuation cells exist for it.
- **Vertical**: first cell has `<w:vMerge w:val="restart"/>`; each cell below
  it in the same column(s) has `<w:vMerge/>` (or `w:val="continue"`) **and
  still exists** with its own (usually empty) content. `w:vMerge` without
  `w:val` = continue. Truncated `vMerge` chains (restart without continues)
  occur in the wild.

Grid math: cell x-positions come from `w:tblGrid`, not from cell widths.
Column width = sum of covered `gridCol` widths (fixed layout); in autofit
layout Word re-derives widths — record `gridCol` regardless.

## Property tables (spec ↔ docx-lib)

`w:tblPr` essentials: `w:tblW` (`dxa|pct|auto`, `pct` = 50ths of a percent),
`w:jc`, `w:tblBorders`, `w:tblCellMar`, `w:tblLayout`, `w:tblStyle`,
`w:tblLook`, `w:tblInd` (twips), `w:tblCellSpacing`, `w:tblCaption`,
`w:tblDescription`.

`w:trPr` essentials: `w:trHeight`, `w:cantSplit`, `w:tblHeader` (repeat header
row), `w:jc`, `w:cnfStyle`.

`w:tcPr` essentials: `w:tcW`, `w:gridSpan`, `w:vMerge`, `w:shd`, `w:vAlign`,
`w:tcBorders`, `w:tcMar`, `w:textDirection`, `w:tcFitText`, `w:hideMark`.

Borders (`w:tcBorders`/`w:tblBorders`/`w:pBdr` children) share the border
attribute set:

```xml
<w:top w:val="single" w:sz="8" w:space="0" w:color="4472C4"/>
```

- `w:val` → `style::BorderStyle` (`nil`, `none`, `single`, `thick`, `double`,
  `dotted`, `dashed`, `dotDash`, `dotDotDash`, `triple`, …). `nil` = no line
  but reserve space/behavior differs from `none` — keep both!
- `w:sz` → **eighths of a point** (`8` = 1 pt). Note: `Border::size_pt` in
  docx-lib stores points — convert `sz / 8.0f`.
- `w:space` → padding above/border in points (only for text borders).
- `w:color` → `RRGGBB` (or `auto`/`none`).

## Mapping to docx-lib

| OOXML | docx-lib |
|-------|----------|
| `w:tbl` | `Table` (`include/docx/model/table.hpp`) |
| `w:tr` | row container inside `Table` (planned) |
| `w:tc` | `Cell` (`cell.hpp`) |
| cell block content | children nodes of `Cell` (paragraphs/tables) |
| `w:tblPr`/`w:tcPr` borders | `style::Border` |

Planned additions: row/cell storage in `Table`, `Cell` as `ContainerNode`,
merge-aware column computation, and `gridSpan`/`vMerge` state on `Cell`.
