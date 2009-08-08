#!/usr/bin/env perl
use strict;
use Getopt::Std;
use List::Util qw(max);

my %opts;
getopt('bch', \%opts);
my $base = $opts{b} || "opcodes";
my $cfile = $opts{c}
    or die "No -c FILE option provided (output filename.c)";
my $hfile = $opts{h}
    or die "No -h FILE option provided (output filename.h)";

open my $c, ">", $cfile;
open my $h, ">", $hfile;

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

sub legalize { (my $a = $_[0]) =~ s/\+/P/g; $a }

# TODO pager
print $h <<EOF;
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
            map { ${\ legalize("MODE_$_") } } sort(keys %modes), "MAX" ] }
};

/// General opcode information
struct opinfo {
    enum op     type;   ///< distinct operation type
    enum mode   mode;   ///< addressing mode
    uint8_t     opcode; ///< entire numeric operation code
    uint8_t     cycles; ///< how many cycles to complete
    uint8_t     bytes;  ///< how many bytes in operation
};

extern int opnames_size;
extern int pages_size;
extern int optable_size[];

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

EOF

local $" = ", ";
my @page_sizes = map { scalar @$_ } @ops;

print $c <<EOF;
#include "$hfile"

#define countof(X) (sizeof (X) / sizeof (X)[0])

////////////////////////////////////////////////////////////////////////////////
// Data declarations
////////////////////////////////////////////////////////////////////////////////

const char *opnames[] = {
#define NAMED(X) [OP_##X] = #X
    @{ [ join ",\n    ", map { "NAMED($_)" } sort(keys %ops) ] }
#undef NAMED
};

int opnames_size = countof(opnames);

const struct {
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

int pages_size = countof(pages);

const struct opinfo optable[${\ scalar @pages}][256] = {
    @{ [
        join ",\n    ", map {
            "[$_->{index}] = {\n        " .
                (join ",\n        ", map { sprintf
                    "[0x%02X] = { OP_%-${w0}s, MODE_%-${w1}s, 0x%02X, %u, %${w3}u }",
                        map { legalize $_ } @{$_}{qw(opcode type mode opcode bytes cycles)}
                } @{ $ops[$_->{index}] }) .
            "\n    }"
        } @pages
    ] }
};

int optable_size[] = { @page_sizes };

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

EOF

# vi:set ts=4 sw=4 et: #
# vim:set syntax=perl: #

