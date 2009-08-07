#!/usr/bin/env perl
use strict;
use List::Util qw(max);

my $base = shift || "opcodes";

my @ops;
my %ops;
my %modes;
my %seen;

# TODO the amount of genericity I am trying to provide here is probably
# unwarranted, and only makes the code mode complex than necessary.
my @pages = (
    { index => 0, prebytes => [      ] },
    { index => 1, prebytes => [ 0x9E ] },
);

while (<>) {
    chomp;
    my ($op, $cyc, $mne, $sz, $mode) = split /\|/;

    my $opcode = (hex $op) & 0xFF;
    my $page_index = 1;
    PAGE:
    for my $page (@pages) {
        my $temp = (hex $op) >> 8;
        my $found;

        for my $pre (reverse @{ $page->{prebytes} }) {
            next PAGE unless $temp;
            next PAGE if ($temp & 0xFF != $pre);
            $temp >>= 8;
            $found = 1;
        }

        if (!$temp || $found) {
            $page_index = $page->{index};
            last;
        }
    }

    my $record = {
        type   => $mne,
        mode   => $mode,
        opcode => $opcode,
        cycles => $cyc,
        bytes  => $sz,
        page   => $page_index,
    };

    # Examples / legends might cause an op to be duplicated
    unless ($seen{$op}++) {
        push @{ $ops[$page_index] }, $record;
        push @{ $ops{$mne}        }, $record;
        push @{ $modes{$mode}     }, $record;
    }
}

my $w0 = max map { length } keys %ops;
my $w1 = max map { length } keys %modes;
my $w2 = max map { length $_->{bytes}  } map { @$_ } @ops;
my $w3 = max map { length $_->{cycles} } map { @$_ } @ops;

# TODO pager
print <<EOF;
#ifndef \U${base}_H_\E
#define \U${base}_H_\E

#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

/// Distinct operations (not literal values)
enum op {
    @{ [ join ",\n    ", map { "OP_$_" } "INVALID", sort(keys %ops), "MAX" ] }
};

/// Addressing modes
enum mode {
    @{ [ join ",\n    ",
            map { (my $a = $_) =~ s/\+/P/g; "MODE_$a" } sort(keys %modes), "MAX" ] }
};

/// General opcode information
struct opinfo {
    enum op     type;   ///< distinct operation type
    enum mode   mode;   ///< addressing mode
    uint8_t     opcode; ///< entire numeric operation code
    uint8_t     cycles; ///< how many cycles to complete
    uint8_t     bytes;  ///< how many bytes in operation
};

////////////////////////////////////////////////////////////////////////////////
// Data declarations
////////////////////////////////////////////////////////////////////////////////

static const char *opnames[] = {
#define NAMED(X) [OP_##X] = #X
    @{ [ join ",\n    ", map { "NAMED($_)" } sort(keys %ops) ] }
#undef NAMED
};

static const struct {
    uint8_t index;          ///< which page this is
    bool    prebyte_cnt;    ///< how many prebytes are required for this page
    uint8_t prebyte_val[1]; ///< what the prebyte value(s) is / are
} pages[] = {
    @{
        local $" = ", ";
        [
        join ",\n    ", map {
            my @p = @{ $_->{prebytes} };
            my $i = $#p < 0 ? 0 : $#p;
            sprintf "[%u] = { %u, %u, { %s } }",
                    $_->{index}, $_->{index}, scalar @p,
                    join ", ", map { sprintf "0x%02X", $_ } @p[0 .. $i];
        } @pages
    ] }
};

static const struct opinfo optable[${\ scalar @pages}][256] = {
    @{ [
        join ",\n    ", map {
            "[$_->{index}] = {\n        " .
                (join ",\n        ", map { sprintf
                    "[0x%02X] = { OP_%-${w0}s, MODE_%-${w1}s, 0x%02X, %u, %${w3}u }",
                        @{$_}{qw(opcode type mode opcode bytes cycles)}
                } @{ $ops[$_->{index}] }) .
            "\n    }"
        } @pages
    ] }
};

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

EOF

# vi:set ts=4 sw=4 et: #
# vim:set syntax=perl: #

