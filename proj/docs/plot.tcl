#! /bin/env tclsh

package require Tk

set width     400
set height    240

set t1 0
set t2 0

set a [expr $::height / 1024.0 ]
set y1 0
set y2 0

# serial reader callback
proc reader {serial} {
	if {[set rc [gets $serial data]] == -1} {
		if {[eof $serial]} {
			fileevent $serial r {}
			close $serial
		}
		return
	}

	puts $data
	
	if [regexp {([0-9]+).*} $data {} d] {
		set ::t2 [expr $::t1+5]
		set ::y2 [expr int( $::height - ($::a * $d) )]
		.c create line $::t1 $::y1 $::t2 $::y2 -fill gray
		set ::y1 $::y2
		set ::t1 $::t2
		if {$::t1 > $::width} { .c xview scroll 5 unit }
	}
}

proc start_cb {} {
	puts $::serial r
	flush $::serial
}

proc stop_cb {} {
	puts $::serial s
	flush $::serial
}

# init serial
catch {
	set serial [open /dev/ttyUSB0 r+]
	fconfigure $serial -mode 115200,n,8,1 -buffering none -blocking 0 -encoding ascii -translation crlf
	fileevent $serial readable [list reader $serial]
} rc

# widgets
frame  .f
button .f.start -text "Start" -command "start_cb"
button .f.stop -text "Stop" -command "stop_cb"
canvas .c -width $::width -height $::height -xscrollincrement 1 -bg beige

pack .f -side left -fill y
pack .f.start .f.stop -side top -anchor n
pack .c -side left -fill both -expand 1

bind . <Destroy> { exit }
