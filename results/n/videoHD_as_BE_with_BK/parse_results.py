import csv
from pprint import pprint
import numpy as np
from copy import deepcopy
import os
import xlsxwriter

template_dict = {"number_of_video_station": int,
                 "throughput_sum": float,
                 "throughput_mean": float,
                 "delay_mean": float,
                 "jitter_mean": float,
                 "tx_packets_sum": float,
                 "lost_packets_sum": float,
                 "lost_percentage": float
                }

mypath = os.path.dirname(os.path.realpath(__file__))
pprint(mypath)
onlyfiles = [f for f in os.listdir(mypath) if os.path.isfile(os.path.join(mypath, f))]
onlyfiles = [f for f in onlyfiles if "csv" in f]

workbook = xlsxwriter.Workbook(mypath + '/results.xlsx')
worksheet = workbook.add_worksheet()
worksheet.write(0, 0, "Sta_num")
worksheet.write(0, 1, "throughput_sum")
worksheet.write(0, 2, "throughput_mean")
worksheet.write(0, 3, "delay_mean")
worksheet.write(0, 4, "jitter_mean")
worksheet.write(0, 5, "tx_packets_sum")
worksheet.write(0, 6, "lost_packets_sum")
worksheet.write(0, 7, "lost_percentage")

row = 1
for file_name in onlyfiles:
    vi_dict = deepcopy(template_dict)
    bk_dict = deepcopy(template_dict)
    vi_thr_list = []
    vi_delay_list = []
    vi_jitter_list = []
    vi_tx_packet_list = []
    vi_lost_list = []
    bk_thr_list = []
    bk_delay_list = []
    bk_jitter_list = []
    bk_tx_packet_list = []
    bk_lost_list = []
    sta_count = 0
    col = 0
    with open(file_name) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        for line in csv_reader:
            # BK
            if "1000" in line[2]:
                try:
                    bk_thr_list.append(float(line[3]))
                    bk_delay_list.append(float(line[4]))
                    bk_jitter_list.append(float(line[5]))
                    bk_tx_packet_list.append(float(line[6]))
                    bk_lost_list.append(float(line[8]))
                except ValueError:
                    pprint("No date to parse")
                    break
            elif "1005" in line[2]:
                try:
                    sta_count += 1
                    vi_thr_list.append(float(line[3]))
                    vi_delay_list.append(float(line[4]))
                    vi_jitter_list.append(float(line[5]))
                    vi_tx_packet_list.append(float(line[6]))
                    vi_lost_list.append(float(line[8]))
                except ValueError:
                    pprint("No date to parse")
                    break
            else:
                print("First line of file")
    vi_dict["number_of_video_station"] = sta_count
    vi_dict["throughput_sum"] = sum(vi_thr_list)
    vi_dict["throughput_mean"] = np.mean(vi_thr_list)
    vi_dict["delay_mean"] = np.mean(vi_delay_list)
    vi_dict["jitter_mean"] = np.mean(vi_jitter_list)
    vi_dict["tx_packets_sum"] = sum(vi_tx_packet_list)
    vi_dict["lost_packets_sum"] = sum(vi_lost_list)
    vi_dict["lost_percentage"] = sum(vi_lost_list) / sum(vi_tx_packet_list)
    worksheet.write(row, 0, vi_dict["number_of_video_station"])
    worksheet.write(row, 1, vi_dict["throughput_sum"])
    worksheet.write(row, 2, vi_dict["throughput_mean"])
    worksheet.write(row, 3, vi_dict["delay_mean"])
    worksheet.write(row, 4, vi_dict["jitter_mean"])
    worksheet.write(row, 5, vi_dict["tx_packets_sum"])
    worksheet.write(row, 6, vi_dict["lost_packets_sum"])
    worksheet.write(row, 7, vi_dict["lost_percentage"])
    row += 1
workbook.close()
"""
    bk_dict["throughput_sum"] = sum(bk_thr_list)
    bk_dict["throughput_mean"] = np.mean(bk_thr_list)
    bk_dict["delay_mean"] = np.mean(bk_delay_list)
    bk_dict["jitter_mean"] = np.mean(bk_jitter_list)
    bk_dict["tx_packets_sum"] = sum(bk_tx_packet_list)
    bk_dict["lost_packets_sum"] = sum(bk_lost_list)
    bk_dict["lost_percentage"] = sum(bk_lost_list) / sum(bk_tx_packet_list)

    pprint(bk_dict)
"""