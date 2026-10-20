# `[Content_Types].xml`

The single mandatory part of every OPC package. Declares the content type of
every other part. Source: **ECMA-376 Part 2**, §"Content Types".

## Structure

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <!-- By extension (case-insensitive match on the last segment's extension) -->
  <Default Extension="rels"
           ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Default Extension="png"  ContentType="image/png"/>
  <Default Extension="jpeg" ContentType="image/jpeg"/>

  <!-- By exact part name (wins over Default; must start with '/') -->
  <Override PartName="/word/document.xml"
            ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
  <Override PartName="/word/styles.xml"
            ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
</Types>
```

Rules:

1. `Default` matches by **extension**, case-insensitively; one `Default` per
   extension (later duplicates of the same extension are invalid).
2. `Override` matches the **full part name** exactly (leading `/` required) and
   takes precedence over `Default`.
3. Every part in the package must resolve to a content type via one of the two.
   Parts with no match make the package invalid — but real-world files do this;
   a tolerant reader falls back to a generic type (`application/octet-stream`
   for binary, `application/xml` for `.xml`) and continues.
4. `[Content_Types].xml` itself has no entry — it is implicit.

## Word-relevant content types

| Part | Content type |
|------|--------------|
| `word/document.xml` | `application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml` |
| `word/styles.xml` | `…wordprocessingml.styles+xml` |
| `word/numbering.xml` | `…wordprocessingml.numbering+xml` |
| `word/settings.xml` | `…wordprocessingml.settings+xml` |
| `word/fontTable.xml` | `…wordprocessingml.fontTable+xml` |
| `word/webSettings.xml` | `…wordprocessingml.webSettings+xml` |
| `word/header*.xml` | `…wordprocessingml.header+xml` |
| `word/footer*.xml` | `…wordprocessingml.footer+xml` |
| `word/footnotes.xml` | `…wordprocessingml.footnotes+xml` |
| `word/endnotes.xml` | `…wordprocessingml.endnotes+xml` |
| `word/comments.xml` | `…wordprocessingml.comments+xml` |
| `word/theme/theme*.xml` | `application/vnd.openxmlformats-officedocument.theme+xml` |
| `docProps/core.xml` | `application/vnd.openxmlformats-package.core-properties+xml` |
| `docProps/app.xml` | `application/vnd.openxmlformats-officedocument.extended-properties+xml` |
| `customXml/item*.xml` | `application/xml` (usually via `Default`) |

(`…` = `application/vnd.openxmlformats-officedocument`)

Note: `.docm`/`.dotm` macro-enabled files additionally declare
`application/vnd.ms-word.document.macroEnabled.main+xml` /
`application/vnd.ms-word.template.macroEnabledTemplate.main+xml`, and contain a
`vbaProject.bin` part (`application/vnd.ms-word.vbaProject`). See
[opc-package.md](opc-package.md).

## Why the library needs this early

- **Reader**: content type (not extension) decides how a part is parsed —
  e.g. only `…wordprocessingml.document.main+xml` (or `…template.main+xml`) is
  a WordprocessingML main document part; the relationship graph from
  `/_rels/.rels` alone can also identify it, and the two should agree.
- **Writer**: must synthesize correct `Default`/`Override` entries for every
  part it emits — Word refuses files with missing or wrong overrides.
- Represent it as a small resolver API in `docx::opc`:
  `content_type_of(part_name)` → exact override, then default-by-extension,
  then fallback.
