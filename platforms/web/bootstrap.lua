-- Apply Bootstrap table styling without requiring Bootstrap-specific markup in Markdown.
function Table(table)
  table.classes:insert("table")
  table.classes:insert("table-hover")
  table.classes:insert("align-middle")
  return pandoc.Div(table, pandoc.Attr("", {"table-responsive"}))
end

-- Keep local Markdown links readable in source and point HTML readers at generated pages.
function Link(link)
  if link.target:match("^[%w%-]+%.md$") or link.target:match("^[%w%-]+%.md#") then
    link.target = link.target:gsub("%.md", ".html", 1)
  end
  return link
end
