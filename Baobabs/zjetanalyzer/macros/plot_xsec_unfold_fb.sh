declare -a arr=( 
#inc_ljet_pt_y_
#inc_ljet_pt_sljetpt_
#inc_ljet_pt_y_
#inc_ljet_pt_sljetpt_
#inc_ljet_pt_y_
#inc_ljet_pt_sljetpt_
inc_ljet_pt_y_fb_
inc_ljet_pt_y_fb_
inc_ljet_pt_y_fb_
)
declare -a title=(
"leading jet pt (GeV)"
"leading jet pt (GeV)"
"leading jet pt (GeV)"
)
# You can access them using echo ${arr[0]}, ${arr[1]}
for ((n =0 ; n<${#arr[@]} ;n++ ))
do
                echo "${arr[$n]}"
                echo "${title[$n]}"
                cat macro_xsec_2D_1012.cxx | sed "s/__HISTNAME__/${arr[$n]}/g"> macro_tmp2_1.C 
                cat macro_tmp2_1.C | sed "s/__LOGX__/true/g"> macro_tmp2_2.C


if [ $n == 1 ] ; then
                cat macro_tmp2_2.C | sed "s/__RANGES__/7/g"> macro_tmp2_3.C
                cat macro_tmp2_3.C | sed "s/__3DEND__/z0_/g"> macro_tmp2_4.C
                cat macro_tmp2_4.C | sed "s/__IS3D__/true/g"> macro_tmp2.C

else

if [ $n == 2 ] ; then
                cat macro_tmp2_2.C | sed "s/__RANGES__/7/g"> macro_tmp2_3.C
                cat macro_tmp2_3.C | sed "s/__3DEND__/z1_/g"> macro_tmp2_4.C
                cat macro_tmp2_4.C | sed "s/__IS3D__/true/g"> macro_tmp2.C

else

                cat macro_tmp2_2.C | sed "s/__RANGES__/7/g"> macro_tmp2_3.C
                cat macro_tmp2_3.C | sed "s/__3DEND__//g"> macro_tmp2_4.C
                cat macro_tmp2_4.C | sed "s/__IS3D__/false/g"> macro_tmp2.C

fi
fi
		cat macro_tmp2.C | sed "s/__NBIN__/1/g"> macro_tmp3.C
		cat macro_tmp3.C | sed "s/__TITLE__/${title[$n]}/g"> macro_tmp.C


		root -l -q macro_tmp.C 
rm macro_tmp2.C
rm macro_tmp2_1.C
rm macro_tmp2_2.C
rm macro_tmp2_3.C
rm macro_tmp2_4.C
rm macro_tmp3.C
rm macro_tmp.C
done

