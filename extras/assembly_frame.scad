
smidge = .4;
smoodge = 1;

pcbX = 20.2 + smidge;
pcbY = 107.01 + smoodge;
pcbZ = 1.6;

depth = 25;

difference(){

    translate([0,0,5])
        cube([pcbX + 10, 80, 17]);
    translate([5, 0, 5])
        cube([pcbX,pcbY,25 * pcbZ]);
}

snip = pcbY - 5;
nerdel = (110 - snip) / 2;

nerdelheight = 6;

difference(){
    translate([-15,-15,0])
        cube([pcbX + 40,110,5]);
    translate([5, nerdel - 15, 0])
        cube([pcbX,snip,100]);
}

translate([5,-15,5])
    cube([pcbX,nerdel, nerdelheight]);
translate([5,95-nerdel,5])
    cube([pcbX,nerdel, nerdelheight]);
