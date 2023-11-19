First version of electron corrections. 

```cpp
Aepcor ec; // e.g. as a class member
ec.init("path/to/e_18UL.txt", Aepres::CB); //e.g. in a class constructor
```


For each data electron, momentum k factor
```cpp
double kData = ec.kScaleDT(pt, eta, phi, r9, run); // for data electron
```

For each MC electron matched with a dressed electron, momentum k factor
```cpp
double urnd = gRandom->Rndm(); // uniform between 0 and 1
double kMC = ec.kSpreadMC(pt, eta, phi, r9, urnd, genPt); 
```
