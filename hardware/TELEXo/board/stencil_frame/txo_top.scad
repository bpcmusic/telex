$fn = 50;

// TXi

pcbX = 19.68;
pcbY = 107.01;
pcbZ = 1.6;

frame = 25;

barOneStart = 0;
barOneEnd = 5;

barTwoStart = 54;
barTwoEnd = 107.01;

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

    }

     translate([-frame * .8, -frame * .8]) {
       linear_extrude(height = 2 * pcbZ)
        text("TXo Top", font = "Core Humanist Sans:style=Bold", size = 8);
     }
 
 }

translate([-7,-frame * .72,0])
    cube([10, pcbZ * .4, pcbZ / 3]);

translate([8,-frame * .72,0])
    cube([10, pcbZ * .4, pcbZ / 3]);
 
translate([16,-frame * .72,0])
    cube([5, pcbZ * .4, pcbZ / 3]);
