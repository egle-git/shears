#include <iostream>
#include <vector>
void infile_lister(){

  ifstream file("chi2_hb.txt");

  string data[4];
  double val=9999.;
  double val2=9999.;
  int niter=0; 
  cout<<"double niter[]={"<<endl;
  while( file ){
      for (int i=0;i<4;i++)
      {
          file>>data[i];          
      }
      val=atof(data[3].c_str());
     // cout <<niter%20<<" "<<data[3] <<"  "<<val<<endl ;
      if(val2>1. &&val<1.)cout<<niter%20<<",";
      val2=atof(data[3].c_str());
      niter++;
  }

 cout<<"};"<<endl;

}
