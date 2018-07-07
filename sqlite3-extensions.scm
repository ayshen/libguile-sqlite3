; Module wrapper.
;
; Usage:
;     (use-modules (experimental sqlite3-extensions))

(define-module (experimental sqlite3-extensions))

(load-extension
    "libguile-sqlite3"
    "libguile_sqlite3_init_module")
(use-modules (experimental sqlite3))


(define-public (with-sqlite3-connection-to filename callback)
    (let ((connection (sqlite3-open filename)))
        (catch #t
            (lambda () (begin
                (callback connection)
                (throw 'sqlite3-success)
            ))
            (lambda (key . args) (begin
                (sqlite3-close connection)
                (if (not (eq? key 'sqlite3-success))
                    (throw key args))))
            ; TODO
            ; pre-unwind-handler to capture the internal stack,
            ; because we will lose it when we rethrow.
        )))


(define (range-internal n result)
    (if (equal? n 0)
        (cons n result)
        (range-internal
            (- n 1)
            (cons n result))))

(define (range n) (range-internal (- n 1) '()))

(define (collect-row statement column-indices)
    (map
        (lambda (col)
            (sqlite3-column statement col))
        column-indices))

(define (collect-rows statement column-indices rows)
    (if (sqlite3-step statement)
        (collect-rows
            statement
            column-indices
            (cons
                (collect-row statement column-indices)
                rows))
        (reverse rows)))

(define (collect-all-rows statement)
    (let ((column-indices
        (range (sqlite3-column-count statement))))
        (collect-rows statement column-indices '())))

(define (bind-parameter statement n parameters)
    (if (not (null? parameters))
    (begin
        (sqlite3-bind statement n (car parameters))
        (bind-parameter
            statement
            (+ n 1)
            (cdr parameters)))))

(define (bind-parameters statement parameters)
    (bind-parameter statement 1 parameters))


(begin
(define* (sqlite3-exec connection sql
                       #:optional (parameters '()))
    (let ((statement (sqlite3-prepare connection sql)))
    (begin
        (bind-parameters statement parameters)
        (let ((results (collect-all-rows statement)))
        (begin
            (sqlite3-finalize statement)
            results
        ))
    ))
)
(export sqlite3-exec))
