#!/usr/bin/env tclsh

set INFILE [lindex $argv 0]
set EXT [lindex $argv 1]
set ROT [lindex $argv 2]
set ZOOM [lindex $argv 3]
set ISO [lindex $argv 4]
set RENDER [lindex $argv 5]
set LOWRES [lindex $argv 6]
set TRANS [lindex $argv 7]
set DENS [lindex $argv 8]
set MOS [lindex $argv 9]


mol new ${INFILE} type ${EXT} first 0 last -1 step 1 waitfor 1 volsets {0 }

display projection Orthographic
display depthcue off
display antialias on
axes location Off
light 1 on
color Display Background white

mol representation CPK 0.800000 0.250000 10.000000 10.000000
color Name A green
color Name N orange
color Name O red
color Name H white
color Name C gray

set sel [atomselect top all]
${sel} set radius 1.25
mol addrep top

scale by ${ZOOM}

if {[string compare ${ROT} "none"] != 0} {
  set rot [split ${ROT} ","]
  foreach r ${rot} {
    set axis [string index $r 0]
    set r [string range $r 1 2]
    rotate ${axis} by $r
  }
}

if {[string compare ${TRANS} "true"] == 0} {
  mol material Transparent
}

if {[string compare ${DENS} "true"] == 0} {
  mol representation Isosurface ${ISO} 0 0 0 1 1
  mol color ColorID 0
  mol addrep top
}

if {[string compare ${MOS} "true"] == 0} {
  mol representation Isosurface ${ISO} 0 0 0 1 1
  mol color ColorID 0
  mol addrep top

  mol representation Isosurface -${ISO} 0 0 0 1 1
  mol color ColorID 1
  mol addrep top
}

if {[string compare ${LOWRES} "true"] == 0 || [string compare ${RENDER} "false"] == 0} {
  display resize 1000 1000
} else {
  display resize 4000 4000
}


if {[string compare ${RENDER} "true"] == 0} {
  set INFILE [file rootname ${INFILE}]

  render TachyonInternal ${INFILE}.tga convert %s -quality 95 ${INFILE}.jpg

  file delete ${INFILE}.tga

  exit
  quit
}
