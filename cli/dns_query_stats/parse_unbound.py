#!/usr/bin/env python2

# Record: [1466512058] unbound[17201:0] info: 10.10.10.10 some.domain.com. A IN

stats={}
with open('unbound.log') as f:
    for line_terminated in f:
        line = line_terminated.rstrip('\n')
        pieces = line.split()

        if pieces[2] == 'info:':
            if pieces[4] in stats:
                stats[pieces[4]] += 1
            else:
                stats[pieces[4]] = 1

filter = [ 'google', 'youtube' ]
for s in sorted(stats, key = stats.get, reverse = True):
    ok = True
    for f in filter:
        if f in s:
            ok = False

    if ok:
        print s, stats[s]
