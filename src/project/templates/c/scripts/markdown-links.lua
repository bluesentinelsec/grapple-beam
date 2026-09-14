function Link(link)
  link.target = link.target:gsub('%.md(#.*)$', '.html%1'):gsub('%.md$', '.html')
  return link
end
