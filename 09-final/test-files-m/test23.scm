(let ((x 3) (y 5))
    x
    y
    (set! y 7)
    (let ((z y))
      y))
