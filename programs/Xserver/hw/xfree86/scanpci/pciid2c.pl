#!/usr/bin/perl

# Automatically generate the data structures for PCI vendor/device lists
# from the pci.ids file.
#
# It should be run as:
#
#   perl pciid2c.pl ../common/xf86PciInfo.h < pci.ids > xf86PciIds.h
#
#
# Copyright � 2002 by The XFree86 Project, Inc.
#
# Author: David Dawes
#
# $XFree86: xc/programs/Xserver/hw/xfree86/scanpci/pciid2c.pl,v 1.1tsi Exp $
#

if (@ARGV[0]) {
    $infofile = @ARGV[0];
}

#
# The basic pci.ids format is:
#   - Vendor lines start with four (lower case) hex digits
#   - Device lines start with one tab followed by four hex digits
#   - Subsystem lines start with two tabs followed by two sets of four
#     hex digits.
#   - Class overrides for devices start with two tabs followed by a "C",
#     followed by four hex digits with the class/subclass value.
#   - Class lines start with a "C".
#   - Comment lines start with a '#'.
#   - Blank lines are ignored.
#
# We allow for extra lines to be appended to modify existing entries or
# add new ones.  To add/modify Device entries without modifying the
# Vendor name, a special vendor name of '"' is used (mnemonic: "ditto").
# Similarly for adding subsystem names without modifying (or adding)
# a corresponding device entry.  To rename an existing entry, simply
# provide the new name.  To remove an existing entry, use the special
# name '-'.
#

while (<STDIN>) {
    # Process data lines
    if (/^([0-9a-f]{4})\s+(.*)/) {
	$vendor = $1;
	if ($2 eq '-') {
	    delete($vendors{$vendor});
	} elsif ($2 ne '"') {
	    $vendors{$vendor} = $2;
	    # Remove " characters
	    $vendors{$vendor} =~ s/"//g;
	    # Remove multiple "?" sequences to avoid trigraphs
	    $vendors{$vendor} =~ s/\?+/\?/g;
	}
    } elsif (/^\t([0-9a-f]{4})\s+(.*)/) {
	$device = $1;
	if ($2 eq '-') {
	    delete($devices{$vendor}{$device});
	} elsif ($2 ne '"') {
	    $devices{$vendor}{$device} = $2;
	    # Remove " characters
	    $devices{$vendor}{$device} =~ s/"//g;
	    # Remove multiple "?" sequences to avoid trigraphs
	    $devices{$vendor}{$device} =~ s/\?+/\?/g;
	}
    } elsif (/^\t\t([0-9a-f]{4})\s+([0-9a-f]{4})\s+(.*)/) {
	$v = $1;
	$s = $2;
	if ($3 eq '-') {
	    delete($subsystems{$v}{$s});
	    delete($devsubsystems{$vendor}{$device}{"$v-$s"});
	} elsif ($3 ne '"') {
	    if ($subsystems{$v}{$s}) {
		#print STDERR "Duplicate subsytem: $v, $s, \"$subsystems{$v}{$s}\", \"$3\"\n";
	    }
	    $subsystems{$v}{$s} = $3;
	    # Remove " characters
	    $subsystems{$v}{$s} =~ s/"//g;
	    # Remove multiple "?" sequences to avoid trigraphs
	    $subsystems{$v}{$s} =~ s/\?+/\?/g;
	    $devsubsystems{$vendor}{$device}{"$v-$s"} = $subsystems{$v}{$s};
	}
    } elsif (/^\t\tC\s+([0-9a-f]{4})/) {
	$classes{$vendor}{$device} = $1;
    }
    # Ignore all other lines.
}

# Find which vendors are "video" vendors.
if ($infofile) {
    open(INFO, "<$infofile") || die "Can't open $infofile";
    while (<INFO>) {
	if (/^#define\s+PCI_VENDOR_.*0x([0-9a-fA-F]{4})/) {
	    $vendor = $1;
	    $vendor =~ tr/A-F/a-f/;
	    $video{$vendor} = 1;
	}
    }
}

#
# This layout is quite different from that used in the old xf86PciInfo.h
# file.  One main difference is that the list is initialised at runtime.
# It's currently a flat list.  This could be improved.
#

# Print out header information.

$proj = "XFree86";
print "/* \$$proj\$ */

/*
 * THIS FILE IS AUTOMATICALLY GENERATED -- DO NOT EDIT
 *
 * It is generated by pciid2c.pl using data from the following files:
 *
 *    ../etc/pci.ids
 *    ../etc/extrapci.ids
 *    ../common/xf86PciInfo.h
 */

/*
 * Copyright � 2002 by the XFree86 Project, Inc.
 *
 * The pci.ids file and the data it contains are from the Linux PCI ID's
 * Project (http://pciids.sf.net/).  It is maintained by Martin Mares
 * <mj\@ucw.cz> and other volunteers.  The pci.ids file contains no
 * copyright notice.
 */

#include \"xf86PciInfo.h\"
#ifndef NULL
#define NULL (void *)0
#endif

";

# The following #ifdefs are used:
#  - INIT_SUBSYS_INFO -- initialise subsystem data
#  - INIT_VENDOR_SUBSYS_INFO -- initialise a vendor<->subsystem table.
#  - VENDOR_INCLUDE_NONVIDEO -- include data for non-video vendors.

# Define static variables with all of the strings.

foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    }
    print "static const char pci_vendor_${vendor}[] = \"$vendors{$vendor}\";\n";
    foreach $device (sort keys %{$devices{$vendor}}) {
	print "static const char pci_device_${vendor}_${device}[] = " .
	      "\"$devices{$vendor}{$device}\";\n";
	foreach $subsys (sort keys %{$devsubsystems{$vendor}{$device}}) {
	    $s = $subsys;
	    ($v) = split /-/, $s;
	    if ($infofile && !$video{$vendor} && $video{$v}) {
		print "#endif\n";
	    }
	    $s =~ s/-/_/;
	    print "#ifdef INIT_SUBSYS_INFO\n";
	    print "static const char pci_subsys_${vendor}_${device}_${s}[] = " .
		  "\"$devsubsystems{$vendor}{$device}{$subsys}\";\n";
	    print "#endif\n";
	    if ($infofile && !$video{$vendor} && $video{$v}) {
		print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
	    }
	}
    }
    if ($infofile && !$video{$vendor}) {
	print "#endif\n";
    }
}


# Pre-initialise the table structures (from the inner to the outer).

# First, the subsys structures.

print "#ifdef INIT_SUBSYS_INFO\n";
foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	$pre = "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    } else {
	undef($pre);
	undef($post);
    }
    foreach $device (sort keys %{$devices{$vendor}}) {
	foreach $subsys (sort keys %{$devsubsystems{$vendor}{$device}}) {
	    $s = $subsys;
	    $s =~ tr/-/_/;
	    ($vid, $sid) = split /_/, $s;
	    if ($pre) {
		print $pre;
		undef($pre);
		$post = "#endif\n";
	    }
	    if ($infofile && !$video{$vendor} && $video{$vid}) {
		print "#endif\n";
	    }
	    print "static const pciSubsystemInfo " .
		  "pci_ss_info_${vendor}_${device}_$s =\n";
	    print "\t{0x$vid, 0x$sid, pci_subsys_${vendor}_${device}_$s, 0};\n";
	    print "#undef pci_ss_info_$s\n";
	    print "#define pci_ss_info_$s pci_ss_info_${vendor}_${device}_$s\n";
	    if ($infofile && !$video{$vendor} && $video{$vid}) {
		print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
	    }
	}
    }
    if ($post) {
	print $post;
	undef($post);
    }
}

# Next, the list of per vendor+device subsystem arrays

foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	$pre = "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    } else {
	undef($pre);
	undef($post);
    }
    foreach $device (sort keys %{$devices{$vendor}}) {
	if (scalar(keys %{$devsubsystems{$vendor}{$device}}) > 0) {
	    if ($pre) {
		print $pre;
		undef($pre);
		$post = "#endif\n";
	    }
	    print "static const pciSubsystemInfo *pci_ss_list_${vendor}_${device}[] = {\n";
	    foreach $sub (sort keys %{$devsubsystems{$vendor}{$device}}) {
		$sub =~ s/-/_/;
		print "\t&pci_ss_info_${vendor}_${device}_${sub},\n";
	    }
	    print "\tNULL\n};\n";
	} else {
	    print "#define pci_ss_list_${vendor}_${device} NULL\n";
	}
    }
    if ($post) {
	print $post;
	undef($post);
    }
}

# Next, the list of per vendor subsystem arrays

print "#ifdef INIT_VENDOR_SUBSYS_INFO\n";
foreach $vendor (sort keys %vendors) {
    if (scalar(keys %{$subsystems{$vendor}}) > 0) {
	if ($infofile && !$video{$vendor}) {
	    print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
	}
	print "static const pciSubsystemInfo *pci_ss_list_${vendor}[] = {\n";
	foreach $sub (sort keys %{$subsystems{$vendor}}) {
	    print "\t&pci_ss_info_${vendor}_${sub},\n";
	}
	print "\tNULL\n};\n";
	if ($infofile && !$video{$vendor}) {
	    print "#endif\n";
	}
    } else {
	print "#define pci_ss_list_${vendor} NULL\n";
    }
}
print "#endif /* INIT_VENDOR_SUBSYS_INFO */\n";
print "#endif /* INIT_SUBSYS_INFO */\n";

# Next the device structures

foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	$pre = "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    } else {
	undef($pre);
	undef($post);
    }
    foreach $device (sort keys %{$devices{$vendor}}) {
	if ($pre) {
	    print $pre;
	    undef($pre);
	    $post = "#endif\n";
	}
	if ($classes{$vendor}{$device}) {
	    $class = "0x$classes{$vendor}{$device}";
	} else {
	    $class = "0";
	}
	print "static const pciDeviceInfo " .
	      "pci_dev_info_${vendor}_${device} = {\n";
	print "\t0x$device, pci_device_${vendor}_${device},\n";
	print "#ifdef INIT_SUBSYS_INFO\n";
	print "\tpci_ss_list_${vendor}_${device},\n";
	print "#else\n";
	print "\tNULL,\n";
	print "#endif\n";
	print "\t$class\n};\n";
    }
    if ($post) {
	print $post;
	undef($post);
    }
}

# Next, the list of per vendor device arrays

foreach $vendor (sort keys %vendors) {
    if (scalar(keys %{$devices{$vendor}}) > 0) {
	if ($infofile && !$video{$vendor}) {
	    print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
	}
	print "static const pciDeviceInfo *pci_dev_list_${vendor}[] = {\n";
	foreach $device (sort keys %{$devices{$vendor}}) {
	    print "\t&pci_dev_info_${vendor}_${device},\n";
	}
	print "\tNULL\n};\n";
	if ($infofile && !$video{$vendor}) {
	    print "#endif\n";
	}
    } else {
	print "#define pci_dev_list_${vendor} NULL\n";
    }
}

# Next, the main vendor list

print "
static const pciVendorInfo pciVendorInfoList[] = {
";

foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    }
    print "\t{0x$vendor, pci_vendor_$vendor, pci_dev_list_$vendor},\n";
    if ($infofile && !$video{$vendor}) {
	print "#endif\n";
    }
}
print "\t{0x0000, NULL, NULL}\n};\n";

# Finally, the main vendor/subsystem list

print "
#if defined(INIT_VENDOR_SUBSYS_INFO) && defined(INIT_SUBSYS_INFO)
static const pciVendorSubsysInfo pciVendorSubsysInfoList[] = {
";

foreach $vendor (sort keys %vendors) {
    if ($infofile && !$video{$vendor}) {
	print "#ifdef VENDOR_INCLUDE_NONVIDEO\n";
    }
    print "\t{0x$vendor, pci_vendor_$vendor, pci_ss_list_$vendor},\n";
    if ($infofile && !$video{$vendor}) {
	print "#endif\n";
    }
}
print "\t{0x0000, NULL, NULL}\n};\n";
print "#endif\n";
