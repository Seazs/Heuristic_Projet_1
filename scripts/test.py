#load result.csv and change the comma separator to a semicolon separator
import csv
import os


# Read the CSV file
def read_csv_file(file_path):
    with open(file_path, 'r') as file:
        reader = csv.reader(file, delimiter=',')
        data = [row for row in reader]
    return data
# Write the CSV file with a different delimiter

def write_csv_file(file_path, data):
    with open(file_path, 'w', newline='') as file:
        writer = csv.writer(file, delimiter=';')
        writer.writerows(data)
# Read the original CSV file
input_file_path = 'results.csv'
output_file_path = 'results_semicolon.csv'
data = read_csv_file(input_file_path)
# Write the data to a new CSV file with semicolon delimiter
write_csv_file(output_file_path, data)
