(letrec ((f (lambda (x) (if (< x 1) 0 (f (+ x -1))))))
  (f 3))
