import sys
import os
import re
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d
from scipy.optimize import curve_fit
import numpy as np
import time

MAX_PLOT_RANGE = 1
daily_infected_cum = {}
daily_tested_cum = {}
daily_non_cum = {}
fitted_data = {}
info = {}
fitted_threshold = 15
OUTPUT_DIR = '.'
R_No_p = 0
R_No_s = 0

def get_file_name(title,ext):
    title = os.path.join(OUTPUT_DIR,title)
    return title+'_'+time.strftime("%Y%m%d-%H%M%S")+'.'+ext

#def exp_model (x, a, b):
#    return a*(b**x)

def exp_model (x, a, b):
    return a*np.exp(b*x)

def read_file(file):
    inp_array = []
    with open(file) as f:
        for val in f:
            if val == '\n':
                break
            inp_array.append(val)
    return inp_array

def read_run_outputs(file):
    dict_influenza = {}
    with open(file) as table:
        for row_idx, line in enumerate(table):
            line = line.strip()
            row = re.split('\t{1,2}|  | \t| ',line)
            if row_idx==0:
                headers = row
                for header in headers:
                    dict_influenza[header.lower()] = []
            else:
                for header,el in zip(headers,row):
                    dict_influenza[header.lower()].append(int(el))

        return dict_influenza

def create_subplot(axis,run,subplt1,subplt2,subplt3,title,n_days, scatter_run=None):
    legend = []

    axis.plot(run['day'][:n_days],run[subplt1][:n_days])
    legend.append(subplt1)
    axis.plot(run['day'][:n_days],run[subplt2][:n_days])
    legend.append(subplt2)
    if subplt3 is not None:
        axis.plot(run['day'][:n_days],run[subplt3][:n_days])
        legend.append(subplt3)

    if scatter_run is not None:
        axis.scatter(scatter_run['day'][:n_days],scatter_run[subplt1][:n_days],s=[1]*n_days)
        legend[0] = legend[0] + " gr " + str(round(info[subplt1]['growth_rate'],4))
        axis.scatter(scatter_run['day'][:n_days],scatter_run[subplt2][:n_days],s=[1]*n_days)
        legend[1] = legend[1] + " gr " + str(round(info[subplt2]['growth_rate'],4))
        if subplt3 is not None:
            axis.scatter(scatter_run['day'][:n_days],scatter_run[subplt3][:n_days],s=[1]*n_days)
            legend[2] = legend[2] + " gr " + str(round(info[subplt3]['growth_rate'],4))

    axis.legend(legend,fontsize=6)
    axis.tick_params(axis='both', labelsize=8)
    axis.set_title(title,fontsize=6)
    axis.set_xticks(range(min(run['day']),1+n_days,2))

    return axis

def get_from_cum(cum_data):
    data = [cum_data[0]]
    for i in range(1,len(cum_data)):
        data.append(cum_data[i]-cum_data[i-1])
    return data

def compute_non_cum_data():
    for key in daily_tested_cum.keys():
        if key == 'day':
            daily_non_cum['day'] = daily_tested_cum['day']
        else:
            daily_non_cum[key] = get_from_cum(daily_tested_cum[key])

    for key in daily_infected_cum.keys():
        if key == 'day' or key == 'recovered_p' or key == 'recovered_s':
            continue
        else:
            daily_non_cum[key] = get_from_cum(daily_infected_cum[key])

def get_fitted_data(data_set, key, lim):
    popt, pcov = curve_fit(exp_model, data_set['day'][:lim], data_set[key][:lim])

    return [exp_model(np.array(data_set['day'][:lim]), *popt), *popt]


def compute_fitted_data(lim):
    fitted_data_keys = ['day','infected_p','submit_p','infected_total','submit','msss_p','msss']
    for key in fitted_data_keys:
        if key == 'day':
            fitted_data['day'] = daily_non_cum['day'][:lim]
        else:
            info[key] = {}
            fitted_data[key], info[key]['intersect'], info[key]['growth_rate'] = get_fitted_data(daily_non_cum, key, lim)

def create_plot():
    fig, ax = plt.subplots(2,3,figsize=(10,7))

    fig.suptitle('Influenza Simulation Run With Surveillance')
    fig.text(.5, .01, 'Reproduction No - Pandemic: '+R_No_p+' Seasonal: '+R_No_s, ha='center')
    ax[0][0] = create_subplot(ax[0][0], daily_non_cum, 'test_p', 'submit_p', 'msss_p', 'pandemic',20)
    ax[0][1] = create_subplot(ax[0][1], daily_non_cum, 'test_s', 'submit_s', 'msss_s', 'seasonal',20)
    ax[0][2] = create_subplot(ax[0][2], daily_non_cum, 'test'  , 'submit'  , 'msss'  , 'total'   ,20)

    ax[1][0] = create_subplot(ax[1][0], daily_non_cum, 'infected_p'     , 'infected_s'     , 'infected_total', 'Infectious Cases', 20)
    #ax[1][1] = create_subplot(ax[1][1], daily_non_cum, 'infected_p_only', 'infected_s_only', None            , 'Infectious Cases', 20)
    ax[1][1] = create_subplot(ax[1][1], fitted_data  , 'infected_p'     , 'submit_p'       , 'msss_p'            , 'Fitted Pandemic Infectious & Submission', fitted_threshold, daily_non_cum)
    ax[1][2] = create_subplot(ax[1][2], fitted_data  , 'infected_total' , 'submit'         , 'msss'            , 'Fitted Infectious & Submission', fitted_threshold, daily_non_cum)

    #plt.show()
    fname = get_file_name('plot_chart','png')
    plt.savefig(fname,dpi=300)
    print("Output Plot:\t",fname)

if __name__ == '__main__':
    daily_infected_cum = read_run_outputs(os.path.join(OUTPUT_DIR,'Run_no0_output_general_stats.txt'))
    daily_tested_cum = read_run_outputs(os.path.join(OUTPUT_DIR,'Run_no0_number_daily_tested.txt'))

    constants = read_file(os.path.join(OUTPUT_DIR,'constants.txt'))
    R_No_p = constants[0]
    R_No_s = constants[1]

    compute_non_cum_data()
    compute_fitted_data(fitted_threshold)

    create_plot()
