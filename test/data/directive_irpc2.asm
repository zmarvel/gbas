; termintated .irp with substitution symbol preceded directly by another
; character that should not be escaped. Should add 10, 11, and 12 to reg a.
.irpc name,012
add a, 1\name
.endr
