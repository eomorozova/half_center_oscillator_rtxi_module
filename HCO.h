

//
// Half center oscillator
//
// by Morozova E.O.

#include <default_gui_model.h>
#include <math.h>
#include <string>

class HCO : public DefaultGUIModel
{
	public:
		HCO(void);
		virtual ~HCO(void);
		virtual void execute(void);

	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

	private:
		void initParameters();

		int nn;

		double Vm[2];
		double Temp;
		double leak_subtract1;
		double membrane_Eleak1;
		double membrane_gleak1;
		double leak_subtract2;
		double membrane_Eleak2;
		double membrane_gleak2;		
		
		double out[2];

		double Vhalf_h;
		double V_th;
		double H_m_tau[2];
		double H_m_inf[2];
		double H_m_gate[2];
		double gnmh[2];
		double p_H;
		double H_h_tau[2];
		double H_h_inf[2];
		double H_h_gate[2];
		double IH[2];
		double EH;
		double gH;
		double tauH;

		double S_inf[2];
		double V_half;
		double V_slope;
		double Svar[2];
		double tauSyn;
		double Isyn[2];
		double Esyn;
		double gsyn;

		double Gmi;
		double Emi;
		double tau_mi;
		double mmi[2];

		double Xinf(double, double, double);
		double tauX(double, double, double, double, double);
		double KR_inf_update(double);

		double m_mi_inf_update(double);

		double dt; // system period
		double systime;
		long long count;
		double RTXIspeed,lastRead_timestep;
};
