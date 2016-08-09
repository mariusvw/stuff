#!/usr/bin/env python2

# Record: Jun 21 16:52:26 dnsmasq[18562]: query[A] some.domain.com from 10.10.10.10

stats={}
with open('dnsmasq.log') as f:
    for line_terminated in f:
        line = line_terminated.rstrip('\n')
        pieces = line.split()

        if pieces[4] == 'query[A]' or pieces[4] == 'query[AAAA]':
            domain = '.'.join(pieces[5].split('.')[-2:])
            if domain in stats:
                stats[domain] += 1
            else:
                stats[domain] = 1

filter = [ 'google' ]
for s in sorted(stats, key = stats.get, reverse = True):
    ok = True
    for f in filter:
        if f in s:
            ok = False

    if ok and stats[s] > 10:
        print s, stats[s]
