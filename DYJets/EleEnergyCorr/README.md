Electron momentum correction package.

```cpp
Aepcor ec; // e.g. as a class member
ec.init("path/to/e_18UL.txt", Aepres::CB); //e.g. in a class constructor
```

For each data electron, momentum k factor
```cpp
double kData = ec.kScaleDT(pt, eta, phi, r9, run);
```

For each MC electron, matched with dressed generated electron, momentum k factor
```cpp
double u = gRandom->Rndm();
double kMC = ec.kSpreadMC(pt, eta, phi, r9, u, ptGen);
```

For other MC electrons only scale (i.e. no resolution) correction can be applied
```cpp
double kMC = ec.kScaleMC(pt, eta, phi, r9);
```

Each function above supports additional following *iset* and *imem* indices to be used to evaluate uncertainties
```
iset imem   Description
0    0      Default
1    0-399  Statistical replicas
2    0      Z pt modeling
3    0      alternative mass range
4    0      nonlinearity (removed non-linear term)
5    0      asymmetric resolution correction
6    0      choice of scale reference point (fix pull means to zero)
7    0      run-inclusive resolution correction
8    0      background systematics
```
