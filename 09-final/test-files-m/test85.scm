(let ((x 1) (y 2) (z 3))
    (let ((y 4))
        (set! y 5)
        (set! x (+ y 1))
      (+ x y z)))
