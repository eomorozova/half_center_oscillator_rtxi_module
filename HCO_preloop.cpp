
#include <HCO_preloop.h>
#include <math.h>

extern "C" Plugin::Object *createRTXIPlugin(void)
{
	return new HCO_preloop();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "Vm1", "Membrane potential (V) of cell #1", DefaultGUIModel::INPUT, },
	{ "Vm2", "Membrane potential (V) of cell #2", DefaultGUIModel::INPUT, },
	{ "Temp1", "Temperature", DefaultGUIModel::INPUT, },
	{ "Iout1", "Output current cell #1", DefaultGUIModel::OUTPUT, },
	{ "Iout2", "Output current cell #2", DefaultGUIModel::OUTPUT, },
	{ "membraneV_1", "Cell 1 membraneV (mV)", DefaultGUIModel::STATE, },
	{ "membraneV_2", "Cell 2 membraneV (mV)", DefaultGUIModel::STATE, },
	{ "Temperature", "Temperature", DefaultGUIModel::STATE, },
	{ "membrane Eleak", "measured leakage reversal potential of the membrane (mV)",	DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{"membrane gleak", "measured leakage conductance of the membrane (mSiemens)", DefaultGUIModel::PARAMETER |DefaultGUIModel::DOUBLE, },
	{ "gH", "H maximal conductance normalized to capacity (mSiemens/uFarad)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "gsyn", "Synaptic maximal conductance normalized to capacity (mSiemens/uFarad)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "EH",	"H reversal potential (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Esyn", "Synaptic reversal potential (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "V_half", "Half-activation of synaptic current", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Vhalf_h", "Half-activation of H-current", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Gmi", "Neuromodulatory current maximal conductance", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{"Emi",	"Neuromodulatory Nernst Potential (mV)", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,	},
	{ "Time (s)", "Time (s)", DefaultGUIModel::STATE, }, 
{	"RTXIspeed",		"RTXIspeed (in msec)",			DefaultGUIModel::STATE,					},
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

HCO_preloop::HCO_preloop(void) : DefaultGUIModel("HCO parameter preloop", ::vars, ::num_vars)
{
	setWhatsThis(
		"<p><b>HCO parameter preloop:</b><br>This module creates a half-center oscillator.</p>");
	createGUI(vars, num_vars);
	initParameters();
	update( INIT);
	refresh();
	resizeMe();
}

HCO_preloop::~HCO_preloop(void) { }

inline double HCO_preloop::Xinf(double V0, double A, double B)
{

  double X = 1.0/(1.0+exp((V0-A)/B));
  return X;
}

inline double HCO_preloop::tauX(double V0, double A, double B, double D, double E)
{
  double X = A - B/(1+exp((V0+D)/E));
  return X;
}

//Swensen 2001
inline double HCO_preloop::m_mi_inf_update(double v)
{
            return (  1/(1+exp((v+21.0)/-8)) );
}

inline double HCO_preloop::KR_inf_update(double v){
            return (tau_H/((1.0/(1.0+exp((v-(-110))/(-13)))) ));
}


void HCO_preloop::execute(void) 
{
	long long timestep = RT::OS::getTime();
	if (lastRead_timestep>0)
    {
	   RTXIspeed = ((timestep - lastRead_timestep) * 1e-6);
	}
	lastRead_timestep  = timestep;

	Vm[0] = input(0)*100; // voltage in mV
	Vm[1] = input(1)*100; // voltage in mV
	Temp = input(2)*10; // temperature

	for (nn=0;nn<2;nn++)
    {
    double leak_subtract = membrane_gleak*(Vm[nn]-membrane_Eleak); //physical leakage of membrane, subtract it to get the leakage defined by the user
// Ih
    	//H_m_tau[nn] = tauX(Vm[nn],2072.0,-999.0,82.2,-8.73); //m, H
	H_m_tau[nn] = KR_inf_update(Vm[nn]); //tau, H
	H_m_inf[nn] = Xinf(Vm[nn],Vhalf_h,7.0); //m, H
	H_m_gate[nn] = H_m_gate[nn] + (1-exp(-RTXIspeed/H_m_tau[nn]))*(H_m_inf[nn] - H_m_gate[nn]);
	gnmh[nn] = gH*H_m_gate[nn];
	IH[nn] = (gnmh[nn])*(Vm[nn]-EH);

// Isyn
   	S_inf[1] = Xinf(Vm[0],V_half,V_slope);
    	S_inf[0] = Xinf(Vm[1],V_half,V_slope);
//	if (Vm[0]>V_half)
//   		{S_inf[1] = tanh((Vm[0]-V_half)/V_slope);}
//	else
//   		{S_inf[1]=0;}

//	if (Vm[1]>V_half)
//    		{S_inf[0] = tanh((Vm[1]-V_half)/V_slope);}
//	else
//		{S_inf[0]=0;}

    	Svar[nn] = Svar[nn] + (1-exp(-RTXIspeed/tau_syn))*(S_inf[nn] - Svar[nn]);
    	Isyn[nn] = gsyn*Svar[nn]*(Vm[nn]-Esyn);

// Imi	
	double tau_mi=4;
	mmi[nn] = mmi[nn] + RTXIspeed*(m_mi_inf_update(Vm[nn]) - mmi[nn]) / tau_mi;	

    out[nn] = - ( -leak_subtract + IH[nn] + Isyn[nn] + Gmi*mmi[nn]*(Vm[nn]-Emi) );
 //   out[nn] = - (IH[nn]);
 //   out[nn] = - (IH[nn] + Gmi*mmi[nn]*(Vm[nn]-Emi));
//    out[0] = - (IH[0] + Gmi*mmi[0]*(Vm[0]-Emi) + Isyn[0]);
//    out[1] = - (IH[1] + Gmi*mmi[1]*(Vm[1]-Emi) + Isyn[1]);

systime=count*dt;
count++;
}

	output(0) = out[0];
	output(1) = out[1];
}


void HCO_preloop::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
	case INIT:
    		setState("membraneV_1",Vm[0]);
    		setState("membraneV_2",Vm[1]);
		setState("Temperature",Temp);
    		setParameter("membrane Eleak",membrane_Eleak);
    		setParameter("membrane gleak",membrane_gleak);

    		setParameter("EH",EH);
    		setParameter("Esyn",Esyn);
    		setParameter("gH",gH);
    		setParameter("gsyn",gsyn);
    		setParameter("V_half",V_half);
    		setParameter("Vhalf_h",Vhalf_h);

    		setParameter("Emi",Emi);
    		setParameter("Gmi",Gmi);

		setState("Time (s)", systime);
		setState("RTXIspeed", RTXIspeed);

		break;
	case MODIFY:

		membrane_Eleak = getParameter("membrane Eleak").toDouble();
            	membrane_gleak = getParameter("membrane gleak").toDouble();

		EH = getParameter("EH").toDouble();
            	gH = getParameter("gH").toDouble();

            	Esyn = getParameter("Esyn").toDouble();
            	gsyn = getParameter("gsyn").toDouble();
		
		Emi = getParameter("Emi").toDouble();
           	Gmi = getParameter("Gmi").toDouble();

         	V_half = getParameter("V_half").toDouble();
         	Vhalf_h = getParameter("Vhalf_h").toDouble();

		break;
	case PERIOD:
		dt = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
		break;
	case PAUSE:
		output(0) = 0.0;
		output(1) = 0.0;
		break;
	case UNPAUSE:
		systime = 0;
		count=0;

		break;
	default:
		break;
	}
}

void HCO_preloop::initParameters()
{

	output(0) = 0;
	output(1) = 0;
	dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
    RTXIspeed = 0.04; //this is the RTXI sampling rate, in msec
	systime = 0;
	count=0;
        nn=0;

// subtract leak current
    membrane_Eleak = 0;//mV
    membrane_gleak = 0.0;//mSiemens

    //H conductance
    Vhalf_h = -50;
    p_H = 1;
    V_th = -15;
    EH = -10; //mV
    gH = 0; //mSiemens
    tau_H=3000;

    //Synaptic Conductance
    Esyn = -80; //mV
    tau_syn = 100; //ms
    V_half = -50.0;  //mV
    V_slope = -2.0;  //mV
 //   V_slope = 10.0;  //mV
    gsyn = 0;

    //Imi
    Emi = 0; //mVolts
    Gmi = 0; //mSiemens

for (nn=0;nn<2;nn++)
    {
    out[nn] = 0;
    Vm[nn] = -50;
    mmi[nn]=0.0;
    

    H_m_inf[nn] = Xinf(-50,55.0,7.0); //m, H
    H_m_gate[nn] = H_m_inf[nn];
    gnmh[nn] = 0.0;
    H_m_tau[nn] = 0.0; //m, H
    IH[nn]=0.0;

    S_inf[nn] = 0;
    Svar[nn] = 0;
    Isyn[nn] = 0;

    }

}


