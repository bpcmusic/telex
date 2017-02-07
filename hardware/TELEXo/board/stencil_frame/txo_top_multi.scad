$fn = 50;

// TXo Top

// framex = 250;


tolerance = .5;

pcbX = 19.68;
pcbY = 107.01;
pcbZ = 1.6;

frame = 2.5;

framex = pcbX + frame;

barOneStart = 0;
barOneEnd = 5 + (tolerance / 2);

barTwoStart = 54;
barTwoEnd = 107.01 + tolerance;

corner = 2.5;

for (a =[0:7]) frame(a * framex);


module frame(xOrigin){

    difference(){

        union(){
            difference() {
                translate([xOrigin-frame, - frame, 0])
                    cube([pcbX + 2 * frame, pcbY + 2 * frame, 2 * pcbZ]);
                translate([xOrigin-tolerance/2, -tolerance/2, 0])
                    cube([pcbX + tolerance, pcbY + tolerance, 2 * pcbZ]);
            }

            translate([xOrigin-tolerance/2,-tolerance/2,0])
                cube([corner, corner, pcbZ]);

            translate([xOrigin + pcbX - corner + (tolerance / 2), -tolerance/2, 0])
                cube([corner, corner, pcbZ]);
            
            translate([xOrigin-tolerance/2, barTwoStart, 0])
                cube([pcbX + tolerance, barTwoEnd - barTwoStart, pcbZ]);

        }

     }
 
 }