import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import numpy as np
import pathlib


# define the path
#pathPart = 'Swift1'
pathPart= 'Clos_Code'
path='/home/sairam/Desktop/Dev/'+pathPart+'/etc'
currentDirectory = pathlib.Path(path)
tag = 'BW'
#tag= 'Rel'
#tag='_Res_' 

sim_runTime = 100000000-10000
#Simulation time = 100M cycles in total - 10K cycles for warmup  
cyc_time = 0.2*(10**-9)
packet_size = 512

#Number of benchmarks for which the test was run
Num_Benchmarks = 12
#The item you want to plot
#plotDat = 'Throughput'
plotDat= 'average delay ('
#plotDat= 'Energy per bit'
#plotDat = 'power' #Use for EPB as well

#Lists to hold necessary data to plot for the four modulation schemes
L_ook_20 =[]
L_ss_20 =[]
L_edac_20 =[]
L_odac_20 =[]
L_ook_12 =[]
L_ss_12 =[]
L_edac_12 =[]
L_odac_12 =[]

L_thru_ook_20 =[]
L_thru_ss_20 =[]
L_thru_edac_20=[]
L_thru_odac_20 =[]
L_thru_ook_12 =[]
L_thru_ss_12 =[]
L_thru_edac_12=[]
L_thru_odac_12 =[]

L_thru = []

YERR_min =[]
YERR_max = []

#power calculated for concentrators and GIs using DSENT
pow_conc =  0.126
pow_GI = 0.1244

def getFiles():
    for currentFile in currentDirectory.iterdir():
#         print(str(currentFile))  
        if tag in str(currentFile) and '.txt' in str(currentFile):
            getData(currentFile)

    return None
'''            
    print(L_ook)
    print(L_ss)
    print(L_edac)
    print(L_odac)
    print(L_thru)
'''        
        
def getData(fileName):    
    if 'OOK' in str(fileName):
        if 'FSR20' in str(fileName):
            fillList(fileName,L_ook_20,L_thru_ook_20)
        elif 'FSR12' in str(fileName):
            fillList(fileName,L_ook_12,L_thru_ook_12)
        print('OOK found')
    elif 'SS' in str(fileName):
        if 'FSR20' in str(fileName):
            fillList(fileName,L_ss_20,L_thru_ss_20)
        elif 'FSR12' in str(fileName):
            fillList(fileName,L_ss_12,L_thru_ss_12)
    elif 'EDAC' in str(fileName):
        if 'FSR20' in str(fileName):
            fillList(fileName,L_edac_20,L_thru_edac_20)
        elif 'FSR12' in str(fileName):
            fillList(fileName,L_edac_12,L_thru_edac_12)
    elif 'ODAC' in str(fileName):
        if 'FSR20' in str(fileName):
            fillList(fileName,L_odac_20,L_thru_odac_20)
        elif 'FSR12' in str(fileName):
            fillList(fileName,L_odac_12,L_thru_odac_12)  
    return None

def fillList(path,List,LThru):
    file_object = open(str(path),'r')
    for line in file_object:
        parts = line.split(':')
        if plotDat in parts[0]:
            List.append(float(parts[1].strip()))
        if "(flits/" in parts[0]:
            LThru.append(float(parts[1].strip()))            
    return None
            
           

def plot():
    L_thru.extend(L_thru_ook_12)
    L_thru.extend(L_thru_ook_20)
    L_thru.extend(L_thru_ss_12)
    L_thru.extend(L_thru_ss_20)
    L_thru.extend(L_thru_edac_12)
    L_thru.extend(L_thru_edac_20)
    L_thru.extend(L_thru_odac_12)
    L_thru.extend(L_thru_odac_20)
    
    
    #Benchmarks=['streamcluster','bodytrack','canneal','facesim','blacksholes','ferret','swaptions','fluidanimate','vips','dedup','freqmine','x-264']
    Benchmarks=['streamcluster','bodytrack','canneal','facesim','blacksholes','ferret','fluidanimate','dedup','freqmine','x-264']#,'swaptions','vips']
    #Benchmarks=['streamcluster','bodytrack','facesim','blacksholes','swaptions','fluidanimate','vips','dedup','x-264']
    
    if 'Clos' in pathPart:
        Label = 'Clos'
        electrical = 16*pow_conc + 8*pow_GI
    else:
        Label = 'SwiftNoC'   
        electrical = 0*pow_conc + 16*pow_GI
        
#Laser and Heating power for Bandwidth and Reliability optimal varieties of Cloas and SwiftNoC        
    if tag in 'BW' and Label in 'Clos':
        MR_heating=np.array([1.5947,1.5947,2.4059,2.4059,1.5947,1.5947,1.5947,1.5947],dtype=np.float)
        MR_min=np.array([1.053,1.053,1.1921,1.1921,1.053,1.053,1.053,1.053],dtype=np.float)
        MR_max=np.array([4.188,4.188,4.944,4.944,4.188,4.188,4.188,4.188],dtype=np.float)
        Laser_min=np.array([4.18,13.38,14.693,8.13,4.18,13.38,4.18,13.38,],dtype=np.float)
        Laser_max=np.array([4.773,14.8,16.127,9.067,4.18,13.38,4.18,13.38,],dtype=np.float)  
        Laser = np.add(Laser_max,Laser_min)/2
    if tag in 'Rel' and Label in 'Clos':
        MR_heating=np.array([1.4565,1.4565,2.2677,2.2677,1.4565,1.4565,1.4565,1.4565],dtype=np.float)
        MR_min=np.array([0.543,0.543,0.3307,0.3307,0.543,0.543,0.543,0.543],dtype=np.float)
        MR_max=np.array([2.158,2.518,1.247,1.247,2.158,2.158,2.158,2.158],dtype=np.float)  
        Laser_min=np.array([1.493,3.82,13.38,32.27,4.48,10.75,4.48,10.75],dtype=np.float)
        Laser_max=np.array([1.67,4.18,14,34.67,4.6,11.227,4.6,11.227],dtype=np.float)
        Laser = np.add(Laser_max,Laser_min)/2
    if tag in 'BW' and Label in 'SwiftNoC':
        MR_heating=np.array([3.6342,3.6342,4.5394,4.5394,3.6342,3.6342,3.6342,3.6342],dtype=np.float)
        MR_min=np.array([2.023,2.023,1.33,1.33,2.023,2.023,2.023,2.023],dtype=np.float)
        MR_max=np.array([8.677,8.677,5.55,5.55,8.677,8.677,8.677,8.677],dtype=np.float)  
        Laser_min=np.array([6.48,3.753,14.693,8.13,6.48,3.753,6.48,3.753],dtype=np.float)
        Laser_max=np.array([7.17,14.8,16.127,9.067,7.17,14.8,7.17,14.8],dtype=np.float)
        Laser = np.add(Laser_max,Laser_min)/2
    if tag in 'Rel' and Label in 'SwiftNoC':
        MR_heating=np.array([3.4865,3.4865,4.2362,4.2362,3.4865,3.4865,3.4865,3.4865],dtype=np.float)
        MR_min=np.array([1.035,1.035,0.676,0.676,1.035,1.035,1.035,1.035],dtype=np.float)
        MR_max=np.array([4.364,4.364,2.79,2.79,4.364,4.364,4.364,4.364],dtype=np.float)    
        Laser_min=np.array([2.147,5.46,6.573,6.313,6.42,4.81,6.42,4.81],dtype=np.float)
        Laser_max=np.array([2.387,5.94,6.773,6.74,6.487,4.95,6.487,4.95],dtype=np.float)
        Laser = np.add(Laser_max,Laser_min)/2
  
    l1 = np.array(L_ook_12,dtype=np.float)  
    l2 = np.array(L_ook_20,dtype=np.float)
    l3 = np.array(L_ss_12,dtype=np.float)
    l4 = np.array(L_ss_20,dtype=np.float)
    l5 = np.array(L_edac_12,dtype=np.float)
    l6 = np.array(L_edac_20,dtype=np.float)  
    l7 = np.array(L_odac_12,dtype=np.float)
    l8 = np.array(L_odac_20,dtype=np.float)
    
    if plotDat in 'power':
        if Label in 'Clos':
            Ncore = 64
        else:
            Ncore =1
        
        pow_indep = np.add(Laser,MR_heating)
        pow_indep_max = np.add(Laser_max,MR_max)
        pow_indep_min = np.add(Laser_min,MR_min)
        
        #For completing power graphs, just extract TxRx power for each modulation
        
        #EPB calculation as follows; to revert get rid of division operation and the denominator
        l1_min = (l1+ float(pow_indep_min[0]) +electrical)/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float)*Ncore)
        l2_min = (l2+ float(pow_indep_min[1]) +electrical)/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float)*Ncore)
        l3_min = (l3+ float(pow_indep_min[2]) +electrical)/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float)*Ncore)
        l4_min = (l4+ float(pow_indep_min[3]) +electrical)/(np.array(L_thru[3*Num_Benchmarks:4*Num_Benchmarks],dtype=np.float)*Ncore)
        l5_min = (l5+ float(pow_indep_min[4]) +electrical)/(np.array(L_thru[4*Num_Benchmarks:5*Num_Benchmarks],dtype=np.float)*Ncore)
        l6_min = (l6+ float(pow_indep_min[5]) +electrical)/(np.array(L_thru[6*Num_Benchmarks:7*Num_Benchmarks],dtype=np.float)*Ncore)
        l7_min = (l7+ float(pow_indep_min[6]) +electrical)/(np.array(L_thru[7*Num_Benchmarks:8*Num_Benchmarks],dtype=np.float)*Ncore)
        l8_min = (l8+ float(pow_indep_min[7]) +electrical)/(np.array(L_thru[8*Num_Benchmarks:],dtype=np.float)*Ncore)
        l1_max = (l1+ float(pow_indep_max[0]) +electrical)/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float)*Ncore)
        l2_max = (l2+ float(pow_indep_max[1]) +electrical)/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float)*Ncore)
        l3_max = (l3+ float(pow_indep_max[2]) +electrical)/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float)*Ncore)
        l4_max = (l4+ float(pow_indep_max[3]) +electrical)/(np.array(L_thru[3*Num_Benchmarks:4*Num_Benchmarks],dtype=np.float)*Ncore) 
        l5_max = (l5+ float(pow_indep_max[4]) +electrical)/(np.array(L_thru[4*Num_Benchmarks:5*Num_Benchmarks],dtype=np.float)*Ncore)
        l6_max = (l6+ float(pow_indep_max[5]) +electrical)/(np.array(L_thru[6*Num_Benchmarks:7*Num_Benchmarks],dtype=np.float)*Ncore)
        l7_max = (l7+ float(pow_indep_max[6]) +electrical)/(np.array(L_thru[7*Num_Benchmarks:8*Num_Benchmarks],dtype=np.float)*Ncore)
        l8_max = (l8+ float(pow_indep_max[7]) +electrical)/(np.array(L_thru[8*Num_Benchmarks:],dtype=np.float)*Ncore)
        l1 = (l1+ float(pow_indep[0]) +electrical)/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float)*Ncore)
        l2 = (l2+ float(pow_indep[1]) +electrical)/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float)*Ncore)
        l3 = (l3+ float(pow_indep[2]) +electrical)/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float)*Ncore)
        l4 = (l4+ float(pow_indep[3]) +electrical)/(np.array(L_thru[3*Num_Benchmarks:4*Num_Benchmarks],dtype=np.float)*Ncore)
        l5 = (l5+ float(pow_indep[4]) +electrical)/(np.array(L_thru[4*Num_Benchmarks:5*Num_Benchmarks],dtype=np.float)*Ncore)
        l6 = (l6+ float(pow_indep[5]) +electrical)/(np.array(L_thru[6*Num_Benchmarks:7*Num_Benchmarks],dtype=np.float)*Ncore)
        l7 = (l7+ float(pow_indep[6]) +electrical)/(np.array(L_thru[7*Num_Benchmarks:8*Num_Benchmarks],dtype=np.float)*Ncore)
        l8 = (l8+ float(pow_indep[7]) +electrical)/(np.array(L_thru[8*Num_Benchmarks:],dtype=np.float)*Ncore)
        for i in range(len(l1)):
            YERR_min.extend([l1_min[i]/l1[i],l2_min[i]/l1[i],l3_min[i]/l1[i],l4_min[i]/l1[i],l5_min[i]/l1[i],l6_min[i]/l1[i],l7_min[i]/l1[i],l8_min[i]/l1[i]])
            YERR_max.extend([l1_max[i]/l1[i],l2_max[i]/l1[i],l3_max[i]/l1[i],l4_max[i]/l1[i],l5_max[i]/l1[i],l6_max[i]/l1[i],l7_max[i]/l1[i],l8_max[i]/l1[i]])                     
      
    fig, ax =plt.subplots()
    
    # if it is power or EPB, min-max error bars are necessary; hence seperate code for plotting for them
    if plotDat in 'power' or plotDat in 'Energy per bit': 
        if l1.size:
            ax.bar(np.arange(len(Benchmarks))-0.2,l1/l1,width=0.1,align='center',color='IndianRed', yerr=[abs(l1/l1 -YERR_min[0::4]),abs(l1/l1 -YERR_max[0::4])],capsize=3,label=Label+'_OOK')
        if l2.size:
            ax.bar(np.arange(len(Benchmarks))-0.1,l2/l1,width=0.1,align='center',color='orange',yerr=[abs(l2/l1 -YERR_min[1::4]),abs(l2/l1 -YERR_max[1::4])],capsize=3,label =Label+'_SS')
        if l3.size:
            ax.bar(np.arange(len(Benchmarks)),l3/l1,width =0.1,align='center',color='Green',yerr=[abs(l3/l1 -YERR_min[2::4]),abs(l3/l1 -YERR_max[2::4])],capsize=3,label =Label+'_EDAC')
        if l4.size:
            ax.bar(np.arange(len(Benchmarks))+0.1,l4/l1,width=0.1,align='center',color='cyan',yerr=[abs(l4/l1 -YERR_min[3::4]),abs(l4/l1 -YERR_max[3::4])],capsize=3,label=Label+'_ODAC')
    else:
        if l1.size:
            ax.bar(np.arange(len(Benchmarks))-0.2,l1/l1,width=0.1,align='center',color='IndianRed', label=Label+'_OOK')
        if l2.size:
            ax.bar(np.arange(len(Benchmarks))-0.1,l2/l1,width=0.1,align='center',color='orange',label =Label+'_SS')
        if l3.size:
            ax.bar(np.arange(len(Benchmarks)),l3/l1,width =0.1,align='center',color='Green',label =Label+'_EDAC')
        if l4.size:
            ax.bar(np.arange(len(Benchmarks))+0.1,l4/l1,width=0.1,align='center',color='cyan',label=Label+'_ODAC')        
  

    ax.set_xticks(np.arange(len(Benchmarks)))
    ax.set_xticklabels(Benchmarks)
    ax.set_xlabel('Benchmarks')
    if plotDat in 'throughput':
        ax.set_ylabel('Normalized Throughput')
    if plotDat in 'average delay (':
        ax.set_ylabel('Normalized Delay')
    if plotDat in 'Energy per bit':
        ax.set_ylabel('Normalized EPB')
    if plotDat in 'power':
        ax.set_ylabel('Normalized EPB')
    
    ax.legend()   
    fig.tight_layout()
    
    if tag is 'BW':
        plt.title('Plotted for Bandwidth Optimal results')
    elif tag is 'Rel':
        plt.title('Plotted for Reliability Optimal results')
    
    plt.show() 
    

getFiles()    
plot() 


''' 
        
    if plotDat in 'Energy per bit':
        pow_indep = np.add(Laser,MR_heating)
        pow_indep_max = np.add(Laser_max,MR_max)
        pow_indep_min = np.add(Laser_min,MR_min)
        l1_min = l1+ (float(pow_indep_min[0]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float))#*packet_size) 
        l2_min = l2+ (float(pow_indep_min[1]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l3_min = l3+ (float(pow_indep_min[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l4_min = l4+ (float(pow_indep_min[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[3*Num_Benchmarks:],dtype=np.float))#*packet_size) 
        l1_max = l1+ (float(pow_indep_max[0]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float))#*packet_size) 
        l2_max = l2+ (float(pow_indep_max[1]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l3_max = l3+ (float(pow_indep_max[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l4_max = l4+ (float(pow_indep_max[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[3*Num_Benchmarks:],dtype=np.float))#*packet_size) 
        l1 = l1+ (float(pow_indep[0]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[0:Num_Benchmarks],dtype=np.float))#*packet_size) 
        l2 = l2+ (float(pow_indep[1]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[Num_Benchmarks:2*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l3 = l3+ (float(pow_indep[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[2*Num_Benchmarks:3*Num_Benchmarks],dtype=np.float))#*packet_size) 
        l4 = l4+ (float(pow_indep[2]) +electrical)*sim_runTime*cyc_time/(np.array(L_thru[3*Num_Benchmarks:],dtype=np.float))#*packet_size) 
        for i in range(len(l1)):
            YERR_min.extend([l1_min[i]/l1[i],l2_min[i]/l1[i],l3_min[i]/l1[i],l4_min[i]/l1[i]])
            YERR_max.extend([l1_max[i]/l1[i],l2_max[i]/l1[i],l3_max[i]/l1[i],l4_max[i]/l1[i]])                       
        print('Yerror min :',YERR_min)
        print()    
        print('YERR max:',YERR_max)    
'''
