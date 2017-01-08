$fn = 50;

// TXi

pcbX = 19.68;
pcbY = 107.01;
pcbZ = 1.6;

frame = 32;

barOneStart = 0;
barOneEnd = 18;

barTwoStart = 71;
barTwoEnd = 77;

barThreeStart = 102;
barThreeEnd = 107.01;

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
        
        translate([0, barThreeStart, 0])
            cube([pcbX, barThreeEnd - barThreeStart, pcbZ]);

    }

     translate([-frame * .8, -frame * .8]) {
       linear_extrude(height = 2 * pcbZ)
        text("TXo Bottom", font = "Core Humanist Sans:style=Bold", size = 8);
     }
 
 }

translate([-13,-frame * .74,0])
    cube([10, pcbZ * .5, pcbZ / 3]);


translate([-4,-frame * .655,0])
    cube([8, pcbZ * .4, pcbZ / 3]);
 
translate([-4,-frame * .755,0])
    cube([7, pcbZ * .4, pcbZ / 3]);

translate([3,-frame * .74,0])
    cube([7, pcbZ * .4, pcbZ / 3]);
 
translate([18,-frame * .74,0])
    cube([7, pcbZ * .4, pcbZ / 3]);
 