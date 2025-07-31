File created date: July 31, 2025 \
Last updated: July 31, 2025 by Liz Farquhar\
This markdown file describes the methods and materials needed to preform a calibration for the C3PO conductivity sensor. 

# Materials needed 
1. (1) C3PO sensor and a way to hold it vertical (a tabletop vice grip works nicely as long as you don't overtighten) 
2. (3) 80 mL glass beakers, cleaned and dried beforehand
3. Kimwipes
4. About 0.5 L of milli-Q water, that is, water that has undergone reverse osmosis and been deionized
5. The (2) calibration solution standards that came with the Atlas conductivity probe: 12880 uS/cm and 80000 uS/cm, \
   where these standards are +- 5 at 25 degrees C

# Procedure 
1. Powered on the C3PO and changed the sampling rate to every 20 seconds
2. Gathered the 3 beakers and emptied the 2 standards into 2 of the glass beakers, ensuring to label each one. The last \
   beaker was filled with an approximately equal amount of milli-Q water for 0 uS/cm.
3. Placed the C3PO in each beaker for 5 minutes each (to have at least a total of 10 measurements per standard), rinsing \
   the probes with milli-Q water and dabbing dry between each beaker with a Kimwipe. Ensure to record the exact times when \
   the sensor is placed in and removed from the solutions. 
4. Create a new folder in /Calibrations/ with the name of the C3PO you are calibrating.
5. Offload the csv data file from the C3PO and upload to the specific folder from Step 4 on this Github page. Create a .txt or csv \
   file of the times that the sensor was placed in and removed from solutions. For an example, see /Calibrations/C3PO_1/step_pts.txt.
6. Upload any other notes or photos you took during the calibration to the same folder. Notes should include date of calibration \
   and who preformed it. 

# Post data analysis 
1. Use the Calibration.ipynb script to create a linear regression between the C3PO readings and the calibration solutions. You will need \
   to change the device name in the script to match whichever C3PO you are calibrating.
2. The output of the conductivity is not temperature compensated. You will need to correct for temperature using the temperature from the \
   RTD probe. The script in /Data Analysis/Conductivity_to_PSS-78_Salinity.R can be used to apply the calibration constants from the \
   linear regression and convert conductivity to salinity using TEOS-10 equations. 
