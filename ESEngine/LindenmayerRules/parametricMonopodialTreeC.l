define depth 8
define initialLength 1.5
define initialRadius 1.5
define angle 34.37

define r1 0.9
define r2 0.8
define a0 45
define a2 45
define d 137.5
define wr 0.707
A(4,1.5)
A(s,w) =(w)F(s)[^(a0)B(s*r2,w*wr)]+(d)A(s*r1,w*wr)
B(s,w) =(w)F(s)[<(a2)C(s*r2,w*wr)]C(s*r1,w*wr)
C(s,w) =(w)F(s)[>(a2)B(s*r2,w*wr)]B(s*r1,w*wr)