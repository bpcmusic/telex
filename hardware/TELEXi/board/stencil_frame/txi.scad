$fn = 50;

// TXi

pcbX = 20.02;
pcbY = 107.32;
pcbZ = 1.6;

frame = 25;

barOneStart = 27;
barOneEnd = 47;

barTwoStart = 90;
barTwoEnd = 107.32;

corner = 2.5;

difference(){

    union(){
        difference() {
            translate([-frame, - frame, 0])
                cube([pcbX + 2 * frame, pcbY + 2 * frame, 2 * pcbZ]);
            cube([pcbX, pcbY, 2 * pcbZ]);
        }

        translate([0, barOneStart, 0])
            cube([pcbX, barOneEnd - barOneStart, pcbZ]);
            
        translate([0, barTwoStart, 0])
            cube([pcbX, barTwoEnd - barTwoStart, pcbZ]);

        cube([corner, corner, pcbZ]);

        translate([pcbX - corner, 0, 0])
            cube([corner, corner, pcbZ]);
    }

     translate([-frame * .8, -frame * .8]) {
       linear_extrude(height = 2 * pcbZ)
        text("TXi Bottom", font = "Core Humanist Sans:style=Bold", size = 8);
     }
 
 }


translate([-4,-frame * .61,0])
    cube([10, pcbZ * .4, pcbZ / 3]);
 
translate([-4,-frame * .74,0])
    cube([10, pcbZ * .4, pcbZ / 3]);

translate([5,-frame * .72,0])
    cube([7, pcbZ * .4, pcbZ / 3]);
 
translate([19.4,-frame * .72,0])
    cube([7, pcbZ * .4, pcbZ / 3]);