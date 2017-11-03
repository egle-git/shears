{
TFile *f2 = new TFile("data.root");
f2->cd();
cout<<"Data2012_1601[60]={";
double sum=0;
for(int i =1; i<61; i++){
  sum+=pup->GetBinContent(i);
}
for(int i =1; i<61; i++){
  cout<<pup->GetBinContent(i)/sum<<",";
}
 cout<<"};"<<endl;
}
