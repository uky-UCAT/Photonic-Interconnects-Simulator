#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec  5 18:07:51 2019

@author: sairam
"""
import numpy as np
from os.path import isfile, join
from os import listdir
import matplotlib.pyplot as plt
import pandas as pd

def getLinesList(filePath):
        linesList =[]
        with open(filePath,'r') as resultFile:
          linesList = list(resultFile)
        return linesList
def extractParametersNoxim(parameters,linesList):
       parameterValues={}
       for line in linesList:
           value = line.split(":")[-1]
           parameter = line.split(":")[0].replace("%","").strip()
           if(parameter in parameters):
                 print("Value :",value)
                 print("parameter :",parameter)
       
#                value = re.search("\d*\.?\d+([eE][-+]?\d+)?",line).group(0)
                 parameterValues[parameter]= float(value.replace("\n",""))
       
       return parameterValues;

def plotResultBarChart(basebenchmarkresults,newbenchmarkresults,parameters):
      
    ylabelvalues={}
    ylabelvalues['Total received packets']="Packets"
    ylabelvalues['Global average delay (cycles)']="cycles"
    ylabelvalues['Global average throughput (flits/cycle)']="flits/cycle"
    ylabelvalues['Energy per bit (J/bit)']="Packets"
    ylabelvalues['Average power dissipation (W)']="W"
    ylabelvalues['Max delay (cycles)']="Cycles"
   
       
    for parameter in parameters:
        benchmarklist=[]
        benchmarkparameterlist=[]
        newbenchmarkparameterlist=[]
        variation_redress = [0.15]
        laser_power = [1.20]
        
        for benchmark in basebenchmarkresults:
            benchmarklist.append(benchmark.replace("_modified",""))
            benchmarkparameterlist.append(basebenchmarkresults[benchmark][parameter])
            newbenchmarkparameterlist.append(newbenchmarkresults[benchmark][parameter])
        x = np.arange(len(benchmarklist))
        width = 0.35
        fig,ax= plt.subplots()
        if(parameter=='Average power dissipation (W)'):
            
            baselinebar= ax.bar(x-width/2,benchmarkparameterlist,width,label='Dynamic Power',color='r')
            ax.bar(x-width/2,laser_power,width,bottom=benchmarkparameterlist,label='Laser Power',color='c')
            ax.bar(x-width/2,variation_redress,width,bottom=np.array(benchmarkparameterlist)+np.array(laser_power),label ='Variation Redress',color='m')
            
            newbar= ax.bar(x+width/2,newbenchmarkparameterlist,width,color='r')
            ax.bar(x+width/2,laser_power,width,bottom=newbenchmarkparameterlist,color='c')
            ax.bar(x+width/2,variation_redress,width,bottom=np.array(newbenchmarkparameterlist)+np.array(laser_power),color='m')
              
            ax.set_ylabel(ylabelvalues[parameter])
            ax.set_title(parameter+" "+"Graph")
            ax.set_xticks(x)
            ax.set_xticklabels(benchmarklist)
            ax.legend()
            
        else:    
            
            baselinebar= ax.bar(x-width/2,benchmarkparameterlist,width,label='Baseline')
            newbar= ax.bar(x+width/2,newbenchmarkparameterlist,width,label='Proposed')
        
            ax.set_ylabel(ylabelvalues[parameter])
            ax.set_title(parameter+" "+"Graph")
            ax.set_xticks(x)
            ax.set_xticklabels(benchmarklist)
            ax.legend()
        def autolabel(rects):
#        """Attach a text label above each bar in *rects*, displaying its height."""
               for rect in rects:
                     height = rect.get_height()
                     ax.annotate('{}'.format(height),
                                 xy=(rect.get_x() + rect.get_width() / 2, height),
                                 xytext=(0, 3),  # 3 points vertical offset
                                 textcoords="offset points",
                                 ha='center', va='bottom')
        
        
#        autolabel(baselinebar)
#        autolabel(newbar)
        
        fig.tight_layout(rect=[0, 1, 1,1])
        plt.xticks(rotation='vertical')
        plt.show()
        fig.savefig(parameter.replace(" ","").replace("/"," per ").replace("(","").replace(")","")+".jpeg")
        
#      X = np.arange(len(benchmarkresults['canneal_modified']))
#      barchart = plt.subplot(111)
#      barchart.bar(X,benchmarkresults['canneal_modified'],width=0.2,color='b',align='center')
#      barchart.bar(X-0.2,benchmarkresults['fluidanimate_modified'],width=0.2,color='g',align='center')
#      barchart.legend(('canneal_modified','fluidanimate_modified'))
#      plt.xticks(X,benchmarkresults['canneal_modified'].keys())
#      plt.title("Benchmark Results",fontsize=17)
#      plt.show()
def comparediffofresults(basebenchmarkresults,newbenchmarkresults,parameters):
    diffofresult={}
    
    for benchmark in basebenchmarkresults.keys():
        diffofbenchmark={}
        basebenchmarkresult = basebenchmarkresults[benchmark]
        newbenchmarkresult = newbenchmarkresults[benchmark]
           
        for parameter in parameters:
            
            diffofbenchmark[parameter] =  newbenchmarkresult[parameter]-basebenchmarkresult[parameter]
        diffofresult[benchmark] = diffofbenchmark
    return diffofresult

baseresultsfolder = "/home/sairam/Desktop/Swift/1M/"
newresultsfolder ="/home/sairam/Desktop/Swift/1M/proposed/"

parameters=['Total received packets','Global average delay (cycles)','Global average throughput (flits/cycle)','Energy per bit (J/bit)','Average power dissipation (W)','Max delay (cycles)']
baseresultfiles = [f for f in listdir(baseresultsfolder) if f.endswith(".txt") if isfile(join(baseresultsfolder, f))]
newresultfiles =[f for f in listdir(newresultsfolder) if f.endswith(".txt") if isfile(join(newresultsfolder, f))] 
benchmarkresults={}
newbenchmarkresults={}

for baseresultfile in baseresultfiles:
    linesOfFile = getLinesList(baseresultsfolder+baseresultfile)
#    print(linesOfFile)
    benchmarkname = baseresultfile.replace(".txt","")
    benchmarkresults[benchmarkname] = extractParametersNoxim(parameters,linesOfFile)
for newresultfile in newresultfiles:
    linesOfFile = getLinesList(newresultsfolder+newresultfile)
#    print(linesOfFile)
    benchmarkname = newresultfile.replace(".txt","")
    newbenchmarkresults[benchmarkname] = extractParametersNoxim(parameters,linesOfFile)

 
#print(comparediffofresults(benchmarkresults,newbenchmarkresults,parameters))   
print(newbenchmarkresults)
#print(benchmarkresults)
plotResultBarChart(benchmarkresults,newbenchmarkresults,parameters)


