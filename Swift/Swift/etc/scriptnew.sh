#!/bin/bash
export SC_SIGNAL_WRITE_CHECK=DISABLE

file_name="OOK_BW_NL_32"
current_time=$(date "+%Y.%m.%d-%H.%M.%S")
new_filename="$file_name-$current_time.txt" 

# ../bin/ultramulti 0.001 >synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.002 >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.004 >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.006 >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.008 >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.01  >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.03  >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.05  >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.09  >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.1   >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.2   >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.3   >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.4   >>synthetic_traffic_ultramultilogic_16_improved.txt
# ../bin/ultramulti 0.6   >>synthetic_traffic_ultramultilogic_16_improved.txt

#../ultramulti streamcluster_modified.txt >> ../Result/$new_filename
#../ultramulti bodytrack_modified.txt >> ../Result/$new_filename
#../ultramulti canneal_modified.txt >> ../Result/$new_filename
#../ultramulti facesim_modified.txt >> ../Result/$new_filename
#../ultramulti blackscholes_modified.txt >> ../Result/$new_filename
#../ultramulti ferret_modified.txt >> ../Result/$new_filename
../ultramulti swaptions_modified.txt >> ../Result/$new_filename
#../ultramulti fluidanimate_modified.txt >> ../Result/$new_filename
../ultramulti vips_modified.txt >> ../Result/$new_filename
#../ultramulti dedup_modified.txt  >> ../Result/$new_filename
#../ultramulti freqmine_modified.txt >> ../Result/$new_filename
#../ultramulti x264_modified.txt >> ../Result/$new_filename
