# half_center_oscillator_rtxi_module

This is a module for [RTXI](http://rtxi.org/) that creates a half-center oscillator circuit out of two neurons, by connecting them with artificial reciprocal inhibitory synapses and adding hyperpolarization-activated inward currents. This module also simulates negative leak current to counteract the leak induced by the electrode penetration. It also simulated neuromodlatory cirrent (IMI), with the votage dependence similar to the one of NMDA current.

Synaptic current is given by the following equations:

Isyn=gsyn∙s(Vpre)∙(Vpost-Esyn);   (1-s∞ )∙ds/dt=(s∞-s)/τs 

Hyperpolarization-activated inward current is given by the following equations:

IH=gH∙r(V)∙(V-EH); 	dr/dt=(r∞-r)/τr

Neuromodulatory current is given by the following equation:

IMI=gMI∙m(V)∙(V-EMI); 	dmi/dt=(m∞-m)/τm

Parameters for the synaptic and H-currents are taken from Sharp et. al, 1996 paper. Parameters for the neuromodulatory current, IMI, are taken from the Swensen et. al, 2001 paper. 

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
5.	gH0 : Maximal conductance of hyperpolarization-activated inward current (H-current)
6.	gsyn0: Maximal conductance of synaptic current
7.	tau IH0: time constant for H-current
8.	tau Isyn0: time constant for synaptic current
9.	Q10 gH: temperature coefficient for H-current conductance
10.	Q10 gsyn: temperature coefficient for synaptic current conductance
11.	Q10 tauH: temperature coefficient for the time constant of H-current
12.	Q10 tausyn: temperature coefficient for the time constant of synaptic current
13.	EH: reversal potential of H-current (mV)
14.	Esyn: reversal potential of synaptic current (mV)
15.	V_half: Half-activation voltage of synaptic current (synaptic threshold voltage)
16.	V_half_h: Half-activation voltage of H-current
17.	Gmi: Maximal conductance of the neuromodulatory current
18.	Emi: reversal potential of the neuromodulatory current

