define depth 1
define initialLength 1.5
define initialRadius 0.6
define angle 34.37

define d1 94.74		/* divergence angle 1 */
define d2 132.63	/* divergence angle 2 */
define a 18.95		/* branching angle */
define lr 1.109		/* elongation rate */
define vr 1.132		define vr 1.732		/* width increase rate */
define startLength 1.2

B(1)F(startLength)+(45)A
A B(vr)F(startLength)[^(a)F(startLength)A]+(d1)[^(a)F(startLength)A]+(d2)[^(a)F(0.5)A]
F(l) F(l*lr)
B(w) B(w*vr)