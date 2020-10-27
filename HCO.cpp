//
// Half-center oscillator
//
//by Morozova E.O. 

#include <HCO.h>
#include <math.h>

extern "C" Plugin::Object * createRTXIPlugin(void)
{
	return new HCO();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "Vm1", "Membrane potential (mV) of cell #1", DefaultGUIModel::INPUT, },
	{ "Vm2", "Membrane potential (mV) of cell #2", DefaultGUIModel::INPUT, },
	{ "Iout1", "Output current cell #1 (nA)", DefaultGUIModel::OUTPUT, },
	{ "Iout2", "Output current cell #2 (nA)", DefaultGUIModel::OUTPUT, },
	{ "membraneV_1", "Cell 1 membraneV (mV)", DefaultGUIModel::STATE, },
	{ "membraneV_2", "Cell 2 membraneV (mV)", DefaultGUIModel::STATE, },
	{ "Temperature", "Temperature", DefaultGUIModel::STATE, },
	{ "membrane Eleak1", "Reversal potential of the leak current of cell #1 (mV)",	DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "membrane gleak1", "Conductance of the leak current of cell #1 (nS)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "membrane Eleak2", "Reversal potential of the leak current of cell #2 (mV)",	DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "membrane gleak2", "Reversal potential of the leak current of cell #2 (nS))", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "gH", "Maximal conductance of H-current (nS)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "gsyn", "Maximal conductance of synaptic current (nS)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "tauH", "Time constant of H-current (msec)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "tauSyn", "Time constan of synaptic current (msec)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "EH",	"Reversal potential of H-current (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Esyn", "Reversal potential of synaptic current (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "V_half", "Half-activation voltage of synaptic current (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Vhalf_h", "Half-activation voltage of H-current (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Gmi", "Maximal conductance of neuromodulatory current (nS)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Emi", "Maximal conductance of neuromodulatory current (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Time (s)", "Time (s)", DefaultGUIModel::STATE, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

HCO::HCO(void) : DefaultGUIModel("HCO", ::vars, ::num_vars)
{
	setWhatsThis(
		"<p><b>HCO:</b><br>This module creates a half-center oscillator.</p>");
	createGUI(vars, num_vars);
	initParameters();
	update(INIT);
	refresh();
	resizeMe();
}

HCO::~HCO(void) { }

// Activation function for synaptic and H-current
inline double HCO::Xinf(double V0, double A, double B)
{
	double X = 1.0 / (1.0 + exp((V0 - A) / B));
	return X;
}

//Activation function of the neuromodulatory current (IMI) (Swensen et al., 2001)
inline double HCO::m_mi_inf_update(double v)
{
	return (1 / (1 + exp((v + 21.0) / -8)));
}

// Time constant of the H-current (Sharp et al., 1996)
inline double HCO::KR_inf_update(double v) {
	return (tauH / ((1.0 / (1.0 + exp((v - (-110)) / (-13))))));
}


void HCO::execute(void)
{
	// Voltage inputs
	Vm[0] = input(0); // should be in mV
	Vm[1] = input(1); // should be in mV

	for (nn = 0; nn < 2; nn++)
	{
		leak_subtract1 = membrane_gleak1 * (Vm[0] - membrane_Eleak1); //subtract leak current induced by the electrode penetration of the cell #1
		leak_subtract2 = membrane_gleak2 * (Vm[1] - membrane_Eleak2); //subtract leak current induced by the electrode penetration of the cell #2
		
	     // Hyperpolarization-activated H-current (IH)
		H_m_tau[nn] = KR_inf_update(Vm[nn]); 
		H_m_inf[nn] = Xinf(Vm[nn], Vhalf_h, 7.0);
		H_m_gate[nn] = H_m_gate[nn] + (1 - exp(-dt / H_m_tau[nn])) * (H_m_inf[nn] - H_m_gate[nn]);
		gnmh[nn] = gH * H_m_gate[nn];
		IH[nn] = (gnmh[nn]) * (Vm[nn] - EH);

		// Synaptic currents (Isyn)
		S_inf[1] = Xinf(Vm[0], V_half, V_slope);
		S_inf[0] = Xinf(Vm[1], V_half, V_slope);
		Svar[nn] = Svar[nn] + (1 - exp(-dt / tauSyn)) * (S_inf[nn] - Svar[nn]);
		Isyn[nn] = gsyn * Svar[nn] * (Vm[nn] - Esyn);

		// Neuromodulatory current (ImI)	
		mmi[nn] = mmi[nn] + dt * (m_mi_inf_update(Vm[nn]) - mmi[nn]) / tau_mi;

		// Current outputs
		out[0] = -(IH[0] + Isyn[0] + Gmi * mmi[0] * (Vm[0] - Emi) + leak_subtract1);
		out[1] = -(IH[1] + Isyn[1] + Gmi * mmi[1] * (Vm[1] - Emi) + leak_subtract2);
		
		systime = count * dt; //time in seconds
		count++;
	}

	output(0) = out[0];
	output(1) = out[1];
}


void HCO::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
	case INIT:
		setState("membraneV_1", Vm[0]); //mV
		setState("membraneV_2", Vm[1]); //mV

		setParameter("membrane Eleak1", membrane_Eleak1); //mV
		setParameter("membrane gleak1", membrane_gleak1); //nS
		setParameter("membrane Eleak2", membrane_Eleak2); //mV
		setParameter("membrane gleak2", membrane_gleak2); //nS

		setParameter("EH", EH); //mV
		setParameter("Esyn", Esyn); //mV
		setParameter("gH", gH); // nS
		setParameter("gsyn", gsyn); // nS
		setParameter("tauH", tauH); // sec
		setParameter("tauSyn", tauSyn); // sec
		setParameter("V_half", V_half); // mV
		setParameter("Vhalf_h", Vhalf_h); // mV

		setParameter("Emi", Emi); //mV
		setParameter("Gmi", Gmi); // nS

		setState("Time (s)", systime);

		break;
	case MODIFY:

		membrane_Eleak1 = getParameter("membrane Eleak1").toDouble();
		membrane_gleak1 = getParameter("membrane gleak1").toDouble();
		membrane_Eleak2 = getParameter("membrane Eleak2").toDouble();
		membrane_gleak2 = getParameter("membrane gleak2").toDouble();

		EH = getParameter("EH").toDouble();
		gH = getParameter("gH").toDouble()*1e-4;
		tauH = getParameter("tauH").toDouble();

		Esyn = getParameter("Esyn").toDouble();
		gsyn = getParameter("gsyn").toDouble()*1e-4;
		tauSyn = getParameter("tauSyn").toDouble();

		Emi = getParameter("Emi").toDouble();
		Gmi = getParameter("Gmi").toDouble();

		V_half = getParameter("V_half").toDouble();
		Vhalf_h = getParameter("Vhalf_h").toDouble();

		break;
	case PERIOD:
		dt = RT::System::getInstance()->getPeriod() * 1e-6; // time in msec
		break;
	case PAUSE:
		output(0) = 0.0;
		output(1) = 0.0;
		break;
	case UNPAUSE:
		systime = 0;
		count = 0;

		break;
	default:
		break;
	}
}

void HCO::initParameters()
{
	output(0) = 0;
	output(1) = 0;
	dt = RT::System::getInstance()->getPeriod() * 1e-6; // msec
	systime = 0;
	count = 0;
	nn = 0;

	// leak current
	membrane_Eleak1 = 0;//mV
	membrane_gleak1 = 0.0;//nS
	membrane_Eleak2 = 0;//mV
	membrane_gleak2 = 0.0;//nS

	//H-current
	Vhalf_h = -50;//mV
	p_H = 1;
	V_th = -15;//mV
	EH = -10; //mV
	gH = 0; //nS
	tauH = 3000; //msec

	//Synaptic current
	Esyn = -80; //mV
	tauSyn = 100; //msec
	V_half = -50.0;  //mV
	V_slope = -2.0;  //mV
	gsyn = 0;

	//Neuromodulatory current
	Emi = 0; //mVolts
	Gmi = 0; //nS
	tau_mi = 4; // msec

	for (nn = 0; nn < 2; nn++)
	{
		out[nn] = 0;
		Vm[nn] = -50;
		mmi[nn] = 0.0;

		H_m_inf[nn] = Xinf(-50, 55.0, 7.0);
		H_m_gate[nn] = H_m_inf[nn];
		gnmh[nn] = 0.0;
		H_m_tau[nn] = 0.0;
		IH[nn] = 0.0;

		S_inf[nn] = 0;
		Svar[nn] = 0;
		Isyn[nn] = 0;

	}

}


