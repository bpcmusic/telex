$fn = 50;

// TXo Top

// framex = 250;


tolerance = .5;

pcbX = 20.02;
pcbY = 107.32;
pcbZ = 1.6;

frame = 2.5;

framex = pcbX + frame;

barOneStart = 27;
barOneEnd = 47;

barTwoStart = 90;
barTwoEnd = 108;

corner = 2.5;

for (a =[0:7]) frame(a * (framex + .5));


module frame(xOrigin){

    union(){
        difference() {
            translate([xOrigin-frame, - frame, 0])
                cube([pcbX + 2 * frame, pcbY + 2 * frame, 2 * pcbZ]);
            translate([xOrigin-tolerance/2, -tolerance/2, 0])
                cube([pcbX + tolerance, pcbY + tolerance, 2 * pcbZ]);
        }

        translate([xOrigin-tolerance/2, barOneStart, 0])
            cube([pcbX + tolerance, barOneEnd - barOneStart, pcbZ]);
            
        translate([xOrigin-tolerance/2, barTwoStart, 0])
            cube([pcbX + tolerance, barTwoEnd - barTwoStart, pcbZ]);
        
        translate([xOrigin-tolerance/2,-tolerance/2,0])
            cube([corner, corner, pcbZ]);

        translate([xOrigin + pcbX - corner + (tolerance / 2), -tolerance/2, 0])
            cube([corner, corner, pcbZ]);
    }
 
 }