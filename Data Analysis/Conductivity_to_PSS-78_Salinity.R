# Using TEOS-10 to calculate salinity from conductivity 
library(gsw)

cond <- read.csv("c3po_julyaugust.csv", header=T)

#apply any calibration values
cond$Cond_uScm.cor <- cond$cond_uscm *0.8 + 262 #the m and b come from calibration experiment


#convert cond from uS/cm to mS/cm
cond$Cond_mScm <- cond$Cond_uScm.cor / 1000
cond$seaPressure <- 0 #zero because these data were taken at atmospheric pressure

cond$practicalSalinity <- gsw_SP_from_C(cond$Cond_mScm,cond$temp_C,cond$seaPressure)

#save new file 
write.csv(cond,"julyaugustSalinity.csv", row.names = FALSE)
