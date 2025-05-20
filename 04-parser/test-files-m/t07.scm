(define best-accumulate
  (lambda (n)
    (/ (* n (- n 1) 2))))

(define ok-accumulate
  (lambda (n partial)
    (if (= n 0)
        partial
        (ok-accumulate (n-1) (+ n partial)))))
(define worst-accumulate
  (lambda (n)
    (if (= n 0)
        0
        (accumulate))))
