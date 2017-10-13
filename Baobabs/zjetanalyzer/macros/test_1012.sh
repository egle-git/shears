declare -a arr=(
Z_y_pt_0_0
Z_y_pt_1_0
Z_y_pt_2_0
Z_y_pt_3_0
Z_y_pt_4_0
Z_y_pt_0_1
Z_y_pt_1_1
Z_y_pt_2_1
Z_y_pt_3_1
Z_y_pt_4_1
Z_y_pt_0_2
Z_y_pt_1_2
Z_y_pt_2_2
Z_y_pt_3_2
Z_y_pt_4_2
Z_pt_y_0_0
Z_pt_y_1_0
Z_pt_y_2_0
Z_pt_y_3_0
Z_pt_y_0_1
Z_pt_y_1_1
Z_pt_y_2_1
Z_pt_y_3_1
Z_pt_y_0_2
Z_pt_y_1_2
Z_pt_y_2_2
Z_pt_y_3_2
inc_ljet_pt_y_fb_0
inc_ljet_pt_y_fb_1
inc_ljet_pt_y_fb_2
inc_ljet_pt_y_fb_3
inc_ljet_pt_y_fb_4
inc_ljet_pt_y_fb_5
inc_ljet_pt_y_fb_6
inc_ljet_y_pt_1
inc_ljet_y_pt_2
inc_ljet_y_pt_3
inc_ljet_y_pt_4
inc_ljet_pt_y_z0_fb_0
inc_ljet_pt_y_z0_fb_1
inc_ljet_pt_y_z0_fb_2
inc_ljet_pt_y_z0_fb_3
inc_ljet_pt_y_z0_fb_4
inc_ljet_pt_y_z0_fb_5
inc_ljet_pt_y_z0_fb_6
inc_ljet_pt_y_z1_fb_0
inc_ljet_pt_y_z1_fb_1
inc_ljet_pt_y_z1_fb_2
inc_ljet_pt_y_z1_fb_3
inc_ljet_pt_y_z1_fb_4
inc_ljet_pt_y_z1_fb_5
inc_ljet_pt_y_z1_fb_6
inc_ljet_y_pt_z0_1
inc_ljet_y_pt_z0_2
inc_ljet_y_pt_z0_3
inc_ljet_y_pt_z0_4
inc_ljet_y_pt_z1_1
inc_ljet_y_pt_z1_2
inc_ljet_y_pt_z1_3
inc_ljet_y_pt_z1_4
mjjjj
n_jet_30_unfold
MZElec
diElecpt
diElecrapidity
diElecphi
ljet_rapidity
ljet_eta
sljet_rapidity
sljet_eta
inc_ljet_pt_sljetpt_0
inc_ljet_pt_sljetpt_1
inc_ljet_pt_sljetpt_2
inc_ljet_pt_sljetpt_3
inc_ljet_pt_sljetpt_z0_0
inc_ljet_pt_sljetpt_z0_1
inc_ljet_pt_sljetpt_z0_2
inc_ljet_pt_sljetpt_z0_3
inc_ljet_pt_sljetpt_z1_0
inc_ljet_pt_sljetpt_z1_1
inc_ljet_pt_sljetpt_z1_2
inc_ljet_pt_sljetpt_z1_3
ljet_rapidity_0
ljet_eta_0
ljet_rapidity_1
ljet_eta_1
ljet_rapidity_2
ljet_eta_2
ljet_rapidity_3
ljet_eta_3
ljet_rapidity_4
ljet_eta_4
ljet_rapidity_5
ljet_eta_5
ljet_rapidity_6
ljet_eta_6
ljet_rapidity_7
ljet_eta_7
ljet_rapidity_8
ljet_eta_8
ljet_rapidity_9
ljet_eta_9
ljet_rapidity_10
ljet_eta_10
ljet_rapidity_11
ljet_eta_11
ljet_rapidity_12
ljet_eta_12
nvtx
jetpt_1
jetpt_2
jetY_1
jetY_2
Elecpt
Eleceta
Elecphi
)
declare -a title=(
"Y(Z) (|Y(j)|<1.5 , 20<P_{T}(Z)<30)"
"Y(Z) (|Y(j)|<1.5 , 30<P_{T}(Z)<50)"
"Y(Z) (|Y(j)|<1.5 , 50<P_{T}(Z)<100)"
"Y(Z) (|Y(j)|<1.5 , 100<P_{T}(Z)<200)"
"Y(Z) (|Y(j)|<1.5 , 200<P_{T}(Z)<1000)"
"Y(Z) (1.5<|Y(j)|<2.4 , 20<P_{T}(Z)<30)"
"Y(Z) (1.5<|Y(j)|<2.4 , 30<P_{T}(Z)<50)"
"Y(Z) (1.5<|Y(j)|<2.4 , 50<P_{T}(Z)<100)"
"Y(Z) (1.5<|Y(j)|<2.4 , 100<P_{T}(Z)<200)"
"Y(Z) (1.5<|Y(j)|<2.4 , 200<P_{T}(Z)<1000)"
"Y(Z) (2.4<|Y(j)|<4.7 , 20<P_{T}(Z)<30)"
"Y(Z) (2.4<|Y(j)|<4.7 , 30<P_{T}(Z)<50)"
"Y(Z) (2.4<|Y(j)|<4.7 , 50<P_{T}(Z)<100)"
"Y(Z) (2.4<|Y(j)|<4.7 , 100<P_{T}(Z)<200)"
"Y(Z) (2.4<|Y(j)|<4.7 , 200<P_{T}(Z)<1000)"
"P_{T}(Z) (|Y(j)|<1.5 , 0.<|Y(Z)|<0.9)"
"P_{T}(Z) (|Y(j)|<1.5 , 0.9<|Y(Z)|<1.5)"
"P_{T}(Z) (|Y(j)|<1.5 , 1.5<|Y(Z)|<2.1)"
"P_{T}(Z) (|Y(j)|<1.5 , 2.1<|Y(Z)|<2.4)"
"P_{T}(Z) (1.5<|Y(j)|<2.4 , 0.<|Y(Z)|<0.9)"
"P_{T}(Z) (1.5<|Y(j)|<2.4 , 0.9<|Y(Z)|<1.5)"
"P_{T}(Z) (1.5<|Y(j)|<2.4 , 1.5<|Y(Z)|<2.1)"
"P_{T}(Z) (1.5<|Y(j)|<2.4 , 2.1<|Y(Z)|<2.4)"
"P_{T}(Z) (2.4<|Y(j)|<4.7 , 0.<|Y(Z)|<0.9)"
"P_{T}(Z) (2.4<|Y(j)|<4.7 , 0.9<|Y(Z)|<1.5)"
"P_{T}(Z) (2.4<|Y(j)|<4.7 , 1.5<|Y(Z)|<2.1)"
"P_{T}(Z) (2.4<|Y(j)|<4.7 , 2.1<|Y(Z)|<2.4)"
"P_{T}(j) (0.0<|Y(j)|<0.5)"
"P_{T}(j) (0.5<|Y(j)|<1.0)"
"P_{T}(j) (1.0<|Y(j)|<1.5)"
"P_{T}(j) (1.5<|Y(j)|<2.0)"
"P_{T}(j) (2.0<|Y(j)|<2.5)"
"P_{T}(j) (2.5<|Y(j)|<3.2)"
"P_{T}(j) (3.2<|Y(j)|<4.7)"
"Y(j) (30<P_{T}(j)<50)"
"Y(j) (50<P_{T}(j)<100)"
"Y(j) (100<P_{T}(j)<200)"
"Y(j) (200<P_{T}(j)<1000)"
"P_{T}(j) (|Y(Z)|<1.0, 0.0<|Y(j)|<0.5)"
"P_{T}(j) (|Y(Z)|<1.0,0.5<|Y(j)|<1.0)"
"P_{T}(j) (|Y(Z)|<1.0,1.0<|Y(j)|<1.5)"
"P_{T}(j) (|Y(Z)|<1.0,1.5<|Y(j)|<2.0)"
"P_{T}(j) (|Y(Z)|<1.0,2.0<|Y(j)|<2.5)"
"P_{T}(j) (|Y(Z)|<1.0,2.5<|Y(j)|<3.2)"
"P_{T}(j) (|Y(Z)|<1.0,3.2<|Y(j)|<4.7)"
"P_{T}(j) (|Y(Z)|>1.0, 0.0<|Y(j)|<0.5)"
"P_{T}(j) (|Y(Z)|>1.0,0.5<|Y(j)|<1.0)"
"P_{T}(j) (|Y(Z)|>1.0,1.0<|Y(j)|<1.5)"
"P_{T}(j) (|Y(Z)|>1.0,1.5<|Y(j)|<2.0)"
"P_{T}(j) (|Y(Z)|>1.0,2.0<|Y(j)|<2.5)"
"P_{T}(j) (|Y(Z)|>1.0,2.5<|Y(j)|<3.2)"
"P_{T}(j) (|Y(Z)|>1.0,3.2<|Y(j)|<4.7)"
"Y(j) (|Y(Z)|<1.0,30<P_{T}(j)<50)"
"Y(j) (|Y(Z)|<1.0,50<P_{T}(j)<100)"
"Y(j) (|Y(Z)|<1.0,100<P_{T}(j)<200)"
"Y(j) (|Y(Z)|<1.0,200<P_{T}(j)<1000)"
"Y(j) (|Y(Z)|>1.0,30<P_{T}(j)<50)"
"Y(j) (|Y(Z)|>1.0,50<P_{T}(j)<100)"
"Y(j) (|Y(Z)|>1.0,100<P_{T}(j)<200)"
"Y(j) (|Y(Z)|>1.0,200<P_{T}(j)<1000)"
"M(jjjj)"
"n_jet_30_unfold"
"MZElec"
"diElecpt"
"diElecrapidity"
"diElecphi"
"ljet_rapidity"
"ljet_eta"
"sljet_rapidity"
"sljet_eta"
"P_{T}(j1) (30<P_{T}(j2)<50)"
"P_{T}(j1) (50<P_{T}(j2)<100)"
"P_{T}(j1) (100<P_{T}(j2)<200)"
"P_{T}(j1) (200<P_{T}(j2)<1000)"
"P_{T}(j1) (|Y(Z)|<1.0,30<P_{T}(j2)<50)"
"P_{T}(j1) (|Y(Z)|<1.0,50<P_{T}(j2)<100)"
"P_{T}(j1) (|Y(Z)|<1.0,100<P_{T}(j2)<200)"
"P_{T}(j1) (|Y(Z)|<1.0,200<P_{T}(j2)<1000)"
"P_{T}(j1) (|Y(Z)|>1.0,30<P_{T}(j2)<50)"
"P_{T}(j1) (|Y(Z)|>1.0,50<P_{T}(j2)<100)"
"P_{T}(j1) (|Y(Z)|>1.0,100<P_{T}(j2)<200)"
"P_{T}(j1) (|Y(Z)|>1.0,200<P_{T}(j2)<1000)"
"ljet_rapidity nvtx<=5"
"ljet_eta nvtx<=5"
"ljet_rapidity 5<nvtx<=10"
"ljet_eta 5<nvtx<=10"
"ljet_rapidity 10<nvtx<=15"
"ljet_eta 10<nvtx<=15"
"ljet_rapidity 15<nvtx<=20"
"ljet_eta 15<nvtx<=20"
"ljet_rapidity 20<nvtx<=25"
"ljet_eta 20<nvtx<=25"
"ljet_rapidity 25<nvtx<=30"
"ljet_eta 25<nvtx<=30"
"ljet_rapidity 30<nvtx<=35"
"ljet_eta 30<nvtx<=35"
"ljet_rapidity 35<nvtx<=40"
"ljet_eta 35<nvtx<=40"
"ljet_rapidity 40<nvtx<=45"
"ljet_eta 40<nvtx<=45"
"ljet_rapidity 45<nvtx<=50"
"ljet_eta 45<nvtx<=50"
"ljet_rapidity 50<nvtx<=55"
"ljet_eta 50<nvtx<=55"
"ljet_rapidity 55<nvtx<=60"
"ljet_eta 65<nvtx<=60"
"ljet_rapidity 60<nvtx<=65"
"ljet_eta 60<nvtx<=65"
"nvtx"
"P_{T}^{jet}"
"P_{T}^{jet}"
"#eta_{jet}"
"#eta_{jet}"
"P_{T}^{e}"
"#eta_{e}"
"#phi_{e}"
)


for ((n =0 ; n<${#arr[@]} ;n++ ))
do

                echo "${arr[$n]}"
                echo "${title[$n]}"
                cat plot_note.C | sed "s/__HISTNAME__/${arr[$n]}/g"> macro_tmp2.C 
if [ $n == 63 ] || [ $n == 60 ] || [ $n == 113 ] ; then
		cat macro_tmp2.C | sed "s/__NBIN__/4/g"> macro_tmp3.C
		cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C
else 

	                     cat macro_tmp2.C | sed "s/__NBIN__/1/g"> macro_tmp3.C
                             cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C		
       
fi
		root -l -q macro_tmp.C 

rm macro_tmp2.C
rm macro_tmp3.C
rm macro_tmp.C
done
