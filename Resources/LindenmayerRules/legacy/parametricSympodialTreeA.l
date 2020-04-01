define depth 6
define initialLength 1.5
define initialRadius 1.2
define angle 34.37

define r1 0.9
define r2 0.7
define a1 5
define a2 65
define wr 0.707
A(4,1)
A(s,w) =(w)F(s)[^(a1)B(s*r2,w*wr)]<(90)[+(a2)B(s*r2,w*wr)]
B(s,w) =(w)F(s)[<(a1)$B(s*r1,w*wr)][>(a2)$B(s*r1,w*wr)]