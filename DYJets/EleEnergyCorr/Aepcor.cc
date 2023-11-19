#ifndef ElectroWeakAnalysis_Aepcor
#define ElectroWeakAnalysis_Aepcor

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <exception>
#include "Aepcor.h"

const double AsymCrystalBall::sqrtPiOver2 = sqrt(AsymCrystalBall::pi/2.0);
const double AsymCrystalBall::sqrt2 = sqrt(2.0);

const std::vector<std::string> Aepres::ExtParCB = {"r", "m", "nL", "nH", "aL", "aH"};
const std::vector<std::string> Aepres::ExtParEG = {"_m", "_s", "_kL", "_kH", "_a"};

Aepres::Aepres(){
    reset();
}

void Aepres::reset(){
    NETA=0;
    NEXT=0;
    NMIN=0;
    std::vector<ResParams>().swap(resol);
}

int Aepres::etaBin(double eta) const{
    double abseta=std::abs(eta);
    for(int i=0; i<NETA-1; ++i) if(abseta<resol[i+1].eta) return i;
    return NETA-1;
}

int Aepres::trkBin(double x, int h, TYPE T) const{
    for(int i=0; i<NEXT-1; ++i) if(x<resol[h].nTrk[T][i+1]) return i;
    return NEXT-1;
}

double Aepres::Sigma(double pt, int H, int F) const{
    const ResParams &rp = resol[H];
    double a = rp.rsPar[0][F];
    double b = rp.rsPar[1][F];
    double c = rp.rsPar[2][F];
    double e = sqrt(a*a + b*b/pt + c*c/pt/pt);
    double d = rp.rsPar[3][F];
    if(std::abs(d)<1e-6) return e; 
    double t = d*pt;
    return e*t/sqrt(e*e+t*t);
}

double Aepres::rndm(int H, int F, double w) const{
    const ResParams &rp = resol[H];
    return rp.nTrk[MC][F]+(rp.nTrk[MC][F+1]-rp.nTrk[MC][F])*w; 
}

double Aepres::kMorph(double gpt, double rpt, int ieta, int ir9, int iRun) const{
    if(_MT==SPREAD) return kSpread(gpt, rpt, ieta, iRun);

    double kold = rpt/gpt;
    double knew = kold;
    const ResParams &rp = resol[ieta];
    double s = Sigma(gpt,ieta,ir9); 
    double u = 0;

    double _pull = (kold-1.0)/s; 
    if(std::abs(_pull) >= _max_pull) return 1.0; 

    u = rp._cdf(MC, ir9, _pull); 
    knew = 1.0 + s*rp._inv(1+iRun, ir9,  u); 

    if(knew<0) return 1.0;
    double r = knew/kold;
    return r;
}


double Aepres::kSpread(double gpt, double rpt, int ieta, int iRun) const{
    const auto &k = resol[ieta].kRes;
    double x = rpt / gpt;
    return (1.0 + (x - 1.0)*k[1+iRun]/k[MC]) / x;
}

double Aepres::kSmear(double pt, double eta, int iRun, double u1, double u2, double kSigma) const{
    int H = etaBin(std::abs(eta));
    int F = trkBin(u1, H); 
    const ResParams &rp = resol[H];
    double s = kSigma*Sigma(pt, H, F); 
    return 1.0 + s * rp._inv(1+iRun, F, u2); 
}

double Aepres::kExtra(double pt, double eta, int n, double u, double w) const{
    int H = etaBin(std::abs(eta));
    int F = n>NMIN ? n-NMIN : 0;
    const ResParams &rp = resol[H];
    double v = rp.nTrk[MC][F]+(rp.nTrk[MC][F+1]-rp.nTrk[MC][F])*w;
    int D = trkBin(v, H, Data);
    double RD = rp.kRes[Data]*Sigma(pt, H, D);
    double RM = rp.kRes[MC]*Sigma(pt, H, F);
    double x = RD>RM ? sqrt(RD*RD-RM*RM)*rp.cb[F].inv(u) : 0;
    if(x<=-1) return 1.0;
    return 1.0 + x;
    return 1.0/(1.0 + x); 
}

double Aepres::kExtra(double pt, int ieta, int ir9, double u) const{
    const ResParams &rp = resol[ieta];
    double d = rp.kRes[Data];
    double m = rp.kRes[MC];
    double x = d>m ? sqrt(d*d-m*m) * Sigma(pt, ieta, ir9) * rp.cb[ir9].inv(u) : 0;
    if(x<=-1) return 1.0;
    return 1.0 + x;
}


Aepcor::Aepcor(){}

Aepcor::Aepcor(std::string filename, Aepres::ResolParameterization rp, Aepres::MorphingType mt){
    init(filename, rp, mt);
}

void Aepcor::reset(){
    NETA=0;
    NPHI=0;
    NRUN=0;
    std::vector<double>().swap(etabin);
    std::vector<double>().swap(extbin);
    std::vector<int>().swap(runbin);
    nset=0;
    std::vector<int>().swap(nmem);
    std::vector<std::vector<Aepone>>().swap(RC);
}


void Aepcor::init(std::string filename, Aepres::ResolParameterization rp, Aepres::MorphingType mt){
    std::ifstream in(filename.c_str());
    if(in.fail()) throw std::invalid_argument("Aepcor::init could not open file " + filename);

    int RMIN(0);
    RETA = 0;
    NEXT = 0;
    std::vector<double> BETA;

    std::string tag;
    int type, sys, mem, var, bin;	
    std::string s;
    while(std::getline(in, s)){
	std::stringstream ss(s); 
	if(s.substr(0,7)=="VERSION") {
	    ss >> s >> __version__;
	    // std::cout << Form("%-8s %s", "Aepcor:", __version__.c_str()) << std::endl; 
	    continue;
	}
	std::string first4=s.substr(0,4);
	if(first4=="NSET"){
	    ss >> tag >> nset;
	    nmem.resize(nset);
	    tvar.resize(nset);
	    RC.resize(nset);
	}
	else if(first4=="NMEM") {
	    ss >> tag;
	    for(int i=0; i<nset; ++i) {
		ss >> nmem[i];
		RC[i].resize(nmem[i]);
	    }
	}
	else if(first4=="TVAR") {
	    ss >> tag;
	    for(int i=0; i<nset; ++i) ss >> tvar[i];
	}
	else if(first4=="RMIN") ss >> tag >> RMIN;
	else if(first4=="NEXT") {
	    ss >> tag >> NEXT;
	    extbin.resize(NEXT+1);
	    for(auto &h: extbin) ss >> h;
	}
	else if(first4=="RETA") {
	    ss >> tag >> RETA;
	    BETA.resize(RETA+1);
	    for(auto &h: BETA) ss >> h;

	}
	else if(first4=="CPHI") {
	    ss >> tag >> NPHI; 
	    DPHI=2*AsymCrystalBall::pi/NPHI;
	}
	else if(first4=="CETA")  {
	    ss >> tag >> NETA;
	    etabin.resize(NETA+1);
	    for(auto& h: etabin) ss >> h;
	}
	else if(first4=="KRUN")  {
	    ss >> tag >> NRUN;
	    runbin.resize(NRUN+1);
	    for(auto& h: runbin) ss >> h;
	}
	else if(first4=="FRUN")  {
	    ss >> tag >> NRUNRES;
	    frunres.resize(NRUNRES+1);
	    for(auto &h: frunres) ss >> h;
	}
	else{ 
	    ss >> sys >> mem >> tag;
	    auto &rc = RC[sys][mem]; 
	    rc.RR.NETA=RETA;
	    rc.RR.NEXT=NEXT;
	    rc.RR.NMIN=RMIN;
	    auto &resol = rc.RR.resol;
	    if(resol.empty()){
		resol.resize(RETA);
		for(size_t ir=0; ir<resol.size(); ++ir){
		    auto &r = resol[ir];
		    r.eta = BETA[ir];
		    r.kRes.resize(NRUNRES+1, 1.0);
		    for(auto i: {0,1})     r.dCB[i].resize(2, 0.0);
		    for(auto i: {2,3,4,5}) r.dCB[i].resize(2, 1.0);
		    for(auto i: {0,4}) r.dEG[i].resize(2, 0.0);
		    for(auto i: {1,2,3}) r.dEG[i].resize(2, 1.0);
		    r.cb.resize(NEXT);
		    r.eg.resize(NEXT);
		    for(auto i:{0,1})r.nTrk[i].resize(NEXT+1);
		    for(auto i:{0,1,2,3}) r.rsPar[i].resize(NEXT);
		}
	    }

	    //init with 1's and 0's
	    auto &cs = rc.cShape;
	    for(TYPE T:{MC,DT}){
		if(cs[T].empty()){
		    cs[T].resize(RETA);
		    for(auto &i: cs[T]) {
			i.resize(NEXT);
			for(auto &k: i) k.reset();
		    }
		}
	    }
	    auto &cp = rc.cPhi;
	    for(TYPE T:{MC,DT}){
		if(cp[T].empty()){
		    cp[T].resize(NETA);
		    for(auto &i: cp[T]) i.resize(NPHI, 1.0);
		}
	    }

	    auto &ck = rc.cRun;
	    if(ck.empty()){
		ck.resize(RETA);
		for(auto& i: ck) i.resize(NRUN, 1.0);
	    }

	    auto loadCBCor = [&](int _k){
		ss >> type;
		for(int i=0; i<RETA; ++i) ss >> resol[i].dCB[_k][type]; 
	    };

	    auto loadEGCor = [&](int _k){
		ss >> type;
		for(int i=0; i<RETA; ++i) ss >> resol[i].dEG[_k][type]; 
	    };


	    if(tag=="R"){
		ss >> var >> bin; 
		for(int i=0; i<NEXT; ++i) {
		    switch(var){
			case 0: ss >> resol[bin].rsPar[var][i]; break;
			case 1: ss >> resol[bin].rsPar[var][i]; break;
			case 2: ss >> resol[bin].rsPar[var][i]; break; 
			case 3: ss >> resol[bin].rsPar[var][i]; break; 
			case 4: ss >> resol[bin].cb[i].m; break; 
			case 5: ss >> resol[bin].cb[i].s; break; 
			case 6: ss >> resol[bin].cb[i].r; break; 
			case 7: ss >> resol[bin].cb[i].aL; break; 
			case 8: ss >> resol[bin].cb[i].nL; break; 
			case 9: ss >> resol[bin].cb[i].aH; break; 
			case 10: ss >> resol[bin].cb[i].nH; break; 
			case 11: ss >> resol[bin].eg[i]._m; break; 
			case 12: ss >> resol[bin].eg[i]._s; break; 
			case 13: ss >> resol[bin].eg[i]._kL; break; 
			case 14: ss >> resol[bin].eg[i]._kH; break; 
			case 15: ss >> resol[bin].eg[i]._a; break; 
			default: break;
		    }
		}
	    }
	    else if(tag=="T") {
		ss >> type >> bin; 
		for(int i=0; i<NEXT+1; ++i) ss >> resol[bin].nTrk[type][i];
	    }
	    else if(tag=="F") {
		ss >> type; //here type includes iRun 
		for(int i=0; i<RETA; ++i) {
		    ss >> resol[i].kRes[type]; 
		    if(type==1) for(int k=2; k<=NRUNRES; ++k) resol[i].kRes[k] = resol[i].kRes[type]; //FIXME move this to the data file 
		}

	    }
	    else if(tag==Aepres::ExtParCB[0]) loadCBCor(0);
	    else if(tag==Aepres::ExtParCB[1]) loadCBCor(1);
	    else if(tag==Aepres::ExtParCB[2]) loadCBCor(2);
	    else if(tag==Aepres::ExtParCB[3]) loadCBCor(3);
	    else if(tag==Aepres::ExtParCB[4]) loadCBCor(4);
	    else if(tag==Aepres::ExtParCB[5]) loadCBCor(5);
	    else if(tag==Aepres::ExtParEG[0]) loadEGCor(0);
	    else if(tag==Aepres::ExtParEG[1]) loadEGCor(1);
	    else if(tag==Aepres::ExtParEG[2]) loadEGCor(2);
	    else if(tag==Aepres::ExtParEG[3]) loadEGCor(3);
	    else if(tag==Aepres::ExtParEG[4]) loadEGCor(4);
	    else if(tag=="S") {
		ss >> type >> var >> bin; 
		for(int i=0; i<NEXT; ++i){
		    auto &x = cs[type][bin][i];
		    if(var==0) { ss >> x.M; x.M = 1.0+x.M/100;}
		    else if(var==1){ ss >> x.A; }
		    else if(var==2){ ss >> x.D; x.D/=100; }
		}
	    }
	    else if(tag=="K") {
		ss >> bin; 
		for(int i=0; i<NRUN; ++i){
		    auto &x = ck[bin][i];
		    ss >> x;
		    x = 1.0 + x/100;
		}
	    }
	    else if(tag=="C") {
		ss >> type >>  bin; 
		for(int i=0; i<NPHI; ++i){
		    auto &x = cp[type][bin][i];
		    ss >> x;
		    x = 1.0 + x/100;
		}
	    }
	}
    }

    for(auto &rcs: RC)
       for(auto &rcm: rcs){
	   rcm.RR._MT = mt; 
           for(auto &r: rcm.RR.resol){
	       r._RP = rp; 
               for(auto &i: r.cb) {
		   i.init();
		   if(rp==Aepres::CB) i.crop(Aepres::_max_pull); 
		   if(mt==Aepres::SPREAD) {
		       i.m = 0; 
		       i._adjust_m = false;
		   }
	       }
	   }
       }
    in.close();
}

const double Aepcor::MPHI=-AsymCrystalBall::pi;

template<typename T>
int Aepcor::getBin(T x, const std::vector<T>& b) const{
    if(x<=b[0]) return 0;
    auto it = upper_bound(b.begin(), b.end(), x);
    if(it==b.end()) return b.size()-2;
    return it - b.begin() -1;
}

int Aepcor::phiBin(double x) const{
    int ibin=(x-MPHI)/DPHI;
    if(ibin<0) return 0; 
    if(ibin>=NPHI) return NPHI-1;
    return ibin;
}

double Aepcor::kScaleDTrun(double pt, double eta, double phi, double r9, int irun, int s, int m) const{
    if(empty()) return 1.0;
    int ieta = getBin(eta, etabin);
    int iAbsEta = ieta>=NETA/2 ? ieta-NETA/2 : NETA/2-ieta-1; 
    int iphi = phiBin(phi);
    int ir9 = getBin(r9, extbin);
    const auto &r = RC[s][m]; 
    return r.cRun[iAbsEta][irun] * r.cShape[DT][iAbsEta][ir9].kCor(pt) * r.cPhi[DT][ieta][iphi];
}

double Aepcor::kScaleDT(double pt, double eta, double phi, double r9, int run, int s, int m) const{
    if(empty()) return 1.0;
    return kScaleDTrun(pt, eta, phi, r9, getBin(run, runbin), s, m); 
}

double Aepcor::kScaleDT4MC(double pt, double eta, double phi, double r9, double uRun, int s, int m) const{
    if(empty()) return 1.0;
    return kScaleDTrun(pt, eta, phi, r9, getBin(uRun, frunres), s, m); 
}


double Aepcor::kScaleMC(double pt, double eta, double phi, double r9, int s, int m) const{
    if(empty()) return 1.0;
    int ieta = getBin(eta, etabin);
    int iAbsEta = ieta>=NETA/2 ? ieta-NETA/2 : NETA/2-ieta-1; 
    int iphi = phiBin(phi);
    int ir9 = getBin(r9, extbin);
    const auto &r = RC[s][m]; 
    return r.cShape[DT][iAbsEta][ir9].kCor(pt) * r.cPhi[MC][ieta][iphi]; 
}

double Aepcor::kSpreadMC(double pt, double eta, double phi, double r9, double uRun, double gt, int s, int m) const{
    if(empty()) return 1.0;
    auto _print = [&](){
	std::cout <<  " " <<pt << " " << eta << " " << phi << " " << r9 << " " << uRun << " " << gt << " " << s << " " << m <<std::endl;
    }; 
    try{
	int ieta = getBin(eta, etabin);
	int iAbsEta = ieta>=NETA/2 ? ieta-NETA/2 : NETA/2-ieta-1; 
	int iphi = phiBin(phi);
	int ir9 = getBin(r9, extbin);
	const auto &r = RC[s][m];
	double k = r.cShape[MC][iAbsEta][ir9].kCor(pt) * r.cPhi[MC][ieta][iphi];
	int iRun = getBin(uRun, frunres);
	if(iRun>=NRUNRES) iRun=NRUNRES-1;
	double _kres =r.RR.kMorph(gt, k*pt, iAbsEta, ir9, iRun);
	return k * _kres;
    }catch(std::exception &e){
	_print();
	throw e;
    }
}

double Aepcor::kSmearMC(double pt, double eta, double phi, double r9, double u, int s, int m) const{
    if(empty()) return 1.0;
    int ieta = getBin(eta, etabin);
    int iAbsEta = ieta>=NETA/2 ? ieta-NETA/2 : NETA/2-ieta-1; 
    int iphi = phiBin(phi);
    int ir9 = getBin(r9, extbin);
    const auto &r = RC[s][m];
    double k = r.cShape[MC][iAbsEta][ir9].kCor(pt) * r.cPhi[MC][ieta][iphi];
    return k * r.RR.kExtra(k*pt, iAbsEta, ir9, u);
}

double Aepcor::kGenSmear(double pt, double eta, double u0, double u1, double u2, double kSigma, int s, int m) const{
    if(empty()) return 1.0;
    int iRun = getBin(u0, frunres);
    if(iRun>=NRUNRES) iRun=NRUNRES-1;
    return RC[s][m].RR.kSmear(pt, eta, iRun, u1, u2, kSigma);
}

#endif

