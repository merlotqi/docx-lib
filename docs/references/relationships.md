# Relationships (`.rels` parts)

Relationships are the navigation graph of a package (ECMA-376 Part 2). Every
part's outgoing edges live in a sibling *relationship part*:

| Source | Relationship part |
|--------|-------------------|
| package root | `/_rels/.rels` |
| `/word/document.xml` | `/word/_rels/document.xml.rels` |
| `/word/styles.xml` | `/word/_rels/styles.xml.rels` (rarely has any) |

## Format

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1"
                Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument"
                Target="word/document.xml"/>
  <Relationship Id="rId2"
                Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/hyperlink"
                Target="https://www.example.com/"
                TargetMode="External"/>
</Relationships>
```

- `Id` — unique **within one relationship part**; IDs are *not* global and are
  frequently reused across parts with different targets. Word generates
  `rId1`, `rId2`, … but any string is legal.
- `Type` — a full URI identifying the relationship kind (table below).
- `Target` — for internal: URI **relative to the relationship part's location**
  (resolve like a URL; `../` escapes; may be percent-escaped). For external:
  an absolute URI.
- `TargetMode` — optional; only `"External"` is defined. Omitted = internal.

## Resolution algorithm (what `docx::opc` must implement)

```
resolve(source_part = "/word/document.xml", target = "../media/image1.png")
  base   = "/word/"                 (directory of the source part)
  return canonicalize("/word/" + "../media/image1.png")
        = "/media/image1.png"
```

Normalize `.`/`..` segments, keep percent-encoding consistent with part-name
comparison (part names compare case-insensitively).

## Relationship types

Base URI `http://schemas.openxmlformats.org/officeDocument/2006/relationships/`
(shorthand `…/rel/`), package-level ones under
`http://schemas.openxmlformats.org/package/2006/relationships/`:

| Type (suffix) | Where it appears | Target |
|---------------|------------------|--------|
| `officeDocument` | `/_rels/.rels` | the main document part |
| `metadata/core-properties` (package ns) | `/_rels/.rels` | `/docProps/core.xml` |
| `extended-properties` | `/_rels/.rels` | `/docProps/app.xml` |
| `styles` | `document.xml.rels` | `/word/styles.xml` |
| `numbering` | `document.xml.rels` | `/word/numbering.xml` |
| `settings` | `document.xml.rels` | `/word/settings.xml` |
| `fontTable` | `document.xml.rels` | `/word/fontTable.xml` |
| `theme` | `document.xml.rels` | `/word/theme/theme1.xml` |
| `webSettings` | `document.xml.rels` | `/word/webSettings.xml` |
| `header` / `footer` | `document.xml.rels` | `/word/header1.xml`, … |
| `footnotes` / `endnotes` | `document.xml.rels` | `/word/footnotes.xml`, … |
| `comments` | `document.xml.rels` | `/word/comments.xml` |
| `image` | `document.xml.rels`, header/footer rels | `/word/media/image1.png` |
| `hyperlink` | any, `TargetMode="External"` | `https://…` |
| `attachedTemplate` | `settings.xml.rels`, External | template path/URL |
| `package` / `oleObject` | object-related | embedded objects |

Strict-mode URIs use the `http://purl.oclc.org/ooxml/officeDocument/2006/relationships/`
base instead — treat both as the same logical type (see
[units-and-namespaces.md](units-and-namespaces.md)).

## How parts consume relationship IDs

XML parts reference relationships via the `r` namespace, e.g. in
`word/document.xml`:

```xml
<w:hyperlink r:id="rId5">…</w:hyperlink>
<w:drawing>…<a:blip r:embed="rId7"/>…</w:drawing>
<w:sectPr>…<w:headerReference r:id="rId3" w:type="default"/>…</w:sectPr>
```

Resolution: look up `rId5` **in `word/_rels/document.xml.rels`** — the
relationship part of the part *containing* the reference. `r:id` values are
meaningless without their part context.

**Writer rule**: when creating content that references a target (image,
hyperlink), allocate a fresh unused `Id` in the correct `.rels` part and emit
both sides atomically.

## Robustness notes

- `Id` uniqueness is per-part; don't build a global ID map.
- Some producers write forward-slash-less or backslash targets — normalize.
- External targets can be `mailto:`, `file:`, or nonsense — never attempt to
  fetch them during parsing; `TargetMode` decides, not the string shape.
- If a referenced `r:id` is missing from the `.rels` part, drop the referencing
  construct with a warning (Word does the same), don't fail the parse.
