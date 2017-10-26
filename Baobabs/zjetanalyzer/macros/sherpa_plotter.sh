declare -a arr=(
GMZElec
GMZElec_mu
GMZElec_st3mu
GMZElec_st3e
GdiElecpt
GdiElecrapidity
GdiElecphi
n_jet_30_gen
Gjetpt_1
Gjetpt_2
GjetY_1
GjetY_2
Gjetpt_3
Gjetpt_4
GjetY_3
GjetY_4

)
declare -a title=(
"MZElec_Gen"
"MZMu_Gen"
"MZMu_Gen_ST3"
"MZElec_Gen_St3"
"diElecpt_Gen"
"diElecrapidity_Gen"
"diElecphi_Gen"
"n_jet_30_gen"
"Gjetpt_1"
"Gjetpt_2"
"GjetY_1"
"GjetY_2"
"Gjetpt_3"
"Gjetpt_4"
"GjetY_3"
"GjetY_4"

)
# You can access them using echo ${arr[0]}, ${arr[1]}
for ((n =0 ; n<${#arr[@]} ;n++ ))
do

                echo "${arr[$n]}"
                echo "${title[$n]}"
                cat sherpa_plotter.C | sed "s/__HISTNAME__/${arr[$n]}/g"> macro_tmp2.C 
if [ $n == 4 ] ; then
		cat macro_tmp2.C | sed "s/__NBIN__/8/g"> macro_tmp3.C
		cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C

else
      if [ $n == 0 ] || [ $n == 7 ] || [ $n == 8 ] ; then
		cat macro_tmp2.C | sed "s/__NBIN__/1/g"> macro_tmp3.C
		cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C
      else 
            if [ $n == 4 ]; then
		cat macro_tmp2.C | sed "s/__NBIN__/8/g"> macro_tmp3.C
		cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C
      else 
	                     cat macro_tmp2.C | sed "s/__NBIN__/1/g"> macro_tmp3.C
                             cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C		
           fi 
      fi 
fi
		root -l -q macro_tmp.C 
#               cat qbh1.cc | sed "s/__mp__/$i/g"> qbh2.cc
#               cat qbh2.cc | sed "s/__mm__/$j/g"> qbh.cc
#               echo -n "$i "
rm macro_tmp2.C
rm macro_tmp3.C
rm macro_tmp.C
done

