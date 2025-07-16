# Using TEOS-10 to calculate salinity from conductivity 
library(gsw)

cond <- read.csv("2025_07_02_C3PO_Offload.csv", header=T)

#apply any calibration values
cond$Cond_uScm.cor <- cond$Cond_uScm.1 *0.8 + 262 #the m and b come from calibration experiment


#convert cond from uS/cm to mS/cm
cond$Cond_mScm <- cond$Cond_uScm.cor / 1000
cond$seaPressure <- 0 #zero because these data were taken at atmospheric pressure

cond$practicalSalinity <- gsw_SP_from_C(cond$Cond_mScm,cond$Temp_C,cond$seaPressure)

#save new file 
write.csv(cond,"ConductivitywithSalinity.csv", row.names = FALSE)
