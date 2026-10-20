# The OPC Package (`.docx` file anatomy)

Authoritative source: **ECMA-376 Part 2 — Open Packaging Conventions**.

## A `.docx` is a ZIP archive

```
my-document.docx        ← ZIP archive (any ZIP library can open it)
```

Normative basics:

- Any ZIP feature legal for OPC may be used: entries may be *stored* (no
  compression) or *deflated*; ZIP64 for large archives.
- Part names are **case-insensitive unique** paths of segments, using `/` as
  separator, no leading `/` in the ZIP entry name (the OPC *pack URI* form is
  `/word/document.xml`; the ZIP entry is `word/document.xml`).
  Segments must not contain `\`, `:`, `?`, `*`, etc. and must be URI-escaped
  when needed (`%20` for spaces is common: `word/media/my%20image.png`).
- **`[Content_Types].xml` must be present** at the archive root — it is the only
  mandatory part (see [content-types.md](content-types.md)).
- Unlike ODF, there is **no** special uncompressed `mimetype` entry; ZIP entry
  order is not normative for `.docx`.

## Typical layout produced by Word

```
[Content_Types].xml
_rels/.rels
docProps/core.xml          ← Dublin Core metadata (title, author, dates)
docProps/app.xml           ← application metadata (word version, page count)
word/document.xml          ← THE document part (main document part)
word/_rels/document.xml.rels
word/styles.xml            ← style definitions
word/settings.xml          ← document settings (zoom, compat, protection…)
word/fontTable.xml         ← embedded font declarations
word/theme/theme1.xml      ← theme (color scheme, fonts, effects)
word/numbering.xml         ← list definitions (only if lists used)
word/header1.xml, …        ← headers/footers
word/footer1.xml, …
word/footnotes.xml         ← footnotes/endnotes (if used)
word/comments.xml          ← tracked comments (if used)
word/media/image1.png      ← embedded images (any binary)
word/embeddings/…          ← embedded OLE objects
word/glossary/…            ← building blocks (AutoText)
customXml/…                 ← custom XML data parts (often data binding)
```

The layout is conventional, not mandated — what is mandatory/allowed is defined
by the relationship graph, not the folder names.

## Parts and relationships: the real model

OPC has exactly two building blocks:

1. **Parts** — the actual content (XML, or binary for media). A part is
   identified by its part name (`/word/document.xml`), has exactly one content
   type, and one payload stream.
2. **Relationships** — directed edges `source --type--> target`, declared in
   *relationship parts* (`*.rels`). See [relationships.md](relationships.md).

You navigate a package by following relationships, never by assuming paths:

```
/_rels/.rels
   └─ officeDocument ──► /word/document.xml          (main document part)
        ├─ styles ─────► /word/styles.xml
        ├─ numbering ──► /word/numbering.xml
        ├─ settings ───► /word/settings.xml
        ├─ theme ──────► /word/theme/theme1.xml
        ├─ fontTable ──► /word/fontTable.xml
        ├─ image ──────► /word/media/image1.png
        └─ hyperlink ──► https://example.com/        (TargetMode="External"
```

Relationship types (URI strings) are listed in
[relationships.md](relationships.md).

## Packaging rules the parser/writer must respect

- **Content types**: every part must be declared — either `Default` by
  extension or `Override` by full part name. See [content-types.md](content-types.md).
- **Root relationships** live in `/_rels/.rels`; the main document part is
  reached from there with type `officeDocument`. Every part `P` with name
  `/a/b/c.xml` stores its own outgoing relationships at `/a/b/_rels/c.xml.rels`.
- **Internal targets** in `.rels` are relative URIs resolved against the
  relationship part's location (`media/image1.png` from
  `word/_rels/document.xml.rels` → `/word/media/image1.png`), may be
  URI-escaped, and resolve like URL resolution.
- **External targets** set `TargetMode="External"` (hyperlinks, attached
  templates, linked images).
- `docProps/core.xml` and `docProps/app.xml` are optional; a package with only
  `[Content_Types].xml` + a main document part is already valid. Word tolerates
  their absence — so must we.

## Minimal valid `.docx`

Exactly three ZIP entries:

1. `[Content_Types].xml` — declaring the document part
2. `_rels/.rels` — pointing `officeDocument` → `word/document.xml`
3. `word/document.xml` — the document itself

```xml
<!-- [Content_Types].xml -->
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels"
           ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/word/document.xml"
            ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
</Types>
```

```xml
<!-- _rels/.rels -->
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1"
                Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument"
                Target="word/document.xml"/>
</Relationships>
```

```xml
<!-- word/document.xml -->
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:body>
    <w:p><w:r><w:t>Hello, world.</w:t></w:r></w:p>
    <w:sectPr/>
  </w:body>
</w:document>
```

## `.docx` vs. related formats

The ZIP/OPC machinery is identical for all Word formats; only the **main part
content type** (and `vbaProject.bin` for macro-enabled files) differs:

| Extension | Main part content type |
|-----------|------------------------|
| `.docx` | `application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml` |
| `.dotx` (template) | `application/vnd.openxmlformats-officedocument.wordprocessingml.template.main+xml` |
| `.docm` (macro) | `application/vnd.ms-word.document.macroEnabled.main+xml` |
| `.dotm` (macro template) | `application/vnd.ms-word.template.macroEnabledTemplate.main+xml` |

So a package-level reader written once serves all of them.

## Implementation notes for `docx::opc`

- Needs: a ZIP reader/writer (stored + deflate), part-name normalization, a
  content-type resolver, and a relationship index — this is the layer
  `include/docx/opc/` targets.
- Keep unknown parts byte-for-byte for lossless round-tripping.
- Validate lazily: real-world files often violate rules above (missing
  overrides, wrong relative targets, backslashes). Resolve what you can, skip
  what you cannot, surface problems as warnings — never hard failures.
