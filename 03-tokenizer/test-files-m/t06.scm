(define valid-bst-node?
  (lambda (bst)
    (or (null? bst)
        (and (left bst) (right bst)))))
