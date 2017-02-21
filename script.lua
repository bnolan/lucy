
print "starting up..."

print("We have root element <" .. Element:nodeName() .. " />")

t = 0
x = 0

Element:setAttribute("position", "0 0 -10")

children = Element:childNodes()

colors = { '#f0a', '#0af', '#0fa' }

function tick (delta)
  t = t + (delta / 1000)

  -- Cycle at 0.25hz
  i = t * 3.14 * 0.25

  for index, node in ipairs(children) do 
    i = i + 0.4
    x = math.sin(i) * 4
    y = math.cos(i * 3 / 2) * 2 + 2
    z = math.cos(i / 5) * 0.5

    if node:nodeName() == 'a-box' then
      node:setAttribute("position", string.format("%.2f %.2f %.2f", x, y, z))
      node:setAttribute("color", colors[index])
    end
  end

end

-- tick()
