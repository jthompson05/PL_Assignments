;; Knuth test.
(define a
  (lambda (k x1 x2 x3 x4 x5)
    (letrec ((b
              (lambda ()
                (set! k (+ k -1))
                (a k b x1 x2 x3 x4))))
      (if (< k 1)
          (+ (x4) (x5))
          (b)))))

(a 10 (lambda () 1) (lambda () -1)
   (lambda () -1) (lambda () 1)
   (lambda () 0))
