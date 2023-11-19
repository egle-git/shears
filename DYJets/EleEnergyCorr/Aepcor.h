#ifndef ElectroWeakAnalysis_Aepcor_H
#define ElectroWeakAnalysis_Aepcor_H

#include <boost/math/special_functions/erf.hpp>

class AsymCrystalBall{
    friend class Aepcor;
    private:
	static constexpr double pi = 3.14159;
	static const double sqrtPiOver2;
	static const double sqrt2;


	double m;
	double s;
	double r;
	double aL;
	double aH;
	double nL;
	double nH;

	double B[2];
	double E1;
	double D;
	double X;
	double N;
	double NA[2];
	double Ns;
	double NC[2];
	double F[2];
	double G[2];
	double K[2];
	double cdfm, cdfa[2];
	double amin;
	double rr[2];

	//to adjust functions with cropping
	double x_max;
	double f_norm;
	double cdf_m;

	bool _adjust_m;

	void init(){
	    x_max = 0;
	    f_norm = 1;
	    cdf_m = 0;
	    _adjust_m = false;
	    initBasic();
	    initParameters();
	    initBoundaryCDFs();
	}

	void initBasic(){
	    amin = std::min(aL, aH); //used for correcting m, want an offset due to change in r, not s
	    for(int i:{0,1}) B[i] = getB(i);
	    for(int i:{0,1}) K[i] = getK(i);
	    for(int i:{0,1}) F[i] = 1-fa(i)*fa(i)/n(i);
	    D = getD();
	}

	void initParameters(){
	    rr[0] = 1-r;
	    rr[1] = 1+r;
	    X = getX();
	    E1 = getE(1);
	    N = getN();
	    for(int i:{0,1}) NA[i] = N*getA(i);
	    Ns = N*s;
	    for(int i:{0,1}) NC[i] = rr[i]*Ns*getC(i);
	    for(int i:{0,1}) G[i]  = rr[i]*s*n(i)/fa(i);
	}

	void initBoundaryCDFs(){
	    cdfa[0] = cdf(m-rr[0]*aL*s, 1.0, false);
	    cdfa[1] = cdf(m+rr[1]*aH*s, 1.0, false);
	    cdfm    = cdf(m, 1.0, false);
	}
    
	void print() const{
	    std::cout << "AsymCrystalBall paarameters: " <<
		         "\n m  = " << m << 
		         "\n s  = " << s << 
		         "\n r  = " << r << 
		         "\n aL = " << aL << 
		         "\n aH = " << aH << 
		         "\n nL = " << nL << 
		         "\n nH = " << nH << std::endl;
	}

	double coreMean(){
	    return m - s*s*(rr[0]*rr[0]*(pdf(m)-pdf(-amin)) + rr[1]*rr[1]*(pdf(amin)-pdf(m))) / (cdf(amin)-cdf(-amin)); 
	}

	double n(int i) const{return i==0?nL:nH;}
	double fa(int i) const{return i==0? std::abs(aL) : std::abs(aH);}
	double ex(int i) const{return exp(-fa(i)*fa(i)/2);}
	double getA(int i) const{return pow(n(i)/fa(i), n(i))*ex(i);}
	double getC(int i) const{return n(i)/fa(i)/(n(i)-1)*ex(i);}
	double getB(int i) const{return n(i)/fa(i)-fa(i);}
	double getX() const{return sqrtPiOver2*(erf(fa(0)/sqrt2)*rr[0] + erf(fa(1)/sqrt2)*rr[1]) + getC(0)*rr[0] + getC(1)*rr[1];}
	double getN() const{return 1.0/s/X;}
	double getF(int i) const {return 1-fa(i)*fa(i)/n(i);}
	double getG(int i) const {return s*n(i)/fa(i);}
	double getD() const{return sqrtPiOver2*erf(fa(0)/sqrt2)+getC(0);}
	double getK(int i) const{return 1.0/(n(i)-1);}
	double getE(int i) const{ return X/getC(i)/rr[i];}

	template <typename Func>
	double mod(Func f, const std::vector<double>& _k) const{ 
	    auto n(*this);
	    n.update_params(_k);
	    return f(n);
	}

    public:
	AsymCrystalBall(double m_=0., double s_=1., double r_=0., double aL_=1., double aH_=1., double nL_=1., double nH_=1.): 
	    m(m_), s(s_), r(r_), aL(aL_), aH(aH_), nL(nL_), nH(nH_){
	    init();
	}

	void crop(double _x_max){
	    x_max = _x_max; 
	    cdf_m = cdf(-x_max, 1.0, false); 
	    f_norm  = 1.0 / (cdf(x_max, 1.0, false)-cdf_m);
	    initBoundaryCDFs();
	}

	void update_params(const std::vector<double>& _k){
	    double m_orig = coreMean();
	    auto _size = _k.size();
	    s *= _k[0];
	    r += _k[1];
	    if(_size>=3)  m += _k[2]; 
	    if(_size>=4)  nL = 1.0+_k[3]*(nL-1); 
	    if(_size>=5)  nH = 1.0+_k[4]*(nH-1);
	    if(_size>=6)  aL *= _k[5]; 
	    if(_size>=7)  aH *= _k[6];

	    initBasic();
	    initParameters();

	    if(_adjust_m){
		//adjust m to keep the core mean unchanged, 
		//ignoring the tails and change of the boundaries by r
		double delta = m_orig - coreMean(); 
		int _it=0;
		while(std::abs(delta)>1.e-05 and ++_it<10) {
		    m += delta;
		    delta = m_orig - coreMean();
		}
	    }
	    
	    if(x_max>0) crop(x_max);  //re-crop
	    initBoundaryCDFs(); 
	}

	double pdf(double x, double ks=1) const{ 
	    double d=(x-m)/(s*ks);
	    if(x_max>0 and std::abs(x)>x_max) return 0;
	    if(d/rr[0] < -aL) return f_norm * NA[0] / ks * pow(B[0]-d/rr[0], -nL);
	    if(d < 0)        return f_norm * N / ks * exp(-d*d/2/rr[0]/rr[0]);
	    if(d/rr[1] < aH) return f_norm * N / ks * exp(-d*d/2/rr[1]/rr[1]); 
	    return f_norm * NA[1] / ks * pow(B[1]+d/rr[1], -nH);
	}
	double cdf(double x, double ks=1, bool _crop=true) const{
	    if(_crop and x_max>0){
		if(x<-x_max) return 0;
		if(x> x_max) return 1;
	    }

	    double _k = _crop ? f_norm : 1.0;
	    double _o = _crop ? cdf_m : 0.0;
	    
	    double d=(x-m)/(s*ks);
	    if(d/rr[0] < -aL) return _k*(NC[0] / pow(F[0]-s*d/G[0], nL-1)-_o);
	    if(d < 0) return _k*(Ns*rr[0] * (D + sqrtPiOver2*erf(d/rr[0]/sqrt2))-_o);     
	    if(d/rr[1] < aH) return _k*(Ns*rr[0] *  D + Ns*rr[1]*sqrtPiOver2*erf(d/rr[1]/sqrt2)-_o);
	    return _k*(NC[1] * (E1 - pow(F[1]+s*d/G[1], 1-nH)) -_o);
	}
	double inv(double _u, double ks=1) const{
	    try{
		double u = _u/f_norm+cdf_m;
		if(u < cdfa[0]) return m + ks*G[0]*(F[0] - pow(NC[0]/u, K[0]));
		if(u < cdfm) return m + rr[0]*sqrt2*s*ks*boost::math::erf_inv((u/Ns/rr[0]-D)/sqrtPiOver2);
		if(u < cdfa[1]) return m + rr[1]*sqrt2*s*ks*boost::math::erf_inv((u-Ns*rr[0]*D)/Ns/rr[1]/sqrtPiOver2);
		return m - ks*G[1]*(F[1] - pow(E1-u/NC[1], -K[1]));
	    }
	    catch(std::exception &e){
		print();
		throw(e);
	    }
	}
	double pdf(double x, const std::vector<double>&_k) const{return mod([x](const AsymCrystalBall& n){return n.pdf(x);}, _k);}
	double cdf(double x, const std::vector<double>&_k) const{return mod([x](const AsymCrystalBall& n){return n.cdf(x);}, _k);}
	double inv(double x, const std::vector<double>&_k) const{return mod([x](const AsymCrystalBall& n){return n.inv(x);}, _k);}

	double weight(double x, double ks, double dr=0){return pdf(x, {ks, dr}) / pdf(x);}
};

class AsymExpGausExp{
    //Adding asymmetry to the core of the ExpGausExp: https://arxiv.org/abs/1603.08591
    friend class Aepcor;

    private:
	static constexpr double pi = 3.14159;
	const double sqrt_piover2;
	const double sqrt_2;

	double _m, _s, _kL, _kH, _a;

	double nL(double d) const{return exp(0.5*_kL*_kL + _kL*d)/_kL*(1-_a)*_s;}
	double nM(double d) const{return sqrt_piover2*erf(-d/sqrt_2)*(1-_a)*_s;}
	double nP(double d) const{return sqrt_piover2*erf( d/sqrt_2)*(1+_a)*_s;}
	double nH(double d) const{return exp(0.5*_kH*_kH - _kH*d) /_kH*(1+_a)*_s;}

	double norm() const{
	    return nL(-_kL) + nM(-_kL) + nP(_kH) +  nH(_kH);
	}

    public:
	AsymExpGausExp(double m=0, double s=1, double kL=1, double kH=1, double a=0):
	    sqrt_piover2(sqrt(pi/2)), sqrt_2(sqrt(2)),
	    _m(m), _s(s), _kL(kL), _kH(kH), _a(a){};

	AsymExpGausExp(const double *p):
	    sqrt_piover2(sqrt(pi/2)), sqrt_2(sqrt(2)),
	    _m(p[0]), _s(p[1]), _kL(p[2]), _kH(p[3]), _a(p[4]){};

	AsymExpGausExp change_by(const double *p) const{
	    return AsymExpGausExp(_m+p[0], _s*p[1], _kL*p[2], _kH*p[3], _a+p[4]);
	}

	double pdf(double x, const double *p=0) const{
	    if(p) return change_by(p).pdf(x);
	    double d = x - _m;
	    double dm = d/_s/(1.0-_a);
	    double dp = d/_s/(1.0+_a);
	    double n = norm();
	    if(dm<-_kL) return exp(0.5*_kL*_kL + _kL * dm) / n;
	    if(d<0) return exp(-0.5*dm*dm) / n;
	    if(dp<_kH) return exp(-0.5*dp*dp) / n;
	    return exp(0.5*_kH*_kH - _kH * dp) / n;
	}

	double cdf(double x, const double *p=0) const{
	    if(p) return change_by(p).cdf(x);
	    double d = x - _m;
	    double dm = d/_s/(1.0-_a);
	    double dp = d/_s/(1.0+_a);
	    double n = norm();
	    if(dm <-_kL) return nL(dm)/ n;
	    if(d  <   0) return ( nL(-_kL) - nM(dm) + nM(-_kL)) / n;
	    if(dp < _kH) return ( nL(-_kL) + nM(-_kL) + nP(dp)) / n;
	    return ( nL(-_kL) + nM(-_kL) + nP(_kH) + nH(_kH) - nH(dp)) / n;
	}

	double inv(double u, const double *p=0) const{
	    if(p) return change_by(p).inv(u);
	    double fkL = cdf(_m - (1.0-_a)*_kL*_s);
	    double fkH = cdf(_m + (1.0+_a)*_kH*_s);
	    double fm0 = cdf(_m);
	    double n = norm();
	    if(u<fkL) return (log(u*n*_kL/(1-_a)/_s) - 0.5*_kL*_kL)/_kL*_s*(1-_a) + _m;
	    if(u<fm0) return _m-_s*(1-_a)*sqrt_2*boost::math::erf_inv((nL(-_kL)+nM(-_kL) - u*n)/sqrt_piover2/_s/(1-_a));
	    if(u<fkH) return _m+sqrt_2*_s*(1+_a)*boost::math::erf_inv((u*n-nL(-_kL)-nM(-_kL))/sqrt_piover2/_s/(1+_a));
	    return _m + _s*(1+_a)*(0.5*_kH*_kH - log((nL(-_kL) + nM(-_kL) + nP(_kH) + nH(_kH) - u*n)*_kH/(1+_a)/_s))/_kH;
	}
};


struct Aepres{
    enum MorphingType {SPREAD, MORPH};
    enum ResolParameterization {CB, EG};
    enum TYPE {MC, Data, Extra};
    static const std::vector<std::string> ExtParCB;
    static const std::vector<std::string> ExtParEG;
    static constexpr double _max_pull = 10.0;

    MorphingType _MT;

    struct ResParams{
	ResolParameterization _RP;
	double eta; 
	//pull correction parameters
	std::vector<double> kRes;  
	std::vector<double> dCB[6]; //the rest: r, m, nL, nH, aL, aH;
	std::vector<double> dEG[5]; //m, ks, kL, kH, r;
	std::vector<double> nTrk[2]; 
	std::vector<double> rsPar[4]; 
	std::vector<AsymCrystalBall> cb;
	std::vector<AsymExpGausExp> eg;
	ResParams():eta(0){}
	std::vector<double> k_cb(int _tp) const {
	    TYPE tp = _tp>0 ? Data : MC;
	    return std::vector<double>({kRes[_tp], 
		                        dCB[0][tp], 
					dCB[1][tp], 
					dCB[2][tp], 
					dCB[3][tp],
					dCB[4][tp],
					dCB[5][tp]
					});
	}

	std::vector<double> k_eg(int _tp) const {
	    TYPE tp = _tp>0 ? Data : MC;
	    return std::vector<double>({dEG[0][tp], 
					dEG[1][tp],
					dEG[2][tp], 
					dEG[3][tp], 
					dEG[4][tp]});
	}
	double _cdf(int _tp, int iext, double x) const{
	    if(_RP==CB) return cb[iext].cdf(x, k_cb(_tp)); 
	    return eg[iext].cdf(x, k_eg(_tp).data()); 
	}

	double _inv(int _tp, int iext, double x) const{
	    if(_RP==CB) return cb[iext].inv(x, k_cb(_tp)); 
	    return eg[iext].inv(x, k_eg(_tp).data()); 
	}
    };

    int NETA;
    int NEXT;
    int NMIN;

    std::vector<ResParams> resol;

    Aepres();

    int etaBin(double x) const;
    int trkBin(double x, int h, TYPE T=MC) const;
    void reset();

    double rndm(int H, int F, double v) const;
    double Sigma(double pt, int H, int F) const;
    double kMorph(double gpt, double rpt, int ieta, int ir9, int iRun) const;
    double kSpread(double gpt, double rpt, int ieta, int iRun) const;
    double kSmear(double pt, double eta, int iRun, double u2, double u3, double kSigma=1) const;
    double kExtra(double pt, double eta, int nlayers, double u, double w) const;
    double kExtra(double pt, int ieta, int ir9, double u) const;
};

// const std::vector<std::string> Aepres::ExtParCB = {"r", "m", "nL", "nH", "aL", "aH"};
// const std::vector<std::string> Aepres::ExtParEG = {"_m", "_s", "_kL", "_kH", "_a"};

class Aepcor{
    private:
	enum TVAR{Default, Replica, Symhes};

	std::string __version__;
	static const double MPHI; 

	int NETA;
	int NPHI; 
	int NRUN;

	int RETA;
	int NEXT;
	int NRUNRES;

	double DPHI;
	std::vector<double> etabin;
	std::vector<double> extbin;
	std::vector<int> runbin;
	std::vector<double> frunres;

	struct ShapeParams{
	    double M; 
	    double A;
	    double D;
	    ShapeParams(){reset();}
	    void reset(){M=1; A=0; D=0;}
	    double kCor(double pt) const{return M + A/pt - D*sqrt(pt);}
	};

	struct Aepone{
	    Aepres RR;
	    std::vector<std::vector<ShapeParams>> cShape[2];
	    std::vector<std::vector<double>> cPhi[2];
	    std::vector<std::vector<double>> cRun;
	};

	int nset;
	std::vector<int> nmem;
	std::vector<int> tvar;
	std::vector<std::vector<Aepone>> RC;
	template<typename T> int getBin(T x, const std::vector<T>& b) const;
	int phiBin(double phi) const;

    public:
	enum TYPE{MC, DT};
	Aepcor(); 
	Aepcor(std::string filename, Aepres::ResolParameterization rp=Aepres::CB, Aepres::MorphingType mt=Aepres::MORPH); 
	void init(std::string filename, Aepres::ResolParameterization rp=Aepres::CB, Aepres::MorphingType mt=Aepres::MORPH); 
	void reset();

	const Aepres& getRes(int s=0, int m=0) const {return RC[s][m].RR;}
	bool empty() const{return RC.empty();}
	double getM(int T, int H, int F, int s=0, int m=0) const{return empty() ? 1.0 : RC[s][m].cShape[T][H][F].M;}
	double getA(int T, int H, int F, int s=0, int m=0) const{return empty() ? 0.0 : RC[s][m].cShape[T][H][F].A;}
	double getD(int T, int H, int F, int s=0, int m=0) const{return empty() ? 0.0 : RC[s][m].cShape[T][H][F].D;}
	double getP(int T, int H, int F, int s=0, int m=0) const{return empty() ? 1.0 : RC[s][m].cPhi[T][H][F];}
	double getR(int H, int F, int s=0, int m=0) const{return empty() ? 1.0 : RC[s][m].cRun[H][F];}

	double getK(int T, int H, int s=0, int m=0) const{return empty() ? 1.0 : RC[s][m].RR.resol[H].kRes[T];}
	double kGenSmear(double pt, double eta, double u0, double u1, double u2, double kSigma=1, int s=0, int m=0) const;
	double kScaleMC(double pt, double eta, double phi, double r9, int s=0, int m=0) const;

	double kScaleDTrun(double pt, double eta, double phi, double r9, int irun, int s=0, int m=0) const;
	double kScaleDT(double pt, double eta, double phi, double r9, int run, int s=0, int m=0) const;
	double kScaleDT4MC(double pt, double eta, double phi, double r9, double uRun, int s=0, int m=0) const;
	double kSpreadMC(double pt, double eta, double phi, double r9, double uRun, double gt, int s=0, int m=0) const;
	double kSmearMC(double pt, double eta, double phi, double r9, double u, int s=0, int m=0) const;
};

#endif

