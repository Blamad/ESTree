#depth, initialLength, initialRaidus, angle, axiom, rules
define depth 5
define initialLength 3
define initialRadius 0.6
define angle 15
fb
b >fa .2
b <fa .2
b ^+fa .2
b -vfa .2
b f[--vvfa][+^fa] .1
b f[-vfa][++^^fa] .1
a ">[<<f(4.5)a][^^++f(4.5)a][vv--f(4.5)a][>>f(4.5)a] .25
a "<[<<f(4.5)a][>>f(3)a] .25
a "^+[^^++f(3)a][vv--f(4.5)a] .25
a "v-[>>>f(4.5)a][++++++++>>>f(4.5)a][-------->>>f(4.5)a] .25