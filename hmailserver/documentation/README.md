# hMailServer documentation source

This folder is the source of truth for the "latest" version of the hMailServer
manual published at https://www.hmailserver.com/documentation/latest/. It is
pulled by a daily cron job on the website server, rendered to HTML, and
published — the website repo does not maintain this content itself.

## Structure

One Markdown file per page. A page with children becomes a directory: its own
content lives in `_index.md` inside that directory, and children are sibling
`.md` files (or further subdirectories) within it.

## Front matter

Every file starts with a YAML front matter block:

```
---
id: 1
title: "Page title"
slug: page_slug
parent: parent_slug   # or "null" for a top-level page
index: 10             # sort order among siblings
is_book: false         # true for a section heading with no page content of its own
updated: 2021-08-15
---
```

- `slug` must be unique across the whole tree and match the filename (minus `.md`).
- `parent` refers to another page's `slug`, not a path.
- `id` is carried over from the legacy CMS for traceability; new pages can pick any unused integer.

## Body

Markdown after the front matter. A block of raw HTML (e.g. a table, or content
with a `style`/`class` attribute) is also allowed and is passed through
unchanged by the website's renderer.
