# half_center_oscillator_rtxi_module

![](https://github.com/eomorozova/half_center_oscillator_rtxi_module/blob/main/HCO_module.png)

This is a module for [RTXI](http://rtxi.org/) that creates a half-center oscillator circuit out of two neurons, by connecting them with artificial reciprocal inhibitory synapses and adding hyperpolarization-activated inward currents. This module also simulates negative leak current to counteract the leak induced by the electrode penetration. It also simulates neuromodlatory current (IMI), with a voltage dependence similar to NMDA current.

Synaptic current is given by the following equations:

Isyn=gsyn∙s(Vpre)∙(Vpost-Esyn);   (1-s∞ )∙ds/dt=(s∞-s)/τs 

Hyperpolarization-activated inward current (H-current) is given by the following equations:

IH=gH∙r(V)∙(V-EH); 	dr/dt=(r∞-r)/τr

Neuromodulatory current is given by the following equation:

IMI=gMI∙m(V)∙(V-EMI); 	dmi/dt=(m∞-m)/τm

Parameters for the synaptic and H-currents are taken from Sharp et. al, 1996 paper. Parameters for the neuromodulatory current, IMI, are taken from Swensen et. al, 2001 paper. 

## Inputs
1.	input(0) -  “Vm1” : Membrane potential of the 1st neuron
2.	input(1) – “Vm2” : Membrane potential of the 2nd neuron

## Outputs
1.	output(0) -  “Iout1” : Current output for the 1st neuron
2.	output(1) – “Iout2” : Current output for the 2nd neuron

## Parameters
1.	membrane Eleak1: Reversal potential of the leak current (mV) for the 1st neuron
2.	membrane gleak1: leak conductance (nS) for the 1st neuron
3.	membrane Eleak2: Reversal potential of the leak current (mV) for the 2nd neuron
4.	membrane gleak2: leak conductance (nS) for the 2nd neuron
5.	gH : Maximal conductance of H-current (nS)
6.	gsyn: Maximal conductance of synaptic current (nS)
7.	tauH: Time constant for H-current (msec)
8.	tauSyn: Time constant for synaptic current (msec)
13.	EH: Reversal potential of H-current (mV)
14.	Esyn: Reversal potential of synaptic current (mV)
15.	V_half: Half-activation voltage of synaptic current (mV)
16.	V_half_h: Half-activation voltage of H-current (mV)
17.	Gmi: Maximal conductance of the neuromodulatory current (nS)
18.	Emi: Reversal potential of the neuromodulatory current (mV)

Example voltage output of two neurons connected via artificial reciprocal synapses generated by this module
![](https://github.com/eomorozova/half_center_oscillator_rtxi_module/blob/main/HCO_example1.png)
