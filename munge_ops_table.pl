#!/usr/bin/env perl
use strict;

my $pattern = qr{
    ^                       # start
    (?:9E)?[[:xdigit:]]{2}  # opcode
    \s+                     # space
    \d+\n                   # cycles
    \s*                     # space
    \w+\n                   # mnemonic
    \d+                     # bytes
    \s+                     # space
    \w+                     # addressing mode
    $                       # end
}xsm;

local $/;       # turn on slurping
local $_ = <>;  # slurp file

for (/$pattern/g) {
    #my ($opcode, $cycles, $mnemonic, $bytes, $mode) = split;
    print join("|", split), "\n";
}

