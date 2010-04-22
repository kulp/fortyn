#!/usr/bin/env perl
use strict;
use warnings;

my $x = qr/(?:[[:xdigit:]]{2})/;

while (<>) {
    next unless /^S[123]/;
    my ($type, $hcount, $rest) = /^S (\d) ($x) ($x+) \s*$/x;
    my $count = ord pack "H2", $hcount;
    my $abytes = $type + 1;
    my $dbytes = $count - $abytes - 1;
    my ($addr, $data, $check) =
        $rest =~ /($x {$abytes}) ($x {$dbytes}) ($x) \s*$/x;
    #print $type; print $addr; print $data; print $check;
    print "[0x$addr] = ", (join ", ", map "0x$_", grep $_, split /(..)/,
            $data), ",\n";
}

