all: cali1  cali2  cali3 

cali1: aigo_arm_cali.cc
	g++ aigo_arm_cali.cc -o cali1

cali2: aigo_arm_cali2.cc
	g++ aigo_arm_cali2.cc -o cali2

cali3: aigo_arm_cali3.cc
	g++ aigo_arm_cali3.cc -o cali3
