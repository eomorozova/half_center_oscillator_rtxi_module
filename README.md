# half_center_oscillator_rtxi_module

This is a module for [RTXI](http://rtxi.org/) that creates a half-center oscillator circuit out of two neurons, by connecting them with artificial reciprocal inhibitory synapses and adding hyperpolarization-activated inward currents.

Synaptic current is given by the following equations:

Isyn=gsyn∙s(Vpre)∙ (Vpost-Esyn);   (1-s∞ )  ⅆs/ⅆt=(s∞-s)/τs 

Hyperpolarization-activated inward current is given by the following equations:

IH=gH∙r(V)∙(V-EH ); 	ⅆr/ⅆt=(r∞-r)/τr

## Inputs
1.	input(0) -  “Vm1” : Membrane potential of the 1st neuron
2.	input(1) – “Vm2” : Membrane potential of the 2nd neuron

## Outputs
1.	output(0) -  “Iout1” : Current output for the 1st neuron
2.	output(1) – “Iout2” : Current output for the 2nd neuron

## Parameters

