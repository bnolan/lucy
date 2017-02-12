
print "starting up..."

t = 0
x = 0

children = Element:childNodes()

function tick (delta)
  t = t + (delta / 1000)

  -- Cycle at 0.25hz
  i = t * 3.14 * 0.25

  -- Cycle at 1hz
  x = math.sin(i) * 10 - 5
  y = math.cos(i * 3 / 2) * 5 + 1.5

  Element:setAttribute("position", string.format("%.2f %.2f 3", x, y))
  Element:setAttribute("color", "#f0a")

  print(string.format("%d childNodes", #children))
end

-- tick()
